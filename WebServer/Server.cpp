#include "Server.h"

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/Socket.h>
#include <Arc/Log.h>

#include "ServerConfig.h"

Server::Server( void )
{
	mp_ServerConfig = New ServerConfig();

	mp_ServerConfig->loadMainConfig("conf/main.cfg");
	mp_ServerConfig->loadMIMETypes("conf/mime.cfg");
}

Server::~Server( void )
{
	delete mp_ServerConfig;

	if (mp_ServerSocket != nullptr)
		mp_ServerSocket->disconnect();
	delete mp_ServerSocket;
}

bool Server::run( void )
{
	const string& ENDLINE = "\r\n";

	mp_ServerSocket = New ServerSocket();
	mp_ServerSocket->bindLocal(mp_ServerConfig->getPort(), SOCKET_TYPE_TCP);

	if (mp_ServerSocket->hasError())
	{
		Log::Error(getClassName(), "Server Socket failed to initialize");
		mp_ServerSocket->disconnect();
		delete mp_ServerSocket;
		return false;
	}

	Log::InfoFmt(getClassName(), "Listening on port %d", mp_ServerConfig->getPort());

	while (true)
	{
		Socket* pClient = mp_ServerSocket->acceptClient();
		Log::InfoFmt(getClassName(), "S: Connection from %s", pClient->getAddress().toString().c_str());

		const string& request = pClient->recvLine();
		Log::InfoFmt(getClassName(), "C: Request: %s", request.c_str());
		ArrayList<string> requestPieces = Arc_StringSplit(request, ' ');

		if (requestPieces.getSize() < 3)
		{
			Log::Error(getClassName(), "Malformed Request");
			pClient->disconnect();
			delete pClient;
			delete mp_ServerSocket;
			continue;
		}

		const string& method = requestPieces[0];
		const string& path = requestPieces[1];
		const string& version = requestPieces[2];

		Log::InfoFmt("Main", "C: Method: %s", method.c_str());
		Log::InfoFmt("Main", "C: Path: %s", path.c_str());
		Log::InfoFmt("Main", "C: Version: %s", version.c_str());

		Map<string, string> headers;
		string line;
		while (true)
		{
			line = pClient->recvLine();
			if (line == "") break;

			Log::InfoFmt("Main", "C: Header: %s", line.c_str());

			ArrayList<string> split = Arc_StringSplit(line, ':', 2);
			if (split.getSize() < 2)
			{
				Log::Error("Main", "Malformed Header");
				continue;
			}
			Arc_TrimRight(split[0]);
			Arc_TrimLeft(split[1]);
			Arc_StringToLower(split[0]);

			headers.add(split[0], split[1]);
		}

		Log::InfoFmt("Main", "S: Read %d Headers", headers.getSize());

		// Web Root
		string realPath = mp_ServerConfig->getWebRoot() + path;

		if (realPath.back() == '/')
		{
			realPath.append("index.html");
		}

		string ext = Arc_FileExtension(realPath);
		string typeString = mp_ServerConfig->getMIMEType(ext);
		ios::openmode mode = ios::in | ios::binary;

		ifstream file(realPath, mode);

		if ( ! file)
		{
			const string& response = "HTTP/1.0 404 Not Found";
			pClient->sendString(response + ENDLINE, false);
			Log::InfoFmt("Main", "S: Response: %s", response.c_str());

			file.close();
			file.open(mp_ServerConfig->getWebRoot() + "/" + mp_ServerConfig->getErrorPage404());
		}
		else
		{
			const string& response = "HTTP/1.0 200 OK";
			pClient->sendString(response + ENDLINE, false);
			Log::InfoFmt("Main", "S: Response: %s", response.c_str());
		}

		stringstream contentTypeHeader;
		contentTypeHeader << "Content-Type: " << typeString;
		pClient->sendString(contentTypeHeader.str() + ENDLINE, false);
		Log::InfoFmt("Main", "S: %s", contentTypeHeader.str().c_str());

		const string& serverHeader = "Server: Coeus 0.1";
		pClient->sendString(serverHeader + ENDLINE, false);
		Log::InfoFmt("Main", "S: %s", serverHeader.c_str());

		stringstream locationHeader;
		locationHeader << "Location: http://" << headers["host"] << path;
		pClient->sendString(locationHeader.str() + ENDLINE);
		Log::InfoFmt("Main", "S: %s", locationHeader.str().c_str());

		// Get file size
		std::streampos fsize;
		fsize = file.tellg();
		file.seekg(0, std::ios::end);
		fsize = file.tellg() - fsize;
		file.seekg(0, std::ios::beg);

		stringstream contentSizeHeader;
		contentSizeHeader << "Content-Size: " << fsize;
		pClient->sendString(contentSizeHeader.str() + ENDLINE, false);
		Log::InfoFmt("Main", "S: %s", contentSizeHeader.str().c_str());

		pClient->sendString(ENDLINE, false);

		const int TMP_BUFFER_SIZE = 4096;

		char tmp_buffer[TMP_BUFFER_SIZE];
		std::streamsize n;
		do
		{
			file.read(tmp_buffer, TMP_BUFFER_SIZE);
			n = file.gcount();

			if (n == 0)
				break;

			pClient->sendBuffer(tmp_buffer, (unsigned int)n);

			if ( ! file )
				break;
		}
		while (n > 0);

		pClient->disconnect();
		delete pClient;
	}
}

#include <iostream>

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/ServerSocket.h>
#include <Arc/Socket.h>

using namespace std;
using namespace Arc;

int main( int argc, char* argv[] )
{
	Arc_InitCore();
	Arc_InitNet();

	const int PORT = 8080;

	ServerSocket ss = ServerSocket();
	ss.bindLocal(PORT, SOCKET_TYPE_TCP);

	if (ss.hasError())
	{
		cout << "Server failed to initialize" << endl;
	}

	cout << "Server listening on port " << PORT << endl;

	ofstream log("http.log");

	while (true)
	{
		Socket* pClient = ss.acceptClient();
		cout << "Connection from " << pClient->getAddress().toString() << endl;
		log << "I: Connection from " << pClient->getAddress().toString() << endl;

		string request = pClient->recvLine();
		log << "C: " << request << endl;
		ArrayList<string> requestPieces = Arc_StringSplit(request, ' ');

		if (requestPieces.getSize() < 3)
		{
			cout << "Error: Malformed Request" << endl;
			pClient->disconnect();
			delete pClient;
			continue;
		}

		string method = requestPieces[0];
		string path = requestPieces[1];
		string version = requestPieces[2];

		cout << "Method: " << method << endl;
		cout << "Path: " << path << endl;
		cout << "Version: " << version << endl;

		cout << "Headers: " << endl;

		Map<string, string> headers;
		string line;
		while (true)
		{
			line = pClient->recvLine();
			log << "C: " << line << endl;

			if (line == "") break;

			cout << line << endl;

			ArrayList<string> split = Arc_StringSplit(line, ':', 2);
			if (split.getSize() < 2)
			{
				cout << "Error: Malformed Header" << endl;
				continue;
			}
			Arc_TrimRight(split[0]);
			Arc_TrimLeft(split[1]);
			Arc_StringToLower(split[0]);

			headers.add(split[0], split[1]);
		}

		cout << "Read " << headers.getSize() << " Headers" << endl;

		// Web Root
		string realPath = "html" + path;

		if (realPath.back() == '/')
		{
			realPath.append("index.html");
		}

		cout << endl;

		ifstream file(realPath);

		if ( ! file)
		{
			pClient->sendString("HTTP/1.0 404 Not Found\r\n", false);
			cout << "HTTP/1.0 404 Not Found\r\n";
			log << "S: " << "HTTP/1.0 404 Not Found\r\n";

			pClient->sendString("Content-Size: 0\r\n", false);
			cout << "Content-Size: 0\r\n";
			log << "S: " << "Content-Size: 0\r\n";

			pClient->sendString("\r\n", false);
			cout << "\r\n";
			log << "S: " << "\r\n";
		}
		else
		{
			pClient->sendString("HTTP/1.0 200 OK\r\n", false);
			cout << "HTTP/1.0 200 OK\r\n";
			log << "S: " << "HTTP/1.0 200 OK\r\n";
			pClient->sendString("Content-Type: text/html\r\n", false);
			cout << "Content-Type: text/html\r\n";
			log << "S: " << "Content-Type: text/html\r\n";
			pClient->sendString("Server: Coeus 0.1\r\n", false);
			cout << "Server: Coeus 0.1\r\n";
			log << "S: " << "Server: Coeus 0.1\r\n";

			stringstream location;
			location << "Location: http://" << headers["host"] << path << "\r\n";
			pClient->sendString(location.str());
			cout << location.str();
			log << "S: " << location.str();

			std::streampos fsize;
			fsize = file.tellg();
			file.seekg(0, std::ios::end);
			fsize = file.tellg() - fsize;
			file.seekg(0, std::ios::beg);

			stringstream contentSize;
			contentSize << "Content-Size: " << fsize << "\r\n";
			pClient->sendString(contentSize.str(), false);
			cout << contentSize.str();
			log << "S: " << contentSize.str();

			pClient->sendString("\r\n", false);
			cout << "\r\n";
			log << "S: " << "\r\n";

			const int TMP_BUFFER_SIZE = 4096;

			log << "S: ";
			char tmp_buffer[TMP_BUFFER_SIZE];
			std::streamsize n;
			do
			{
				file.read(tmp_buffer, TMP_BUFFER_SIZE);
				n = file.gcount();

				if (n == 0)
					break;

				pClient->sendBuffer(tmp_buffer, (unsigned int)n);
				log.write(tmp_buffer, (unsigned int)n);

				if ( ! file )
					break;
			}
			while (n > 0);

			log.flush();
		}

		pClient->disconnect();
		delete pClient;
	}

	log.close();

	Arc_TermNet();

	return 0;
}
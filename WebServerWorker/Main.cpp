#include <iostream>
#include <sstream>
#include <fstream>

#include <dirent.h>

#include <Arc/ArcCore.h>
#include <Arc/Log.h>

#include <Arc/Socket.h>

using namespace std;
using namespace Arc;

const string NAME = "Coeus Web Server Worker";

Map<string, string> getHeaders( Socket* pCurrClient )
{
	Map<string, string> headers;
	string line;
	while (true)
	{
		line = pCurrClient->recvLine();
		if (line == "") break;

		Log::InfoFmt(NAME, "C: %s", line.c_str());

		ArrayList<string> split = Arc_StringSplit(line, ':', 2);
		if (split.getSize() < 2)
		{
			Log::Error(NAME, "Malformed Header");
			continue;
		}
		Arc_TrimRight(split[0]);
		Arc_TrimLeft(split[1]);
		Arc_StringToLower(split[0]);

		headers.add(split[0], split[1]);
	}

	Log::InfoFmt(NAME, "Read %d Headers", headers.getSize());

	return headers;
}

std::streamsize getFileSize( std::ifstream& file )
{
	std::streampos fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;
	file.seekg(0, std::ios::beg);

	return fsize;
}

string recvLine( Socket* pCurrClient )
{
	const string& line =  pCurrClient->recvLine();
	Log::InfoFmt(NAME, "C: %s", line.c_str());

	return line;
}

bool sendString( Socket* pCurrClient, const string& str )
{
	if (pCurrClient->sendString(str, false) > 0)
	{
		Log::InfoFmt(NAME, "S: %s", str.c_str());

		return true;
	}

	return false;
}

bool sendLine( Socket* pCurrClient, const string& line )
{
	if (pCurrClient->sendString(line + "\r\n", false) > 0)
	{
		Log::InfoFmt(NAME, "S: %s", line.c_str());

		return true;
	}

	return false;
}

bool sendFile( Socket* pCurrClient, std::ifstream& file )
{
	if ( ! pCurrClient || pCurrClient->hasError())
		return false;

	if ( ! file)
		return false;

	const int TMP_BUFFER_SIZE = 4096;

	char tmp_buffer[TMP_BUFFER_SIZE];
	std::streamsize n;
	do
	{
		file.read(tmp_buffer, TMP_BUFFER_SIZE);
		n = file.gcount();

		if (n == 0)
			break;

		pCurrClient->sendBuffer(tmp_buffer, (unsigned int)n);

		if ( ! file )
			break;
	}
	while (n > 0);

	return true;
}

void sendDirectoryList( Socket* pCurrClient, const string& path, const string& realPath )
{
	stringstream output;

	output << "<html>\r\n"
		<< "<head>\r\n"
		<< "<title>Directory Listing for " << path << "</title>\r\n"
		<< "</head>\r\n"
		<< "<body>\r\n"
		<< "<h1>Directory Listing for " << path << "</h1>\r\n"
		<< "<table>\r\n"
		<< "<tr><th>Icon</th><th>Name</th><th>Type</th></tr>";

	DIR *dir;
	struct dirent *ent;

	dir = opendir(realPath.c_str());
	if (dir != NULL) 
	{
		while ((ent = readdir (dir)) != NULL)
		{
			string type = "";
			string name = string(ent->d_name);
			string href = "";

			switch (ent->d_type) 
			{
			case DT_REG:

				type = "File";

				break;
			case DT_DIR:

				type = "Directory";

				break;
			}

			if (name != "." && name != "..")
			{
				href = path + name;
			}

			if (name == ".." && path.length() > 1)
			{
				string tmpPath = path;
				tmpPath.pop_back();
				size_t lastSlash = tmpPath.rfind('/');
				if (lastSlash != string::npos)
				{
					href = tmpPath.substr(0, lastSlash + 1);
				}
			}

			output << "<tr><td>" << type.front() << "</td><td>";

			if (href.length() > 0)
			{
				output << "<a href=\"" << href << "\">" << name << "</a>";
			}
			else
				output << name;

			output << "</td><td>" << type << "</td></tr>\r\n";
		}

		closedir(dir);

	}

	output << "</table>\r\n"
		<< "</body>\r\n"
		<< "</html>\r\n";

	string outStr = output.str();

	stringstream contentSizeHeader;
	contentSizeHeader << "Content-Size: " << outStr.length();
	sendLine(pCurrClient, contentSizeHeader.str());

	pCurrClient->sendString("\r\n", false);

	pCurrClient->sendString(outStr, false);
}

int main( int argc, char* argv[] )
{
	Socket* pCurrClient;
	bool directoryListing = false;

	const string& request = recvLine();
	ArrayList<string> requestPieces = Arc_StringSplit(request, ' ');

	if (requestPieces.getSize() < 3)
	{
		Log::Error(NAME, "Malformed Request");
		pCurrClient->disconnect();
		delete pCurrClient;
		exit;
	}

	const string& method = requestPieces[0];
	const string& path = requestPieces[1];
	const string& version = requestPieces[2];

	Map<string, string>& headers = getHeaders();

	string realPath = mp_ServerConfig->getWebRoot() + path;

	ifstream file;

	if (realPath.back() == '/')
	{
		bool defaultFound = false;
		const ArrayList<string>& defaults = mp_ServerConfig->getDefaults();
		for (auto it = defaults.itConstBegin(); it != defaults.itConstEnd(); ++it)
		{
			const string& def = (*it);
			file.open(realPath + def, ios::in | ios::binary);

			if (file)
			{
				realPath += def;
				defaultFound = true;
				break;
			}
		}

		if ( ! defaultFound)
		{
			directoryListing = true;
		}
	}
	else
		file.open(realPath, ios::in | ios::binary);

	if ( ! directoryListing && ! file)
	{
		const string& response = "HTTP/1.0 404 Not Found";
		sendLine(pCurrClient, response);

		file.close();
		file.open(mp_ServerConfig->getWebRoot() + "/" + mp_ServerConfig->getErrorPage404());
	}
	else
	{
		const string& response = "HTTP/1.0 200 OK";
		sendLine(pCurrClient, response);
	}

	const string& serverHeader = "Server: Coeus " + mp_ServerConfig->getVersionString();
	sendLine(pCurrClient, serverHeader);

	stringstream locationHeader;
	locationHeader << "Location: http://" << headers["host"] << path;
	sendLine(pCurrClient, locationHeader.str());

	if (directoryListing)
	{
		sendLine(pCurrClient, "Content-Type: text/html");

		sendDirectoryList(pCurrClient, path, realPath);
	}
	else
	{
		string ext = Arc_FileExtension(realPath);
		string mimeType = mp_ServerConfig->getMIMEType(ext);

		stringstream contentTypeHeader;
		contentTypeHeader << "Content-Type: " << mimeType;
		sendLine(pCurrClient, contentTypeHeader.str());

		const std::streamsize& fileSize = getFileSize(file);

		stringstream contentSizeHeader;
		contentSizeHeader << "Content-Size: " << fileSize;
		sendLine(pCurrClient, contentSizeHeader.str());

		pCurrClient->sendString("\r\n", false);

		Log::InfoFmt(NAME, "Sending File: <%s>", realPath.c_str());
		sendFile(pCurrClient, file);
	}

	pCurrClient->disconnect();
}
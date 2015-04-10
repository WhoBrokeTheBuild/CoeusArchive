#include "Worker.h"

#include "Server.h"
#include "ServerConfig.h"

Worker::~Worker()
{
}

Map<string, string> Worker::getHeaders()
{
	Map<string, string> headers;
	string line;
	while (true)
	{
		line = mp_CurrClient->recvLine();
		if (line == "") break;

		Log::InfoFmt(getClassName(), "C: %s", line.c_str());

		ArrayList<string> split = Arc_StringSplit(line, ':', 2);
		if (split.getSize() < 2)
		{
			Log::Error(getClassName(), "Malformed Header");
			continue;
		}
		Arc_TrimRight(split[0]);
		Arc_TrimLeft(split[1]);
		Arc_StringToLower(split[0]);

		headers.add(split[0], split[1]);
	}
	Log::InfoFmt(getClassName(), "Read %d Headers", headers.getSize());

	return headers;
}

std::streamsize Worker::getFileSize(std::ifstream& file)
{
	std::streampos fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;
	file.seekg(0, std::ios::beg);

	return fsize;
}

string Worker::recvLine()
{
	const string& line = mp_CurrClient->recvLine();
	Log::InfoFmt(getClassName(), "C: %s", line.c_str());

	return line;
}

bool Worker::sendString(const string& str)
{
	if (mp_CurrClient->sendString(str, false) > 0)
	{
		Log::InfoFmt(getClassName(), "S: %s", str.c_str());

		return true;
	}

	return false;
}

bool Worker::sendLine(const string& line)
{
	if (mp_CurrClient->sendString(line + "\r\n", false) > 0)
	{
		Log::InfoFmt(getClassName(), "S: %s", line.c_str());

		return true;
	}

	return false;
}

bool Worker::sendFile(std::ifstream& file)
{
	if (!mp_CurrClient || mp_CurrClient->hasError())
		return false;

	if (!file)
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

		mp_CurrClient->sendBuffer(tmp_buffer, (unsigned int)n);

		if (!file)
			break;
	} while (n > 0);

	return true;
}

void Worker::sendDirectoryList(const string& path, const string& realPath)
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
		while ((ent = readdir(dir)) != NULL)
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
	sendLine(contentSizeHeader.str());

	mp_CurrClient->sendString("\r\n", false);

	mp_CurrClient->sendString(outStr, false);
}

bool Worker::run()
{
	bool directoryListing = false;

	const string& request = recvLine();
	ArrayList<string> requestPieces = Arc_StringSplit(request, ' ');

	if (requestPieces.getSize() < 3)
	{
		Log::Error(getClassName(), "Malformed Request");
		mp_CurrClient->disconnect();
		delete mp_CurrClient;
		return false;
	}

	const string& method = requestPieces[0];
	const string& path = requestPieces[1];
	const string& version = requestPieces[2];

	Map<string, string>& headers = getHeaders();

	string realPath = mp_Server->getServerConfig()->getWebRoot() + path;

	ifstream file;

	if (realPath.back() == '/')
	{
		bool defaultFound = false;
		const ArrayList<string>& defaults = mp_Server->getServerConfig()->getDefaults();
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

		if (!defaultFound)
		{
			directoryListing = true;
		}
	}
	else
		file.open(realPath, ios::in | ios::binary);

	if (!directoryListing && !file)
	{
		const string& response = "HTTP/1.0 404 Not Found";
		sendLine(response);

		file.close();
		file.open(mp_Server->getServerConfig()->getWebRoot() + "/" + mp_Server->getServerConfig()->getErrorPage404());
	}
	else
	{
		const string& response = "HTTP/1.0 200 OK";
		sendLine(response);
	}

	const string& serverHeader = "Server: Coeus " + mp_Server->getVersionString();
	sendLine(serverHeader);

	stringstream locationHeader;
	locationHeader << "Location: http://" << headers["host"] << path;
	sendLine(locationHeader.str());

	if (directoryListing)
	{
		sendLine("Content-Type: text/html");

		sendDirectoryList(path, realPath);
	}
	else
	{
		string ext = Arc_FileExtension(realPath);
		string mimeType = mp_Server->getServerConfig()->getMIMEType(ext);

		stringstream contentTypeHeader;
		contentTypeHeader << "Content-Type: " << mimeType;
		sendLine(contentTypeHeader.str());

		const std::streamsize& fileSize = getFileSize(file);

		stringstream contentSizeHeader;
		contentSizeHeader << "Content-Size: " << fileSize;
		sendLine(contentSizeHeader.str());

		mp_CurrClient->sendString("\r\n", false);

		Log::InfoFmt(getClassName(), "Sending File: <%s>", realPath.c_str());
		sendFile(file);
	}

	mp_CurrClient->disconnect();
	return true;
}

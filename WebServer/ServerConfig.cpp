#include "ServerConfig.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Arc/StringFunctions.h>

using namespace std;

bool ServerConfig::loadMainConfig( const string& filename )
{
	ifstream file(filename);

	if ( ! file)
		return false;

	string line;
	while ( ! file.eof())
	{
		getline(file, line);

		if (line.length() != 0 && line[0] == '#')
			continue;

		unsigned int endOfCmd = min(line.find(' '), line.find('\t'));

		if (endOfCmd == string::npos)
			continue;

		string command = line.substr(0, endOfCmd);
		Arc_StringToLower(command);

		unsigned int startOfData = string::npos;
		for (unsigned int i = command.length(); i < line.length(); ++i)
		{
			if (line[i] != ' ' && line[i] != '\t')
			{
				startOfData = i - 1;
				break;
			}
		}

		if (startOfData == string::npos)
			continue;

		const string& data = line.substr(startOfData, string::npos);
		const string& trimData = Arc_GetTrim(data);

		if (command == "port")
		{
			stringstream ss;
			ss << trimData;
			ss >> m_Port;
		}
		else if (command == "webroot")
		{
			m_WebRoot = trimData;
		}
		else if (command == "defaults")
		{
			const string& cleanData = cleanWhitespace(data);

			ArrayList<string> defaults = Arc_StringSplit(cleanData, ' ');

			for (unsigned int i = 0; i < defaults.getSize(); ++i)
				if (defaults[i].length() != 0)
					m_Defaults.add(defaults[i]);
		}
		else if (command == "ErrorPage404")
		{
			m_ErrorPage404 = trimData;
		}
	}

	return false;
}

bool ServerConfig::loadMIMETypes( const string& filename )
{
	ifstream file(filename);

	if ( ! file)
		return false;

	string line;
	while ( ! file.eof())
	{
		getline(file, line);

		if (line.length() != 0 && line[0] == '#')
			continue;

		const string& newLine = cleanWhitespace(line);

		const ArrayList<string>& pieces = Arc_StringSplit(newLine, ' ');

		if (pieces.getSize() < 2)
			continue;

		const string& mimeType = pieces[0];
		m_MimeTypes.add(mimeType, ArrayList<string>());
		ArrayList<string>& extensions = m_MimeTypes[mimeType];
		
		for (unsigned int i = 1; i < pieces.getSize(); ++i)
			extensions.add(pieces[i]);
	}

	file.close();

	return true;
}

string ServerConfig::getMIMEType( const string& extension ) const
{
	for (auto it = m_MimeTypes.itConstBegin(); it != m_MimeTypes.itConstEnd(); ++it)
		if (it->second.contains(extension))
			return it->first;

	return "unknown";
}

string ServerConfig::cleanWhitespace( const string& str )
{
	string copy = Arc_GetTrim(str);

	string newStr;
	int whitespaceStart = -1;
	for (unsigned int i = 0; i < copy.length(); ++i)
	{
		if (copy[i] == ' ' || copy[i] == '\t')
		{
			if (whitespaceStart == -1)
			{
				newStr += ' ';
				whitespaceStart = i;
			}

			if (i != copy.length() - 1 && copy[i + 1] != ' ' && copy[i + 1] != '\t')
				whitespaceStart = -1;
		}
		else
			newStr += copy[i];
	}

	return newStr;
}
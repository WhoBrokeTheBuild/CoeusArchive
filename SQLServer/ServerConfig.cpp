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
		else if (command == "dataroot")
		{
			m_DataRoot = trimData;
		}
	}

	return true;
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
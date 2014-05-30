#include <iostream>

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/Log.h>

using namespace std;
using namespace Arc;

ArrayList<string> cleanAndSplitStatement( const string& str )
{
	bool inSingleQuotes = false;
	bool inDoubleQuotes = false;
	bool inBackQuotes = false;
	bool escape = false;

	ArrayList<string> pieces;
	pieces.add("");

	bool inStartingWhitespace = true;

	for (unsigned int i = 0; i < str.length(); ++i)
	{
		const char& prev = (i == 0 ? 0 : str[i - 1]);
		char ch = str[i];

		if (inStartingWhitespace)
		{
			if (ch != ' ' && ch != '\t')
				inStartingWhitespace = false;
			else
				continue;
		}

		if (escape)
		{
			escape = false;

			if (ch != '\\')
			{
				pieces.getBack() += ch;
				continue;
			}
		}
		else if (ch == '\\')
			escape = true;

		if (inSingleQuotes)
		{
			if (ch == '\'')
			{
				inSingleQuotes = false;
				pieces.getBack() += ch;
				continue;
			}
		}
		else if (inDoubleQuotes)
		{
			if (ch == '"')
			{
				inDoubleQuotes = false;
				pieces.getBack() += ch;
				continue;
			}
		}
		else if (inBackQuotes)
		{
			if (ch == '`')
			{
				inBackQuotes = false;
				pieces.getBack() += ch;
				continue;
			}
		}

		if ( ! inDoubleQuotes && ! inBackQuotes && ch == '\'')
			inSingleQuotes = true;
		else if ( ! inSingleQuotes && ! inBackQuotes && ch == '"')
			inDoubleQuotes = true;
		else if ( ! inSingleQuotes && ! inDoubleQuotes && ch == '`')
			inBackQuotes = true;

		if ( ! inSingleQuotes && ! inDoubleQuotes && ! inBackQuotes )
		{
			if (ch == ';')
				break;

			if (ch == '\t')
				ch = ' ';

			if (ch == ' ')
			{
				if (prev == ' ' || prev == '\t')
					continue;

				pieces.add("");
			}
			else
				pieces.getBack() += tolower(ch);
		}
		else
		{
			pieces.getBack() += ch;
		}
	}

	if (pieces.getBack().length() == 0)
		pieces.popBack();

	return pieces;
}

bool processStatement( string& rawStmt )
{
	const ArrayList<string>& stmt = cleanAndSplitStatement(rawStmt);

	if (stmt.isEmpty())
		return false;
	for (unsigned int i = 0; i < stmt.getSize(); ++i)
		printf("%s\r\n", stmt[i].c_str());

	const string& cmd = stmt[0];

	if (cmd.length() == 0)
		return false;

	if (cmd == "select")
	{
	}
	else if (cmd == "update")
	{

	}
	else if (cmd == "create")
	{

	}
	else if (cmd == "delete")
	{

	}
	else if (cmd == "show")
	{

	}
	else if (cmd == "use")
	{

	}
	else if (cmd == "exit")
	{
		return true;
	}

	return false;
}

void runConsole( void )
{
	printf("Coeus SQL Server\r\n");

	string line;
	bool shouldExit = false;
	while ( ! shouldExit)
	{
		printf("> ");
		getline(cin, line);

		shouldExit = processStatement(line);
	}
}

int main( int argc, char* argv[] )
{
	Arc_InitCore();
	Arc_InitNet();

	Log::AddInfoOutput("stdout", false);
	Log::AddErrorOutput("stderr", false);
	Log::AddInfoOutput("logs/info.log");
	Log::AddErrorOutput("logs/error.log");

	runConsole();

	Log::CloseOutputs();

	Arc_TermNet();

	return 0;
}
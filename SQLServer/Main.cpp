#include <iostream>
#include <fstream>

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>
#include <Arc/Buffer.h>

#include <Arc/Log.h>

using namespace std;
using namespace Arc;

ArrayList<string> VALID_COMMANDS = ArrayList<string>();
string DB_ROOT = "db/";

bool database_exists( const string& name )
{
	ifstream db(DB_ROOT + name + ".cdb");

	return db;
}

bool create_database( const string& name )
{
	CreateDirectory((DB_ROOT + name).c_str());
}

string stripQuotes( const string& str )
{
	if (str.length() == 0)
		return str;

	const char& first = str[0];
	const char& last = str[str.length() - 1];

	if (first == last && (first == '\'' || first == '"' || first == '`'))
	{
		return str.substr(1, str.length() - 2);
	}

	return str;
}

ArrayList<string> cleanAndSplitStatement( const string& str )
{
	bool inSingleQuotes = false;
	bool inDoubleQuotes = false;
	bool inBackQuotes = false;
	bool escape = false;

	ArrayList<string> pieces;
	pieces.add("");

	string fromLastSpace = "";

	bool inStartingWhitespace = true;

	for (unsigned int i = 0; i < str.length(); ++i)
	{
		const char& prev = (i == 0 ? 0 : str[i - 1]);
		char ch = str[i];
		string& back = pieces.getBack();

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
				back += ch;
				fromLastSpace += ch;
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
				back += ch;
				fromLastSpace += ch;
				continue;
			}
		}
		else if (inDoubleQuotes)
		{
			if (ch == '"')
			{
				inDoubleQuotes = false;
				back += ch;
				fromLastSpace += ch;
				continue;
			}
		}
		else if (inBackQuotes)
		{
			if (ch == '`')
			{
				inBackQuotes = false;
				back += ch;
				fromLastSpace += ch;
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

				if (VALID_COMMANDS.contains(fromLastSpace))
				{
					back = back.substr(0, back.length() - fromLastSpace.length());
					if (back.length() == 0)
						pieces.popBack();

					pieces.add(fromLastSpace);
					pieces.add("");
					continue;
				}
				else
					pieces.getBack() += ch;

				fromLastSpace = "";
			}
			else
			{
				pieces.getBack() += tolower(ch);
				fromLastSpace += tolower(ch);
			}
		}
		else
		{
			pieces.getBack() += ch;
			fromLastSpace += ch;
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
		if (stmt.getSize() < 3)
		{
			printf("Error: Malformed Command");
		}

		const string& what = stmt[1];
		const string& name = stripQuotes(stmt[2]);

		if (what == "database")
		{
			create_database(name);
		}
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

	VALID_COMMANDS.add("select");
	VALID_COMMANDS.add("from");
	VALID_COMMANDS.add("where");
	VALID_COMMANDS.add("or");
	VALID_COMMANDS.add("and");
	VALID_COMMANDS.add("not");
	VALID_COMMANDS.add("update");
	VALID_COMMANDS.add("create");
	VALID_COMMANDS.add("delete");
	VALID_COMMANDS.add("show");
	VALID_COMMANDS.add("use");
	VALID_COMMANDS.add("database");
	VALID_COMMANDS.add("databases");
	VALID_COMMANDS.add("table");
	VALID_COMMANDS.add("tables");
	VALID_COMMANDS.add("exit");

	runConsole();

	Log::CloseOutputs();

	Arc_TermNet();

	return 0;
}
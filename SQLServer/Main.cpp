#include <iostream>
#include <fstream>

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>
#include <Arc/Buffer.h>

#include <Arc/Log.h>
#include <Arc/FileFunctions.h>

using namespace std;
using namespace Arc;

ArrayList<string> VALID_COMMANDS = ArrayList<string>();
string DB_ROOT = "db/";

string current_db = "";

bool table_exists( const string& name )
{
	return true;
}

bool create_table( const string& name )
{
	if (current_db == "")
	{
		// error
		return false;
	}

	Buffer buff;
	buff.appendChar('C');
	buff.appendChar('D');
	buff.appendChar('B');

	buff.appendInt(0); // Number of fields
	buff.appendInt(UINT_MAX); // No Primary Key

	buff.appendLong(0);

	ofstream table(DB_ROOT + current_db + "/" + name + ".cdb", ios::out | ios::binary);
	buff.writeToStream(table);
	table.close();

	return true;
}

bool database_exists( const string& name )
{
	return true;
}

bool create_database( const string& name )
{
	return Arc_CreateDir(DB_ROOT + name);
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

	const string& cmd = stmt[0];

	if (cmd.length() == 0)
		return false;

	if (cmd == "select")
	{

	}
	else if (cmd == "insert")
	{

	}
	else if (cmd == "update")
	{

	}
	else if (cmd == "create")
	{
		if (stmt.getSize() < 3)
		{
			printf("Error: Malformed Command\r\n");
			return false;
		}

		const string& what = stmt[1];
		const string& name = stripQuotes(stmt[2]);

		if (what == "database")
		{
			create_database(name);
			printf("Database `%s` created\r\n", name.c_str());
		}
		else if (what == "table")
		{
			create_table(name);
			printf("Table `%s`.`%s` created\r\n", current_db.c_str(), name.c_str());
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
		if (stmt.getSize() < 2)
		{
			printf("Error: Malformed Command\r\n");
			return false;
		}

		const string& name = stripQuotes(stmt[1]);

		if (database_exists(name))
		{
			current_db = name;
			printf("Database changed to `%s`\r\n", name.c_str());
		}
		else
		{
			// error
		}
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
	string statement = "";
	bool shouldExit = false;
	while ( ! shouldExit)
	{
		if (statement.length() == 0)
			printf("> ");
		else
			printf(">> ");
		getline(cin, line);

		string tmpLine = line;
		Arc_StringToLower(tmpLine);
		if (tmpLine == "exit")
			line += ';';

		size_t endOfStmt = line.find(';');

		if (endOfStmt == string::npos)
		{
			statement += line + ' ';
			continue;
		}

		do
		{
			statement += line + ' ';

			line = line.substr(0, endOfStmt);
			endOfStmt = line.find(';');

			shouldExit = processStatement(statement);
			statement = "";
		}
		while (endOfStmt != string::npos);

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

	VALID_COMMANDS.add("insert");
	VALID_COMMANDS.add("into");
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
	VALID_COMMANDS.add("alter");

	runConsole();

	Log::CloseOutputs();

	Arc_TermNet();

	return 0;
}
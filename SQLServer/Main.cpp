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
Map<string, int> TYPE_MAP = Map<string, int>();

string DB_ROOT = "db/";

string current_db = "";

void stripQuotes( string& str )
{
	if (str.length() == 0)
		return;

	Arc_Trim(str);

	const char& first = str[0];
	const char& last = str[str.length() - 1];

	if (first == last && (first == '\'' || first == '"' || first == '`'))
	{
		str = str.substr(1, str.length() - 2);
	}
}

string getStripQuotes( const string& str )
{
	string tmpStr = str;
	stripQuotes(tmpStr);
	return tmpStr;
}

ArrayList<ArrayList<string>> cleanAndSplitList( const string& str )
{
	string tmpStr = str;
	Arc_Trim(tmpStr);

	ArrayList<ArrayList<string>> pieces;

	if (tmpStr.length() < 2)
		return pieces;

	pieces.add(ArrayList<string>());
	pieces.getBack().add("");

	if (tmpStr.front() == '(' && tmpStr.back() == ')')
		tmpStr = tmpStr.substr(1, tmpStr.length() - 2);

	bool inSingleQuotes = false;
	bool inDoubleQuotes = false;
	bool inBackQuotes = false;
	bool escape = false;

	bool inStartingWhitespace = true;

	for (unsigned int i = 0; i < tmpStr.length(); ++i)
	{
		const char& prev = (i == 0 ? 0 : tmpStr[i - 1]);
		char ch = tmpStr[i];
		string& backPiece = pieces.getBack().getBack();

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
				backPiece += ch;
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
				backPiece += ch;
				continue;
			}
		}
		else if (inDoubleQuotes)
		{
			if (ch == '"')
			{
				inDoubleQuotes = false;
				backPiece += ch;
				continue;
			}
		}
		else if (inBackQuotes)
		{
			if (ch == '`')
			{
				inBackQuotes = false;
				backPiece += ch;
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
			if (ch == ',')
			{
				pieces.add(ArrayList<string>());
				pieces.getBack().add("");
				inStartingWhitespace = true;
			}
			else if (ch == ' ')
			{
				pieces.getBack().add("");
				pieces.getBack().getBack() += ch;
			}
			else
			{
				backPiece += tolower(ch);
			}
		}
		else
		{
			backPiece += ch;
		}
	}

	if (pieces.getBack().getBack().length() == 0)
		pieces.getBack().popBack();

	for (auto it = pieces.itBegin(); it != pieces.itEnd(); ++it)
	{
		ArrayList<string>& parts = (*it);

		if (parts.isEmpty())
			continue;

		for (unsigned int i = 0; i < parts.getSize(); ++i)
			stripQuotes(parts[i]);

		Arc_Trim(parts.getBack());
	}

	return pieces;
}

ArrayList<string> cleanAndSplitStatement( const string& str )
{
	bool inSingleQuotes = false;
	bool inDoubleQuotes = false;
	bool inBackQuotes = false;
	bool inParenthesis = false;
	int parenLevel = 0;
	bool escape = false;

	ArrayList<string> pieces;
	pieces.add("");

	string fromLastSpace = "";

	bool inStartingWhitespace = true;

	for (unsigned int i = 0; i < str.length(); ++i)
	{
		const char& prev = (i == 0 ? 0 : str[i - 1]);
		char ch = str[i];
		string& backPiece = pieces.getBack();

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
				backPiece += ch;
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
				backPiece += ch;
				fromLastSpace += ch;
				continue;
			}
		}
		else if (inDoubleQuotes)
		{
			if (ch == '"')
			{
				inDoubleQuotes = false;
				backPiece += ch;
				fromLastSpace += ch;
				continue;
			}
		}
		else if (inBackQuotes)
		{
			if (ch == '`')
			{
				inBackQuotes = false;
				backPiece += ch;
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

		if (ch == '(')
		{
			if ( ! inParenthesis)
			{
				inParenthesis = true;
				pieces.add("(");
				fromLastSpace = "";
			}
			parenLevel++;
			continue;
		}
		else if (ch == ')')
		{
			parenLevel--;
			if (parenLevel == 0)
			{
				inParenthesis = false;
				backPiece += ")";
				pieces.add("");
				fromLastSpace = "";
			}
			continue;
		}

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
					backPiece = backPiece.substr(0, backPiece.length() - fromLastSpace.length());
					if (backPiece.length() == 0)
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
				backPiece += tolower(ch);
				fromLastSpace += tolower(ch);
			}
		}
		else
		{
			backPiece += ch;
			fromLastSpace += ch;
		}
	}

	if (pieces.getBack().length() == 0)
		pieces.popBack();

	return pieces;
}

bool table_exists( const string& name )
{
	return true;
}

bool create_table( const string& name, const string& data = "" )
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

	if (data.length() == 0)
	{
		buff.appendInt(0); // Number of fields
		buff.appendInt(UINT_MAX); // No Primary Key

		buff.appendLong(0);
	}
	else
	{
		ArrayList<ArrayList<string>> fields = cleanAndSplitList(data);

		buff.appendInt(fields.getSize()); // Number of fields

		for (unsigned int fieldID = 0; fieldID < fields.getSize(); ++fieldID)
		{
			const string& name = fields[fieldID][0];
			string type = fields[fieldID][1];
			bool auto_inc = false;
			string def = "";
			int length = 0;

			if (name.substr(0, 11) == "primary_key")
			{
				continue;
			}

			size_t tmp = type.find('(');
			if (tmp != string::npos)
			{
				size_t tmpEnd = type.find(')');
				string strLen = type.substr(tmp, tmpEnd - tmp);
				Arc_ParseInt(strLen);
				type = type.substr(0, tmp);
			}

			if (fields[fieldID].getSize() >= 4)
			{
				const string& extra = fields[fieldID][2];

				if (extra == "default")
				{
					def = fields[fieldID][3];
				}
				else if (extra == "auto_increment")
				{
					auto_inc = true;
				}
			}

			const int& typeID = TYPE_MAP[type];

			buff.appendString(name);
			buff.appendChar(0);

			buff.appendShort(typeID);

			if (type == "char" || type == "varchar" || type == "text")
			{
				buff.appendLong(length);
			}
			else if (type == "int" || type == "tinyint")
			{
				buff.appendBool(auto_inc);
			}

			if (type == "char" || type == "varchar" || type == "text")
			{
				buff.appendString(def);
				buff.appendChar(0);
			}
			else if (type == "int" || type == "tinyint")
			{
				int intDef = Arc_ParseInt(def);

				if (type == "int")
					buff.appendInt(intDef);
				else
					buff.appendShort(intDef);
			}
			else if (type == "bool")
			{
				bool boolDef = Arc_ParseBool(def);
				buff.appendBool(boolDef);
			}
			else if (type == "float" || type == "double")
			{
				double fltDef = Arc_ParseDouble(def);

				if (type == "float")
					buff.appendFloat((float)fltDef);
				else
					buff.appendDouble(fltDef);
			}
			// TODO: Add Timestamp, Date, Time, & Datetime

		}
	}

	ofstream table(DB_ROOT + current_db + "/" + name + ".cdb", ios::out | ios::binary);
	buff.writeToStream(table);
	table.close();

	return true;
}

bool drop_table( const string& name )
{
	return true; // STUB
}

bool database_exists( const string& name )
{
	return true;
}

bool create_database( const string& name )
{
	return Arc_CreateDir(DB_ROOT + name);
}

bool drop_database( const string& name )
{
	return true; // STUB
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
	else if (cmd == "delete")
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
		const string& name = getStripQuotes(stmt[2]);

		if (what == "database")
		{
			create_database(name);
			printf("Database `%s` created\r\n", name.c_str());
		}
		else if (what == "table")
		{
			const string& data = (stmt.getSize() >= 3 ? stmt[3] : "");
			create_table(name, data);
			printf("Table `%s`.`%s` created\r\n", current_db.c_str(), name.c_str());
		}
	}
	else if (cmd == "drop")
	{
		if (stmt.getSize() < 3)
		{
			printf("Error: Malformed Command\r\n");
			return false;
		}

		const string& what = stmt[1];
		const string& name = getStripQuotes(stmt[2]);

		if (what == "database")
		{
			drop_database(name);
			printf("Database `%s` dropped\r\n", name.c_str());
		}
		else if (what == "table")
		{
			drop_table(name);
			printf("Table `%s`.`%s` dropped\r\n", current_db.c_str(), name.c_str());
		}
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

		const string& name = getStripQuotes(stmt[1]);

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
		else
		{
			statement += line.substr(0, endOfStmt);
		}

		shouldExit = processStatement(statement);
		statement = "";
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
	VALID_COMMANDS.add("drop");
	VALID_COMMANDS.add("delete");
	VALID_COMMANDS.add("show");
	VALID_COMMANDS.add("use");
	VALID_COMMANDS.add("database");
	VALID_COMMANDS.add("databases");
	VALID_COMMANDS.add("table");
	VALID_COMMANDS.add("tables");
	VALID_COMMANDS.add("exit");
	VALID_COMMANDS.add("alter");

	TYPE_MAP.add("char",		1);
	TYPE_MAP.add("varchar",		2);
	TYPE_MAP.add("text",		3);
	TYPE_MAP.add("bool",		4);
	TYPE_MAP.add("tinyint",		5);
	TYPE_MAP.add("int",			6);
	TYPE_MAP.add("float",		7);
	TYPE_MAP.add("double",		8);
	TYPE_MAP.add("timestamp",	9);
	TYPE_MAP.add("date",		10);
	TYPE_MAP.add("time",		11);
	TYPE_MAP.add("datetime",	12);

	runConsole();

	Log::CloseOutputs();

	Arc_TermNet();

	return 0;
}
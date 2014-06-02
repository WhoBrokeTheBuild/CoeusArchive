#include "Interpreter.h"

#include <Arc/Buffer.h>
#include <Arc/StringFunctions.h>
#include <iostream>
#include <fstream>

using namespace std;

Interpreter* gp_Interpreter = nullptr;

ArrayList<CSLStatement*> Interpreter::s_Statements;

Map<string, CSLVarType> Interpreter::s_Vars;

Map<string, string> Interpreter::s_StringVars;
Map<string, int> Interpreter::s_IntVars;
Map<string, float> Interpreter::s_FloatVars;

void Interpreter::processStatement( const string& stmt )
{
	string tmpStmt = stmt;
	Arc_Trim(tmpStmt);

	if (tmpStmt.length() == 0)
		return;

	CSLStatement* stmtObj = New CSLStatement();
	stmtObj->buildStatement(tmpStmt);
	s_Statements.add(stmtObj);
	stmtObj->execute();
}

Interpreter::Interpreter( const string& filename )
{
	ifstream file(filename);

	if ( ! file)
	{
		printf("Error: %s does not exist\r\n", filename.c_str());
		return;
	}

	Buffer buff;
	buff.setDataFromStream(file);

	bool inCSL = false;
	bool inString = false;
	bool inParen = false;
	bool inLineComment = false;
	bool inBlockComment = false;
	string stmt = "";
	bool escape = false;

	while ( ! buff.endOfBuffer())
	{
		const char& last = buff.readCharAt(buff.getReadIndex());
		char ch = buff.readNextChar();
		const char& next = buff.readCharAt(buff.getReadIndex());

		if (inCSL)
		{
			if ( ! inString)
			{
				if ( ! inBlockComment && (ch == '/' && next == '*'))
				{
					inBlockComment = true;
					buff.setReadIndex(buff.getReadIndex() + 1);
					continue;
				}
				else if (inBlockComment && (ch == '*' && next == '/'))
				{
					inBlockComment = false;
					buff.setReadIndex(buff.getReadIndex() + 1);
					continue;
				}
				else if (ch == '/' && next == '/')
				{
					inLineComment = true;
					buff.setReadIndex(buff.getReadIndex() + 1);
					continue;
				}
				else if (ch == '=' && next == '>')
				{
					if (stmt.length() > 0)
					{
						processStatement(stmt);
						stmt = "";
					}

					inCSL = false;
					buff.setReadIndex(buff.getReadIndex() + 1);
					continue;
				}
				else if (ch == '\n')
				{
					inLineComment = false;
					processStatement(stmt);
					stmt = "";
					continue;
				}
				else if (ch == '\'')
				{
					inString = true;
				}
			}
			else
			{
				if (escape)
				{
					switch (ch)
					{
					case '\\':

						break;
					case 'n':

						ch = '\n';

						break;
					case 'r':

						ch = '\r';

						break;
					case 't':

						ch = '\t';

						break;
					}

					escape = false;
				}
				else if (ch == '\\')
				{
					escape = true;
					continue;
				}
				else if (ch == '\'')
				{
					inString = false;
				}
			}

			if ( ! inLineComment && ! inBlockComment)
				stmt += ch;
		}
		else
		{
			if (ch == '<' && next == '=')
			{
				inCSL = true;
				buff.setReadIndex(buff.getReadIndex() + 1);
				continue;
			}

			printf("%c", ch);
		}
	}
}

Interpreter::~Interpreter(void)
{
	while ( ! s_Statements.isEmpty())
		delete s_Statements.popBack();
}

void Interpreter::SetStringVar( const string& name, const string& data )
{
	if (s_Vars.containsKey(name))
	{
		if (GetVarType(name) == VAR_TYPE_STRING)
		{
			s_StringVars[name] = data;
			return;
		}
		else
			RemoveVar(name);
	}

	s_Vars.add(name, VAR_TYPE_STRING);
	s_StringVars.add(name, data);
}

void Interpreter::SetIntVar( const string& name, const int& data )
{
	if (s_Vars.containsKey(name))
	{
		if (GetVarType(name) == VAR_TYPE_INT)
		{
			s_IntVars[name] = data;
			return;
		}
		else
			RemoveVar(name);
	}

	s_Vars.add(name, VAR_TYPE_INT);
	s_IntVars.add(name, data);
}

void Interpreter::SetFloatVar( const string& name, const float& data )
{
	if (s_Vars.containsKey(name))
	{
		if (GetVarType(name) == VAR_TYPE_FLOAT)
		{
			s_FloatVars[name] = data;
			return;
		}
		else
			RemoveVar(name);
	}

	s_Vars.add(name, VAR_TYPE_FLOAT);
	s_FloatVars.add(name, data);
}

void Interpreter::RemoveVar( const string& name )
{
	s_Vars.removeKey(name);
	s_StringVars.removeKey(name);
	s_IntVars.removeKey(name);
	s_FloatVars.removeKey(name);
}
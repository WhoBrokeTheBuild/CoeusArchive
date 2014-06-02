#include <iostream>
#include <fstream>

#include <Arc/ArcCore.h>
#include <Arc/Buffer.h>

using namespace std;
using namespace Arc;

#include "CSLStatement.h"

void processStatement( const string& stmt )
{
	string tmpStmt = stmt;
	Arc_Trim(tmpStmt);

	if (tmpStmt.length() == 0)
		return;

	CSLStatement stmtObj;
	stmtObj.buildStatement(tmpStmt);
	stmtObj.execute();
}

int main( int argc, char* argv[] )
{
	if (argc <= 1)
	{
		printf("Error: Please specify file to parse\r\n");
		return 1;
	}

	const string& filename = argv[1];
	ifstream file(filename);

	if ( ! file)
	{
		printf("Error: %s does not exist\r\n", filename.c_str());
		return 1;
	}

	Buffer buff;
	buff.setDataFromStream(file);

	bool inCSL = false;
	bool inString = false;
	bool inParen = false;
	bool inLineComment = false;
	bool inBlockComment = false;
	string stmt = "";

	while ( ! buff.endOfBuffer())
	{
		const char& last = buff.readCharAt(buff.getReadIndex());
		const char& ch = buff.readNextChar();
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
			else if (ch == '\'')
			{
				inString = false;
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

	return 0;
}
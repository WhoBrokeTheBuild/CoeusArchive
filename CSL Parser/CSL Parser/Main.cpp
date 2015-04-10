#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

enum NodeType
{
	INVALID_NODE_TYPE = -1,
	
	STATEMENT_NODE_TYPE,
	NUMBER_NODE_TYPE,
	STRING_NODE_TYPE,
	OPERATOR_NODE_TYPE,
	VARIABLE_NODE_TYPE,
	COMMAND_NODE_TYPE,
	PARENTHESIS_NODE_TYPE,
	CURLY_BRACKET_NODE_TYPE,

	NUM_NODE_TYPES
};

const string NODE_TYPE_NAMES[NUM_NODE_TYPES] = 
{
	"Statement",
	"Number",
	"String",
	"Operator",
	"Variable",
	"Command",
	"Parenthesis",
	"Curly Bracket"
};

struct Node
{
	NodeType Type;

	vector<Node*> ChildNodes;

	string Data;

	Node( void ) :
		Type(INVALID_NODE_TYPE)
	{ }

	~Node( void )
	{
		for (unsigned int i = 0; i < ChildNodes.size(); ++i)
			delete ChildNodes[i];
		ChildNodes.clear();
	}
};
typedef struct Node Node;

streamsize getFileSize( ifstream& fp );
string readFile( const string& filename );
string condenseFile( const string& fileBuffer );
string readNextTerm( const string& file, NodeType& oType, unsigned int offset = 0 );
void buildParseTree( const string& stmt, Node* pParentNode = nullptr );
vector<Node*> parseStatements( const string& buffer );
string readNextStatement( const string& file, unsigned int offset = 0 );

streamsize getFileSize( ifstream& fp )
{
	streamsize fileSize = 0;
	fp.seekg(0, ifstream::end);
	fileSize = fp.tellg();
	fp.seekg(0);
	return fileSize;
}

string readFile( const string& filename )
{
	ifstream fp = ifstream(filename);

	const streamsize& fileSize = getFileSize(fp);

	string fileBuffer;
	fileBuffer.resize((unsigned int)fileSize);
	fp.read(&fileBuffer[0], fileSize);

	fp.close();

	return fileBuffer;
}

string condenseFile( const string& fileBuffer )
{
	const unsigned& fileSize = fileBuffer.length();

	string condensedBuffer;
	condensedBuffer.resize(fileSize);

	bool inSingleQuoteString = false,
		inDoubleQuoteString = false,
		inLineComment = false;

	streamsize condensedSize = 0;
	for (unsigned long i = 0; i < fileSize; ++i)
	{
		const char& ch = fileBuffer[i];

		if (inLineComment)
		{
			if (ch == '\n')
				inLineComment = false;
			else
				continue;
		}

		if (ch == '\'')
		{
			if (inSingleQuoteString)
				inSingleQuoteString = false;
			else
				inSingleQuoteString = true;
		}

		if (ch == '"')
		{
			if (inDoubleQuoteString)
				inDoubleQuoteString = false;
			else
				inDoubleQuoteString = true;
		}

		if ( ! inSingleQuoteString && ! inDoubleQuoteString)
		{
			if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
				continue;

			if (ch == '#')
			{
				inLineComment = true;
				continue;
			}
		}

		condensedBuffer[(unsigned int)condensedSize] = ch;
		condensedSize++;

	}

	condensedBuffer.resize((unsigned int)condensedSize);

	return condensedBuffer;
}

string readNextTerm( const string& file, NodeType& oType, unsigned int offset /*= 0*/ )
{
	bool inSingleQuoteString = false,
		 inDoubleQuoteString = false,
		 inVariable = false,
		 inCurlyBrackets = false,
		 inParenthesis = false,
		 inNumber = false,
		 inCommand = false,
		 inOperator = false;

	const char& first = file[offset];

	oType = INVALID_NODE_TYPE;

	if (first == ';')
	{
		return readNextTerm(file, oType, offset + 1);
	}
	if (first == '@')
	{
		inVariable = true;
		oType = VARIABLE_NODE_TYPE;
	}
	else if (first == '(')
	{
		inParenthesis = true;
		oType = PARENTHESIS_NODE_TYPE;
	}
	else if (first == '{')
	{
		inCurlyBrackets = true;
		oType = CURLY_BRACKET_NODE_TYPE;
	}
	else if (first == '\'')
	{
		inSingleQuoteString = true;
		oType = STRING_NODE_TYPE;
	}
	else if (first == '"')
	{
		inDoubleQuoteString = true;
		oType = STRING_NODE_TYPE;
	}
	else if (first >= '0' && first <= '9')
	{
		inNumber = true;
		oType = NUMBER_NODE_TYPE;
	}
	else if ( (first >= 'a' && first <= 'z') || (first >= 'A' && first <= 'Z') )
	{
		inCommand = true;
		oType = COMMAND_NODE_TYPE;
	}
	else if ( first == '=' ||
			  first == '<' ||
			  first == '>' ||
			  first == '+' ||
			  first == '-' ||
			  first == '/' ||
			  first == '*' ||
			  first == '%' ||
			  first == '!' ||
			  first == '|' ||
			  first == '&' )
	{
		inOperator = true;
		oType = OPERATOR_NODE_TYPE;
	}

	unsigned int i;
	for (i = offset + 1; i < file.length(); ++i)
	{
		const char& ch = file[i];

		if (ch == '\'' && inSingleQuoteString)
			break;

		if (ch == '"' && inDoubleQuoteString)
			break;

		if (inSingleQuoteString || inDoubleQuoteString)
			continue;

		if (inCurlyBrackets)
		{
			if (ch == '}')
				break;

			continue;
		}

		if (inParenthesis)
		{
			if (ch == ')')
				break;

			continue;
		}

		if (inVariable)
		{
			if ( (ch >= 'a' && ch <= 'z') ||
				 (ch >= 'A' && ch <= 'Z') ||
				 (ch >= '0' && ch <= '9') ||
				 ch == '_' )
			{
				continue;
			}

			--i;
			break;
		}

		if (inNumber)
		{
			if (ch >= '0' && ch <= '9')
				continue;

			--i;
			break;
		}

		if (inOperator)
		{
			if ( ch == '=' ||
				 ch == '<' ||
				 ch == '>' ||
				 ch == '+' ||
				 ch == '-' ||
				 ch == '/' ||
				 ch == '*' ||
				 ch == '%' ||
				 ch == '!' ||
				 ch == '|' ||
				 ch == '&' )
			{
				continue;
			}

			--i;
			break;
		}

		if (inCommand)
		{
			if ( (ch >= 'a' && ch <= 'z') || 
				 (ch >= 'A' && ch <= 'Z') )
			{
				continue;
			}

			--i;
			break;
		}
	}

	return file.substr(offset, i - offset + 1);
}

void buildParseTree( const string& stmt, Node* pParentNode /*= nullptr*/ )
{
	Node* pNode = nullptr;
	string term;
	NodeType type;
	unsigned int offset = 0;
	do 
	{
		term = readNextTerm(stmt, type, offset);
		if (term.empty())
			break;

		offset += term.length();

		pNode = new Node();
		pNode->Type = type;
		pNode->Data = term;

		if (pParentNode != nullptr)
		{
			pParentNode->ChildNodes.push_back(pNode);
		}

		if (type == PARENTHESIS_NODE_TYPE || type == CURLY_BRACKET_NODE_TYPE)
		{
			pNode->ChildNodes = parseStatements(term.substr(1, term.length() - 2));
		}
	} 
	while ( ! term.empty());
}

string readNextStatement( const string& file, unsigned int offset /*= 0*/ )
{
	unsigned int curlyBracketDepth = 0,
				 parenthesisDepth = 0;

	const char& first = file[offset];

	if (first == ';')
		offset++;

	unsigned int i;
	for (i = offset; i < file.length(); ++i)
	{
		const char& ch = file[i];

		if (curlyBracketDepth == 0 && parenthesisDepth == 0 && ch == ';')
			break;

		if (ch == '{')
			curlyBracketDepth++;

		if (ch == '}')
		{
			curlyBracketDepth--;
			if (curlyBracketDepth == 0)
				break;
		}

		if (ch == '(')
			parenthesisDepth++;

		if (ch == ')')
		{
			parenthesisDepth--;
			if (parenthesisDepth == 0)
				break;
		}
	}

	return file.substr(offset, i - offset + 1);
}

vector<Node*> parseStatements( const string& buffer )
{
	vector<Node*> nodes;

	string stmt;
	unsigned int offset = 0;
	do 
	{
		stmt = readNextStatement(buffer, offset);
		if (stmt[0] == '\0')
			break;

		offset += stmt.length();

		Node* pNode = new Node();
		pNode->Type = STATEMENT_NODE_TYPE;
		buildParseTree(stmt, pNode);
		nodes.push_back(pNode);
	}
	while ( ! stmt.empty());

	return nodes;
}

void outRepeat( ostream& out, const string& str, unsigned int count )
{
	for (unsigned int i = 0; i < count; ++i)
		out << str;
}

void printTree( Node* pNode, ostream& out, unsigned int depth = 0 )
{
	outRepeat(out, "\t", depth);
	out << "Type: " << NODE_TYPE_NAMES[pNode->Type] << endl;
	outRepeat(out, "\t", depth);
	out << "Data: " << pNode->Data << endl;
	
	for (unsigned int i = 0; i < pNode->ChildNodes.size(); ++i)
		printTree(pNode->ChildNodes[i], out, depth + 1);
}

enum VarType
{
	INVALID_VAR_TYPE = -1,

	VAR_TYPE_NUMBER,
	VAR_TYPE_STRING,

	NUM_VAR_TYPES
};

void process( Node* pNode )
{
	map<string, VarType> variables;
	map<string, string> strVars;
	map<string, int> numVars;

	Node* pSubject = nullptr;

	for (unsigned int i = 0; i < pNode->ChildNodes.size(); ++i)
	{
		Node* pChild = pNode->ChildNodes[i];

		if (pChild->Type == STATEMENT_NODE_TYPE)
		{
			process(pNode);
			continue;
		}

		if (pSubject == nullptr && pChild->Type == VARIABLE_NODE_TYPE)
		{
			pSubject = pChild;
			continue;
		}

		if (pChild->Type == OPERATOR_NODE_TYPE)
		{
			if (pSubject != nullptr)
			{
				Node* pValue = (pNode->ChildNodes.size() >= (i + 1) ? pNode->ChildNodes[i + 1] : nullptr);

				switch (pChild->Data[0])
				{
				case '=':

					if (pChild->Data.length() == 1)
					{
						// Assignment operator
						
						if (pValue != nullptr)
						{
							VarType varType = INVALID_VAR_TYPE;

							switch (pValue->Type)
							{
							case NUMBER_NODE_TYPE:

								varType = VAR_TYPE_NUMBER;

								break;
							case STRING_NODE_TYPE:

								varType = VAR_TYPE_STRING;

								break;
							}

							bool hasVar = false;
							for (auto it = variables.begin(); it != variables.end(); ++it)
							{
								if (it->first == pSubject->Data)
									hasVar = true;
							}

							if (hasVar)
							{
								if (variables[pSubject->Data] != varType)
								{
									VarType oldVarType = variables[pSubject->Data];

									switch (oldVarType)
									{
									case VAR_TYPE_NUMBER:

										numVars.erase(numVars.find(pSubject->Data));

										break;
									case VAR_TYPE_STRING:

										strVars.erase(strVars.find(pSubject->Data));

										break;
									}
								}
							}

							variables[pSubject->Data] = varType;

							stringstream ss;

							switch (varType)
							{
							case VAR_TYPE_NUMBER:

								ss << pValue->Data;
								int num;
								ss >> num;

								numVars.insert(pair<string, int>(pSubject->Data, num));

								break;
							case VAR_TYPE_STRING:

								strVars.insert(pair<string, string>(pSubject->Data, pValue->Data));

								break;
							}
						}
					}
					else if (pChild->Data[1] == '=')
					{
						// Equality Operator

					}

					break;
				case '>':

					break;
				case '<':

					break;
				case '+':

					break;
				case '-':

					break;
				case '*':

					break;
				case '/':

					break;
				case '%':

					break;
				case '!':

					break;
				case '|':

					break;
				case '&':

					break;
				}
			}
		}
	}
}

int main( int argc, char** argv )
{
	const string FILENAME = "test.csl";

	const string& fileBuffer = readFile(FILENAME);
	const string& condensedBuffer = condenseFile(fileBuffer);

	ofstream condensedFp("test.csl.1");
	condensedFp << condensedBuffer;
	condensedFp.close();

	const vector<Node*>& parseTrees = parseStatements(condensedBuffer);

	ofstream treeFp("test.csl.2");

	for (unsigned int i = 0; i < parseTrees.size(); ++i)
	{
		printTree(parseTrees[i], cout);
		printTree(parseTrees[i], treeFp);
	}

	treeFp.close();

	system("PAUSE");

	return 0;
}
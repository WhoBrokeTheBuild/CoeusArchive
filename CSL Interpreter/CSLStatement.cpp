#include "CSLStatement.h"

#include <iostream>

bool CSLStatement::buildStatement( const string& stmt )
{
	string tmp = "";

	bool inString = false;
	char ch = 0;
	char last = 0;
	char next = 0;
	for (unsigned int i = 0; i < stmt.length(); ++i)
	{
		last = ch;
		ch = stmt[i];
		next = (stmt.length() > i + 1 ? stmt[i + 1] : 0);

		if (inString)
		{
			if (last != '\\' && ch == '\'')
			{
				inString = false;
				addConstStringChunk(tmp);
				tmp = "";
			}
			else if (ch != '\\')
				tmp += ch;
		}
		else
		{
			if (last != '\\' && ch == '\'')
			{
				inString = true;
			}
			else if (ch != '\\')
			{
				if (ch == '+' || 
					ch == '=' ||
					ch == '*' ||
					ch == '/' || 
					ch == '%' ||
					ch == '>' ||
					ch == '<' ||
					ch == '!' ||
					ch == '&' ||
					ch == '|')
				{
					if (tmp.length() > 0)
					{
						addChunk(tmp);
						tmp = "";
						tmp += ch;
					}

					if ((ch == '+' && next == '+') ||
						(ch == '-' && next == '-') ||
						(ch == '&' && next == '&') ||
						(ch == '|' && next == '|') ||
						( (ch == '+' ||
						   ch == '-' || 
						   ch == '*' ||
						   ch == '/' ||
						   ch == '%' ||
						   ch == '=' ||
						   ch == '<' ||
						   ch == '>' ||
						   ch == '!')
						 && next == '='))
					{
						tmp += next;
						addOperatorChunk(tmp);
						tmp = "";
						++i;
					}
				}
				else if (ch == ' ')
				{
					addChunk(tmp);
					tmp = "";
				}
				else
					tmp += ch;
			}
		}
	}

	return false;
}

bool CSLStatement::addChunk( const string& chnk )
{
	const CSLChunkType& type = findChunkType(chnk);

	switch (type)
	{
	case CHUNK_TYPE_COMMAND:
		
		addCommandChunk(chnk);

		break;
	case CHUNK_TYPE_FUNC:

		break;
	case CHUNK_TYPE_OP:

		addOperatorChunk(chnk);

		break;
	case CHUNK_TYPE_PAREN:

		addParenthesisChunk(chnk);

		break;
	case CHUNK_TYPE_VAR:

		addVariableChunk(chnk);

		break;
	case CHUNK_TYPE_CONST_STRING:

		addConstStringChunk(chnk);

		break;
	case CHUNK_TYPE_CONST_INT:

		addConstIntChunk(0);

		break;
	case CHUNK_TYPE_CONST_FLOAT:

		addConstFloatChunk(0.0f);

		break;
	}

	return false;
}

bool CSLStatement::addCommandChunk( const string& cmd )
{
	cout << "Command chunk: " << cmd << endl;
	return false;
}

bool CSLStatement::addOperatorChunk( const string& op )
{
	cout << "Operator chunk: " << op << endl;
	return false;
}

bool CSLStatement::addVariableChunk( const string& name )
{
	cout << "Variable chunk: " << name << endl;
	return false;
}

bool CSLStatement::addConstStringChunk( const string& data )
{
	cout << "Const String chunk: " << data << endl;
	return false;
}

bool CSLStatement::addConstIntChunk( const int& data )
{
	cout << "Const Int chunk: " << data << endl;
	return false;
}

bool CSLStatement::addConstFloatChunk( const float& data )
{
	cout << "Const Float chunk: " << data << endl;
	return false;
}

bool CSLStatement::addFunctionChunk( const string& func, const ArrayList<string>& params )
{
	cout << "Function chunk: " << func << endl;
	return false;
}

bool CSLStatement::addParenthesisChunk( const string& inner )
{
	cout << "Parenthesis chunk: " << inner << endl;
	return false;
}

bool CSLStatement::execute( void )
{
	return false;
}

CSLChunkType CSLStatement::findChunkType( const string& data )
{
	if (data.length() == 0)
		return INVALID_CHUNK_TYPE;

	if (data[0] == '@')
		return CHUNK_TYPE_VAR;

	bool isCommand = false;
	for (unsigned int i = 0; i < NUM_COMMANDS; ++i)
	{
		if (data == CSL_COMMANDS[i])
		{
			isCommand = true;
			break;
		}
	}

	if (isCommand)
		return CHUNK_TYPE_COMMAND;

	bool isOperator = false;
	for (unsigned int i = 0; i < NUM_OPERATORS; ++i)
	{
		if (data == CSL_OPERATORS[i])
		{
			isOperator = true;
			break;
		}
	}

	if (isOperator)
		return CHUNK_TYPE_OP;

	bool isNumeric = true;
	bool isFloat = false;
	bool hasParen = false;

	for (unsigned int i = 0; i < data.length(); ++i)
	{
		 if (data[i] == '.')
		{
			if (isFloat) // Too many decimals
			{
				isNumeric = false;
				isFloat = false;
				break;
			}
			else
				isFloat = true;
		}
		else if ( ! isdigit(data[i]))
		{
			isNumeric = false;
			break;
		}
	}

	if (isNumeric)
	{
		if (isFloat)
			return CHUNK_TYPE_CONST_FLOAT;

		return CHUNK_TYPE_CONST_INT;
	}

	if (data.length() >= 2)
	{
		if (data.front() == '\'' && data.back() == '\'')
		{
			return CHUNK_TYPE_CONST_STRING;
		}
	}

	return INVALID_CHUNK_TYPE;
}

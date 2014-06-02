#include "CSLStatement.h"

#include <iostream>
#include <Arc/StringFunctions.h>

string CSLStatement::readNextChunk( const string& data )
{
	string tmp;
	bool isCmd = false;
	bool isFunc = false;

	const char& first = data.front();
	const char& next = (data.length() >= 1 ? data[1] : 0);

	if ((first == '+' && next == '+') ||
		(first == '-' && next == '-') ||
		(first == '&' && next == '&') ||
		(first == '|' && next == '|') ||
		( (first == '+' ||
		   first == '-' ||
		   first == '*' ||
		   first == '/' ||
		   first == '%' ||
		   first == '=' ||
		   first == '<' ||
		   first == '>' ||
		   first == '!')
		  && next == '='))
	{
		addOperatorChunk(data.substr(0, 2));
		tmp = data.substr(2);
		Arc_Trim(tmp);
		return tmp;
	}

	if (first == '+' ||
		first == '-' ||
		first == '*' ||
		first == '/' ||
		first == '%' ||
		first == '=' ||
		first == '>' ||
		first == '<' ||
		first == '!' ||
		first == '&' ||
		first == '|')
	{
		addOperatorChunk(data.substr(0, 1));
		tmp = data.substr(1);
		Arc_Trim(tmp);
		return tmp;
	}

	if (first == '\'')
	{
		for (unsigned int i = 1; i < data.length(); ++i)
		{
			if (data[i] == '\'')
			{
				addConstStringChunk(data.substr(1, i - 1));
				tmp = data.substr(i + 1);
				Arc_Trim(tmp);
				return tmp;
			}
		}

		return "";
	}

	if (first == '@')
	{
		string varName = "";

		for (unsigned int i = 1; i < data.length(); ++i)
		{
			const char& ch = data[i];

			if (ch == ' ' || 
				ch == '+' ||
				ch == '-' ||
				ch == '*' ||
				ch == '/' ||
				ch == '%' ||
				ch == '=' ||
				ch == '>' ||
				ch == '<' ||
				ch == '!' ||
				ch == '&' ||
				ch == '|')
			{
				break;
			}

			varName += ch;
		}

		addVariableChunk(varName);
		tmp = data.substr(varName.length() + 1);
		Arc_Trim(tmp);
		return tmp;
	}

	string cmd = "";
	for (unsigned int i = 0; i < NUM_COMMANDS; ++i)
	{
		if (data.substr(0, CSL_COMMANDS[i].length()) == CSL_COMMANDS[i])
		{
			isCmd = true;
			cmd = CSL_COMMANDS[i];
			break;
		}
	}

	if (isCmd)
	{
		addCommandChunk(cmd);
		tmp = data.substr(cmd.length());
		Arc_Trim(tmp);
		return tmp;
	}

	return "";
}


bool CSLStatement::buildStatement( const string& stmt )
{
	if (stmt.length() == 0)
		return false;

	string tmp = stmt;
	while (tmp.length() > 0)
	{
		tmp = readNextChunk(tmp);
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
	cout << endl;
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
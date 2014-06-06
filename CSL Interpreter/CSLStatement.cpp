#include "CSLStatement.h"

#include <iostream>
#include <Arc/StringFunctions.h>
#include <Arc/ParseFunctions.h>

#include "Interpreter.h"

string CSLStatement::readNextChunk( const string& data )
{
	string tmp;
	bool isCmd = false;
	bool isFunc = false;

	const char& first = data.front();
	const char& next = (data.length() >= 1 ? data[1] : 0);

	// Check for Operator Chunk

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

	// Check for Statement/Parenthesis Chunk

	if (first == '(')
	{
		bool isFor = false;

		if ( ! chunks.isEmpty() && chunks.getFront()->Type == CHUNK_TYPE_COMMAND)
		{
			CSLCommandChunk* pCmdChnk = (CSLCommandChunk*)chunks.getFront();

			if (pCmdChnk->Command == COMMAND_FOR)
			{
				isFor = true;
			}
		}

		int depth = 1;
		int end = -1;
		for (unsigned int i = 1; i < data.length(); ++i)
		{
			if (data[i] == '(')
				++depth;
			else if (data[i] == ')')
			{
				--depth;
				if (depth == 0)
				{
					end = i;
					break;
				}
			}
		}

		if (end != -1)
		{
			string inner = data.substr(1, end - 1);
			if (isFor)
			{
				ArrayList<string> parts = Arc_StringSplit(inner, ';');
				if (parts.getSize() != 3)
				{
					// Error
					return "";
				}

				for (unsigned int i = 0; i < 3; ++i)
				{
					Arc_Trim(parts[i]);
					addStatementChunk(parts[i]);
				}
			}
			else
			{
				Arc_Trim(inner);
				addStatementChunk(inner);
			}

			tmp = data.substr(end);
			Arc_Trim(tmp);
			return tmp;
		}
	}

	// Check for Const String Chunk

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

	// Check for Const Int/Float Chunk

	if (isdigit(first) || first == '.')
	{
		bool isFloat = false;
		bool hasParen = false;
		int end = 1;
		for (unsigned int i = 1; i < data.length(); ++i)
		{
			if (data[i] == '.')
			{
				if (isFloat) // Too many decimals
				{
					// Error
					return "";
				}
				else
					isFloat = true;
			}
			else if ( ! isdigit(data[i]))
			{
				end = i;
				break;
			}
		}

		string num = data.substr(0, end);

		if (isFloat)
			addConstFloatChunk(Arc_ParseFloat(num));

		addConstIntChunk(Arc_ParseInt(num));

		tmp = data.substr(end);
		Arc_Trim(tmp);
		return tmp;
	}

	// Check for Variable Chunk

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

	// Check for Command Chunk

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

	cout << endl << "<ul>" << endl << endl;
	cout << "<li><b>Base: " << stmt << "</b></li>" << endl;

	string tmp = stmt;
	while (tmp.length() > 0)
	{
		tmp = readNextChunk(tmp);
	}

	cout << endl << "</ul>" << endl << endl;

	return false;
}

bool CSLStatement::addCommandChunk( const string& cmd )
{
	CSLCommand cmdType = INVALID_COMMAND;

	for (unsigned int i = 0; i < NUM_COMMANDS; ++i)
	{
		if (cmd == CSL_COMMANDS[i])
		{
			cmdType = (CSLCommand)i;
			break;
		}
	}

	if (cmdType == INVALID_COMMAND)
	{
		// Error
		return false;
	}

	cout << "<li>Command chunk: " << cmd << "</li>" << endl;

	CSLCommandChunk* pChnk = New CSLCommandChunk(cmdType);
	chunks.add(pChnk);
	return true;
}

bool CSLStatement::addOperatorChunk( const string& op )
{
	CSLOperator opType = INVALID_OPERATOR;

	for (unsigned int i = 0; i < NUM_OPERATORS; ++i)
	{
		if (op == CSL_OPERATORS[i])
		{
			opType = (CSLOperator)i;
			break;
		}
	}

	if (opType == INVALID_OPERATOR)
	{
		// Error
		return false;
	}

	cout << "<li>Operator chunk: " << op << "</li>" << endl;

	CSLOperatorChunk* pChnk = New CSLOperatorChunk(opType);
	chunks.add(pChnk);
	return true;
}

bool CSLStatement::addVariableChunk( const string& name )
{
	if (name.length() == 0)
		return false;

	cout << "<li>Variable chunk: @" << name << "</li>" << endl;

	CSLVariableChunk* pChnk = New CSLVariableChunk(name);
	chunks.add(pChnk);
	return true;
}

bool CSLStatement::addConstStringChunk( const string& data )
{
	cout << "<li>Const String chunk: '" << data << "'</li>" << endl;

	CSLConstStringChunk* pChnk = New CSLConstStringChunk(data);
	chunks.add(pChnk);
	return true;
}

bool CSLStatement::addConstIntChunk( const int& data )
{
	cout << "<li>Const Int chunk: " << data << "</li>" << endl;

	CSLConstIntChunk* pChnk = New CSLConstIntChunk(data);
	chunks.add(pChnk);
	return true;
}

bool CSLStatement::addConstFloatChunk( const float& data )
{
	cout << "<li>Const Float chunk: " << data << "</li>" << endl;

	CSLConstFloatChunk* pChnk = New CSLConstFloatChunk(data);
	chunks.add(pChnk);
	return true;
}

bool CSLStatement::addFunctionChunk( const string& func, const ArrayList<string>& params )
{
	cout << "<li>Function chunk: " << func << "</li>" << endl;
	return false;
}

bool CSLStatement::addStatementChunk( const string& stmt )
{
	cout << "<li>Statement chunk: " << stmt << "</li>" << endl;

	CSLStatement* pStmt = New CSLStatement();
	pStmt->buildStatement(stmt);

	CSLStatementChunk* pChnk = New CSLStatementChunk(pStmt);
	chunks.add(pChnk);
	return true;
}

bool CSLStatement::execute( void )
{
	if (chunks.isEmpty())
		return true;

	CSLChunk* pChnk = chunks.getFront();

	switch (pChnk->Type)
	{
	case CHUNK_TYPE_COMMAND:
		{
			CSLCommandChunk* pCmdChnk = (CSLCommandChunk*)pChnk;

			switch (pCmdChnk->Command)
			{
			case COMMAND_OUT:

				for (unsigned int i = 1; i < chunks.getSize(); ++i)
				{
					switch (chunks[i]->Type)
					{
					case CHUNK_TYPE_CONST_STRING:
						{
							CSLConstStringChunk* pTmp = (CSLConstStringChunk*)chunks[i];
							cout << pTmp->Data;
						}
						break;
					case CHUNK_TYPE_CONST_INT:
						{
							CSLConstIntChunk* pTmp = (CSLConstIntChunk*)chunks[i];
							cout << pTmp->Data;
						}
						break;
					case CHUNK_TYPE_CONST_FLOAT:
						{
							CSLConstFloatChunk* pTmp = (CSLConstFloatChunk*)chunks[i];
							cout << pTmp->Data;
						}
						break;
					case CHUNK_TYPE_VAR:
						{
							CSLVariableChunk* pTmp = (CSLVariableChunk*)chunks[i];
							const string& varName = pTmp->VarName;

							switch (Interpreter::GetVarType(varName))
							{
							case VAR_TYPE_STRING:

								cout << Interpreter::GetStringVar(varName);

								break;
							case VAR_TYPE_INT:

								cout << Interpreter::GetIntVar(varName);

								break;
							case VAR_TYPE_FLOAT:

								cout << Interpreter::GetFloatVar(varName);

								break;
							}
						}
						break;
					}
				}

				break;
			}
		}
		break;
	case CHUNK_TYPE_OP:
		{
			CSLOperatorChunk* pTmp = (CSLOperatorChunk*)pChnk;

			if (chunks.getSize() < 2)
			{
				// Error
				return false;
			}

			if (pTmp->Op != OPERATOR_PLUS_PLUS && pTmp->Op != OPERATOR_MINUS_MINUS)
			{
				// Error
				return false;
			}

			if (chunks[1]->Type != CHUNK_TYPE_VAR)
			{
				// Error
				return false;
			}

			CSLVariableChunk* pVar = (CSLVariableChunk*)chunks[1];
			CSLVarType type = Interpreter::GetVarType(pVar->VarName);

			if (type != VAR_TYPE_INT && type != VAR_TYPE_FLOAT)
			{
				// Error
				return false;
			}

			if (pTmp->Op == OPERATOR_PLUS_PLUS)
			{
				if (type == VAR_TYPE_INT)
					++Interpreter::GetIntVar(pVar->VarName);
				else
					++Interpreter::GetFloatVar(pVar->VarName);
			}
			else if (pTmp->Op == OPERATOR_MINUS_MINUS)
			{
				if (type == VAR_TYPE_INT)
					--Interpreter::GetIntVar(pVar->VarName);
				else
					--Interpreter::GetFloatVar(pVar->VarName);
			}
		}
		break;
	case CHUNK_TYPE_VAR:
		{
			CSLVariableChunk* pTmp = (CSLVariableChunk*)pChnk;

		}
		break;
	}

	return true;
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
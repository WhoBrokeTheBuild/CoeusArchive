#ifndef COEUS_CSL_INTERPRETER_CSL_CHUNKS_H
#define COEUS_CSL_INTERPRETER_CSL_CHUNKS_H

#include <Arc/ManagedObject.h>

#include <Arc/ArrayList.h>

#include "CSLChunkType.h"
#include "CSLCommand.h"
#include "CSLOperator.h"

using namespace Arc;

class CSLStatement;

struct CSLChunk
	: public ManagedObject
{
public:

	CSLChunk( const CSLChunkType& type )
		: Type(type)
	{ }

	virtual inline string getClassName( void ) const { return "CSL Chunk"; }

	CSLChunkType Type;

};

struct CSLCommandChunk
	: public CSLChunk
{
public:

	CSLCommandChunk( const CSLCommand& cmd )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_COMMAND),
		  Command(cmd)
	{ }

	virtual inline string getClassName( void ) const { return "CSL Command Chunk"; }

	CSLCommand Command;

};

struct CSLOperatorChunk
	: public CSLChunk
{
public:

	inline CSLOperatorChunk( const CSLOperator& op )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_OP),
		  Op(op)
	{ }

	virtual inline string getClassName( void ) const { return "CSL Operator Chunk"; }

	CSLOperator Op;
};

struct CSLFunctionChunk
	: public CSLChunk
{
public:

	inline CSLFunctionChunk( const string& funcName, const ArrayList<CSLStatement*> params )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_FUNC),
		  FuncName(funcName),
		  Params(params)
	{ }

	virtual ~CSLFunctionChunk( void );

	virtual inline string getClassName( void ) const { return "CSL Function Chunk"; }

	string FuncName;
	ArrayList<CSLStatement*> Params;

};

struct CSLStatementChunk
	: public CSLChunk
{
public:

	inline CSLStatementChunk( CSLStatement* inner )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_STMT),
	      Inner(inner)
	{ }

	virtual ~CSLStatementChunk( void );

	virtual inline string getClassName( void ) const { return "CSL Statement Chunk"; }

	CSLStatement* Inner;

};

struct CSLVariableChunk
	: public CSLChunk
{
public:

	inline CSLVariableChunk( const string& varName )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_VAR),
		  VarName(varName)
	{ }

	virtual inline string getClassName( void ) const { return "CSL Variable Chunk"; }

	string VarName;

};

struct CSLConstStringChunk
	: public CSLChunk
{
public:

	inline CSLConstStringChunk( const string& data )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_CONST_STRING),
		  Data(data)
	{ }

	virtual inline string getClassName( void ) const { return "CSL Const String Chunk"; }

	string Data;
};

struct CSLConstIntChunk
	: public CSLChunk
{
public:

	inline CSLConstIntChunk( const int& data )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_CONST_INT),
		  Data(data)
	{ }

	virtual inline string getClassName( void ) const { return "CSL Const Int Chunk"; }

	int Data;
};

struct CSLConstFloatChunk
	: public CSLChunk
{
public:

	inline CSLConstFloatChunk( const float& data )
		: CSLChunk(CSLChunkType::CHUNK_TYPE_CONST_FLOAT),
		  Data(data)
	{ }

	virtual inline string getClassName( void ) const { return "CSL Const Float Chunk"; }

	float Data;
};

#endif // COEUS_CSL_INTERPRETER_CSL_CHUNKS_H
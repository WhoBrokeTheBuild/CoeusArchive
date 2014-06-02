#ifndef COEUS_CSL_INTERPRETER_CSL_STATEMENT_H
#define COEUS_CSL_INTERPRETER_CSL_STATEMENT_H

#include <Arc/ManagedObject.h>
#include <Arc/ArrayList.h>

#include "CSLChunks.h"

using namespace std;
using namespace Arc;

class CSLStatement
	: public ManagedObject
{
public:

	inline CSLStatement( void )
		: chunks()
	{ }

	inline ~CSLStatement( void )
	{
		while ( ! chunks.isEmpty())
			delete chunks.popBack();
	}

	virtual inline string getClassName( void ) const { return "CSL Statement"; }

	bool buildStatement( const string& stmt );

	bool addChunk( const string& chnk );

	bool addCommandChunk( const string& cmd );
	bool addOperatorChunk( const string& op );
	bool addVariableChunk( const string& name );

	bool addConstStringChunk( const string& data );
	bool addConstIntChunk( const int& data );
	bool addConstFloatChunk( const float& data );

	bool addFunctionChunk( const string& func, const ArrayList<string>& params );
	bool addParenthesisChunk( const string& inner );

	bool execute( void );

private:

	CSLChunkType findChunkType(const string& data);

	ArrayList<CSLChunk*> chunks;

};

#endif // COEUS_CSL_INTERPRETER_CSL_STATEMENT_H
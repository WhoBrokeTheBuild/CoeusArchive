#ifndef COEUS_CSL_INTERPRETER_INTERPRETER_H
#define COEUS_CSL_INTERPRETER_INTERPRETER_H

#include <Arc/ManagedObject.h>
#include <Arc/ArrayList.h>

#include "CSLStatement.h"

using namespace Arc;

class Interpreter :
	public ManagedObject
{
public:

	Interpreter( const string& filename );

	virtual ~Interpreter( void );

	virtual inline string getClassName( void ) const { return "Interpreter"; }

private:

	void processStatement( const string& stmt );

	ArrayList<CSLStatement*> m_Statements;

};

extern Interpreter* gp_Interpreter;

#endif // COEUS_CSL_INTERPRETER_INTERPRETER_H
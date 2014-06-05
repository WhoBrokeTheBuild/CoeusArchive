#ifndef COEUS_CSL_INTERPRETER_INTERPRETER_H
#define COEUS_CSL_INTERPRETER_INTERPRETER_H

#include <Arc/ManagedObject.h>
#include <Arc/ArrayList.h>
#include <Arc/Map.h>

#include "CSLStatement.h"
#include "CSLVarType.h"

using namespace Arc;

class Interpreter :
	public ManagedObject
{
public:

	Interpreter( const string& filename );

	virtual ~Interpreter( void );

	virtual inline string getClassName( void ) const { return "Interpreter"; }

	static CSLVarType GetVarType( const string& name ) { return (s_Vars.containsKey(name) ? s_Vars[name] : INVALID_VAR_TYPE); }

	static void RemoveVar( const string& name );

	static string&	GetStringVar( const string& name )	{ return s_StringVars[name]; }
	static int&		GetIntVar	( const string& name )	{ return s_IntVars[name]; }
	static float&	GetFloatVar	( const string& name )	{ return s_FloatVars[name]; }

	static void SetStringVar( const string& name, const string& data );
	static void SetIntVar	 ( const string& name, const int& data );
	static void SetFloatVar ( const string& name, const float& data );

private:

	void processStatement( const string& stmt );

	static ArrayList<CSLStatement*> s_Statements;

	static Map<string, CSLVarType> s_Vars;

	static Map<string, string>	s_StringVars;
	static Map<string, int>		s_IntVars;
	static Map<string, float>	s_FloatVars;

};

extern Interpreter* gp_Interpreter;

#endif // COEUS_CSL_INTERPRETER_INTERPRETER_H
#include "Interpreter.h"

int main( int argc, char* argv[] )
{
	if (argc <= 1)
	{
		printf("Error: Please specify file to parse\r\n");
		return 1;
	}

	const string& filename = argv[1];
	gp_Interpreter = New Interpreter(filename);

	delete gp_Interpreter;

	return 0;
}
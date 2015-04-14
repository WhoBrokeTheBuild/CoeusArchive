#include <iostream>

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/Log.h>

#include "SQLServer.h"

using Arc::Log;

int main( int argc, char* argv[] )
{
	Arc::Arc_InitCore();
	Arc::Arc_InitNet();

	Log::AddInfoOutput("stdout", false);
	Log::AddErrorOutput("stderr", false);
	Log::AddInfoOutput("logs/info.log");
	Log::AddErrorOutput("logs/error.log");

	SQLServer server;
	server.run();

	Log::CloseOutputs();

	Arc::Arc_TermNet();

	return 0;
}
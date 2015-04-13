#include <iostream>

#include <Arc/ArcCore.h>
#include <Arc/ArcNet.h>

#include <Arc/Log.h>

#include "WebServer.h"

using namespace std;
using namespace Arc;

int main( int argc, char* argv[] )
{
	Arc_InitCore();
	Arc_InitNet();

	Log::AddInfoOutput("stdout", false);
	Log::AddErrorOutput("stderr", false);
	Log::AddInfoOutput("logs/info.log");
	Log::AddErrorOutput("logs/error.log");

	WebServer server;
	server.run();

	Log::CloseOutputs();

	Arc_TermNet();

	return 0;
}
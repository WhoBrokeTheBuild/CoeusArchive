#include "Worker.h"

#include <Arc/Log.h>

using Arc::Log;

bool Worker::run( void )
{
	while (true)
	{
		mp_CurrClient->sendString("CoeusSQL> ");
		const string& line = mp_CurrClient->recvLine();
		Log::InfoFmt(getClassName(), "%s", line.c_str());

		if (line == "exit")
			break;
	}

	return true;
}
#include "CSLChunks.h"

#include "CSLStatement.h"

CSLFunctionChunk::~CSLFunctionChunk(void)
{
	while ( ! Params.isEmpty())
		delete Params.popBack();
}

CSLStatementChunk::~CSLStatementChunk(void)
{
	delete Inner;
}

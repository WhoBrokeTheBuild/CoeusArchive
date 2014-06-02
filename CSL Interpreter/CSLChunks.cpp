#include "CSLChunks.h"

#include "CSLStatement.h"

CSLFunctionChunk::~CSLFunctionChunk(void)
{
	while ( ! Params.isEmpty())
		delete Params.popBack();
}

CSLParenthesisChunk::~CSLParenthesisChunk(void)
{
	delete Inner;
}

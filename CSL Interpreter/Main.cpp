#include <iostream>
#include <fstream>

#include <Arc/ArcCore.h>
#include <Arc/Buffer.h>

using namespace std;
using namespace Arc;

int main( int argc, char* argv[] )
{
	if (argc <= 1)
	{
		printf("Error: Please specify file to parse\r\n");
		return 1;
	}

	const string& filename = argv[1];
	ifstream file(filename);

	if ( ! file)
	{
		printf("Error: %s does not exist\r\n", filename.c_str());
		return 1;
	}

	Buffer buff;
	buff.setDataFromStream(file);

	bool inCSL = false;

	while ( ! buff.endOfBuffer())
	{
		const char& last = buff.readCharAt(buff.getReadIndex());
		const char& ch = buff.readNextChar();
		const char& next = buff.readCharAt(buff.getReadIndex());

		if (inCSL)
		{
			if (ch == '=' && next == '>')
			{
				inCSL = false;
				buff.setReadIndex(buff.getReadIndex() + 1);
				continue;
			}
		}
		else
		{
			if (ch == '<' && next == '=')
			{
				inCSL = true;
				buff.setReadIndex(buff.getReadIndex() + 1);
				continue;
			}

			printf("%c", ch);
		}
	}

	return 0;
}
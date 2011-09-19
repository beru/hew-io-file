
#include <stdio.h>

#include "HEWIO.h"
#include <vector>

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("usage: hewio.exe iofilename\n");
		return -1;
	}
	FILE* f = fopen(argv[1], "rb");
	size_t fileSize = GetFileSize(f);
	std::vector<char> buff(fileSize);
	fread(&buff[0], 1, fileSize, f);
	fclose(f);
	
	HEWIO::Data data;

	data.Parse(&buff[0], buff.size());
	
	
	
	return 0;
}


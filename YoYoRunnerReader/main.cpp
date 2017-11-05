#include "function.h"

int main(int argc, char** argv)
{
	char *srcStr, *folder;
#ifndef DEBUG
	if (argc < 3)
	{
		printf("Usage: ./%s pathToSrc pathToDst\n", argv[0]);
		exit(1337);
	}
	srcStr = argv[1];
	dstStr = argv[2];
#else
	srcStr = "data.win";
	folder = "dump";
#endif

	system("mkdir dump");

	FILE* src = fopen(srcStr, "rb");
	if (!src)
	{
		printf("Failed to open %s\n", srcStr);
		exit(1);
	}

	//dumpBinaries(src);

	Chunk chunk;
	while (1)
	{
		if (fread(&chunk, sizeof(chunk), 1, src) != 1)
			break;

		//add sentinel
		char chunkName[5];
		memcpy(chunkName, chunk.name, 4);
		chunkName[4] = '\0';

		long chunkTop = ftell(src);
		long chunkLast = chunkTop + chunk.size;

		printf("%s, (size : %d @ offset : 0x%09x): ", chunkName, chunk.size, chunkTop);

		if (strcmp(chunkName, "FORM") == 0)
		{			
			uint32_t totalSize = chunk.size;
			printf("\n\n");
		}
		else
		{
			if (strcmp(chunkName, "SCPT") == 0)
			{
				readList(entryNum, entryOffsets, src);
				printf("%u scripts\n", entryNum);

				FILE *scriptFile = fopen("dump/script.csv", "wb");

				if (!scriptFile)
					continue;

				ScriptDefinitionPrintCSVHeader(scriptFile);

				readTarray(ScriptDefinition, scripts, entryNum, src);

				for (int i = 0; i < entryNum; i++) {
					readStringAt(name, scripts[i].nameOffset, src);
					ScriptDefinitionPrintCSV(scriptFile, scripts[i], name);
				}

				fclose(scriptFile);
				fseek(src, chunkLast, SEEK_SET);
			}
			else if (strcmp(chunkName, "TXTR") == 0)
			{
				extractTexture(src);
			}
			else
			{
				char* hum;
				hum = (char*)malloc(chunk.size);
				fread(hum, 1, chunk.size, src);
				printf("\nUnknow type %s, Skipping\n", chunkName);
			}			
		}
	}

	system("Pause");

	return 0;
}
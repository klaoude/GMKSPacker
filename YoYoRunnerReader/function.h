#pragma once

#include "structs.h"

#define readT(__TYPE__, __VAR__, __FILE__) \
    __TYPE__ __VAR__;\
    if (fread(&__VAR__, sizeof(__TYPE__), 1, __FILE__) != 1) {\
        printf("failed to readT file\n");\
    }\

#define readTarray(__TYPE__, __VAR__, __NUM__, __FILE__) \
    __TYPE__ __VAR__[999];\
	if (fread(__VAR__, sizeof(__TYPE__), __NUM__, __FILE__) != __NUM__) {\
		printf("failed to readTarray file\n");\
	}\


#define readByte(__VAR__, __FILE__) \
    readT(char, __VAR__, __FILE__)

#define readBytes(__VAR__, __NUM__, __FILE__) \
    readTarray(char, __VAR__, __NUM__, __FILE__)

#define read32(__VAR__, __FILE__) \
    readT(uint32_t, __VAR__, __FILE__)

#define read32array(__VAR__, __NUM__, __FILE__) \
    readTarray(uint32_t, __VAR__, __NUM__, __FILE__)

#define readList(__SIZE_VAR__, __ARRAY_VAR__, __FILE__) \
    read32(__SIZE_VAR__, __FILE__);\
    read32array(__ARRAY_VAR__, __SIZE_VAR__, __FILE__);

#define readStringAt(__VAR__, __OFFSET__, __FILE__) \
    char __VAR__[10000];\
    fseek(__FILE__, __OFFSET__, SEEK_SET);\
    fscanf(__FILE__, "%s", __VAR__);

void dumpBinaries(FILE* file)
{
	printf("Dumping all binaries...\n");
	Chunk chunk;
	FILE* dumping;
	while (1)
	{
		if (fread(&chunk, sizeof(chunk), 1, file) != 1)
			break;

		char chunkName[5];
		memcpy(chunkName, chunk.name, 4);
		chunkName[4] = '\0';

		if (strcmp(chunkName, "FORM") != 0)
		{
			printf("	Dumping %s...", chunkName);
			std::string fileName;
			fileName = "dump/" + std::string(chunkName) + ".bin";
			dumping = fopen(fileName.c_str(), "wb");

			for (int i = 0; i < chunk.size; i++)
				fputc(fgetc(file), dumping);

			printf(" Done !\n");
		}		
	}
}

GEN8 binToGEN8(FILE* bin)
{
	GEN8 gen = GEN8();

	if (bin == NULL)
		return gen;

	gen.Debug = fgetc(bin);
	fread(&gen.func, 3, 1, bin);
	fread(&gen.FilenameOffset, 4, 1, bin);
	fread(&gen.ConfigOffset, 4, 1, bin);
	fread(&gen.LastObj, 4, 1, bin);
	fread(&gen.LastTile, 4, 1, bin);
	fread(&gen.GameID, 4, 1, bin);
	fread(&gen.Unknown, 4*4, 1, bin);
	fread(&gen.NameOffset, 4, 1, bin);
	fread(&gen.Major, 4, 1, bin);
	fread(&gen.Minor, 4, 1, bin);
	fread(&gen.Release, 4, 1, bin);
	fread(&gen.Build, 4, 1, bin);
	fread(&gen.DefaultWindowWidth, 4, 1, bin);
	fread(&gen.DefaultWindowHeight, 4, 1, bin);
	fread(&gen.Info, sizeof(InfoFlags), 1, bin);
	fread(&gen.LicenseMD5, 0x10, 1, bin);

	return gen;
}

uint32_t SwapEnd32(uint32_t v) 
{ 
	return((v & 0xFF000000) >> 24 | (v & 0x00FF0000) >> 8 | (v & 0x0000FF00) << 8 | (v & 0x000000FF) << 24); 
}

void extractTexture(FILE* file)
{
	system("mkdir Textures");

	uint32_t number;
	fread(&number, sizeof(uint32_t), 1, file);

	printf("%u texures\n", number);

	uint32_t fileOffset = ftell(file);

	std::vector<TXTR> textures;
	for (uint32_t i = 0; i < number; i++)
	{
		uint32_t offset;
		fseek(file, fileOffset + i*4, SEEK_SET);
		fread(&offset, sizeof(uint32_t), 1, file);
		fseek(file, offset, SEEK_SET);
		uint32_t junk;
		fread(&junk, sizeof(uint32_t), 1, file);
		fread(&offset, sizeof(uint32_t), 1, file);
		textures.push_back({ junk, offset });
	}

	FILE* dst;
	char* path;
	for (int i = 0; i < number - 1; i++)
	{
		path = (char*)malloc(16);
		sprintf(path, "textures/%02d.png", i);
		dst = fopen(path, "wb");

		printf("	Extrating %02d.png @ 0x%08x -> 0x%08x (size : 0x%08x) ...", i, textures[i].Offset, textures[i + 1].Offset, textures[i + 1].Offset - textures[i].Offset);

		fseek(file, textures[i].Offset, SEEK_SET);
		for (int j = textures[i].Offset; j < textures[i + 1].Offset; j++)
			fputc(fgetc(file), dst);

		printf(" Done !\n");

		fclose(dst);
		free(path);
	}	

	printf("	Extrating %02d.png @ %08x...", number - 1, textures[number - 1].Offset);

	path = (char*)malloc(16);
	sprintf(path, "textures/%02d.png", number - 1);
	dst = fopen(path, "wb");
	char get[4];
	do
	{
		fread(&get, 1, 4, file);
		fwrite(get, 1, 4, dst);
	} while (get[0] != 'I' && get[1] != 'E' && get[2] != 'N' && get[3] != 'D');

	fclose(dst);
	free(path);
	printf(" Done !\n");
}

//std::vector<STRG> getSTRGChunk(Chunk chunk, FILE* file)
//{
//	
//}
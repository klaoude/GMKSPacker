#pragma once

#include <iostream>
#include <vector>
#include <string> 

#define DEBUG 1

enum InfoFlags : uint32_t
{
	Fullscreen = 0x0001,
	SyncVertex1 = 0x0002,
	SyncVertex2 = 0x0004,
	Interpolate = 0x0008,
	Unknown = 0x0010,
	ShowCursor = 0x0020,
	Sizeable = 0x0040,
	ScreenKey = 0x0080,
	SyncVertex3 = 0x0100,
	StudioVersionB1 = 0x0200,
	StudioVersionB2 = 0x0400,
	StudioVersionB3 = 0x0800,
	StudioVersionMask,
	SteamEnabled = 0x1000,
	LocalDataEnabled = 0x2000,
	BorderlessWindow = 0x4000
};

typedef struct
{
	char name[4];
	uint32_t size;
} Chunk;

typedef struct
{
	char Debug; // 1-byte boolean
	char func[3]; // unknown function (0x00000E)
	uint32_t FilenameOffset; // offset to string
	uint32_t ConfigOffset; // idem
	uint32_t LastObj; // possibly offset
	uint32_t LastTile; // idem
	uint32_t GameID;
	uint32_t Unknown[4]; // 0, 0, 0, 0
	uint32_t NameOffset; // to string
	uint32_t Major, Minor, Release, Build; // version stuff
	uint32_t DefaultWindowWidth, DefaultWindowHeight;
	InfoFlags Info;
	char LicenseMD5[0x10]; // the file itself has no checksum or anything
	uint32_t LicenseCRC32;
	unsigned long Timestamp; // 64-bit UNIX time
	uint32_t DisplayNameOffset; // to string
	uint32_t ActiveTargets; // probably a flags value indicating for which platforms
						// the file is built for
						// no target flag values are known at the time of writing this
	uint32_t UnknownFunc[4]; // unknown function (0, 0x00000016, 0, 0xFFFA068C)
	uint32_t SteamAppID;
	uint32_t NumberCount; // amount of weird numbers
	uint32_t Numbers[]; // weird numbers; starting from 0 up to NumberCount - 1...

} GEN8;

typedef struct 
{ 
	uint32_t NameOffset; // to string
	uint32_t ValueOffset; // ARGB, possibly string 
} Constant;
typedef struct
{
	uint32_t Unknown[2]; //Unknown { 0x80000000, 2 }
	InfoFlags FlagsDup; // duplicate from GEN8
	uint32_t UnknownTab[0x8]; // { 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0 }
	uint32_t UnknowVar; //Unknow
	std::vector<Constant> Constant;
} OPTN;

typedef struct
{
	uint32_t OffsetToString;
	uint32_t AWeirdValue;
	uint32_t AnotherString;
	uint32_t MoreWeirdInts[3];
} ExtnData;

typedef struct
{
	uint32_t Strings0[3]; // to STRG
	std::vector<uint32_t> MoreStrings; // to STRG
	uint32_t Strings1[2]; // etc
	std::vector<ExtnData> Data;
	uint32_t YetAnotherString;
	uint32_t AnotherInt;
	uint32_t LooksLikeAHashValue[4];

} EXTN;

enum SoundEntryFlags : uint32_t
{
	IsEmbedded = 0x01,
	IsCompressed = 0x02,
	Normal
};
typedef struct
{
	uint32_t NameOffset; // offset to string (usually file name without ext.)
	SoundEntryFlags Flags;
	uint32_t TypeOffset; // offset to string (file extension)
	uint32_t FileOffset; // offset to string (file name, including ext.)
	uint32_t Unknown; // 0
	float Volume; // default volume
	float Pitch; // default pitch
	int32_t GroupID; // to AGRP
	int32_t AudioID; // -1 if unused? Only makes sense when embedded?

} SOND;

typedef struct
{
	uint32_t NameOffset;
} AGRP;

typedef struct
{
	uint32_t NameOffset; // offset to name string
	uint32_t Width, Height;
	uint32_t Left, Right, Bottom, Top;
	uint32_t Unknown[3]; // something with collision masks
	uint32_t BBoxMode;
	uint32_t SepMasks;
	uint32_t OriginX, OriginY;
	uint32_t TextureCount;
	uint32_t TextureOffset; // to TPAG
} SPRT;

typedef struct
{
	uint32_t NameOffset;
	uint32_t Unknown[3]; // 0, 0, 0
	uint32_t TextureOffset; // TPAG iirc
} BGND;

typedef struct
{
	float X;
	float Y;
	float Speed;
} PathPoint;
typedef struct
{
	uint32_t NameOffset;
	uint32_t IsSmooth;
	uint32_t IsClosed;
	uint32_t Precision;
	uint32_t PointCount;
	PathPoint Points[];
} PATH;

typedef struct
{
	uint32_t NameOffset;
	uint32_t codeID;
} SCPT;

typedef struct 
{
	uint32_t NameOffset;
	int32_t DataLength;
	std::vector<char> Bytecode;
} CODE;

typedef struct
{
	uint32_t Length;
	std::vector<char> Characters; //ASCII
} STRG;

typedef struct
{
	uint32_t Unknown;
	uint32_t Offset;
} TXTR;

typedef struct {
	uint32_t nameOffset;
	int32_t id;
} ScriptDefinition;

extern void ScriptDefinitionPrintCSVHeader(FILE *file) {
	fprintf(file, "name,id\n");
}

extern void ScriptDefinitionPrintCSV(FILE *file, ScriptDefinition d, char *name) {
	fprintf(file, "%s,%d\n", name, d.id);
}
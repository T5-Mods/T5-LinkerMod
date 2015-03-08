#pragma once
#include <WTypes.h>
#include "r_material.h"

enum XAssetType
{
  ASSET_TYPE_XMODELPIECES = 0x0,
  ASSET_TYPE_PHYSPRESET = 0x1,
  ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
  ASSET_TYPE_DESTRUCTIBLEDEF = 0x3,
  ASSET_TYPE_XANIMPARTS = 0x4,
  ASSET_TYPE_XMODEL = 0x5,
  ASSET_TYPE_MATERIAL = 0x6,
  ASSET_TYPE_TECHNIQUE_SET = 0x7,
  ASSET_TYPE_IMAGE = 0x8,
  ASSET_TYPE_SOUND = 0x9,
  ASSET_TYPE_SOUND_PATCH = 0xA,
  ASSET_TYPE_CLIPMAP = 0xB,
  ASSET_TYPE_CLIPMAP_PVS = 0xC,
  ASSET_TYPE_COMWORLD = 0xD,
  ASSET_TYPE_GAMEWORLD_SP = 0xE,
  ASSET_TYPE_GAMEWORLD_MP = 0xF,
  ASSET_TYPE_MAP_ENTS = 0x10,
  ASSET_TYPE_GFXWORLD = 0x11,
  ASSET_TYPE_LIGHT_DEF = 0x12,
  ASSET_TYPE_UI_MAP = 0x13,
  ASSET_TYPE_FONT = 0x14,
  ASSET_TYPE_MENULIST = 0x15,
  ASSET_TYPE_MENU = 0x16,
  ASSET_TYPE_LOCALIZE_ENTRY = 0x17,
  ASSET_TYPE_WEAPON = 0x18,
  ASSET_TYPE_WEAPONDEF = 0x19,
  ASSET_TYPE_WEAPON_VARIANT = 0x1A,
  ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1B,
  ASSET_TYPE_FX = 0x1C,
  ASSET_TYPE_IMPACT_FX = 0x1D,
  ASSET_TYPE_AITYPE = 0x1E,
  ASSET_TYPE_MPTYPE = 0x1F,
  ASSET_TYPE_MPBODY = 0x20,
  ASSET_TYPE_MPHEAD = 0x21,
  ASSET_TYPE_CHARACTER = 0x22,
  ASSET_TYPE_XMODELALIAS = 0x23,
  ASSET_TYPE_RAWFILE = 0x24,
  ASSET_TYPE_STRINGTABLE = 0x25,
  ASSET_TYPE_PACK_INDEX = 0x26,
  ASSET_TYPE_XGLOBALS = 0x27,
  ASSET_TYPE_DDL = 0x28,
  ASSET_TYPE_GLASSES = 0x29,
  ASSET_TYPE_EMBLEMSET = 0x2A,
  ASSET_TYPE_COUNT = 0x2B,
  ASSET_TYPE_STRING = 0x2B,
  ASSET_TYPE_ASSETLIST = 0x2C,
};

union XAssetHeader
{
	Material *material;
	MaterialPixelShader *pixelShader;
	MaterialVertexShader *vertexShader;
	MaterialTechniqueSet *techniqueSet;
	GfxImage *image;
	void *data;
};

struct XAsset
{
	XAssetType type;
	XAssetHeader header;
};

struct XAssetEntry
{
	XAsset asset;
	char zoneIndex;
	bool inuse;
	unsigned __int16 nextHash;
	unsigned __int16 nextOverride;
	unsigned __int16 usageFrame;
};

union XAssetEntryPoolEntry
{
	XAssetEntry entry;
	XAssetEntryPoolEntry *next;
};

enum FF_DIR
{
  FFD_DEFAULT = 0x0,
  FFD_MOD_DIR = 0x1,
  FFD_USER_MAP = 0x2,
};

struct XZoneName
{
  char name[64];
  int flags;
  int fileSize;
  FF_DIR dir;
  bool loaded;
};

static BYTE* db_hashTable = nullptr;
static XAssetEntryPoolEntry* g_assetEntryPool = nullptr;
XZoneName* g_zoneNames = nullptr;

const char * DB_GetXAssetTypeName(int type);
unsigned int DB_HashForName(const char *name, XAssetType type);
const char* DB_GetXAssetName(XAsset *asset);
XAssetEntry* DB_FindXAssetEntry(XAssetType type, const char *name);
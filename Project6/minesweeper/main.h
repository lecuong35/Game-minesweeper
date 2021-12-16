#pragma once
#include <iostream>
#include <ctime>
#include <Windows.h>

#include "resource.h"

#pragma comment(lib, "winmm.lib")
#define AUDIO(FileName) (PlaySound(MAKEINTRESOURCE(FileName),GetModuleHandle(NULL),SND_RESOURCE | SND_ASYNC));

#define ConsoleWidth 80
#define ConsoleHeight 35

#define ClickLeft 0x5A
#define ClickRight 0x58

struct TableStruct
{
	short SRow = 0; // Dong.
	short SCol = 0; // Cot.
	short SMineCount = 0; // So bom.
	short SOpenOCount = 0; // So o da mo.
	short SFlagCount = 0; // So co.
	short STime = 0;
};

struct BoxStruct
{
	bool BLandMine = false; // Co bom.
	bool BOpened = false; // Da mo.
	bool BFlag = false; // Cam co.
	short SNeighborMine = 0; // Bom lan can
	bool BVisited = false;  // trong qua trinh de quy da xet qua diem nay chua
};

// Trang, Vi tri dang chon, Tong muc.
extern short SPages, SSelectLocation, STotalCatalog;

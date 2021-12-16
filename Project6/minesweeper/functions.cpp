#include "main.h"
#include "functions.h"
#include "Console.h"
#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<cstring>

using namespace std;

TableStruct Table;
BoxStruct** Box;

// Vi tri con tro hien tai.
COORD CCurLocation;

// Su dung phim.
bool BUseKeyboard = false;

// Toa do x, y ve bang.
short SXCoord;

short SYCoord;

// Cap nhat trang thai choi game.
bool BPlayGameStatus = false;

// Thoi gian vao game.
int ITime = 0;

void matrixCreate()
{
	Box = new BoxStruct * [Table.SRow];
	for (int i = 0; i < Table.SRow; ++i)
	{
		Box[i] = new BoxStruct[Table.SCol];
	}
}

void matrixDelete()
{
	for (int i = 0; i < Table.SRow; ++i)
	{
		delete[] Box[i];
	}
	delete[] Box;
}

void tableCoordSave()
{
	SXCoord = ((ConsoleWidth / 2) - Table.SRow);
	SYCoord = (((ConsoleHeight - 6) - Table.SCol) / 2) + 7;
}

void init(short SRow, short SCol, short SMineCount)
{
	Table.SRow = SRow;
	Table.SCol = SCol;
	Table.SMineCount = SMineCount;
	Table.SOpenOCount = 0;
	Table.SFlagCount = 0;
	if (SRow == 9) Table.SSuggest = 2;
	if (SRow == 16) Table.SSuggest = 5;
	if (SRow == 24) Table.SSuggest = 12;


	matrixCreate();
	randomMineCreate();
	tableCoordSave();
	CCurLocation = { 0, 0 };
	BPlayGameStatus = true;
	ITime = GetTickCount64();

	drawTable();
	drawPlayGameStatus(1, 0, 0);
}

short xCoord(short SX) // Toa do x ve bang.
{
	return ((SX * 2) + SXCoord);
}

short yCoord(short SY) // Toa do y ve bang.
{
	return (SY + SYCoord);
}

void randomMineCreate()
{
	short SSoBom = Table.SMineCount;
	short SI, SJ; // SI vi tri dong, SJ vi tri cot ta se random.
	srand(time(NULL)); // Reset time.
	while (SSoBom)
	{
		/* CT: muon random trong khoang x, y thi ta se rand() % (y - x + 1) + x
		VD: Random trong khoang 0 -> 8 => CT: rand() % (8 - 0 + 1) + 0 = rand() % 9 = rand() % Table.SDong.
		*/
		SI = rand() % Table.SRow;
		SJ = rand() % Table.SCol;
		if (Box[SI][SJ].BLandMine)
			continue;

		Box[SI][SJ].BLandMine = true;
		--SSoBom; // Cap nhat lai so luong bom.
	}
}

void clickRight(short SX, short SY) // Cam co.
{
	if (!Box[SX][SY].BOpened && BPlayGameStatus)
	{
		if (Box[SX][SY].BFlag)
		{
			Box[SX][SY].BFlag = false;
			Table.SFlagCount--; // ?
		}
		else
		{
			Box[SX][SY].BFlag = true;;
			Table.SFlagCount++; // ?
		}

		AUDIO(IDR_WAVE2);
	}
	drawTable();

	deleteRow(4, 1);
	drawPlayGameStatus(1, 0, 0);
}

void suggest()
{
	if (Table.SSuggest > 0)
	{
		short i = 0;
		short j = 0;
		short size = Table.SCol;
		bool BBreak = false;
		for (i = 0; i < size; ++i) 
		{
			for (j = 0; j < size; ++j)
			{
				if (Box[i][j].BLandMine && !Box[i][j].BFlag && !Box[i][j].BOpened && BPlayGameStatus)
				{
					Box[i][j].BFlag = true;
					Table.SFlagCount++;
					drawTable();
					deleteRow(4, 1);
					drawPlayGameStatus(1, 0, 0);
					BBreak = true;
					//clickRight(i, j);
					break;
				}
			}
			if (BBreak) break;

		}
	}
	Table.SSuggest--;
}

short neighborMineCount(short SX, short SY)
{
	short SDem = 0;
	for (int i = SX - 1; i <= SX + 1; ++i)
	{
		for (int j = SY - 1; j <= SY + 1; ++j)
		{
			// Xet nhung vi tri khong hop => tiep tuc lap.
			if (i < 0 || i >= Table.SRow || j < 0 || j >= Table.SCol || (i == SX && j == SY))
			{
				continue;
			}

			// Xet xem o co bom hay khong. Co tang len dem 1.
			if (Box[i][j].BLandMine)
			{
				++SDem;
			}
		}
	}
	return SDem;
}

void boxOpen(short SX, short SY)//them co da xet --> tranh de quy qua nhieu, quy uoc dat ten bien
{
	// Cach 1: Su dung de quy.
	if (!Box[SX][SY].BOpened && !Box[SX][SY].BFlag)
	{
		Box[SX][SY].BOpened = true;
		Box[SX][SY].BVisited = true;
		if (Box[SX][SY].BLandMine) // O co boom.
		{
			lose(); // => thua.
		}
		else
		{
			Table.SOpenOCount++;
			short SSoBomLanCan = neighborMineCount(SX, SY);
			if (SSoBomLanCan) // Co bom lan can.
			{
				Box[SX][SY].SNeighborMine = SSoBomLanCan;
			}
			else // O rong.
			{
				// Duyet cac o lan can va goi de quy.
				for (int i = SX - 1; i <= SX + 1; ++i)
				{
					for (int j = SY - 1; j <= SY + 1; ++j)
					{
						// Xet nhung vi tri khong hop le => tiep tuc lap.
						if (i < 0 || i >= Table.SRow || j < 0 || j >= Table.SCol || Box[i][j].BVisited)
						{
							continue;
						}

						// Goi de quy.
						else {
							 boxOpen(i, j);
						}
					}
				}
			}
		}
	}

	// Cach 2: Su dung khu de quy(ap dung hang doi queue trong C++).
}

bool flagStatistic()
{
	return ((Table.SOpenOCount + Table.SMineCount) == (Table.SRow * Table.SCol));
}

void clickLeft(short SX, short SY) // open box
{
	if (!Box[SX][SY].BOpened && !Box[SX][SY].BFlag && BPlayGameStatus)
	{
		boxOpen(SX, SY);

		if (BPlayGameStatus)
		{
			drawTable();
			if (flagStatistic())
			{
				win();
			}
			else
			{
				AUDIO(IDR_WAVE1);
			}
		}
	}
}

void win()
{
	BPlayGameStatus = false;
	matrixDelete(); // Giai phong con tro.
	SPages = 5;
	deleteRow(4, 1);
	drawPlayGameStatus(2, 2, 0); // Cap nhat lai trang thai la thang.

	AUDIO(IDR_WAVE4);
}

void lose()
{
	// Hien bom an va kiem tra cam co dung hay sai.
	for (int i = 0; i < Table.SRow; ++i)
	{
		for (int j = 0; j < Table.SCol; ++j)
		{
			if (Box[i][j].BFlag) // Co cam co.
			{
				if (Box[i][j].BLandMine)
				{
					drawBox(j, i, 15); // Cam co dung(cam co bom).
				}
				else
				{
					drawBox(j, i, 14); // Cam co sai(cam co khong co bom).
				}
			}
			else // Khong co cam co.
			{
				if (Box[i][j].BLandMine) // Co bom -> Hien bom.
				{
					drawBox(j, i, 9); // Hien bom an.
				}
			}
		}
	}

	BPlayGameStatus = false;
	matrixDelete(); // Giai phong con tro.
	SPages = 4;
	deleteRow(4, 1);
	drawPlayGameStatus(3, 3, 0); // Cap nhat lai trang thai la thua.

	AUDIO(IDR_WAVE3);
}

void playingContentSave()
{

}

/* Y tuong xu ly menu:
1) Trang menu chinh.
2) Trang menu chon cap do.
3) Trang choi game.
4) Trang thua.
5) Trang thang.
6) Trang luu game.
*/

void keyboardProcessing(KEY_EVENT_RECORD key)
{
	if (key.bKeyDown) // Co nhan phim.
	{
		switch (key.wVirtualKeyCode)
		{
		case VK_UP: // Mui ten len.
			switch (SPages)
			{
			case 1: // Menu chinh.
				if (SSelectLocation == 1 || SSelectLocation == 0)
				{
					SSelectLocation = STotalCatalog;
				}
				else
				{
					SSelectLocation -= 1;
				}

				drawMainMenu(SSelectLocation);
				break;
			case 2: // Menu chon cap do.
				if (STotalCatalog == 4)
				{
					if (SSelectLocation == 0)
					{
						SSelectLocation = STotalCatalog - 1;
					}
					else
					{
						SSelectLocation -= 1;
					}

					drawLevelMenu(SSelectLocation);
				}
				break;
			case 3: // Trang choi game.
				if (BPlayGameStatus)
				{
					BUseKeyboard = true;
					CCurLocation.Y = ((CCurLocation.Y == 0) ? Table.SRow - 1 : CCurLocation.Y - 1);
					drawTable();
				}
				break;
			case 4: // Trang thua.
				drawPlayGameStatus(3, 3, (SSelectLocation == 0) ? 1 : 0);
				break;
			case 5: // Trang thang.
				drawPlayGameStatus(2, 2, (SSelectLocation == 0) ? 1 : 0);
				break;
			case 6: // Trang luu lai.
				drawPlayGameStatus(1, 1, (SSelectLocation == 0) ? 1 : 0);
				break;
			}
			break;
		case VK_DOWN: // Mui ten xuong.
			switch (SPages)
			{
			case 1: // Menu chinh.
				if (SSelectLocation == STotalCatalog)
				{
					(STotalCatalog == 4) ? SSelectLocation = 1 : SSelectLocation = 0;
				}
				else
				{
					SSelectLocation += 1;
				}
				drawMainMenu(SSelectLocation);
				break;
			case 2: // Menu chon cap do.
				if (STotalCatalog == 4)
				{
					if (SSelectLocation == STotalCatalog - 1)
					{
						SSelectLocation = 0;
					}
					else
					{
						SSelectLocation += 1;
					}

					drawLevelMenu(SSelectLocation);
				}
				break;
			case 3: // Trang choi game.
				if (BPlayGameStatus)
				{
					BUseKeyboard = true;
					CCurLocation.Y = ((CCurLocation.Y == Table.SRow - 1) ? 0 : CCurLocation.Y + 1);
					drawTable();
				}
				break;
			case 4: // Trang thua.
				drawPlayGameStatus(3, 3, (SSelectLocation == 0) ? 1 : 0);
				break;
			case 5: // Trang thang.
				drawPlayGameStatus(2, 2, (SSelectLocation == 0) ? 1 : 0);
				break;
			case 6: // Trang luu lai
				drawPlayGameStatus(1, 1, (SSelectLocation == 1) ? 0 : 1);
				break;
			}
			break;
		case VK_LEFT: // Mui ten trai.
			if (BPlayGameStatus)
			{
				BUseKeyboard = true;
				CCurLocation.X = ((CCurLocation.X == 0) ? Table.SCol - 1 : CCurLocation.X - 1);
				drawTable();
			}
			break;
		case VK_RIGHT: // Mui ten phai.
			if (BPlayGameStatus)
			{
				BUseKeyboard = true;
				CCurLocation.X = ((CCurLocation.X == Table.SCol - 1) ? 0 : CCurLocation.X + 1);
				drawTable();
			}
			break;
		case VK_RETURN: // Phim Enter.
			switch (SPages)
			{
			case 1: // Menu chinh.
				if (SSelectLocation == 0)
				{

				}
				else if (SSelectLocation == 1)
				{
					SPages = 2;
					deleteRow(4, 5);
					drawLevelMenu(0);
				}
				else if (SSelectLocation == 2) // Trang bang diem.
				{
					SPages = 7;
					deleteRow(4, 10);
					drawHighScore(0);
				}
				else if (SSelectLocation == 3) // Trang thong tin.
				{
					SPages = 8;
					deleteRow(4, 10);
					thongTin(0);
				}
				else
				{
					exit(0);
				}
				break;
			case 2: // Menu chon cap do.
				if (SSelectLocation == 0) // Muc de 9 * 9 va 10 bom.
				{
					SPages = 3; // Cap nhat lai la dang choi game.
					deleteRow(4, 10);
					init(9, 9, 10);
				}
				else if (SSelectLocation == 1) // Muc trung binh 16 * 16 va 40 bom.
				{
					SPages = 3; // Cap nhat lai la dang choi game.
					deleteRow(4, 7);
					init(16, 16, 40);
				}
				else if (SSelectLocation == 2) // Muc kho 24 * 24 va 99 bom.
				{
					SPages = 3; // Cap nhat lai la dang choi game.
					deleteRow(4, 10);
					init(24, 24, 99);
				}
				else
				{
					SPages = 1; // Cap nhat lai la menu chinh.
					deleteRow(4, 10);
					drawMainMenu(0);
				}
				break;
			case 4: // Trang thua.
				if (SSelectLocation)
				{
					SPages = 1; // Tro ve menu chinh.
					deleteRow(3, ConsoleHeight - 3);
					drawMainMenu(0);
				}
				else
				{
					SPages = 3; // Trang choi game.
					deleteRow(3, ConsoleHeight - 3);
					init(Table.SRow, Table.SCol, Table.SMineCount);
				}
				break;
			case 5: // Trang thang.

				if (SSelectLocation)
				{
					SPages = 1; // Tro ve menu chinh.
					deleteRow(3, ConsoleHeight - 3);
					drawMainMenu(1);
				}
				else
				{
					luuDiem();
					SPages = 1; // Tro ve menu chinh.
					deleteRow(3, ConsoleHeight - 3);
					drawMainMenu(1);
				}
				break;

			case 6: // Trang luu lai.
				if (SSelectLocation)
				{
					SPages = 1; // Tro ve menu chinh.
					deleteRow(3, ConsoleHeight - 3);
					drawMainMenu(1);
				}
				else
				{
					// Luu game -> Xu file.
				}
				break;
			case 7:
				if (SSelectLocation == 0)
				{
					SPages = 1;
					deleteRow(4, ConsoleHeight - 3);
					drawMainMenu(1);
					break;
				}
				break;
			case 8:
				if (SSelectLocation == 0)
				{
					SPages = 1;
					deleteRow(4, ConsoleHeight - 3);
					drawMainMenu(1);
					break;
				}
				break;
			}
			break;

		case VK_ESCAPE: // Phim ESC(thoat).
			switch (SPages)
			{
			case 1: // Menu chinh.
				exit(0);
				break;
			case 2: // Menu chon cap do.
				SPages = 1; // Cap nhat lai thanh trang menu chinh.
				deleteRow(4, 10);
				drawMainMenu(0);
				break;
			case 3: // Dang choi game.
				if (BPlayGameStatus)
				{
					BPlayGameStatus = false;
					SPages = 1;
					deleteRow(3, ConsoleHeight - 3);
					//drawPlayGameStatus(1, 1, 0);
					Table.STime = 0;
					drawMainMenu(1);
				}
				break;
			case 4: // Trang thua.
			case 5: // Trang thang.
				SPages = 2;
				deleteRow(3, ConsoleHeight - 3);
				drawLevelMenu(0);
				break;
			case 6: // Trang luu lai.
				if (!BPlayGameStatus)
				{
					BPlayGameStatus = false;
					SPages = 5;

					deleteRow(3, 2);
					drawPlayGameStatus(1, 0, 0);
				}
				break;
			case 7:
				SPages = 1; // Cap nhat lai thanh trang menu chinh.
				deleteRow(4, ConsoleHeight - 3);
				drawMainMenu(1);
				break;
			case 8:
				SPages = 1; // Cap nhat lai thanh trang menu chinh.
				deleteRow(4, ConsoleHeight - 3);
				drawMainMenu(1);
				break;
			} break;

		case ClickLeft: // Phim Z - Mo O.
			if (SPages == 3 && BPlayGameStatus)
				clickLeft(CCurLocation.Y, CCurLocation.X);
			break;
		case ClickRight: // Phim X - Cam Co.
			if (SPages == 3 && BPlayGameStatus)
				clickRight(CCurLocation.Y, CCurLocation.X);
			break;

		case Suggest: // phim S - goi y
			if (SPages == 3 && BPlayGameStatus)
				suggest();
			break;
		}
	}
}

void eventProcessing()
{
	while (1)
	{
		DWORD DWNumberOfEvents = 0; // Luu lai su kien hien tai.
		DWORD DWNumberOfEventsRead = 0; // Luu lai so luong su kien da duoc loc.

		HANDLE HConsoleInput = GetStdHandle(STD_INPUT_HANDLE); // Thiet bi dau vao.
		GetNumberOfConsoleInputEvents(HConsoleInput, &DWNumberOfEvents); // Dat su kien dau vao cua giao dien cho bien DWNumberOfEvents.

		if (DWNumberOfEvents)
		{
			INPUT_RECORD* IREventBuffer = new INPUT_RECORD[DWNumberOfEvents]; // Con tro EventBuffer.
			ReadConsoleInput(HConsoleInput, IREventBuffer, DWNumberOfEvents, &DWNumberOfEventsRead); // Dat cac su kien duoc luu tru vao con EventBuffer.

			// Chay vong lap de doc su kien.
			for (DWORD i = 0; i < DWNumberOfEvents; ++i)
			{
				if (IREventBuffer[i].EventType == KEY_EVENT) // Neu la su kien phim.
				{
					keyboardProcessing(IREventBuffer[i].Event.KeyEvent);
				}
				//else if (IREventBuffer[i].EventType == MOUSE_EVENT) // Su kien chuot.
			}
		}

		if (BPlayGameStatus && ((Table.SOpenOCount > 0) || (Table.SFlagCount > 0)))
		{
			int ITemp = ITime + 1000;
			if (GetTickCount64() > ITemp)
			{
				ITime = GetTickCount64();
				Table.STime++;

				drawPlayGameStatus(1, 0, 0);
			}
		}
	}
}

// Design
void drawGameTitle()
{
	short i;
	for (i = 0; i < ConsoleWidth; ++i)
	{
		printf("%c", 45);
	}
	LPSTR StrTitle = (char*)"Game Do Min - Nhom 1 CPP\n";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTitle) / 2), 1, 5, 0, StrTitle);
	setColor(7);
	for (i = 0; i < ConsoleWidth; ++i)
	{
		printf("%c", 45);
	}
}

void drawBox(short SX, short SY, short STypes)
{
	switch (STypes)
	{
	case 0: // Rong mau xanh la.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 9, 15, (char*)"  ");
		break;
	case 1: // So 1 xanh duong. So 1 -> 8 la nen trang.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 9, 15, (char*)"1 ");
		break;
	case 2: // So 2 xanh la.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 2, 15, (char*)"2 ");
		break;
	case 3: // So 3 do.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 12, 15, (char*)"3 ");
		break;
	case 4: // So 4 xanh duong dam.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 1, 15, (char*)"4 ");
		break;
	case 5: // So 5 do dam.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 4, 15, (char*)"5 ");
		break;
	case 6: // So 6 CYAN dam.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 3, 15, (char*)"6 ");
		break;
	case 7: // So 7 den
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 0, 15, (char*)"7 ");
		break;
	case 8: // So 8 hong.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 13, 15, (char*)"8 ");
		break;
	case 9: // Bom. Nen do, chu den.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 0, 12, (char*)"B ");
		break;
	case 10: // O chan.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 0, 8, (char*)"  ");
		break;
	case 11: // O le.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 0, 7, (char*)"  ");
		break;
	case 12: // Theo doi con tro.
		setBackgroundColorTextXY(xCoord(SX) + 1, yCoord(SY), 0, 13, (char*)" ");
		break;
	case 13: // Cam co. Nen vang nhat, chu do.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 12, 14, (char*)"P ");
		break;
	case 14: // Cam co khong co bom => cam co sai. Nen cam, chu trang.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 15, 6, (char*)"Px");
		break;
	case 15: // Cam co co bom => cam co dung. Nen vang nhat, chu do.
		setBackgroundColorTextXY(xCoord(SX), yCoord(SY), 12, 14, (char*)"B ");
		break;
	}
}

void drawTable()
{
	for (int i = 0; i < Table.SRow; ++i)
	{
		for (int j = 0; j < Table.SCol; ++j)
		{
			/*// Cach 1:
			if (((i % 2) && (j % 2)) || !((i % 2) || (j % 2)))
			{
				drawBox(j, i, 10);
			}
			else
			{
				drawBox(j, i, 11);
			}*/

			// Cach 2:
			if (Box[i][j].BFlag)
				drawBox(j, i, 13);
			else if (Box[i][j].SNeighborMine)
				drawBox(j, i, Box[i][j].SNeighborMine);
			else if (Box[i][j].BOpened) // O rong.
				drawBox(j, i, 0);
			else if ((i + j) % 2) // O le.
				drawBox(j, i, 11);
			else // O chan.
				drawBox(j, i, 10);

			if (BUseKeyboard)
				drawBox(CCurLocation.X, CCurLocation.Y, 12);
		}
	}
}

void drawPlayGameStatus(short SStatus, short SOptions, short SIndex)
{
	SSelectLocation = SIndex;
	STotalCatalog = 2;

	setBackgroundColorTextXY(1, 3, 15, 0, (char*)"Ban Do: %d * %d", Table.SRow, Table.SCol);
	setBackgroundColorTextXY(1, 4, 15, 0, (char*)"So Bom: %d", Table.SMineCount - Table.SFlagCount);
	setBackgroundColorTextXY(ConsoleWidth - 15, 3, 15, 0, (char*)"Thoi Gian: %d", Table.STime);

	// Ve menu thang thua.
	LPSTR StrTextOptionMenu;
	if (SOptions == 1)
	{
		StrTextOptionMenu = (char*)"  Luu Lai  ";
		setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextOptionMenu) / 2), 3, 15, ((SIndex == 0) ? 2 : 0), StrTextOptionMenu);
	}
	if (SOptions == 2)
	{
		StrTextOptionMenu = (char*)"  Luu Diem  ";
		setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextOptionMenu) / 2), 3, 15, ((SIndex == 0) ? 2 : 0), StrTextOptionMenu);
	}
	if (SOptions == 3)
	{
		StrTextOptionMenu = (char*)"  Choi Lai  ";
		setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextOptionMenu) / 2) + 1, 3, 15, ((SIndex == 0) ? 2 : 0), StrTextOptionMenu);
	}

	if (SOptions >= 1)
	{
		StrTextOptionMenu = (char*)"  Thoat  ";
		setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextOptionMenu) / 2), 4, 15, ((SIndex == 1) ? 2 : 0), StrTextOptionMenu);
	}

	// Ve text trang thai.
	if (SStatus == 1) // 1 Dang choi game.
	{
		setBackgroundColorTextXY(ConsoleWidth - 22, 4, 15, 0, (char*)"Trang Thai: %s", "Dang Choi");
	}
	if (SStatus == 2) // 2 win.
	{
		setBackgroundColorTextXY(ConsoleWidth - 22, 4, 14, 0, (char*)"Trang Thai: %s", "Thang");
	}
	if (SStatus == 3) // 3 Thua
	{
		setBackgroundColorTextXY(ConsoleWidth - 22, 4, 12, 0, (char*)"Trang Thai: %s", "Thua");
	}
	std::cout << "\n";
	setColor(7);
	short i;
	for (i = 0; i < ConsoleWidth; ++i)
	{
		printf("%c", 45);
	}
}

void drawMainMenu(short SIndex)
{
	// Cap nhat lai vi tri dang chon va tong muc cua menu.
	SSelectLocation = SIndex;
	STotalCatalog = 4;

	/*if (std::ifstream("luu.kdn"))
	{
		setBackgroundColorTextXY((ConsoleWidth / 2) - 7, 7, 15, ((SIndex == 0) ? 2 : 0), (char*)"  TIEP TUC  ");
		++STotalCatalog;
	}*/

	// Ve menu.
	LPSTR StrTextMainMenu = (char*)"  GAME MOI  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextMainMenu) / 2), 7, 15, ((SIndex == 1) ? 2 : 0), StrTextMainMenu);

	StrTextMainMenu = (char*)"  BANG DIEM  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextMainMenu) / 2), 8, 15, ((SIndex == 2) ? 2 : 0), StrTextMainMenu);

	StrTextMainMenu = (char*)"  THONG TIN  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextMainMenu) / 2), 9, 15, ((SIndex == 3) ? 2 : 0), StrTextMainMenu);

	StrTextMainMenu = (char*)"  THOAT  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextMainMenu) / 2), 10, 15, ((SIndex == 4) ? 2 : 0), StrTextMainMenu);
}

void drawLevelMenu(short SIndex)
{
	// Cap nhat lai vi tri dang chon va tong muc cua menu.
	SSelectLocation = SIndex;
	STotalCatalog = 4;

	LPSTR StrTextLevelMenu = (char*)"CHON CAP DO";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextLevelMenu) / 2), 4, 1, 0, StrTextLevelMenu);

	// Ve menu.
	StrTextLevelMenu = (char*)"  DE (9 * 9 VA 10 BOM)  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextLevelMenu) / 2), 7, 15, ((SIndex == 0) ? 2 : 0), StrTextLevelMenu);

	StrTextLevelMenu = (char*)" TRUNG BINH (16 * 16 VA 40 BOM)  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextLevelMenu) / 2), 8, 15, ((SIndex == 1) ? 2 : 0), StrTextLevelMenu);

	StrTextLevelMenu = (char*)" KHO (24 * 24 VA 99 BOM)  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextLevelMenu) / 2), 9, 15, ((SIndex == 2) ? 2 : 0), StrTextLevelMenu);

	StrTextLevelMenu = (char*)"  QUAY LAI  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextLevelMenu) / 2), 10, 15, ((SIndex == 3) ? 2 : 0), StrTextLevelMenu);
}

void drawHighScore(short SIndex)
{
	vector<string> lines9;
	vector<string> lines16;
	vector<string> lines24;
	string line9x9, line16x16, line24x24;

	short rank = 0;

	ifstream ifile_9;
	ifile_9.open("file9x9.txt");
	while (getline(ifile_9, line9x9)) 
	{
		lines9.push_back(line9x9);
	}
	ifile_9.close();

	ifstream ifile_16;
	ifile_16.open("file16x16.txt");
	while(getline(ifile_16, line16x16))
	{
		lines16.push_back(line16x16);
	}
	ifile_16.close();

	ifstream ifile_24;
	ifile_24.open("file_24x24.txt");
	while (getline(ifile_24, line24x24))
	{
		lines24.push_back(line24x24);
	}
	ifile_24.close();


	cout << "\n\n                               HIGH SCORE";
	cout << "\n\n\n\n   EASY (time in seconds):  ";
	for (const auto& i : lines9)
	{
		cout << (rank+1) <<"-"<<i << "s" << "  ";
		rank++;
	}
	rank = 0;
	cout << "\n\n   MEDIUM (time in seconds): ";
	for (const auto& i : lines16)
	{
		cout << (rank + 1) << "-" << i << "s" << "  ";
		rank++;
	}
	rank = 0;
	cout << "\n\n   HARD (time in seconds):  ";
	for (const auto& i : lines24)
	{
		cout << (rank + 1) << "-" << i << "s" << "  ";
		rank++;
	}
	cout << "\n\n\n";

	SSelectLocation = SIndex;
	STotalCatalog = 1;
	LPSTR StrTextHighScore = (char*)"  BACK  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextHighScore) / 2), 17, 15, ((SIndex == 0) ? 2 : 0), StrTextHighScore);
}
void luuDiem() {
	short time = Table.STime;
	short level = Table.SCol;
	vector<short> arr;
	ifstream read;
	if (level == 9)
	{
		read.open("file9x9.txt");
	}
	else if (level == 16)
	{
		read.open("file16x16.txt");
	}
	else
	{
		read.open("file24x24.txt");
	}
	
	if (read.is_open())
	{
		short i = -1;
		while (read >> i)
		{
			if (i == -1) break;
			arr.push_back(i);
		}
	}
	arr.push_back(time);
	sort(arr.begin(), arr.end());
	read.close();

	ofstream write;
	if (level == 9)
	{
		write.open("file9x9.txt", ios::trunc);
	}
	else if (level == 16)
	{
		write.open("file16x16.txt", ios::trunc);
	}
	else
	{
		write.open("file24x24.txt", ios::trunc);
	}


	if (write.is_open())
	{
		short size = arr.size();
		short j = 0;
		while (j < size)
		{
			write << arr[j];
			write << "\n";
			++j;
		}
	}
	
	write.close();
}

void thongTin(short SIndex) 
{
	// Khai báo vector để lưu các dòng đọc được
	vector<string> lines;
	string line;

	//Mở file bằng ifstream
	ifstream input_file;
	input_file.open("HD.txt");
	//Kiểm tra file đã mở thành công chưa
	if (!input_file.is_open()) {
		cout << "Could not open the file "<< endl;
		//return 0;
	}

	//Đọc từng dòng trong
	while (getline(input_file, line)) {
		lines.push_back(line);//Lưu từng dòng như một phần tử vào vector lines.
	}

	//Đóng file
	input_file.close();

	//Xuất từng dòng từ lines và in ra màn hình
	for (const auto& i : lines)
		cout << i << endl;

	SSelectLocation = SIndex;
	STotalCatalog = 1;
	LPSTR StrTextHighScore = (char*)"  BACK  ";
	setBackgroundColorTextXY((ConsoleWidth / 2) - (strlen(StrTextHighScore) / 2), 25, 15, ((SIndex == 0) ? 2 : 0), StrTextHighScore);
	
}



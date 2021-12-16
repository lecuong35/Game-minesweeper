#include "main.h"
#include "Console.h"
#include "functions.h"

// Trang, Vi tri dang chon, Tong muc.
short SPages, SSelectLocation, STotalCatalog;

int main()
{
	try
	{
		consoleResize(ConsoleWidth, ConsoleHeight); // Thay doi kich thuoc man hinh console.

		SetConsoleTitle(TEXT("Game Do Min - Nhom 1 CPP")); // Dat tieu do game.

		LoadIcon(NULL, (LPCWSTR)MAKEINTRESOURCEA(IDI_ICON1)); // Thay doi icon .exe

		drawGameTitle();

		cursor(false); // An con tro.
		drawMainMenu(1);
		SPages = 1;
		eventProcessing();
	}
	catch (const std::exception& exp)
	{
		int iImages[] = { 1, 2, 3, 4, 5 };
		std::cout << "Error.\n";
	}

	return 0;
}

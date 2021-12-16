#pragma once
#include <iostream>
#include <Windows.h>
#include <Shlwapi.h>
#include <conio.h>

void consoleResize(SHORT width, SHORT height); // resize 2d table
void clrscr(void);                             // clear table 
void gotoXY(SHORT x, SHORT y);                 // move cursor 
SHORT whereX();                                // define cursor
SHORT whereY();
void setColor(WORD color);
void setBackgroundColor(WORD color);
void setBackgroundColorTextXY(SHORT x, SHORT y, WORD color, WORD background, LPSTR str, ...);
void cursor(BOOL bVisible = TRUE, DWORD dwSize = 25);
void deleteRow(SHORT SStartPos, SHORT SNumberRow);

/*

TextEdition - A C library for the creation of text boxes with SDL
Copyright (C) 2013 Cokie (cokie.forever@gmail.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef TEXTEDITION

#define TEXTEDITION
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include "ttf_secure.h"  //You can disable this inclusion if you don't intend to use multi-threading.


#define TE_STYLE_NORMAL         0x00000000
#define TE_STYLE_MULTILINE      0x00000001
#define TE_STYLE_VSCROLL        0x00000011
#define TE_STYLE_HSCROLL        0x00000100
#define TE_STYLE_AUTOJUMP       0x00001001
#define TE_STYLE_READONLY       0x00010000
#define TE_STYLE_JUSTDISPLAY    0x00110000
#define TE_STYLE_BLITRGBA       0x01000000

#define TE_BLITSTYLE_BLENDED    1
#define TE_BLITSTYLE_SHADED     2
#define TE_BLITSTYLE_SOLID      3

#define TE_NO_ACCESS            0x00000000
#define TE_ACCESS_READ          0x00000001
#define TE_ACCESS_WRITE         0x00000010
#define TE_FULL_ACCESS          0x00000011


typedef struct
{
    char c;
    int x,y;

    char prevC;
    int prevX,prevY,prevState;
} TE_CharInfo;

typedef struct
{
    int w, h;
} TE_Dimension;

typedef struct
{
    int begin, end;
} TE_Interval;

typedef struct
{
    //standard
    char *text;
    SDL_Rect pos;
    TTF_Font *font;
    SDL_Color colorFG;
    int style;

    //advanced
    int blitStyle, hSpace;
    SDL_Color colorBG, colorFGSelect, colorBGSelect;
    SDL_Rect blitSurfPos;
    SDL_Surface *blitSurf;

    //system
    TE_CharInfo **tab;
    TE_Dimension dimTab[256];
    int cursorPos, prevCursorPos, mouseStatus, numLines, lastLine, fontHeight, textLength;
    int offsetX, offsetY, xmax;
    int focus, insert, HSBCaught, VSBCaught, anchorHSB, anchorVSB;
    Uint32 idWriterThread;
    int nbReaders;
    TE_Interval selection;
    SDL_Surface *tmpSurf, *tmpSurfSave, *HScrollBar, *VScrollBar;
    SDL_Rect posHSB, posVSB;
} TextEdition;


int TE_Init(void);
void TE_Quit(void);

int TE_NewTextEdition(TextEdition *te, int length, SDL_Rect pos, TTF_Font *font, SDL_Color colorFG, int style);
int TE_DeleteTextEdition(TextEdition *te);
int TE_UpdateTextEdition(TextEdition *te, int i);
int TE_DisplayTextEdition(TextEdition *te);
int TE_HoldTextEdition(TextEdition *te, SDL_Event event);

SDL_Surface* TE_RenderText(const char text[], TextEdition te, int inverted);
int TE_SetEditionText(TextEdition *te, const char text[]);
int TE_SetFocusEdition(TextEdition *te, int focus);
int TE_GetFocusEdition(TextEdition *te);
int TE_SetCursorPos(TextEdition *te, int cursorPos);
int TE_GetCursorPos(TextEdition *te);

int TE_LockEdition(TextEdition *te, Uint32 access);
int TE_UnlockEdition(TextEdition *te);

#endif

#ifndef TEXTEDITION

#define TEXTEDITION
#include <stdlib.h>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"


#if SDL_BYTEORDER == SDL_BIG_ENDIAN

#define RED_MASK   0xFF000000
#define GREEN_MASK 0x00FF0000
#define BLUE_MASK  0x0000FF00
#define ALPHA_MASK 0x000000FF

#else

#define RED_MASK   0x000000FF
#define GREEN_MASK 0x0000FF00
#define BLUE_MASK  0x00FF0000
#define ALPHA_MASK 0xFF000000

#endif


#define TE_STYLE_NORMAL         0x0000000
#define TE_STYLE_MULTILINE      0x0000001
#define TE_STYLE_VSCROLL        0x0000011
#define TE_STYLE_HSCROLL        0x0000100
#define TE_STYLE_AUTOJUMP       0x0001001
#define TE_STYLE_READONLY       0x0010000
#define TE_STYLE_JUSTDISPLAY    0x0110000

#define HasMultilineStyle(s)    (((s)&TE_STYLE_MULTILINE)==TE_STYLE_MULTILINE)
#define HasVScrollStyle(s)      (((s)&TE_STYLE_VSCROLL)==TE_STYLE_VSCROLL)
#define HasHScrollStyle(s)      (((s)&TE_STYLE_HSCROLL)==TE_STYLE_HSCROLL)
#define HasAutoJumpStyle(s)     (((s)&TE_STYLE_AUTOJUMP)==TE_STYLE_AUTOJUMP)
#define HasReadOnlyStyle(s)     (((s)&TE_STYLE_READONLY)==TE_STYLE_READONLY)
#define HasJustDisplayStyle(s)  (((s)&TE_STYLE_JUSTDISPLAY)==TE_STYLE_JUSTDISPLAY)

#define TE_BLITSTYLE_BLENDED    1
#define TE_BLITSTYLE_SHADED     2
#define TE_BLITSTYLE_SOLID      3

#define INVALID_CHAR            '\r'
#define INVALID_PTR             0xFFFFFFF

#define IsCharOK(c)             ((c) != INVALID_CHAR && (c))
#define IsLineOK(te,l)          ((l) < (te).numLines && (l) <= (te).lastLine)

#define WidthChar(te,c)         (te).dimTab[(int)((c)+128)].w
#define HeigthChar(te,c)        (te).dimTab[(int)((c)+128)].h

#define HSBHeight(te)           ((te).HScrollBar ? (te).HScrollBar->h : 0)
#define VSBWidth(te)            ((te).VScrollBar ? (te).VScrollBar->w : 0)

#define IsInRect(pt,rect)       ((pt).x>=(rect).x && (pt).x<=(rect).x+(rect).w && (pt).y>=(rect).y && (pt).y<=(rect).y+(rect).h)
#define IsRectInRect(rIn,rExt)  ((rIn).x>=(rExt).x && (rIn).x+(rIn).w<=(rExt).x+(rExt).w && (rIn).y>=(rExt).y && (rIn).y+(rIn).h<=(rExt).y+(rExt).h)
#define IsInSelection(i,s)      (((i)>=(s).begin && (i)<(s).end) || ((i)<(s).begin && (i)>=(s).end))

#define AreSameColor(c1,c2)     ((c1).r==(c2).r && (c1).g==(c2).g && (c1).b==(c2).b)

#define Ceil(x)                 ((x)>=0 ? ceil(x) : -ceil(-(x)) )


typedef struct
{
    char c;
    int x,y;

    char prevC;
    int prevX,prevY,prevState;
} CharInfo;

typedef struct
{
    int w, h;
} Dimension;

typedef struct
{
    int begin, end;
} Interval;

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
    CharInfo **tab;
    Dimension dimTab[256];
    int cursorPos, prevCursorPos, mouseStatus, numLines, lastLine, fontHeight, textLength;
    int offsetX, offsetY, xmax;
    int focus, insert, HSBCaught, VSBCaught, anchorHSB, anchorVSB;
    Interval selection;
    SDL_Surface *tmpSurf, *tmpSurfSave, *HScrollBar, *VScrollBar;
    SDL_Rect posHSB, posVSB;
} TextEdition;

int TE_Init(void);
void TE_Quit(void);

int TE_NewTextEdition(TextEdition *te, int length, SDL_Rect pos, TTF_Font *font, SDL_Color colorFG, int style);
void TE_DeleteTextEdition(TextEdition *te);
int TE_UpdateTextEdition(TextEdition *te, int i);
int TE_DisplayTextEdition(TextEdition *te);
int TE_HoldTextEdition(TextEdition *te, SDL_Event event);

SDL_Surface* TE_RenderText(const char text[], TextEdition te, int inverted);
int TE_SetEditionText(TextEdition *te, const char text[]);
int TE_SetFocusEdition(TextEdition *te, int focus);
int TE_GetFocusEdition(TextEdition *te);
int TE_SetCursorPos(TextEdition *te, int cursorPos);
int TE_GetCursorPos(TextEdition *te);

#endif

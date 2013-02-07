#include "textedition.h"

int DisplayCursor(TextEdition te);
int DisplayScrollBars(TextEdition *te);
int SetHSBFromOffset(TextEdition *te);
int SetVSBFromOffset(TextEdition *te);
int SetOffsetFromHSB(TextEdition *te);
int SetOffsetFromVSB(TextEdition *te);
int HoldKeyPressing(TextEdition *te, SDL_Event event);
int HoldKeyPressing_Ctrl(TextEdition *te, SDL_Event event);
int HoldCursorPosition(TextEdition *te, SDL_Event event);
int HoldCursorPosition_Keyboard(TextEdition *te, SDL_Event event, int *l, int *c);
int OffsetCorrection(TextEdition *te);
int WordHead(const char text[], int i);
int LineLength(CharInfo line[]);
void GetPositionInEdition(TextEdition te, int i, int *l, int *c);
int GetPositionInText(TextEdition te, int l, int c);
int GetLineFromYPosition(TextEdition te, int y);
int GetCharFromXPosition(TextEdition te, int l, int x);
int NewLetter(CharInfo **tab, int l, int c);
int NewLine(TextEdition *te, int l);
int DimLetter(TextEdition te, char c, int *w, int *h);
int WidthWord(TextEdition te, int i);
SDL_Color ColorInverse(SDL_Color color);
int DeleteChar(char text[], unsigned int pos);
int InsertChar(char text[], unsigned int pos, char c, unsigned int length);
int InsertString(char text[], unsigned int pos, char string[], unsigned int lengthMax);
int DeleteSelection(TextEdition *te);
SDL_Cursor* CreateCursor(const char *image[]);
int SetHSBLength(TextEdition *te, int length);
int SetVSBLength(TextEdition *te, int length);
int IsMouseOverHSB(TextEdition te, int x, int y);
int IsMouseOverVSB(TextEdition te, int x, int y);
int DeleteAllChars(char text[], char c);

int TE_initialized = 0;
SDL_Cursor *TE_EditionCursor = NULL,
           *TE_NormalCursor = NULL;
char *TE_clipBoard = NULL;



int TE_NewTextEdition(TextEdition *te, int length, SDL_Rect pos, TTF_Font *font, SDL_Color colorFG, int style)
{
    int w, h, i;
    SDL_Rect pos2;

    if (!TE_initialized)
        TE_Init();

    if (!te || !font || length <= 0)
        return 0;

    te->textLength = length;
    te->text = malloc(sizeof(char)*(length+1));
    if (!te->text)
        return 0;
    te->text[0] = '\0';
    te->pos = pos;
    te->font = font;
    te->colorFG = colorFG;
    te->style = style;


    te->HScrollBar = NULL;
    te->VScrollBar = NULL;
    SetHSBLength(te, 5);
    SetVSBLength(te, 5);

    if (te->pos.x < 0)
        te->pos.x = 0;
    if (te->pos.y < 0)
        te->pos.y = 0;
    if (te->pos.w < 20)
        te->pos.w = 20;
    if (te->pos.h < (te->fontHeight=TTF_FontHeight(font)))
        te->pos.h = te->fontHeight;
    if (te->blitStyle > 3 || te->blitStyle < 1)
        te->blitStyle = TE_BLITSTYLE_BLENDED;
    if (te->hSpace <= 0)
        te->hSpace = TTF_FontLineSkip(font);
    if (!HasMultilineStyle(style))
        te->pos.h = te->hSpace + HSBHeight(*te);
    if (AreSameColor(te->colorBG,te->colorFG))
    {
        te->colorBG = ColorInverse(te->colorFG);
        te->colorBGSelect = te->colorFG;
        te->colorFGSelect = te->colorBG;
    }
    if (!te->blitSurf)
        te->blitSurf = SDL_GetVideoSurface();
    /*te->blitSurfPos.x = 0;
    te->blitSurfPos.y = 0;*/

    te->tab = malloc(sizeof(CharInfo*));
    te->tab[0] = malloc(sizeof(CharInfo));
    memset(&(te->tab[0][0]),0,sizeof(CharInfo));
    te->numLines = 1;
    te->lastLine = 0;
    te->offsetX = 0;
    te->offsetY = 0;
    te->focus = 0;
    te->insert = 1;
    te->prevCursorPos = -1;
    te->mouseStatus = 0;

    te->HSBCaught = 0;
    te->VSBCaught = 0;

    for (i=1 ; i<256 ; i++)
    {
        DimLetter(*te, (char)(i-128), &w, &h);
        te->dimTab[i].w = w;
        te->dimTab[i].h = h;
    }

    WidthChar(*te, '\0') = 0;
    HeigthChar(*te, '\0') = te->fontHeight;
    WidthChar(*te, '\n') = WidthChar(*te, ' ');
    HeigthChar(*te, '\n') = HeigthChar(*te, ' ');
    WidthChar(*te, INVALID_CHAR) = 0;
    HeigthChar(*te, INVALID_CHAR) = te->fontHeight;

    pos.x = 0;
    pos.y = 0;
    pos2 = te->pos;
    te->tmpSurf = SDL_CreateRGBSurface(SDL_HWSURFACE, pos2.w, pos2.h, 32, 0,0,0,0);
    SDL_BlitSurface(te->blitSurf, &pos2, te->tmpSurf, &pos);
    te->tmpSurfSave = SDL_CreateRGBSurface(SDL_HWSURFACE, pos2.w, pos2.h, 32, 0,0,0,0);
    SDL_BlitSurface(te->blitSurf, &pos2, te->tmpSurfSave, &pos);

    return 1;
}

void TE_DeleteTextEdition(TextEdition *te)
{
    int l;

    if (te->text)
        free(te->text);
    te->text = NULL,
    te->textLength = 0;

    if (te->blitSurf && te->blitSurf != SDL_GetVideoSurface())
        SDL_FreeSurface(te->blitSurf);
    te->blitSurf = NULL;

    if (te->tab)
    {
        for (l=0 ; l<te->numLines ; l++)
        {
            if (te->tab[l])
                free(te->tab[l]);
        }

        free(te->tab);
        te->tab = NULL;
    }

    if (te->tmpSurf)
        SDL_FreeSurface(te->tmpSurf);
    te->tmpSurf = NULL;
    if (te->tmpSurfSave)
        SDL_FreeSurface(te->tmpSurfSave);
    te->tmpSurfSave = NULL;

    if (te->HScrollBar)
        SDL_FreeSurface(te->HScrollBar);
    te->HScrollBar = NULL;
    if (te->VScrollBar)
        SDL_FreeSurface(te->VScrollBar);
    te->VScrollBar = NULL;
}


int TE_UpdateTextEdition(TextEdition *te, int i)
{
    int c, l, x, y, done=0;

    i = WordHead(te->text, i-1);
    GetPositionInEdition(*te, i, &l, &c);
    x = te->tab[l][c].x;
    y = te->tab[l][c].y;

    te->xmax = 0;

    for(; te->text[i] && !done ; i++)
    {
        if (!te->tab[l][c].c)
            NewLetter(te->tab, l, c);
        te->tab[l][c].c = te->text[i];
        te->tab[l][c].x = x;
        te->tab[l][c].y = y;
        x += WidthChar(*te,te->text[i]);
        c++;

        if (te->xmax < x)
            te->xmax = x;

        if (HasMultilineStyle(te->style))
        {
            if (  (te->text[i] == '\n' || (c>0 && HasAutoJumpStyle(te->style) && (te->text[i] == '-' || te->text[i] == ' ') && WidthWord(*te, i+1) >= te->pos.w-x-VSBWidth(*te)))
                || (WidthChar(*te,te->text[i+1]) >= te->pos.w-x && HasAutoJumpStyle(te->style)) )
            {
                te->tab[l][c].x = x;
                te->tab[l][c].y = y;
                for (; te->tab[l][c].c ; c++)
                    te->tab[l][c].c = INVALID_CHAR;

                if (++l >= te->numLines)
                    NewLine(te, l);
                x = 0;
                y = l*te->hSpace;
                c = 0;
            }
        }
    }

    te->tab[l][c].x = x;
    te->tab[l][c].y = y;
    te->lastLine = l;
    for (; l<te->numLines ; l++)
    {
        for (; te->tab[l][c].c ; c++)
            te->tab[l][c].c = INVALID_CHAR;
        c=0;
    }

    SetHSBFromOffset(te);
    SetVSBFromOffset(te);

    return 1;
}

int TE_DisplayTextEdition(TextEdition *te)
{
    int l, c, state, i=0, upY = -1, downY = -1;
    char buf[2] = "";
    SDL_Rect pos, rect = te->pos;
    SDL_Surface *surf;
    CharInfo *ci, *fci, *lci;

    rect.x = 0; rect.y = 0;
    rect.w -= VSBWidth(*te);
    rect.h -= HSBHeight(*te);

    for (l=0 ; IsLineOK(*te,l) ; l++)
    {
        fci = NULL; lci = NULL;
        for (c=0 ; IsCharOK(te->tab[l][c].c) ; c++)
        {
            ci = &(te->tab[l][c]);

            buf[0] = ci->c == '\n' ? ' ' : ci->c;
            pos.x = ci->x + te->offsetX;
            pos.y = ci->y + te->offsetY;

            pos.w = WidthChar(*te,buf[0]);
            pos.h = HeigthChar(*te,buf[0]);
            state = te->focus && IsInSelection(i,te->selection) && !HasJustDisplayStyle(te->style);

            if (IsRectInRect(pos, rect))
            {
                if (i == te->prevCursorPos || i == te->cursorPos || state != ci->prevState || pos.x != ci->prevX || pos.y != ci->prevY || buf[0] != ci->prevC)
                {
                    surf = TE_RenderText(buf, *te, state);
                    if (te->blitStyle != TE_BLITSTYLE_SHADED)
                        SDL_BlitSurface(te->tmpSurfSave, &pos, te->tmpSurf, &pos);
                    SDL_BlitSurface(surf, NULL, te->tmpSurf, &pos);
                    SDL_FreeSurface(surf);
                }

                lci = ci;
                if (!fci)
                    fci = ci;
            }

            ci->prevState = state;
            ci->prevX = pos.x;
            ci->prevY = pos.y;
            ci->prevC = buf[0];

            i++;
        }

        if (!lci)
            lci = &(te->tab[l][c]);
        else
        {
            downY = pos.y;
            if (upY < 0)
                upY = pos.y;
        }
        pos.x = lci->x + te->offsetX + WidthChar(*te,lci->c);
        pos.y = lci->y + te->offsetY;
        pos.h = te->hSpace;
        if ((pos.w = te->pos.w - pos.x)>0)
            SDL_BlitSurface(te->tmpSurfSave, &pos, te->tmpSurf, &pos);

        if (fci)
        {
            pos.x = 0;
            pos.y = fci->y + te->offsetY;
            pos.h = te->hSpace;
            if ((pos.w = fci->x + te->offsetX)>0)
                SDL_BlitSurface(te->tmpSurfSave, &pos, te->tmpSurf, &pos);
        }

        for (; te->tab[l][c].c ; c++)
            te->tab[l][c].prevC = INVALID_CHAR;
    }

    pos.y = downY + te->hSpace;
    pos.x = 0;
    pos.w = te->pos.w;
    if ((pos.h = te->pos.h - pos.y)>0)
        SDL_BlitSurface(te->tmpSurfSave, &pos, te->tmpSurf, &pos);

    pos.y = 0;
    pos.x = 0;
    pos.w = te->pos.w;
    if ((pos.h = upY)>0)
        SDL_BlitSurface(te->tmpSurfSave, &pos, te->tmpSurf, &pos);

    for (; l<te->numLines ; l++)
    {
        for (c=0 ; te->tab[l][c].c ; c++)
            te->tab[l][c].prevC = INVALID_CHAR;
    }

    if (!HasJustDisplayStyle(te->style))
        DisplayCursor(*te);
    te->prevCursorPos = te->cursorPos;

    DisplayScrollBars(te);

    SDL_BlitSurface(te->tmpSurf, NULL, te->blitSurf, &(te->pos));

    return 1;
}

int DisplayCursor(TextEdition te)
{
    int c,l;
    SDL_Rect pos, rect = te.pos;
    SDL_Surface *surf;

    if (!te.focus)
        return 0;

    rect.x = 0;
    rect.y = 0;
    rect.w -= VSBWidth(te);
    rect.h -= HSBHeight(te);
    GetPositionInEdition(te, te.cursorPos, &l, &c);

    pos.x = te.tab[l][c].x + te.offsetX;
    pos.y = te.tab[l][c].y + te.offsetY;
    pos.w = 1;
    pos.h = te.fontHeight;

    if (IsRectInRect(pos, rect))
    {
        surf = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, TTF_FontHeight(te.font), 32, 0,0,0,0);
        SDL_FillRect(surf, 0, SDL_MapRGB(surf->format, te.colorFG.r, te.colorFG.g, te.colorFG.b));
        SDL_BlitSurface(surf, NULL, te.tmpSurf, &pos);
        SDL_FreeSurface(surf);

        return 1;
    }
    else return 0;
}

int DisplayScrollBars(TextEdition *te)
{
    if (te->HScrollBar)
        SDL_BlitSurface(te->HScrollBar, NULL, te->tmpSurf, &(te->posHSB));
    if (te->VScrollBar)
        SDL_BlitSurface(te->VScrollBar, NULL, te->tmpSurf, &(te->posVSB));

    return 1;
}

int SetHSBFromOffset(TextEdition *te)
{
    int w = te->pos.w - VSBWidth(*te),
        length = te->xmax == 0 ? w : (w * te->pos.w) / te->xmax;

    if (te->offsetX > 0)
        te->offsetX = 0;
    if (te->xmax > w && te->offsetX < w - te->xmax)
        te->offsetX = w - te->xmax;

    if (length < w && SetHSBLength(te, length))
    {
        te->posHSB.y = te->pos.h - te->HScrollBar->h;
        te->posHSB.x = -((w - length) * te->offsetX) / (te->xmax - w);
        return 1;
    }
    else
    {
        if (te->HScrollBar)
            SDL_FreeSurface(te->HScrollBar);
        te->HScrollBar = NULL;
        return 0;
    }
}

int SetVSBFromOffset(TextEdition *te)
{
    int h = te->pos.h - HSBHeight(*te),
        length = (h * te->pos.h) / (te->tab[te->lastLine][0].y + te->hSpace);
    double z = te->tab[te->lastLine][0].y + te->hSpace - h;

    if (te->offsetY >= 0 || z <= 0)
        te->offsetY = 0;
    if (z > 0 && te->offsetY < (z = -(Ceil(z/te->hSpace)*te->hSpace)) )
        te->offsetY = z;

    if (length < h && SetVSBLength(te, length))
    {
        te->posVSB.y = ((h - length) * te->offsetY) / z;
        te->posVSB.x = te->pos.w - te->VScrollBar->w;
        return 1;
    }
    else
    {
        if (te->VScrollBar)
            SDL_FreeSurface(te->VScrollBar);
        te->VScrollBar = NULL;
        return 0;
    }
}

int SetOffsetFromHSB(TextEdition *te)
{
    int w = te->pos.w - VSBWidth(*te);

    if (!te->HScrollBar)
        return 0;

    if (te->posHSB.x < 0)
        te->posHSB.x = 0;
    if (te->posHSB.x > te->pos.w - VSBWidth(*te) - te->HScrollBar->w)
        te->posHSB.x = te->pos.w - VSBWidth(*te) - te->HScrollBar->w;

    te->offsetX = -(te->posHSB.x * (te->xmax - w)) / (w - te->HScrollBar->w);
    return 1;
}

int SetOffsetFromVSB(TextEdition *te)
{
    int h = te->pos.h - HSBHeight(*te);
    double z = te->tab[te->lastLine][0].y + te->hSpace - h;

    if (!te->VScrollBar)
        return 0;

    if (te->posVSB.y < 0)
        te->posVSB.y = 0;
    if (te->posVSB.y > te->pos.h - HSBHeight(*te) - te->VScrollBar->h)
        te->posVSB.y = te->pos.h - HSBHeight(*te) - te->VScrollBar->h;

    te->offsetY = -(te->posVSB.y * (Ceil(z/te->hSpace)*te->hSpace)) / (h - te->VScrollBar->h);
    te->offsetY = Ceil(te->offsetY*1.0/te->hSpace) * te->hSpace;
    return 1;
}


int TE_HoldTextEdition(TextEdition *te, SDL_Event event)
{
    int d1 = HoldCursorPosition(te, event),
        d2 = HoldKeyPressing(te, event);

    if (d1 || d2)
        OffsetCorrection(te);

    return 1;
}

int HoldKeyPressing(TextEdition *te, SDL_Event event)
{
    char c, done=1;
    int ctrl, num;
    Uint8 *keyState = SDL_GetKeyState(&num);

    if (event.type != SDL_KEYDOWN || !te->focus || HasJustDisplayStyle(te->style))
        return 0;

    ctrl = SDLK_LCTRL<num && keyState[SDLK_LCTRL];

    if (ctrl)
        done = HoldKeyPressing_Ctrl(te, event);
    else if (HasReadOnlyStyle(te->style))
        return 0;
    else
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_DELETE:
                if (te->selection.begin == te->selection.end)
                    DeleteChar(te->text, te->cursorPos);
                else DeleteSelection(te);
                TE_UpdateTextEdition(te, 0);
                break;
            case SDLK_BACKSPACE:
                if (te->selection.begin == te->selection.end)
                {
                    if (te->cursorPos > 0)
                        DeleteChar(te->text, -- te->cursorPos);
                }
                else DeleteSelection(te);
                TE_UpdateTextEdition(te, 0);
                break;
            case SDLK_INSERT:
                te->insert = !te->insert;
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                event.key.keysym.unicode = '\n';
            default:
                if (event.key.keysym.unicode)
                {
                    if (event.key.keysym.unicode<256)
                        c = event.key.keysym.unicode;
                    else c = '?';

                    if (te->selection.begin != te->selection.end)
                        DeleteSelection(te);
                    if (te->insert)
                    {
                        if (!InsertChar(te->text, te->cursorPos, c, te->textLength))
                        {
                            done = 0;
                            te->cursorPos--;
                        }
                    }
                    else te->text[te->cursorPos] = c;
                    TE_UpdateTextEdition(te, 0);
                    te->cursorPos++;
                }
                else done = 0;

                break;
        }
    }

    return done;
}

int HoldKeyPressing_Ctrl(TextEdition *te, SDL_Event event)
{
    int done=0;

    switch (event.key.keysym.sym)
    {
        case 'x':
        case 'c':
            if (te->selection.begin != te->selection.end)
            {
                if (TE_clipBoard)
                    free(TE_clipBoard);
                if ( (TE_clipBoard = malloc(sizeof(char)*(te->selection.end-te->selection.begin+1))) )
                {
                    strncpy(TE_clipBoard, te->text+te->selection.begin, te->selection.end-te->selection.begin);
                    TE_clipBoard[te->selection.end-te->selection.begin] = '\0';
                }

                if (event.key.keysym.sym == 'x' && !HasReadOnlyStyle(te->style))
                {
                    DeleteSelection(te);
                    done = 1;
                }
            }

            break;
        case 'v':
            if (TE_clipBoard && !HasReadOnlyStyle(te->style))
            {
                if (InsertString(te->text, te->cursorPos, TE_clipBoard, te->textLength))
                {
                    te->cursorPos += strlen(TE_clipBoard);
                    done = 1;
                }
            }
            break;
        case 'q':
            te->selection.begin = 0;
            te->selection.end = strlen(te->text);
        default:
            break;
    }

    if (done)
        TE_UpdateTextEdition(te, 0);
    return done;
}

int HoldCursorPosition(TextEdition *te, SDL_Event event)
{
    int l,c,x,y,done=1,num,select=0;
    Uint8 *keyState = SDL_GetKeyState(&num);
    SDL_Rect pt, rect = te->pos;

    rect.x += te->blitSurfPos.x;
    rect.y += te->blitSurfPos.y;
    rect.w -= VSBWidth(*te);
    rect.h -= HSBHeight(*te);

    select = SDLK_LSHIFT<num && keyState[SDLK_LSHIFT];

    switch(event.type)
    {
        case SDL_MOUSEMOTION:
            if ((SDL_GetMouseState(&x, &y)&SDL_BUTTON(SDL_BUTTON_LEFT)))
            {
                if (te->focus && !te->HSBCaught && !te->VSBCaught)
                {
                    l = GetLineFromYPosition(*te, y);
                    c = GetCharFromXPosition(*te, l, x);
                    select = 1;
                }
                else
                {
                    if (te->HSBCaught)
                    {
                        te->posHSB.x = x - te->anchorHSB - rect.x;
                        SetOffsetFromHSB(te);
                    }
                    else if (te->VSBCaught)
                    {
                        te->posVSB.y = y - te->anchorVSB - rect.y;
                        SetOffsetFromVSB(te);
                    }
                    done = 0;
                }
            }
            else
            {
                pt.x = x; pt.y = y;
                if (TE_EditionCursor && SDL_GetCursor() != TE_EditionCursor && IsInRect(pt,rect) && !HasJustDisplayStyle(te->style))
                {
                    SDL_SetCursor(TE_EditionCursor);
                    te->mouseStatus = 1;
                }
                else if ( TE_NormalCursor && SDL_GetCursor() != TE_NormalCursor && ((!IsInRect(pt,rect) && te->mouseStatus) || HasJustDisplayStyle(te->style)) )
                {
                    SDL_SetCursor(TE_NormalCursor);
                    te->mouseStatus = 0;
                }

                done=0;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if ( (te->focus = IsInRect(event.button,rect)) )
                {
                    l = GetLineFromYPosition(*te, event.button.y);
                    c = GetCharFromXPosition(*te, l, event.button.x);
                }
                else
                {
                    if (IsMouseOverHSB(*te,event.button.x,event.button.y))
                    {
                        te->HSBCaught = 1;
                        te->focus = 1;
                        te->anchorHSB = event.button.x - te->posHSB.x - rect.x;
                    }
                    else if (IsMouseOverVSB(*te,event.button.x,event.button.y))
                    {
                        te->VSBCaught = 1;
                        te->focus = 1;
                        te->anchorVSB = event.button.y - te->posVSB.y - rect.y;
                    }
                    done = 0;
                }
            }
            else
            {
                if (te->focus && event.button.button == SDL_BUTTON_WHEELDOWN)
                {
                    te->offsetY -= te->hSpace;
                    SetVSBFromOffset(te);
                }
                else if (te->focus && event.button.button == SDL_BUTTON_WHEELUP)
                {
                    te->offsetY += te->hSpace;
                    SetVSBFromOffset(te);
                }

                done = 0;
            }

            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                te->HSBCaught = 0;
                te->VSBCaught = 0;
            }
            done = 0;
            break;

        case SDL_KEYDOWN:
            done = HoldCursorPosition_Keyboard(te, event, &l, &c);
            break;

        default:
            done=0;
            break;
    }

    if (done)
    {
        te->cursorPos = GetPositionInText(*te, l, c);
        if (!select)
            te->selection.begin = te->cursorPos;
        te->selection.end = te->cursorPos;
    }

    return done;
}

int HoldCursorPosition_Keyboard(TextEdition *te, SDL_Event event, int *l2, int *c2)
{
    int l,c,done=1,ctrl,num;
    Uint8 *keyState = SDL_GetKeyState(&num);

    if (event.type != SDL_KEYDOWN || !te->focus)
        return 0;

    ctrl = SDLK_LCTRL<num && keyState[SDLK_LCTRL];

    GetPositionInEdition(*te, te->cursorPos, &l, &c);
    switch (event.key.keysym.sym)
    {
        case SDLK_DOWN:
            if (IsLineOK((*te),l+1))
            {
                c = GetCharFromXPosition(*te, l+1, te->tab[l][c].x + te->offsetX + te->pos.x + te->blitSurfPos.x);
                l++;
            }
            else c = LineLength(te->tab[l]);
            break;
        case SDLK_UP:
            if (l>0)
            {
                c = GetCharFromXPosition(*te, l-1, te->tab[l][c].x + te->offsetX + te->pos.x + te->blitSurfPos.x);
                l--;
            }
            else c = 0;
            break;
        case SDLK_RIGHT:
            if (!IsCharOK(te->tab[l][c+1].c) && IsLineOK((*te),l+1))
            {
                c = 0;
                l++;
            }
            else if (IsCharOK(te->tab[l][c].c))
            {
                c++;
                if (ctrl)
                    for(; IsCharOK(te->tab[l][c].c) && te->tab[l][c-1].c != ' ' ; c++);
            }
            break;
        case SDLK_LEFT:
            if (!c && l>0)
            {
                l--;
                c = LineLength(te->tab[l])-1;
            }
            else if (c>0)
            {
                c--;
                if (ctrl)
                    for(; c>0 && te->tab[l][c-1].c != ' ' ; c--);
            }
            break;
        case SDLK_HOME:
            c = 0;
            l = 0;
            break;
        case SDLK_END:
            for (; IsLineOK(*te,l+1) ; l++);
            for (c=0 ; IsCharOK(te->tab[l][c].c) ; c++);
            break;
        default:
            done=0;
            break;
    }

    if (done)
    {
        if (c2)
            *c2 = c;
        if (l2)
            *l2 = l;
    }

    return done;
}


int OffsetCorrection(TextEdition *te)
{
    SDL_Rect pos, rect = te->pos;
    int c,l,doneX=1,doneY=1;

    rect.x = 0;
    rect.y = 0;
    rect.w -= VSBWidth(*te);
    rect.h -= HSBHeight(*te);
    GetPositionInEdition(*te, te->cursorPos, &l, &c);

    pos.x = te->tab[l][c].x + te->offsetX;
    pos.y = te->tab[l][c].y + te->offsetY;
    pos.w = 1;
    pos.h = te->fontHeight;
    if (pos.x < rect.x)
        te->offsetX = rect.x - te->tab[l][c].x;
    else if (pos.x + pos.w > rect.x + rect.w)
        te->offsetX = rect.x + rect.w - (te->tab[l][c].x + pos.w);
    else doneX = 0;
    if (pos.y < rect.y)
        te->offsetY = Ceil((rect.y - te->tab[l][c].y)*1.0/te->hSpace) * te->hSpace;
    else if (pos.y + pos.h > rect.y + rect.h)
        te->offsetY = Ceil((rect.y + rect.h - (te->tab[l][c].y + pos.h))*1.0/te->hSpace) * te->hSpace;
    else doneY = 0;

    if (doneX)
        SetHSBFromOffset(te);
    if (doneY)
        SetVSBFromOffset(te);

    return doneX || doneY;
}


int WordHead(const char text[], int i)
{
    if (!text)
        return -1;

    for (; i > 0 ; i--)
    {
        if (text[i] == ' ' || text[i] == '-' || text[i] == '\n')
            return i;
    }

    return 0;
}

int LineLength(CharInfo line[])
{
    int i;
    for (i=0 ; IsCharOK(line[i].c) ; i++);
    return i;
}

void GetPositionInEdition(TextEdition te, int i, int *l2, int *c2)
{
    int c=0, l, done = 0;

    for (l=0 ; IsLineOK(te,l) && !done ; l++)
    {
        for (c=0 ; IsCharOK(te.tab[l][c].c) && !done ; c++)
        {
            if (--i<0)
                done = 1;
        }
    }

    l--;
    if (done)
        c--;

    if (c2)
        *c2 = c;
    if (l2)
        *l2 = l;
}

int GetPositionInText(TextEdition te, int l2, int c2)
{
    int c, l, i=0;

    for (l=0 ; IsLineOK(te,l) && l<=l2 ; l++)
    {
        for (c=0 ; IsCharOK(te.tab[l][c].c) && (c<c2 || l<l2) ; c++)
            i++;
    }

    return i;
}

int GetLineFromYPosition(TextEdition te, int y)
{
    int l, lmax=te.numLines-1;

    y -= te.pos.y + te.offsetY + te.blitSurfPos.y;
    l = y / te.hSpace;
    return l>lmax ? lmax: (l<0 ? 0:l);
}

int GetCharFromXPosition(TextEdition te, int l, int x)
{
    int i;
    if (!te.tab[l])
        return -1;

    x -= te.pos.x + te.offsetX + te.blitSurfPos.x;
    for (i=0 ; IsCharOK(te.tab[l][i].c) && (te.tab[l][i].x + WidthChar(te,te.tab[l][i].c)/2) < x ; i++);
    if (!IsCharOK(te.tab[l][i].c) && IsLineOK(te,l+1))
        i--;

    return i;
}

int NewLetter(CharInfo **tab, int l, int c)
{
    int i = c;
    CharInfo *ptr;

    for (; tab[l][i].c ; i++);
    ptr = malloc(sizeof(CharInfo)*(i+2));
    if (!ptr)
        return 0;

    for(i++ ; i>c ; i--)
        memcpy(ptr+i, &(tab[l][i-1]), sizeof(CharInfo));
    (ptr+i)->prevC = '\0';
    for(i-- ; i>=0 ; i--)
        memcpy(ptr+i, &(tab[l][i]), sizeof(CharInfo));

    free(tab[l]);
    tab[l] = ptr;

    return 1;
}

int NewLine(TextEdition *te, int l)
{
    int i;
    CharInfo **tab;

    tab = malloc(sizeof(CharInfo*)*(++ te->numLines));
    if (!tab)
        return 0;

    for(i = te->numLines-1 ; i>l ; i--)
        tab[i] = te->tab[i-1];
    tab[i] = malloc(sizeof(CharInfo));
    if (!tab[i])
    {
        free(tab);
        return 0;
    }
    tab[i][0].c = '\0';
    tab[i][0].prevC = '\0';
    for(i-- ; i>=0 ; i--)
        tab[i] = te->tab[i];

    free(te->tab);
    te->tab = tab;

    return 1;
}

int DimLetter(TextEdition te, char c, int *w, int *h)
{
    char buf[2] = "";
    SDL_Surface *surface;

    buf[0] = c;
    if (!(surface = TE_RenderText(buf, te, 0)))
        return 0;
    if (w)
        *w = surface->w;
    if (h)
        *h = surface->h;

    SDL_FreeSurface(surface);
    return 1;
}

int WidthWord(TextEdition te, int i)
{
    int j, w=0;
    for (j=i ; IsCharOK(te.text[j]) && te.text[j] != ' ' && te.text[j] != '.' && te.text[j] != ',' && te.text[j] != '-' && te.text[j] != '\n' ; j++)
        w += WidthChar(te,te.text[j]);
    for (; te.text[j] == ' ' || te.text[j] == '.' || te.text[j] == ',' || te.text[j] == '-' ; j++)
        w += WidthChar(te,te.text[j]);
    return w;
}

SDL_Surface* TE_RenderText(const char text[], TextEdition te, int inverted)
{
    if (!text)
        return NULL;
    if (inverted)
        te.blitStyle = TE_BLITSTYLE_SHADED;

    switch(te.blitStyle)
    {
        case TE_BLITSTYLE_BLENDED:
            return TTF_RenderText_Blended(te.font, text, te.colorFG);
        case TE_BLITSTYLE_SHADED:
            if (inverted)
                return TTF_RenderText_Shaded(te.font, text, te.colorFGSelect, te.colorBGSelect);
            else return TTF_RenderText_Shaded(te.font, text, te.colorFG, te.colorBG);
        case TE_BLITSTYLE_SOLID:
            return TTF_RenderText_Solid(te.font, text, te.colorFG);
        default:
            break;
    }

    return NULL;
}

SDL_Color ColorInverse(SDL_Color color)
{
    color.r = 255-color.r;
    color.g = 255-color.g;
    color.b = 255-color.b;
    return color;
}


int DeleteChar(char text[], unsigned int pos)
{
    if (pos >= strlen(text))
        return 0;
    strcpy(text+pos, text+pos+1);
    return 1;
}

int InsertChar(char text[], unsigned int pos, char c, unsigned int lengthMax)
{
    char *buf;
    unsigned int l = strlen(text);

    if (l+1 >= lengthMax || pos > l)
        return 0;
    if ( !(buf=malloc(sizeof(char)*(lengthMax+1))) )
        return 0;

    sprintf(buf, "%c%s", c, text+pos);
    strcpy(text+pos, buf);
    free(buf);
    return 1;
}

int InsertString(char text[], unsigned int pos, char string[], unsigned int lengthMax)
{
    char *buf;
    unsigned int l1 = strlen(text),
                 l2 = strlen(string);

    if (l1+l2 >= lengthMax || pos > l1)
        return 0;
    if ( !(buf=malloc(sizeof(char)*(lengthMax+1))) )
        return 0;

    sprintf(buf, "%s%s", string, text+pos);
    strcpy(text+pos, buf);
    free(buf);
    return 1;
}

int DeleteSelection(TextEdition *te)
{
    int begin = te->selection.begin,
        end = te->selection.end;

    if (te->selection.begin == te->selection.end)
        return 0;
    if (begin > end)
    {
        begin = te->selection.end;
        end = te->selection.begin;
    }

    strcpy(te->text+begin, te->text+end);
    te->cursorPos = begin;
    te->selection.begin = begin;
    te->selection.end = begin;
    return 1;
}


int TE_Init(void)
{
    const char *data[] = {
      "16 16 3 1",
      " 	c None",
      ".	c #FFFFFF",
      "+	c #000000",
      "                ",
      "     .. ..      ",
      "     .+++.      ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "      .+.       ",
      "     .+++.      ",
      "     .. ..      ",
      "                ",
      "7,7"
    };

    SDL_EnableUNICODE(1);
    TE_EditionCursor = CreateCursor(data);
    TE_NormalCursor = SDL_GetCursor();

    TE_initialized = 1;

    return 1;
}

void TE_Quit(void)
{
    if (TE_EditionCursor)
        SDL_FreeCursor(TE_EditionCursor);

    TE_initialized = 0;
}

SDL_Cursor* CreateCursor(const char *image[])   //Creates a cursor from XPM data. Provided by the SDL documentation.
{
  int i, row, col;
  Uint8 data[4*16];
  Uint8 mask[4*16];
  int hot_x, hot_y;

  i = -1;
  for ( row=0; row<16; ++row ) {
    for ( col=0; col<16; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case '+':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
        default:
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 16, 16, hot_x, hot_y);
}


int SetHSBLength(TextEdition *te, int length)
{
    SDL_Rect rect = {0,0,0,0};

    if (length <= 0 || !HasHScrollStyle(te->style))
        return 0;
    if (te->HScrollBar && te->HScrollBar->w == length)
        return 1;

    if (te->HScrollBar)
        SDL_FreeSurface(te->HScrollBar);

    if (!(te->HScrollBar = SDL_CreateRGBSurface(SDL_HWSURFACE, length, 10, 32, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK)))
        return 0;

    SDL_FillRect(te->HScrollBar, NULL, SDL_MapRGBA(te->HScrollBar->format, 0, 0, 0, SDL_ALPHA_TRANSPARENT));

    rect.w = length;
    rect.h = 1;
    rect.y = 5;
    SDL_FillRect(te->HScrollBar, &rect, SDL_MapRGBA(te->HScrollBar->format, 255, 0, 0, 20));

    rect.y++;
    SDL_FillRect(te->HScrollBar, &rect, SDL_MapRGBA(te->HScrollBar->format, 255, 0, 0, 70));

    rect.y++;
    SDL_FillRect(te->HScrollBar, &rect, SDL_MapRGBA(te->HScrollBar->format, 255, 0, 0, 120));

    rect.y++;
    SDL_FillRect(te->HScrollBar, &rect, SDL_MapRGBA(te->HScrollBar->format, 255, 0, 0, 70));

    rect.y++;
    SDL_FillRect(te->HScrollBar, &rect, SDL_MapRGBA(te->HScrollBar->format, 255, 0, 0, 20));

    SDL_SetAlpha(te->HScrollBar, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

    return 1;
}


int SetVSBLength(TextEdition *te, int length)
{
    SDL_Rect rect = {0,0,0,0};

    if (length <= 0 || !HasVScrollStyle(te->style))
        return 0;
    if (te->VScrollBar && te->VScrollBar->h == length)
        return 1;

    if (te->VScrollBar)
        SDL_FreeSurface(te->VScrollBar);

    if (!(te->VScrollBar = SDL_CreateRGBSurface(SDL_HWSURFACE, 10, length, 32, RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK)))
        return 0;

    SDL_FillRect(te->VScrollBar, NULL, SDL_MapRGBA(te->VScrollBar->format, 0, 0, 0, SDL_ALPHA_TRANSPARENT));

    rect.h = length;
    rect.w = 1;
    rect.x = 5;
    SDL_FillRect(te->VScrollBar, &rect, SDL_MapRGBA(te->VScrollBar->format, 255, 0, 0, 20));

    rect.x++;
    SDL_FillRect(te->VScrollBar, &rect, SDL_MapRGBA(te->VScrollBar->format, 255, 0, 0, 70));

    rect.x++;
    SDL_FillRect(te->VScrollBar, &rect, SDL_MapRGBA(te->VScrollBar->format, 255, 0, 0, 120));

    rect.x++;
    SDL_FillRect(te->VScrollBar, &rect, SDL_MapRGBA(te->VScrollBar->format, 255, 0, 0, 70));

    rect.x++;
    SDL_FillRect(te->VScrollBar, &rect, SDL_MapRGBA(te->VScrollBar->format, 255, 0, 0, 20));

    SDL_SetAlpha(te->VScrollBar, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

    return 1;
}


int IsMouseOverHSB(TextEdition te, int x, int y)
{
    SDL_Rect pt, rect = te.posHSB;

    if (!te.HScrollBar)
        return 0;

    pt.x = x - te.blitSurfPos.x - te.pos.x;
    pt.y = y - te.blitSurfPos.y - te.pos.y;

    rect.w = te.HScrollBar->w;
    rect.h = te.HScrollBar->h;

    return IsInRect(pt,rect);
}

int IsMouseOverVSB(TextEdition te, int x, int y)
{
    SDL_Rect pt, rect = te.posVSB;

    if (!te.VScrollBar)
        return 0;

    pt.x = x - te.blitSurfPos.x - te.pos.x;
    pt.y = y - te.blitSurfPos.y - te.pos.y;

    rect.w = te.VScrollBar->w;
    rect.h = te.VScrollBar->h;

    return IsInRect(pt,rect);
}

int TE_SetEditionText(TextEdition *te, const char text[])
{
    char *buf;

    if (!text || !(buf = malloc(sizeof(char)*(strlen(text)+1))) )
        return -1;

    strcpy(buf, text);
    DeleteAllChars(buf, '\r');

    strncpy(te->text, buf, te->textLength);
    free(buf);

    TE_UpdateTextEdition(te, 0);
    return strlen(te->text);
}

int DeleteAllChars(char text[], char c)
{
    char *p = text;

    if (!text)
        return 0;

    while ( (p=strchr(p,c)) )
        strcpy(p, p+1);

    return 1;
}

int TE_SetFocusEdition(TextEdition *te, int focus)
{
	te->focus = focus;
	return 1;
}

int TE_GetFocusEdition(TextEdition *te)
{
	return te->focus;
}

int TE_SetCursorPos(TextEdition *te, int cursorPos)
{
    if (cursorPos >= 0 && cursorPos <= strlen(te->text))
    {
        te->cursorPos = cursorPos;
        OffsetCorrection(te);
        return 1;
    }
    else return 0;
}

int TE_GetCursorPos(TextEdition *te)
{
    return te->cursorPos;
}

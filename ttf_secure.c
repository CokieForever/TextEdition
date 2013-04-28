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

#include "ttf_secure.h"

typedef struct MutexElmt
{
    SDL_sem *mutex;  //We actually use semaphores instead of mutex to allow auto-blocking calls
    struct MutexElmt *next;
} MutexElmt;

static int WaitForYourTurn(void);
static void YouAreOver(void);

static MutexElmt *mainMutexList = NULL;
static SDL_mutex *priorMutex = NULL;

static int WaitForYourTurn(void)
{
    MutexElmt *currentElmt = NULL,
              *lastElmt = NULL;

    if ( !(currentElmt = malloc(sizeof(MutexElmt))) )
        return 0;

    currentElmt->next = NULL;
    if ( !(currentElmt->mutex = SDL_CreateSemaphore(0)) )
    {
        free(currentElmt);
        return 0;
    }

    if (!priorMutex)
        priorMutex = SDL_CreateMutex();
    SDL_mutexP(priorMutex);

    if (!mainMutexList)
    {
        mainMutexList = currentElmt;
        SDL_mutexV(priorMutex);
    }
    else
    {
        lastElmt = mainMutexList;
        while (lastElmt->next)
            lastElmt = lastElmt->next;
        lastElmt->next = currentElmt;

        SDL_mutexV(priorMutex);
        SDL_SemWait(currentElmt->mutex);
    }

    return 1;
}

static void YouAreOver(void)
{
    MutexElmt *nextElmt;

    SDL_mutexP(priorMutex);

    nextElmt = mainMutexList->next;

    SDL_DestroySemaphore(mainMutexList->mutex);
    free(mainMutexList);

    mainMutexList = nextElmt;
    if (nextElmt)
        SDL_SemPost(nextElmt->mutex);

    SDL_mutexV(priorMutex);
}


#ifdef TTF_OpenFont
#undef TTF_OpenFont
#endif
TTF_Font* TTFS_OpenFontSecure(const char *fileName, int ptSize)
{
    TTF_Font *font;

    if (!WaitForYourTurn())
        return NULL;
    font = TTF_OpenFont(fileName, ptSize);
    YouAreOver();

    return font;
}

#ifdef TTF_CloseFont
#undef TTF_CloseFont
#endif
void TTFS_CloseFontSecure(TTF_Font *font)
{
    if (!WaitForYourTurn())
        return;
    TTF_CloseFont(font);
    YouAreOver();
}


#ifdef TTF_RenderText_Blended
#undef TTF_RenderText_Blended
#endif
#ifdef TTF_RenderText_Shaded
#undef TTF_RenderText_Shaded
#endif
#ifdef TTF_RenderText_Solid
#undef TTF_RenderText_Solid
#endif
SDL_Surface* TTFS_RenderTextSecure(TTF_Font *font, const char *text, SDL_Color c1, SDL_Color c2, int type)
{
    SDL_Surface *surf;

    if (!WaitForYourTurn())
        return NULL;
    switch (type)
    {
        case TTFS_BLENDED:
            surf = TTF_RenderText_Blended(font, text, c1);
            break;
        case TTFS_SHADED:
            surf = TTF_RenderText_Shaded(font, text, c1, c2);
            break;
        default:
            surf = TTF_RenderText_Solid(font, text, c1);
            break;
    }
    YouAreOver();

    return surf;
}

#ifdef TTF_FontLineSkip
#undef TTF_FontLineSkip
#endif
int TTFS_FontLineSkipSecure(TTF_Font *font)
{
    int lineSkip;

    if (!WaitForYourTurn())
        return -1;
    lineSkip = TTF_FontLineSkip(font);
    YouAreOver();

    return lineSkip;
}

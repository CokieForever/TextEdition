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

#ifndef TTFSECUREH

#define TTFSECUREH

#include <SDL.h>
#include <SDL_ttf.h>

TTF_Font* TTFS_OpenFontSecure(const char *fileName, int ptSize);
void TTFS_CloseFontSecure(TTF_Font *font);
SDL_Surface* TTFS_RenderTextSecure(TTF_Font *font, const char *text, SDL_Color c1, SDL_Color c2, int type);
int TTFS_FontLineSkipSecure(TTF_Font *font);


#define TTFS_BLENDED    0
#define TTFS_SHADED     1
#define TTFS_SOLID      2

#define TTF_OpenFont(n,s)                   TTFS_OpenFontSecure(n,s)
#define TTF_CloseFont(f)                    TTFS_CloseFontSecure(f)
#define TTF_RenderText_Blended(f,t,c)       TTFS_RenderTextSecure(f,t,c,c,TTFS_BLENDED)
#define TTF_RenderText_Shaded(f,t,c1,c2)    TTFS_RenderTextSecure(f,t,c1,c2,TTFS_SHADED)
#define TTF_RenderText_Solid(f,t,c)         TTFS_RenderTextSecure(f,t,c,c,TTFS_SOLID)
#define TTF_FontLineSkip(f)                 TTFS_FontLineSkipSecure(f)





 #endif


/*

TextEdition - A C library for the creation of text boxes with SDL
Copyright (C) 2015 Cokie (cokie.forever@gmail.com)

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

#include "textedition.h"


int main (int argc, char *argv[])
{
    TextEdition te;
    SDL_Rect pos;
    TTF_Font *font;
    SDL_Color color;
    SDL_Window *screen;
    SDL_Renderer *renderer;
    SDL_Texture *bgTexture;
    SDL_Event event;
    int done = 0;

    //To avoid warnings
    argc++; argv++;

    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);

    if (TTF_Init() < 0)
    {
       fprintf(stderr, "Unable to init SDL_ttf : %s\n", TTF_GetError());
       return EXIT_FAILURE;
    }
    atexit(TTF_Quit);

    if (IMG_Init(IMG_INIT_PNG) < 0)
    {
       fprintf(stderr, "Unable to init SDL_image : %s\n", IMG_GetError());
       return EXIT_FAILURE;
    }
    atexit(IMG_Quit);

    if ( SDL_CreateWindowAndRenderer(640, 480, 0, &screen, &renderer) == -1)
    {
        fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(screen, "TextEdition");

    bgTexture = IMG_LoadTexture(renderer, "ImageFond.png");

    pos.x = 50; pos.y = 140;
    pos.w = 525; pos.h = 275;
    font = TTF_OpenFont("default.ttf", 10);
    color.r = 255; color.g = 255; color.b = 255;
    memset(&te, 0, sizeof(TextEdition));
    te.blitStyle = TE_BLITSTYLE_BLENDED;
    te.colorBG = color;
    TE_NewTextEdition(&te, 5000, pos, font, color, TE_STYLE_MULTILINE | TE_STYLE_HSCROLL | TE_STYLE_VSCROLL | TE_STYLE_BLITRGBA, renderer);

    TE_SetEditionText(&te, "\
Fran�ais (complet)\n\
\n\
En informatique, une zone de texte ou un champ de saisie (en anglais text box) est un �l�ment de base des interfaces graphiques (un widget ou contr�le) qui permet de proposer � l'utilisateur d'entrer du texte, par exemple pour qu'il soit ensuite utilis� par un programme, ou envoy� � un site web dans le cas d'un formulaires de page Web.\n\
Graphiquement une zone de texte ressemble � un rectangle de taille quelconque � l'int�rieur duquel se trouve le texte saisi par l'utilisateur. Les bordures de la zone de texte sont en g�n�ral bien mises en �vidence par un effet de relief.\n\
\n\
On peut distinguer deux types de zones de texte :\n\
    les zones de texte dans lesquelles on ne peut rentrer qu'une seule ligne de texte. Il s'agit par exemple des champs de recherche comme celui d'un moteur de recherche.\n\
    les zones de texte dans lesquelles on peut rentrer un texte complet avec �ventuellement des retours � la ligne. Celles-ci sont g�n�ralement �quip�es de barres de d�filement lorsqu'elles ne sont pas assez grandes pour afficher tout le texte. Certaines d'entre elles, utilis�es conjointement avec d'autres widgets,\n\
    permettent m�me de mettre en forme le texte saisi, c'est-�-dire de le souligner, le mettre en gras, etc.\n\
\n\
Habituellement lorsque l'utilisateur passe le pointeur de sa souris au-dessus d'une zone de texte, celui-ci change de forme (il se transforme en une sorte de I majuscule) pour indiquer � l'utilisateur qu'il peut cliquer pour que la zone de texte obtienne le focus.\n\
Si l'utilisateur effectue cette derni�re op�ration, un curseur se met � clignoter pour lui indiquer l'endroit o� le texte qu'il va �ventuellement taper au clavier sera ins�r�.\n\
\n\
Les zones de texte sont le plus souvent vides lorsque l'utilisateur les d�couvre. Toutefois leur contenu peut �tre modifi� par le programme. Cela peut permettre par exemple de remplir une zone de texte pr�vue pour permettre � l'utilisateur de r�pondre � une question dont la r�ponse est souvent la m�me.\n\
Ceci lui �vitera ainsi de la remplir et donc de gagner du temps.\n\
Les zones de texte peuvent �galement �tre verrouill�es en �criture pour emp�cher les utilisateurs d'y �crire quoi que ce soit ou de modifier une r�ponse. Cette possibilit� permet d'indiquer � l'utilisateur que sous certaines conditions, non remplies en l'occurrence, il pourrait modifier le texte du champs.\n\
En g�n�ral, il est possible d'effectuer des op�rations de copier-coller dans les zones de texte.\n\
\n\
\n\
English (short)\n\
\n\
A text box, text field or text entry box is a kind of widget used when building a graphical user interface (GUI). A text box's purpose is to allow the user to input text information to be used by the program.\n\
User-interface guidelines recommend a single-line text box when only one line of input is required, and a multi-line text box only if more than one line of input may be required. Non-editable text boxes can serve the purpose of simply displaying text.\n\
\n\
A typical text box is a rectangle of any size, possibly with a border that separates the text box from the rest of the interface. Text boxes may contain zero, one, or two scrollbars. Text boxes usually display a text cursor (commonly a blinking vertical line), indicating the current region of text being edited.\n\
It is common for the mouse cursor to change its shape when it hovers over a text box."
    );

    while (!done)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {

            case SDL_QUIT:
                done = 1;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = 1;
                break;
            default:
                break;
        }

        if (!done)
        {
            if (bgTexture)
                SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
            else
                SDL_RenderClear(renderer);

            TE_HoldTextEdition(&te, event);
            TE_DisplayTextEdition(&te);

            SDL_RenderPresent(renderer);
        }
    }

    TE_DeleteTextEdition(&te);
    TE_Quit();
    TTF_CloseFont(font);
    printf("Exited cleanly\n");
    return EXIT_SUCCESS;
}

#include "textedition.h"


int main (void)
{
    TextEdition te;
    SDL_Rect pos, posZero = {0,0,0,0};
    TTF_Font *font;
    SDL_Color color;
    SDL_Surface *screen, *background;
    SDL_Event event;
    int done = 0;

    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
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


    screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return 1;
    }
    SDL_WM_SetCaption("TextEdition", NULL);

    if ( (background = IMG_Load("ImageFond.png")) )
        SDL_BlitSurface(background, NULL, screen, &posZero);

    pos.x = 50; pos.y = 140;
    pos.w = 525; pos.h = 275;
    font = TTF_OpenFont("default.ttf", 10);
    color.r = 255; color.g = 255; color.b = 255;
    memset(&te, 0, sizeof(TextEdition));
    te.blitStyle = TE_BLITSTYLE_BLENDED;
    te.colorBG = color;
    TE_NewTextEdition(&te, 5000, pos, font, color, TE_STYLE_MULTILINE | TE_STYLE_HSCROLL | TE_STYLE_VSCROLL | TE_STYLE_JUSTDISPLAY);

    TE_SetEditionText(&te, "\
Français (complet)\n\
\n\
En informatique, une zone de texte ou un champ de saisie (en anglais text box) est un élément de base des interfaces graphiques (un widget ou contrôle) qui permet de proposer à l'utilisateur d'entrer du texte, par exemple pour qu'il soit ensuite utilisé par un programme, ou envoyé à un site web dans le cas d'un formulaires de page Web.\n\
Graphiquement une zone de texte ressemble à un rectangle de taille quelconque à l'intérieur duquel se trouve le texte saisi par l'utilisateur. Les bordures de la zone de texte sont en général bien mises en évidence par un effet de relief.\n\
\n\
On peut distinguer deux types de zones de texte :\n\
    les zones de texte dans lesquelles on ne peut rentrer qu'une seule ligne de texte. Il s'agit par exemple des champs de recherche comme celui d'un moteur de recherche.\n\
    les zones de texte dans lesquelles on peut rentrer un texte complet avec éventuellement des retours à la ligne. Celles-ci sont généralement équipées de barres de défilement lorsqu'elles ne sont pas assez grandes pour afficher tout le texte. Certaines d'entre elles, utilisées conjointement avec d'autres widgets,\n\
    permettent même de mettre en forme le texte saisi, c'est-à-dire de le souligner, le mettre en gras, etc.\n\
\n\
Habituellement lorsque l'utilisateur passe le pointeur de sa souris au-dessus d'une zone de texte, celui-ci change de forme (il se transforme en une sorte de I majuscule) pour indiquer à l'utilisateur qu'il peut cliquer pour que la zone de texte obtienne le focus.\n\
Si l'utilisateur effectue cette dernière opération, un curseur se met à clignoter pour lui indiquer l'endroit où le texte qu'il va éventuellement taper au clavier sera inséré.\n\
\n\
Les zones de texte sont le plus souvent vides lorsque l'utilisateur les découvre. Toutefois leur contenu peut être modifié par le programme. Cela peut permettre par exemple de remplir une zone de texte prévue pour permettre à l'utilisateur de répondre à une question dont la réponse est souvent la même.\n\
Ceci lui évitera ainsi de la remplir et donc de gagner du temps.\n\
Les zones de texte peuvent également être verrouillées en écriture pour empêcher les utilisateurs d'y écrire quoi que ce soit ou de modifier une réponse. Cette possibilité permet d'indiquer à l'utilisateur que sous certaines conditions, non remplies en l'occurrence, il pourrait modifier le texte du champs.\n\
En général, il est possible d'effectuer des opérations de copier-coller dans les zones de texte.\n\
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

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
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
            TE_HoldTextEdition(&te, event);
            TE_DisplayTextEdition(&te);

            SDL_Flip(screen);
        }
    }

    TE_DeleteTextEdition(&te);
    TE_Quit();
    TTF_CloseFont(font);
    if (background)
        SDL_FreeSurface(background);
    printf("Exited cleanly\n");
    return EXIT_SUCCESS;
}

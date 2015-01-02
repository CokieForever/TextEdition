-----------------------------------------------------------------
TextEdition - A C library for the creation of text boxes with SDL
-----------------------------------------------------------------

Author: Cokie (cokie.forever@gmail.com)
Version: 0.18 beta (Jan 2nd, 2014)
Operating systems: Windows, Linux
License: GNU GPL v2.0 (see the LICENSE.txt file or http://www.gnu.org/licenses/gpl-2.0.html)

Description:
TextEdition is a C library designed to easily create text boxes in SDL windows.
As from the version 0.17, TextEdition is compatible with SDL 2.0 only. To use with SDL 1.2, use the version 0.16.

Main function description:

int TE_NewTextEdition(TextEdition *te, int length, SDL_Rect pos, TTF_Font *font, SDL_Color colorFG, int style, SDL_Renderer *renderer)

Creates a text box (called "Text Edition") with different parameters such as the position and the dimensions, the font, the color, and more.


Use :
	TextEdition *te: a pointer to a library-defined structure of type "TextEdition". Some members of this structure can be set before the call, but others should be set to zero (with the standard function memset()). See the "Advanced Settings" section for more information. Cannot be NULL.
	int length: the maximum length of the text you allow in the Text Edition. Must be strictly positive.
	SDL_Rect pos: specifies the position and dimensions of the Text Edition. The position is relative to the Blit Surface (see "Advanced settings"). If an incorrect position or dimension is specified, the function will correct it.
	TTF_Font *font: the font you want the Text Edition to use to display the text. Cannot be NULL.
	SDL_Color colorFG: the color you want the Text Edition to use to display the text.
	int style: an OR-combination of the following flags:
		TE_STYLE_NORMAL: No special functionnality.
		TE_STYLE_MULTILINE: The Text Edition can display a text on more than one line.
		TE_STYLE_VSCROLL: A vertical scroll bar is displayed on the right side of the Text Edition. Implies TE_STYLE_MULTILINE.
		TE_STYLE_HSCROLL: A horizontal scroll bar is displayed on the bottom of the Text Edition.
		TE_STYLE_AUTOJUMP: The text automatically jumps to the next line when no more space is available. Implies TE_STYLE_MULTILINE. Consequently, the horizontal scroll bar becomes useless and is never displayed.
		TE_STYLE_READONLY: The text is displayed and can be selected, but it cannot be modified by the user.
		TE_STYLE_JUSTDISPLAY: The text is just displayed : no edition or selection is possible, and the mouse cursor doesn't change when it is over the Text Edition (Implies TE_STYLE_READONLY). Useful when you just need a multiline display (without any edition functionnality).
		TE_STYLE_BLITRGBA: The Text Edition will use a custom function to handle RGBA->RGBA blits. The rendering will be better but slower.
	SDL_Renderer *renderer: The renderer to use to blit the text box. Most of the time, you will just give the renderer associated with your window as it is, but you might want to modify it in order to blit the text box on an another texture for example.



Advanced settings :

You can set the members of the TextEdition structure before calling the function, in order to have extended features.

	int blitStyle: one of the following values:
		TE_BLITSTYLE_BLENDED (default): the text is displayed slowly but the rendering is better.
		TE_BLITSTYLE_SHADED: the text is displayed quickly with a good rendering, but with a background color. You have to specify it with the colorBG member.
		TE_BLITSTYLE_SOLID: the text is displayed quickly but the rendering is less nice.
	int hSpace: the space, in pixels, you want to have between each line of text. By default (if this space is null or negative), the Text Edition will use a custom value which depends on the font.
	SDL_Color colorBG: the color you want to use as background color when the blitStyle TE_BLITSTYLE_SHADED is specified.
	SDL_Color colorFGSelect: the color of the text when it is selected.
	SDL_Color colorBGSelect: the background color of a selected text.
		If colorBG and colorFG are identical, the function will automatically use the following colors: colorBG and colorFGSelect will be the inverse of colorFG, and colorBGSelect will be the same as colorFG.
	SDL_Surface *blitSurface: the surface on which you want the text to be displayed. By default (if NULL), this is directly the window/texture associated with your renderer.
	SDL_Rect blitSurfPos: the position of the Blit Surface / Texture, relative to your window. By default, the Blit Surface is the window itself, so this member is set to zero, but you must fill this member if the Blit Surface is not the window.
	


Return value:

1 if the Text Edition is created, 0 otherwise.
The TextEdition structure is modified by the function.

The Text Edition should be freed by a call to the function TE_DeleteEdition() when you no longer need it.




How to properly create and manage a text box?

To create and manage a text box with this function, you have to respect some rules.
First, as from the version 0.17, the text boxes are always transparent - even if TE_STYLE_BLITRGBA is not specified, so you don't have to struggle with the background anymore.
After having created your Text Edition, to manage it, you have to call the function TE_HoldTextEdition() each time you retrieve an event from SDL. You must pass this event to the function (see the example file).
While the Text Edition is active, it is highly recommended not to modify any member of the structure, except the style member to add or retrieve one of the following flags (do not touch the other ones!): TE_STYLE_READONLY ; TE_STYLE_JUSTDISPLAY. Some functions are provided to modify some characteristics of the Text Edition, such as the text or the cursor position. If you want to modify other characteristics, you have to destroy the Edition and to create another one.
When you no longer need the Text Edition, use the function TE_DeleteEdition() to free it. After the call, the structure is still valid, but all the pointers are NULL. You can use it to create another Text Edition with the same parameters. However, if the blitSurface was a custom surface, it is deleted by the deleting function, you should reallocate it and set the blitSurf parameter again (it is automatically set to NULL).
If you are using Text Editions in a multithreading context, you should include "ttf_secure.h" in "textedition.h" and all files which can make calls to the SDL_ttf library. Moreover, you should use TE_LockEdition() and TE_UnlockEdition() functions before directly accessing and/or modifying TextEditions. Once again, it is recommended to use only the provided functions to modify the Text Editions.
When you're done with the library, call TE_Quit() to free the memory.


See http://ikturm.wordpress.com/projects-2/current-projects/textedition/ for more information.








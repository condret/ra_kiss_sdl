/*
  kiss_sdl widget toolkit
  Copyright (c) 2016 Tarvo Korrovits <tkorrovi@mail.com>
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would
     be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

  kiss_sdl version 1.2.0
*/

#ifndef _kiss_sdl_h
#define _kiss_sdl_h

#ifndef RESDIR
#define RESDIR ""
#endif

#if defined(_MSC_VER)
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#elif defined(__APPLE__)
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#endif
#include <r_util.h>
#include <string.h>
#include <stdio.h>

#define KISS_MAX_LENGTH 200
#define KISS_MIN_LENGTH 10
#define KISS_MAX_LABEL  500
#define KISS_MAGIC      12345

enum {
	WINDOW_TYPE,
	RENDERER_TYPE,
	TEXTURE_TYPE,
};

/* Length is number of elements, size is allocated size */
typedef struct kiss_array {
	void **data;
	int *id;
	int length;
	int size;
	int ref;
} kiss_array;

typedef struct rk_image_t {
	SDL_Texture *image;
	int magic;
	int w;
	int h;
} RKImage;

typedef struct rk_font_t {
	TTF_Font *font;
	int magic;
	int fontheight;
	int spacing;
	int lineheight;
	int advance;
	int ascent;
} RKFont;

typedef struct rk_window_t {
	int visible;
	int focus;
	SDL_Rect rect;
	int decorate;
	SDL_Color bg;
	struct rk_window_t *wdw;
} RKWindow;

typedef struct rk_label_t {
	int visible;
	SDL_Rect rect;
	char text[KISS_MAX_LABEL];
	SDL_Color textcolor;
	RKFont font;
	RKWindow *wdw;
} RKLabel;

typedef struct rk_button_t {
	int visible;
	int focus;
	SDL_Rect rect;
	int textx;
	int texty;
	char text[KISS_MAX_LENGTH];
	int active;
	int prelight;
	SDL_Color textcolor;
	RKFont font;
	RKImage normalimg;
	RKImage activeimg;
	RKImage prelightimg;
	RKWindow *wdw;
} RKButton;

typedef struct rk_select_button_t {
	int visible;
	int focus;
	SDL_Rect rect;
	int selected;
	RKImage selectedimg;
	RKImage unselectedimg;
	RKWindow *wdw;
} RKSelectButton;

typedef struct rk_vertical_scrollbar_t {
	int visible;
	int focus;
	SDL_Rect uprect;
	SDL_Rect downrect;
	SDL_Rect sliderrect;
	SDL_Rect *wheelrect;
	int maxpos;
	double fraction;
	double step;
	ut32 lasttick;
	int downclicked;
	int upclicked;
	int sliderclicked;
	RKImage up;
	RKImage down;
	RKImage vslider;
	RKWindow *wdw;
} RKVScrollbar;

typedef struct rk_horizontal_scrollbar_t {
	int visible;
	int focus;
	SDL_Rect leftrect;
	SDL_Rect rightrect;
	SDL_Rect sliderrect;
	int maxpos;
	double fraction;
	double step;
	ut32 lasttick;
	int leftclicked;
	int rightclicked;
	int sliderclicked;
	RKImage left;
	RKImage right;
	RKImage hslider;
	RKWindow *wdw;
} RKHScrollbar;

typedef struct rk_progressbar_t {
	int visible;
	SDL_Rect rect;
	SDL_Rect barrect;
	int width;
	double fraction;
	double step;
	SDL_Color bg;
	ut32 lasttick;
	int run;
	RKImage bar;
	RKWindow *wdw;
} RKProgressbar;

typedef struct rk_entry_t {
	int visible;
	int focus;
	SDL_Rect rect;
	int decorate;
	int textx;
	int texty;
	char text[KISS_MAX_LENGTH];
	int active;
	int textwidth;
	int selection[4];
	int cursor[2];
	SDL_Color normalcolor;
	SDL_Color activecolor;
	SDL_Color bg;
	RKFont font;
	RKWindow *wdw;
} RKEntry;

typedef struct rk_textbox_t {
	int visible;
	int focus;
	SDL_Rect rect;
	int decorate;
	RPVector *lines;
	SDL_Rect textrect;
	int firstline;
	int maxlines;
	int textwidth;
	int highlightline;
	int selectedline;
	int selection[4];
	int cursor[2];
	SDL_Color textcolor;
	SDL_Color hlcolor;
	SDL_Color bg;
	RKFont font;
	RKWindow *wdw;
} RKTextbox;

typedef struct rk_combobox_t {
	int visible;
	char text[KISS_MAX_LENGTH];
	RKEntry entry;
	RKWindow window;
	RKVScrollbar vscrollbar;
	RKTextbox textbox;
	RKImage combo;
	RKWindow *wdw;
} RKCombobox;

extern SDL_Color kiss_white, kiss_black, kiss_green, kiss_blue,
	kiss_lightblue;
extern RKFont kiss_textfont, kiss_buttonfont;
extern RKImage kiss_normal, kiss_prelight, kiss_active, kiss_bar,
	kiss_up, kiss_down, kiss_left, kiss_right, kiss_vslider,
	kiss_hslider, kiss_selected, kiss_unselected, kiss_combo;
extern double kiss_spacing;
extern int kiss_textfont_size, kiss_buttonfont_size;
extern int kiss_click_interval, kiss_progress_interval;
extern int kiss_slider_padding;
extern int kiss_border, kiss_edge;
extern int kiss_screen_width, kiss_screen_height;

#ifdef __cplusplus
extern "C" {
#endif

#define rk_pointinrect(_x, _y, rect)					\
	((_x) >= ((rect)->x) && (_x) < ((rect)->x) + ((rect)->w) &&	\
	(_y) >= ((rect)->y) && (_y) < ((rect)->y) + ((rect)->h))	\

#define rk_makerect(rect, _x, _y, _w, _h)	\
	do {					\
		(rect).x = (_x);		\
		(rect).y = (_y);		\
		(rect).w = (_w);		\
		(rect).h = (_h);		\
	} while(0)


int rk_utf8next (char *str, int index);
int rk_utf8prev (char *str, int index);
int rk_utf8fix (char *str);
char *rk_string_copy (char *dest, size_t size, char *str1, char *str2);
char *rk_backspace (char *str);
int rk_array_init (kiss_array *a);
void *rk_array_data (kiss_array *a, int index);
int rk_array_id (kiss_array *a, int index);
int rk_array_assign (kiss_array *a, int index, int id, void *data);
int rk_array_append (kiss_array *a, int id, void *data);
int rk_array_insert (kiss_array *a, int index, int id, void *data);
int rk_array_remove (kiss_array *a, int index);
int rk_array_free (kiss_array *a);
ut32 rk_getticks (void);
int rk_maxlength (RKFont font, int width, char *str1, char *str2);
int rk_textwidth (RKFont font, char *str1, char *str2);
int rk_renderimage (SDL_Renderer *renderer, RKImage image, int x, int y, SDL_Rect *clip);
int rk_rendertext (SDL_Renderer *renderer, char *text, int x, int y, RKFont font, SDL_Color color);
int rk_fillrect (SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color);
int rk_decorate (SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color, int edge);
int rk_image_init (RKImage *image, char *fname, kiss_array *a, SDL_Renderer *renderer);
RKImage *rk_image_new (char *fname, SDL_Renderer *renderer);
int rk_font_init (RKFont *font, char *fname, int size);
RKFont *rk_font_new (char *fname, int size);
SDL_Renderer *rk_init (char *title, kiss_array *a, int w, int h);
int rk_clean (kiss_array *a);
void rk_fini (kiss_array *a);
int rk_window_init (RKWindow *window, RKWindow *wdw, int decorate, int x, int y, int w, int h);
RKWindow *rk_window_new (RKWindow *wdw, int decorate, int x, int y, int w, int h);
int rk_window_event (RKWindow *window, SDL_Event *event, int *draw);
int rk_window_draw (RKWindow *window, SDL_Renderer *renderer);
int rk_label_init (RKLabel *label, RKWindow *wdw, char *text, int x, int y);
RKLabel *rk_label_new (RKWindow *wdw, char *text, int x, int y);
int rk_label_draw (RKLabel *label, SDL_Renderer *renderer);
int rk_button_init (RKButton *button, RKWindow *wdw, char *text, int x, int y);
RKButton *rk_button_new (RKWindow *wdw, char *text, int x, int y);
int rk_button_event (RKButton *button, SDL_Event *event, int *draw);
int rk_button_draw (RKButton *button, SDL_Renderer *renderer);
int rk_selectbutton_init (RKSelectButton *selectbutton, RKWindow *wdw, int x, int y);
RKSelectButton *rk_selectbutton_new (RKWindow *wdw, int x, int y);
int rk_selectbutton_event (RKSelectButton *selectbutton, SDL_Event *event, int *draw);
int rk_selectbutton_draw (RKSelectButton *selectbutton, SDL_Renderer *renderer);
int rk_vscrollbar_init (RKVScrollbar *vscrollbar, RKWindow *wdw, SDL_Rect *wheelrect, int x, int y, int h);
RKVScrollbar *rk_vscrollbar_new (RKWindow *wdw, SDL_Rect *wheelrect, int x, int y, int h);
int rk_vscrollbar_event (RKVScrollbar *vscrollbar, SDL_Event *event,int *draw);
int rk_vscrollbar_draw (RKVScrollbar *vscrollbar, SDL_Renderer *renderer);
int rk_hscrollbar_init (RKHScrollbar *hscrollbar, RKWindow *wdw, int x, int y, int w);
RKHScrollbar *rk_hscrollbar_new (RKWindow *wdw, int x, int y, int w);
int rk_hscrollbar_event (RKHScrollbar *hscrollbar, SDL_Event *event, int *draw);
int rk_hscrollbar_draw (RKHScrollbar *hscrollbar, SDL_Renderer *renderer);
int rk_progressbar_init (RKProgressbar *progressbar, RKWindow *wdw, int x, int y, int w);
RKProgressbar *rk_progressbar_new (RKWindow *wdw, int x, int y, int w);
int rk_progressbar_event (RKProgressbar *progressbar, SDL_Event *event, int *draw);
int rk_progressbar_draw (RKProgressbar *progressbar, SDL_Renderer *renderer);
int rk_entry_init (RKEntry *entry, RKWindow *wdw, int decorate, char *text, int x, int y, int w);
RKEntry *rk_entry_new (RKWindow *wdw, int decorate, char *text, int x, int y, int w);
int rk_entry_event (RKEntry *entry, SDL_Event *event, int *draw);
int rk_entry_draw (RKEntry *entry, SDL_Renderer *renderer);
int rk_textbox_init (RKTextbox *textbox, RKWindow *wdw, int decorate, RPVector *a, int x, int y, int w, int h);
int rk_textbox_event (RKTextbox *textbox, SDL_Event *event, int *draw);
int rk_textbox_draw (RKTextbox *textbox, SDL_Renderer *renderer);
int rk_combobox_init (RKCombobox *combobox, RKWindow *wdw, char *text, RPVector *a, int x, int y, int w, int h);
int rk_combobox_event (RKCombobox *combobox, SDL_Event *event,int *draw);
int rk_combobox_draw (RKCombobox *combobox, SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif

#endif /* _kiss_sdl_h */

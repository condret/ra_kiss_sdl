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
	FONT_TYPE,
	ARRAY_TYPE
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

typedef struct kiss_window {
	int visible;
	int focus;
	SDL_Rect rect;
	int decorate;
	SDL_Color bg;
	struct kiss_window *wdw;
} kiss_window;

typedef struct rk_label_t {
	int visible;
	SDL_Rect rect;
	char text[KISS_MAX_LABEL];
	SDL_Color textcolor;
	RKFont font;
	kiss_window *wdw;
} RKLabel;

typedef struct kiss_button {
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
	kiss_window *wdw;
} kiss_button;

typedef struct kiss_selectbutton {
	int visible;
	int focus;
	SDL_Rect rect;
	int selected;
	RKImage selectedimg;
	RKImage unselectedimg;
	kiss_window *wdw;
} kiss_selectbutton;

typedef struct kiss_vscrollbar {
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
	kiss_window *wdw;
} kiss_vscrollbar;

typedef struct kiss_hscrollbar {
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
	kiss_window *wdw;
} kiss_hscrollbar;

typedef struct kiss_progressbar {
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
	kiss_window *wdw;
} kiss_progressbar;

typedef struct kiss_entry {
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
	kiss_window *wdw;
} kiss_entry;

typedef struct kiss_textbox {
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
	kiss_window *wdw;
} kiss_textbox;

typedef struct kiss_combobox {
	int visible;
	char text[KISS_MAX_LENGTH];
	kiss_entry entry;
	kiss_window window;
	kiss_vscrollbar vscrollbar;
	kiss_textbox textbox;
	RKImage combo;
	kiss_window *wdw;
} kiss_combobox;

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

#define rk_pointinrect(x, y, rect) \
	(x >= ((rect)->x) && x < ((rect)->x) + ((rect)->w) && \
	 y >= ((rect)->y) && y < ((rect)->y) + ((rect)->h))

int rk_makerect (SDL_Rect *rect, int x, int y, int h, int w);
//int rk_pointinrect (int x, int y, SDL_Rect *rect);
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
int rk_font_init (RKFont *font, char *fname, kiss_array *a, int size);
RKFont *rk_font_new (char *fname, int size);
SDL_Renderer *rk_init (char *title, kiss_array *a, int w, int h);
int rk_clean (kiss_array *a);
int rk_window_init (kiss_window *window, kiss_window *wdw, int decorate, int x, int y, int w, int h);
int rk_window_event (kiss_window *window, SDL_Event *event, int *draw);
int rk_window_draw (kiss_window *window, SDL_Renderer *renderer);
int rk_label_init (RKLabel *label, kiss_window *wdw, char *text, int x, int y);
RKLabel *rk_label_new (kiss_window *wdw, char *text, int x, int y);
int rk_label_draw (RKLabel *label, SDL_Renderer *renderer);
int rk_button_init (kiss_button *button, kiss_window *wdw, char *text, int x, int y);
int rk_button_event (kiss_button *button, SDL_Event *event, int *draw);
int rk_button_draw (kiss_button *button, SDL_Renderer *renderer);
int rk_selectbutton_init (kiss_selectbutton *selectbutton, kiss_window *wdw, int x, int y);
int rk_selectbutton_event (kiss_selectbutton *selectbutton, SDL_Event *event, int *draw);
int rk_selectbutton_draw (kiss_selectbutton *selectbutton, SDL_Renderer *renderer);
int rk_vscrollbar_init (kiss_vscrollbar *vscrollbar, kiss_window *wdw, SDL_Rect *wheelrect, int x, int y, int h);
int rk_vscrollbar_event (kiss_vscrollbar *vscrollbar, SDL_Event *event,int *draw);
int rk_vscrollbar_draw (kiss_vscrollbar *vscrollbar, SDL_Renderer *renderer);
int rk_hscrollbar_init (kiss_hscrollbar *hscrollbar, kiss_window *wdw, int x, int y, int w);
int rk_hscrollbar_event (kiss_hscrollbar *hscrollbar, SDL_Event *event, int *draw);
int rk_hscrollbar_draw (kiss_hscrollbar *hscrollbar, SDL_Renderer *renderer);
int rk_progressbar_init (kiss_progressbar *progressbar, kiss_window *wdw, int x, int y, int w);
int rk_progressbar_event (kiss_progressbar *progressbar, SDL_Event *event, int *draw);
int rk_progressbar_draw (kiss_progressbar *progressbar, SDL_Renderer *renderer);
int rk_entry_init (kiss_entry *entry, kiss_window *wdw, int decorate, char *text, int x, int y, int w);
int rk_entry_event (kiss_entry *entry, SDL_Event *event, int *draw);
int rk_entry_draw (kiss_entry *entry, SDL_Renderer *renderer);
int rk_textbox_init (kiss_textbox *textbox, kiss_window *wdw, int decorate, RPVector *a, int x, int y, int w, int h);
int rk_textbox_event (kiss_textbox *textbox, SDL_Event *event, int *draw);
int rk_textbox_draw (kiss_textbox *textbox, SDL_Renderer *renderer);
int rk_combobox_init (kiss_combobox *combobox, kiss_window *wdw, char *text, RPVector *a, int x, int y, int w, int h);
int rk_combobox_event (kiss_combobox *combobox, SDL_Event *event,int *draw);
int rk_combobox_draw (kiss_combobox *combobox, SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif

#endif /* _kiss_sdl_h */

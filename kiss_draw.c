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

  kiss_sdl version 1.2.4
*/

#include "kiss_sdl.h"
#include <r_types.h>

RKFont kiss_textfont, kiss_buttonfont;
RKImage kiss_normal, kiss_prelight, kiss_active, kiss_bar;
RKImage kiss_up, kiss_down, kiss_left, kiss_right, kiss_vslider;
RKImage kiss_hslider, kiss_selected, kiss_unselected, kiss_combo;
int kiss_screen_width, kiss_screen_height;
int kiss_textfont_size = 15;
int kiss_buttonfont_size = 12;
int kiss_click_interval = 140;
int kiss_progress_interval = 50;
int kiss_slider_padding = 2;
int kiss_edge = 2;
int kiss_border = 6;
double kiss_spacing = 0.5;
SDL_Color kiss_white = { 255, 255, 255, 255 };
SDL_Color kiss_black = { 0, 0, 0, 255 };
SDL_Color kiss_green = { 0, 150, 0, 255 };
SDL_Color kiss_blue = { 0, 0, 255, 255 };
SDL_Color kiss_lightblue = { 200, 225, 255, 255 };

ut32 rk_getticks (void) {
	return SDL_GetTicks ();
}

/* Can be rewritten for proportional fonts */
int rk_maxlength (RKFont font, int width, char *str1, char *str2) {
	char buf[KISS_MAX_LENGTH];
	int n, i;

	n = 0;
	if (!str1 && !str2) {
		return -1;
	}
	rk_string_copy (buf, KISS_MAX_LENGTH, str1, str2);
	/* Maximum length + 1 for '\0', by the rule */
	for (i = 0; buf[i]; i += rk_utf8next (buf, i)) {
		if (++n * font.advance > width) {
			return i + 1;
		}
	}
	return i + 1;
}

/* Works also with proportional fonts */
int rk_textwidth (RKFont font, char *str1, char *str2) {
	char buf[KISS_MAX_LENGTH];
	int width;

	if (!str1 && !str2) {
		return -1;
	}
	rk_string_copy (buf, KISS_MAX_LENGTH, str1, str2);
	TTF_SizeUTF8 (font.font, buf, &width, NULL);
	return width;
}

int rk_renderimage (SDL_Renderer *renderer, RKImage image, int x, int y, SDL_Rect *clip) {
	SDL_Rect dst;

	if (!renderer || !image.image) {
		return -1;
	}
	rk_makerect (&dst, x, y, image.w, image.h);
	if (clip) {
		dst.w = clip->w;
		dst.h = clip->h;
	}
	SDL_RenderCopy (renderer, image.image, clip, &dst);
	return 0;
}

int rk_rendertext (SDL_Renderer *renderer, char *text, int x, int y, RKFont font, SDL_Color color) {
	SDL_Surface *surface;
	RKImage image;

	if (!text || !renderer || !font.font) {
		return -1;
	}
	surface = TTF_RenderUTF8_Blended (font.font, text, color);
	image.image = SDL_CreateTextureFromSurface (renderer, surface);
	SDL_QueryTexture (image.image, NULL, NULL, &image.w, &image.h);
	if (surface) {
		SDL_FreeSurface (surface);
	}
	rk_renderimage (renderer, image, x, y, NULL);
	SDL_DestroyTexture (image.image);
	return 0;
}

int rk_fillrect (SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color) {
	if (!renderer || !rect) {
		return -1;
	}
	SDL_SetRenderDrawColor (renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect (renderer, rect);
	return 0;
}

int rk_decorate (SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color,
	int edge) {
	SDL_Rect outlinerect;
	int d, i;

	d = 2 * edge;
	if (!renderer || !rect || rect->w < d + 6 || rect->h < d + 6) {
		return -1;
	}
	SDL_SetRenderDrawColor (renderer, color.r, color.g, color.b, color.a);
	for (i = 0; i < 2; i++) {
		rk_makerect (&outlinerect, rect->x + edge + i,
			rect->y + edge + i, rect->w - d - i - i,
			rect->h - d - i - i);
		SDL_RenderDrawRect (renderer, &outlinerect);
	}
	return 0;
}

int rk_image_init (RKImage *image, char *fname, kiss_array *a, SDL_Renderer *renderer) {
	char buf[KISS_MAX_LENGTH];

	if (!image || !fname) {
		return -1;
	}
	rk_string_copy (buf, KISS_MAX_LENGTH, RESDIR, fname);
	if (!(image->image = IMG_LoadTexture (renderer, buf))) {
		fprintf (stderr, "Cannot load image %s\n", fname);
		return -1;
	}
	if (a) {
		rk_array_append (a, TEXTURE_TYPE, image->image);
	}
	SDL_QueryTexture (image->image, NULL, NULL, &image->w, &image->h);
	image->magic = KISS_MAGIC;
	return 0;
}

RKImage *rk_image_new (char *fname, SDL_Renderer *renderer) {
	if (!fname || !renderer) {
		return NULL;
	}
	RKImage *img = R_NEW0 (RKImage);
	if (!img) {
		return NULL;
	}
	if (!rk_image_init (img, fname, NULL, renderer)) {
		R_FREE (img);
	}
	return img;
}

int rk_font_init (RKFont *font, char *fname, int size) {
	char buf[KISS_MAX_LENGTH];

	if (!font || !fname) {
		return -1;
	}
	rk_string_copy (buf, KISS_MAX_LENGTH, RESDIR, fname);
	if (!(font->font = TTF_OpenFont (buf, size))) {
		fprintf (stderr, "Cannot load font %s\n", fname);
		return -1;
	}
	font->fontheight = TTF_FontHeight (font->font);
	font->spacing = (int)kiss_spacing * font->fontheight;
	font->lineheight = font->fontheight + font->spacing;
	font->ascent = TTF_FontAscent (font->font);
	TTF_GlyphMetrics (font->font, 'W', NULL, NULL, NULL, NULL, &(font->advance));
	font->magic = KISS_MAGIC;
	return 0;
}

RKFont *rk_font_new (char *fname, int size) {
	if (!fname) {
		return NULL;
	}
	RKFont *font = R_NEW0 (RKFont);
	if (!font) {
		return NULL;
	}
	if (rk_font_init (font, fname, size)) {
		R_FREE (font);
	}
	return font;
}

SDL_Renderer *rk_init (char *title, kiss_array *a, int w, int h) {
	SDL_Renderer *renderer;
	SDL_Rect srect;
	int r = 0;

	SDL_Init (SDL_INIT_EVERYTHING);
	SDL_GetDisplayBounds (0, &srect);
	if (!a || w > srect.w || h > srect.h) {
		SDL_Quit ();
		return NULL;
	}
	kiss_screen_width = w;
	kiss_screen_height = h;
	IMG_Init (IMG_INIT_PNG);
	TTF_Init ();
	rk_array_init (a);
	SDL_Window *window = SDL_CreateWindow (title, srect.w / 2 - w / 2, srect.h / 2 - h / 2, w, h, SDL_WINDOW_SHOWN);
	if (window) {
		rk_array_append (a, WINDOW_TYPE, window);
	}
	renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer) {
		rk_array_append (a, RENDERER_TYPE, renderer);
	}
	if (rk_font_init (&kiss_textfont, "kiss_font.ttf", kiss_textfont_size)) {
		goto beach;
	}
	if (rk_font_init (&kiss_buttonfont, "kiss_font.ttf", kiss_buttonfont_size)) {
		TTF_CloseFont (kiss_textfont.font);
		return NULL;
	}
	r += rk_image_init (&kiss_normal, "kiss_normal.png", a, renderer);
	r += rk_image_init (&kiss_prelight, "kiss_prelight.png", a, renderer);
	r += rk_image_init (&kiss_active, "kiss_active.png", a, renderer);
	r += rk_image_init (&kiss_bar, "kiss_bar.png", a, renderer);
	r += rk_image_init (&kiss_vslider, "kiss_vslider.png", a, renderer);
	r += rk_image_init (&kiss_hslider, "kiss_hslider.png", a, renderer);
	r += rk_image_init (&kiss_up, "kiss_up.png", a, renderer);
	r += rk_image_init (&kiss_down, "kiss_down.png", a, renderer);
	r += rk_image_init (&kiss_left, "kiss_left.png", a, renderer);
	r += rk_image_init (&kiss_right, "kiss_right.png", a, renderer);
	r += rk_image_init (&kiss_combo, "kiss_combo.png", a, renderer);
	r += rk_image_init (&kiss_selected, "kiss_selected.png", a, renderer);
	r += rk_image_init (&kiss_unselected, "kiss_unselected.png", a, renderer);
	if (r) {
		TTF_CloseFont (kiss_textfont.font);
		TTF_CloseFont (kiss_buttonfont.font);
beach:
		rk_clean (a);
		TTF_Quit ();
		IMG_Quit ();
		SDL_Quit ();
		return NULL;
	}
	return renderer;
}

int rk_clean (kiss_array *a) {
	if (!a) {
		return -1;
	}
	if (a->length) {
		int i;
		for (i = a->length - 1; i >= 0; i--) {
			switch (rk_array_id (a, i)) {
			case TEXTURE_TYPE:
				SDL_DestroyTexture ((SDL_Texture *)rk_array_data (a, i));
				break;
			case RENDERER_TYPE:
				SDL_DestroyRenderer ((SDL_Renderer *)rk_array_data (a, i));
				break;
			case WINDOW_TYPE:
				SDL_DestroyWindow ((SDL_Window *)rk_array_data (a, i));
				break;
			default:
				free (a->data[i]);
			}
		}
	}
	a->length = 0;
	rk_array_free (a);
	return 0;
}

void rk_fini (kiss_array *a) {
	TTF_CloseFont (kiss_textfont.font);
	TTF_CloseFont (kiss_buttonfont.font);
	rk_clean (a);
	TTF_Quit ();
	IMG_Quit ();
	SDL_Quit ();
}

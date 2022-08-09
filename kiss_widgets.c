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

int rk_window_init (RKWindow *window, RKWindow *wdw, int decorate, int x, int y, int w, int h) {
	if (!window) {
		return -1;
	}
	window->bg = kiss_white;
	rk_makerect (window->rect, x, y, w, h);
	window->decorate = decorate;
	window->visible = 0;
	window->focus = 1;
	window->wdw = wdw;
	return 0;
}

RKWindow *rk_window_new (RKWindow *wdw, int decorate, int x, int y, int w, int h) {
	RKWindow *window = R_NEW0 (RKWindow);
	if (window) {
		if (rk_window_init (window, wdw, decorate, x, y, w, h)) {
			R_FREE (window);
		}
	}
	return window;
}

int rk_window_event (RKWindow *window, SDL_Event *event, int *draw) {
	if (!window || !window->visible || !event) {
		return 0;
	}
	*draw |= (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED);
	if (!window->focus && (!window->wdw || (window->wdw && !window->wdw->focus))) {
		return 0;
	}
	return (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &window->rect));
}

int rk_window_draw (RKWindow *window, SDL_Renderer *renderer) {
	if (window && window->wdw) {
		window->visible = window->wdw->visible;
	}
	if (!window || !window->visible || !renderer) {
		return 0;
	}
	rk_fillrect (renderer, &window->rect, window->bg);
	if (window->decorate) {
		rk_decorate (renderer, &window->rect, kiss_blue, kiss_edge);
	}
	return 1;
}

int rk_label_init (RKLabel *label, RKWindow *wdw, char *text, int x, int y) {
	if (!label || !text) {
		return -1;
	}
	if (label->font.magic != KISS_MAGIC) {
		label->font = kiss_textfont;
	}
	label->textcolor = kiss_black;
	rk_makerect (label->rect, x, y, 0, 0);
	rk_string_copy (label->text, KISS_MAX_LABEL, text, NULL);
	label->visible = 0;
	label->wdw = wdw;
	return 0;
}

RKLabel *rk_label_new (RKWindow *wdw, char *text, int x, int y) {
	if (!text) {
		return NULL;
	}
	RKLabel *label = R_NEW0(RKLabel);
	if (!label) {
		return NULL;
	}
	rk_label_init (label, wdw, text, x, y);
	return label;
}

int rk_label_draw (RKLabel *label, SDL_Renderer *renderer) {
	char buf[KISS_MAX_LABEL], *p;

	if (label && label->wdw) {
		label->visible = label->wdw->visible;
	}
	if (!label || !label->visible || !renderer) {
		return 0;
	}
	int y = label->rect.y + label->font.spacing / 2;
	int len = strlen (label->text);
	if (len > KISS_MAX_LABEL - 2) {
		label->text[len - 1] = '\n';
	} else {
		strcat (label->text, "\n");
	}
	for (p = label->text; *p; p = strchr (p, '\n') + 1) {
		rk_string_copy (buf, strcspn (p, "\n") + 1, p, NULL);
		rk_rendertext (renderer, buf, label->rect.x, y, label->font, label->textcolor);
		y += label->font.lineheight;
	}
	label->text[len] = 0;
	return 1;
}

int rk_button_init (RKButton *button, RKWindow *wdw, char *text, int x, int y) {
	if (!button || !text) {
		return -1;
	}
	if (button->font.magic != KISS_MAGIC) {
		button->font = kiss_buttonfont;
	}
	if (button->normalimg.magic != KISS_MAGIC) {
		button->normalimg = kiss_normal;
	}
	if (button->activeimg.magic != KISS_MAGIC) {
		button->activeimg = kiss_active;
	}
	if (button->prelightimg.magic != KISS_MAGIC) {
		button->prelightimg = kiss_prelight;
	}
	rk_makerect (button->rect, x, y, button->normalimg.w, button->normalimg.h);
	button->textcolor = kiss_white;
	rk_string_copy (button->text, KISS_MAX_LENGTH, text, NULL);
	button->textx = x + button->normalimg.w / 2 -
		rk_textwidth (button->font, text, NULL) / 2;
	button->texty = y + button->normalimg.h / 2 -
		button->font.fontheight / 2;
	button->active = 0;
	button->prelight = 0;
	button->visible = 0;
	button->focus = 0;
	button->wdw = wdw;
	return 0;
}

RKButton *rk_button_new(RKWindow *wdw, char *text, int x, int y) {
	if (!text) {
		return NULL;
	}
	RKButton *button = R_NEW0 (RKButton);
	if (!button) {
		return NULL;
	}
	if (rk_button_init (button, wdw, text, x, y)) {
		free (button);
		return NULL;
	}
	return button;
}

int rk_button_event (RKButton *button, SDL_Event *event, int *draw) {
	if (!button || !button->visible || !event) {
		return 0;
	}
	*draw |= (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED);
	if (!button->focus && (!button->wdw ||
		(button->wdw && !button->wdw->focus))) {
		return 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &button->rect)) {
		button->active = 1;
		*draw = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP &&
		rk_pointinrect (event->button.x, 
		event->button.y, &button->rect) && button->active) {
		button->active = 0;
		*draw = 1;
		return 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		rk_pointinrect (event->motion.x, event->motion.y, &button->rect)) {
		button->prelight = 1;
		*draw = 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		!rk_pointinrect (event->motion.x, event->motion.y, &button->rect)) {
		button->prelight = 0;
		*draw = 1;
		if (button->active) {
			button->active = 0;
			return 1;
		}
	}
	return 0;
}

int rk_button_draw (RKButton *button, SDL_Renderer *renderer) {
	if (button && button->wdw) {
		button->visible = button->wdw->visible;
	}
	if (!button || !button->visible || !renderer) {
		return 0;
	}
	if (button->active) {
		rk_renderimage (renderer, button->activeimg,
			button->rect.x, button->rect.y, NULL);
	} else if (button->prelight && !button->active) {
		rk_renderimage (renderer, button->prelightimg,
			button->rect.x, button->rect.y, NULL);
	} else {
		rk_renderimage (renderer, button->normalimg,
			button->rect.x, button->rect.y, NULL);
	}
	rk_rendertext (renderer, button->text, button->textx, button->texty,
		button->font, button->textcolor);
	return 1;
}

int rk_selectbutton_init (RKSelectButton *selectbutton, RKWindow *wdw, int x, int y) {
	if (!selectbutton) {
		return -1;
	}
	if (selectbutton->selectedimg.magic != KISS_MAGIC) {
		selectbutton->selectedimg = kiss_selected;
	}
	if (selectbutton->unselectedimg.magic != KISS_MAGIC) {
		selectbutton->unselectedimg = kiss_unselected;
	}
	rk_makerect (selectbutton->rect, x, y,
		selectbutton->selectedimg.w, selectbutton->selectedimg.h);
	selectbutton->selected = 0;
	selectbutton->focus = 0;
	selectbutton->wdw = wdw;
	return 0;
}

RKSelectButton *rk_selectbutton_new (RKWindow *wdw, int x, int y) {
	RKSelectButton *selectbutton = R_NEW0 (RKSelectButton);
	if (!selectbutton) {
		return NULL;
	}
	if (rk_selectbutton_init (selectbutton, wdw, x, y)) {
		free (selectbutton);
		return NULL;
	}
	return selectbutton;
}

int rk_selectbutton_event (RKSelectButton *selectbutton, SDL_Event *event, int *draw) {
	if (!selectbutton || !selectbutton->visible || !event) {
		return 0;
	}
	*draw |= (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED);
	if (!selectbutton->focus &&
		(!selectbutton->wdw || (selectbutton->wdw && !selectbutton->wdw->focus))) {
		return 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &selectbutton->rect)) {
		selectbutton->selected ^= 1;
		*draw = 1;
		return 1;
	}
	return 0;
}

int rk_selectbutton_draw (RKSelectButton *selectbutton, SDL_Renderer *renderer) {
	if (selectbutton && selectbutton->wdw) {
		selectbutton->visible = selectbutton->wdw->visible;
	}
	if (!selectbutton || !selectbutton->visible || !renderer) {
		return 0;
	}
	if (selectbutton->selected) {
		rk_renderimage (renderer, selectbutton->selectedimg,
			selectbutton->rect.x, selectbutton->rect.y, NULL);
	} else {
		rk_renderimage (renderer, selectbutton->unselectedimg,
			selectbutton->rect.x, selectbutton->rect.y, NULL);
	}
	return 1;
}

int rk_vscrollbar_init (RKVScrollbar *vscrollbar, RKWindow *wdw, SDL_Rect *wheelrect, int x, int y, int h) {
	if (!vscrollbar) {
		return -1;
	}
	if (vscrollbar->up.magic != KISS_MAGIC) {
		vscrollbar->up = kiss_up;
	}
	if (vscrollbar->down.magic != KISS_MAGIC) {
		vscrollbar->down = kiss_down;
	}
	if (vscrollbar->vslider.magic != KISS_MAGIC) {
		vscrollbar->vslider = kiss_vslider;
	}
	if (vscrollbar->up.h + vscrollbar->down.h + 2 * kiss_edge +
		2 * kiss_slider_padding + vscrollbar->vslider.h > h) {
		return -1;
	}
	rk_makerect (vscrollbar->uprect, x, y + kiss_edge,
		vscrollbar->up.w, vscrollbar->up.h + kiss_slider_padding);
	rk_makerect (vscrollbar->downrect, x,
		y + h - vscrollbar->down.h - kiss_slider_padding - kiss_edge,
		vscrollbar->down.w, vscrollbar->down.h + kiss_slider_padding);
	rk_makerect (vscrollbar->sliderrect, x, y + vscrollbar->uprect.h + kiss_edge,
		vscrollbar->vslider.w, vscrollbar->vslider.h);
	vscrollbar->wheelrect = wheelrect;
	vscrollbar->maxpos = h - 2 * kiss_slider_padding - 2 * kiss_edge -
		vscrollbar->up.h - vscrollbar->down.h - vscrollbar->vslider.h;
	vscrollbar->fraction = 0.;
	vscrollbar->step = 0.1;
	vscrollbar->upclicked = 0;
	vscrollbar->downclicked = 0;
	vscrollbar->sliderclicked = 0;
	vscrollbar->lasttick = 0;
	vscrollbar->visible = 0;
	vscrollbar->focus = 0;
	vscrollbar->wdw = wdw;
	return 0;
}

RKVScrollbar *rk_vscrollbar_new (RKWindow *wdw, SDL_Rect *wheelrect, int x, int y, int h) {
	RKVScrollbar *vsb = R_NEW0 (RKVScrollbar);
	if (vsb) {
		if (rk_vscrollbar_init (vsb, wdw, wheelrect, x, y, h)) {
			R_FREE (vsb);
		}
	}
	return vsb;
}

static void vnewpos(RKVScrollbar *vscrollbar, double step, int *draw) {
	*draw = 1;
	vscrollbar->fraction += step;
	vscrollbar->lasttick = rk_getticks ();
	if (vscrollbar->fraction < -0.000001) {
		vscrollbar->fraction = 0.;
	}
	if (vscrollbar->fraction > 0.999999) {
		vscrollbar->fraction = 1.;
	}
	vscrollbar->sliderrect.y = vscrollbar->uprect.y + vscrollbar->uprect.h +
		(int)(vscrollbar->fraction * vscrollbar->maxpos + 0.5);
	if (vscrollbar->fraction > 0.000001 && vscrollbar->fraction < 0.999999) {
		return;
	}
	vscrollbar->upclicked = 0;
	vscrollbar->downclicked = 0;
}

int rk_vscrollbar_event (RKVScrollbar *vscrollbar, SDL_Event *event, int *draw) {
	if (!vscrollbar || !vscrollbar->visible) {
		return 0;
	}
	if (!(SDL_GetMouseState (NULL, NULL) & SDL_BUTTON (SDL_BUTTON_LEFT))) {
		vscrollbar->upclicked = 0;
		vscrollbar->downclicked = 0;
		vscrollbar->lasttick = 0;
	} else if (vscrollbar->upclicked && rk_getticks () > vscrollbar->lasttick + kiss_click_interval) {
		vnewpos (vscrollbar, -vscrollbar->step, draw);
		return 1;
	} else if (vscrollbar->downclicked && rk_getticks () > vscrollbar->lasttick + kiss_click_interval) {
		vnewpos (vscrollbar, vscrollbar->step, draw);
		return 1;
	}
	if (!event) {
		return 0;
	}
	*draw |= (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED);
	if (!vscrollbar->focus && (!vscrollbar->wdw || (vscrollbar->wdw && !vscrollbar->wdw->focus))) {
		return 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &vscrollbar->uprect) &&
		vscrollbar->step > 0.000001) {
		if (vscrollbar->fraction > 0.000001) {
			vscrollbar->upclicked = 1;
			if (vscrollbar->wdw) {
				vscrollbar->wdw->focus = 0;
			}
			vscrollbar->focus = 1;
		}
		vscrollbar->lasttick = rk_getticks () - kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &vscrollbar->downrect) &&
		vscrollbar->step > 0.000001) {
		if (vscrollbar->fraction < 0.999999) {
			vscrollbar->downclicked = 1;
			if (vscrollbar->wdw) {
				vscrollbar->wdw->focus = 0;
			}
			vscrollbar->focus = 1;
		}
		vscrollbar->lasttick = rk_getticks () - kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &vscrollbar->sliderrect) &&
		vscrollbar->step > 0.000001) {
		if (vscrollbar->wdw) {
			vscrollbar->wdw->focus = 0;
		}
		vscrollbar->focus = 1;
		vscrollbar->sliderclicked = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP) {
		vscrollbar->upclicked = 0;
		vscrollbar->downclicked = 0;
		vscrollbar->lasttick = 0;
		if (vscrollbar->wdw) {
			vscrollbar->wdw->focus = 1;
		}
		vscrollbar->focus = 0;
		vscrollbar->sliderclicked = 0;
	} else if (event->type == SDL_MOUSEMOTION && 
		(event->motion.state & SDL_BUTTON (SDL_BUTTON_LEFT)) && vscrollbar->sliderclicked) {
		vnewpos (vscrollbar, 1. * event->motion.yrel / vscrollbar->maxpos, draw);
		return 1;
	} else if (event->type == SDL_MOUSEWHEEL && vscrollbar->wheelrect) {
		int x, y;
		SDL_GetMouseState (&x, &y);
		if (!rk_pointinrect (x, y, vscrollbar->wheelrect)) {
			return 0;
		}
		if (event->wheel.y > 0) {
			vnewpos (vscrollbar, -vscrollbar->step, draw);
			return 1;
		}
		if (event->wheel.y < 0) {
			vnewpos (vscrollbar, vscrollbar->step, draw);
			return 1;
		}
	}
	return 0;
}

int rk_vscrollbar_draw (RKVScrollbar *vscrollbar, SDL_Renderer *renderer) {
	if (vscrollbar && vscrollbar->wdw) {
		vscrollbar->visible = vscrollbar->wdw->visible;
	}
	if (!vscrollbar || !vscrollbar->visible || !renderer) {
		return 0;
	}
	vscrollbar->sliderrect.y = vscrollbar->uprect.y + vscrollbar->uprect.h +
		(int)(vscrollbar->fraction * vscrollbar->maxpos);
	rk_renderimage (renderer, vscrollbar->up, vscrollbar->uprect.x, vscrollbar->uprect.y, NULL);
	rk_renderimage (renderer, vscrollbar->down, vscrollbar->downrect.x,
		vscrollbar->downrect.y + kiss_slider_padding, NULL);
	rk_renderimage (renderer, vscrollbar->vslider,
		vscrollbar->sliderrect.x, vscrollbar->sliderrect.y, NULL);
	return 1;
}

int rk_hscrollbar_init (RKHScrollbar *hscrollbar, RKWindow *wdw, int x, int y, int w) {
	if (!hscrollbar) {
		return -1;
	}
	if (hscrollbar->left.magic != KISS_MAGIC) {
		hscrollbar->left = kiss_left;
	}
	if (hscrollbar->right.magic != KISS_MAGIC) {
		hscrollbar->right = kiss_right;
	}
	if (hscrollbar->hslider.magic != KISS_MAGIC) {
		hscrollbar->hslider = kiss_hslider;
	}
	if (hscrollbar->left.w + hscrollbar->right.w + 2 * kiss_edge +
		2 * kiss_slider_padding + hscrollbar->hslider.w > w) {
		return -1;
	}
	rk_makerect (hscrollbar->leftrect, x + kiss_edge, y,
		hscrollbar->left.w + kiss_slider_padding, hscrollbar->left.h);
	rk_makerect (hscrollbar->rightrect, x + w - hscrollbar->right.w - kiss_slider_padding - kiss_edge,
		y, hscrollbar->right.w + kiss_slider_padding, hscrollbar->right.h);
	rk_makerect (hscrollbar->sliderrect, x + hscrollbar->leftrect.w + kiss_edge,
		y, hscrollbar->hslider.w, hscrollbar->hslider.h);
	hscrollbar->maxpos = w - 2 * kiss_slider_padding - 2 * kiss_edge -
		hscrollbar->left.w - hscrollbar->right.w - hscrollbar->hslider.w;
	hscrollbar->fraction = 0.;
	hscrollbar->step = 0.1;
	hscrollbar->leftclicked = 0;
	hscrollbar->rightclicked = 0;
	hscrollbar->sliderclicked = 0;
	hscrollbar->lasttick = 0;
	hscrollbar->visible = 0;
	hscrollbar->focus = 0;
	hscrollbar->wdw = wdw;
	return 0;
}

RKHScrollbar *rk_hscrollbar_new (RKWindow *wdw, int x, int y, int w) {
	RKHScrollbar *hsb = R_NEW0 (RKHScrollbar);
	if (hsb) {
		if (rk_hscrollbar_init (hsb, wdw, x, y, w)) {
			R_FREE (hsb);
		}
	}
	return hsb;
}

static void hnewpos(RKHScrollbar *hscrollbar, double step, int *draw) {
	*draw = 1;
	hscrollbar->fraction += step;
	hscrollbar->lasttick = rk_getticks ();
	if (hscrollbar->fraction < -0.000001) {
		hscrollbar->fraction = 0.;
	}
	if (hscrollbar->fraction > 0.999999) {
		hscrollbar->fraction = 1.;
	}
	hscrollbar->sliderrect.x = hscrollbar->leftrect.x + hscrollbar->leftrect.w +
		(int)(hscrollbar->fraction * hscrollbar->maxpos + 0.5);
	if (hscrollbar->fraction > 0.000001 && hscrollbar->fraction < 0.999999) {
		return;
	}
	hscrollbar->leftclicked = 0;
	hscrollbar->rightclicked = 0;
}

int rk_hscrollbar_event (RKHScrollbar *hscrollbar, SDL_Event *event, int *draw) {
	if (!hscrollbar || !hscrollbar->visible) {
		return 0;
	}
	if (!(SDL_GetMouseState (NULL, NULL) & SDL_BUTTON (SDL_BUTTON_LEFT))) {
		hscrollbar->leftclicked = 0;
		hscrollbar->rightclicked = 0;
		hscrollbar->lasttick = 0;
	} else if (hscrollbar->leftclicked && rk_getticks () > hscrollbar->lasttick + kiss_click_interval) {
		hnewpos (hscrollbar, -hscrollbar->step, draw);
		return 1;
	} else if (hscrollbar->rightclicked && rk_getticks () > hscrollbar->lasttick + kiss_click_interval) {
		hnewpos (hscrollbar, hscrollbar->step, draw);
		return 1;
	}
	if (!event) {
		return 0;
	}
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED) {
		*draw = 1;
	}
	if (!hscrollbar->focus && (!hscrollbar->wdw || (hscrollbar->wdw && !hscrollbar->wdw->focus))) {
		return 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &hscrollbar->leftrect)) {
		if (hscrollbar->fraction > 0.000001) {
			hscrollbar->leftclicked = 1;
			if (hscrollbar->wdw)
				hscrollbar->wdw->focus = 0;
			hscrollbar->focus = 1;
		}
		hscrollbar->lasttick = rk_getticks () - kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &hscrollbar->rightrect) &&
		hscrollbar->step > 0.000001) {
		if (hscrollbar->fraction < 0.999999) {
			hscrollbar->rightclicked = 1;
			if (hscrollbar->wdw) {
				hscrollbar->wdw->focus = 0;
			}
			hscrollbar->focus = 1;
		}
		hscrollbar->lasttick = rk_getticks () - kiss_click_interval - 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &hscrollbar->sliderrect) &&
		hscrollbar->step > 0.000001) {
		if (hscrollbar->wdw) {
			hscrollbar->wdw->focus = 0;
		}
		hscrollbar->focus = 1;
		hscrollbar->sliderclicked = 1;
	} else if (event->type == SDL_MOUSEBUTTONUP) {
		hscrollbar->leftclicked = 0;
		hscrollbar->rightclicked = 0;
		hscrollbar->lasttick = 0;
		if (hscrollbar->wdw) {
			hscrollbar->wdw->focus = 1;
		}
		hscrollbar->focus = 0;
		hscrollbar->sliderclicked = 0;
	} else if (event->type == SDL_MOUSEMOTION &&
		(event->motion.state & SDL_BUTTON (SDL_BUTTON_LEFT)) && hscrollbar->sliderclicked) {
		hnewpos (hscrollbar, 1. * event->motion.xrel / hscrollbar->maxpos, draw);
		return 1;
	}
	return 0;
}

int rk_hscrollbar_draw (RKHScrollbar *hscrollbar, SDL_Renderer *renderer) {
	if (hscrollbar && hscrollbar->wdw) {
		hscrollbar->visible = hscrollbar->wdw->visible;
	}
	if (!hscrollbar || !hscrollbar->visible || !renderer) {
		return 0;
	}
	hscrollbar->sliderrect.x = hscrollbar->leftrect.x + hscrollbar->leftrect.w +
		(int)(hscrollbar->fraction * hscrollbar->maxpos);
	rk_renderimage (renderer, hscrollbar->left, hscrollbar->leftrect.x,
		hscrollbar->leftrect.y, NULL);
	rk_renderimage (renderer, hscrollbar->right,
		hscrollbar->rightrect.x + kiss_slider_padding,
		hscrollbar->rightrect.y, NULL);
	rk_renderimage (renderer, hscrollbar->hslider,
		hscrollbar->sliderrect.x, hscrollbar->sliderrect.y, NULL);
	return 1;
}

int rk_progressbar_init (RKProgressbar *progressbar, RKWindow *wdw, int x, int y, int w) {
	if (!progressbar || w < 2 * kiss_border + 1) {
		return -1;
	}
	if (progressbar->bar.magic != KISS_MAGIC) {
		progressbar->bar = kiss_bar;
	}
	progressbar->bg = kiss_white;
	rk_makerect (progressbar->rect, x, y, w,
		progressbar->bar.h + 2 * kiss_border);
	rk_makerect (progressbar->barrect, x + kiss_border,
		y + kiss_border, 0, progressbar->bar.h);
	progressbar->width = w - 2 * kiss_border;
	progressbar->fraction = 0.;
	progressbar->step = 0.02;
	progressbar->lasttick = 0;
	progressbar->run = 0;
	progressbar->visible = 0;
	progressbar->wdw = wdw;
	return 0;
}

RKProgressbar *rk_progressbar_new (RKWindow *wdw, int x, int y, int w) {
	RKProgressbar *pbar = R_NEW0 (RKProgressbar);
	if (pbar) {
		if (rk_progressbar_init (pbar, wdw, x, y, w)) {
			R_FREE (pbar);
		}
	}
	return pbar;
}

int rk_progressbar_event (RKProgressbar *progressbar, SDL_Event *event, int *draw) {
	if (!progressbar || !progressbar->visible) {
		return 0;
	}
	if (progressbar->run && rk_getticks () > progressbar->lasttick + kiss_progress_interval) {
		progressbar->fraction += progressbar->step;
		if (progressbar->fraction > 0.999999) {
			progressbar->run = 0;
			progressbar->fraction = 1.;
		}
		progressbar->lasttick = rk_getticks ();
		*draw = 1;
	}
	return 1;
}

int rk_progressbar_draw (RKProgressbar *progressbar, SDL_Renderer *renderer) {
	SDL_Rect clip;

	if (progressbar && progressbar->wdw) {
		progressbar->visible = progressbar->wdw->visible;
	}
	if (!progressbar || !progressbar->visible || !renderer) {
		return 0;
	}
	rk_fillrect (renderer, &progressbar->rect, progressbar->bg);
	rk_decorate (renderer, &progressbar->rect, kiss_blue, kiss_edge);
	progressbar->barrect.w = (int)(progressbar->width * progressbar->fraction + 0.5);
	rk_makerect (clip, 0, 0, progressbar->barrect.w, progressbar->barrect.h);
	rk_renderimage (renderer, progressbar->bar, progressbar->barrect.x,
		progressbar->barrect.y, &clip);
	return 1;
}

int rk_entry_init (kiss_entry *entry, RKWindow *wdw, int decorate, char *text, int x, int y, int w) {
	if (!entry || !text) {
		return -1;
	}
	if (entry->font.magic != KISS_MAGIC) {
		entry->font = kiss_textfont;
	}
	if (w < 2 * kiss_border + entry->font.advance) {
		return -1;
	}
	entry->bg = kiss_white;
	entry->normalcolor = kiss_black;
	entry->activecolor = kiss_blue;
	entry->textwidth = w - 2 * kiss_border;
	rk_string_copy (entry->text, rk_maxlength (entry->font, entry->textwidth, text, NULL), text, NULL);
	rk_makerect (entry->rect, x, y, w, entry->font.fontheight + 2 * kiss_border);
	entry->decorate = decorate;
	entry->textx = x + kiss_border;
	entry->texty = y + kiss_border;
	entry->active = 0;
	entry->visible = 0;
	entry->focus = 0;
	entry->wdw = wdw;
	return 0;
}

int rk_entry_event (kiss_entry *entry, SDL_Event *event, int *draw) {
	if (!entry || !entry->visible || !event) {
		return 0;
	}
	*draw |= (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED);
	if (!entry->focus && (!entry->wdw || (entry->wdw && !entry->wdw->focus))) {
		return 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN && !entry->active &&
		rk_pointinrect (event->button.x, event->button.y, &entry->rect)) {
		entry->active = 1;
		SDL_StartTextInput ();
		if (entry->wdw) {
			entry->wdw->focus = 0;
		}
		entry->focus = 1;
		*draw = 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
		entry->active = 0;
		SDL_StopTextInput ();
		if (entry->wdw) {
			entry->wdw->focus = 1;
		}
		entry->focus = 0;
		*draw = 1;
		return 1;
	} else if (event->type == SDL_TEXTINPUT && entry->active) {
		if (rk_textwidth (entry->font, entry->text,
			event->text.text) < entry->textwidth &&
			strlen (entry->text) + strlen (event->text.text) < KISS_MAX_LENGTH) {
			strcat (entry->text, event->text.text);
		}
		*draw = 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
		rk_backspace (entry->text);
		*draw = 1;
	} else if (event->type == SDL_KEYDOWN && entry->active &&
		(event->key.keysym.mod & KMOD_CTRL) && event->key.keysym.scancode == SDL_SCANCODE_U) {
		strcpy (entry->text, "");
		*draw = 1;
	} else if (event->type == SDL_MOUSEBUTTONDOWN && entry->active &&
		rk_pointinrect (event->button.x, event->button.y, &entry->rect)) {
		strcpy (entry->text, "");
		*draw = 1;
	}
	return 0;
}

int rk_entry_draw (kiss_entry *entry, SDL_Renderer *renderer) {
	SDL_Color color;

	if (entry && entry->wdw) {
		entry->visible = entry->wdw->visible;
	}
	if (!entry || !entry->visible || !renderer) {
		return 0;
	}
	rk_fillrect (renderer, &entry->rect, entry->bg);
	color = kiss_blue;
	if (entry->active) {
		color = kiss_green;
	}
	if (entry->decorate) {
		rk_decorate (renderer, &entry->rect, color, kiss_edge);
	}
	color = entry->normalcolor;
	if (entry->active) {
		color = entry->activecolor;
	}
	rk_rendertext (renderer, entry->text, entry->textx, entry->texty, entry->font, color);
	return 1;
}

int rk_textbox_init (RKTextbox *textbox, RKWindow *wdw, int decorate,
	RPVector *lines, int x, int y, int w, int h) {
	if (!textbox || !lines) {
		return -1;
	}
	if (textbox->font.magic != KISS_MAGIC) {
		textbox->font = kiss_textfont;
	}
	if (h - 2 * kiss_border < textbox->font.lineheight) {
		return -1;
	}
	textbox->bg = kiss_white;
	textbox->textcolor = kiss_black;
	textbox->hlcolor = kiss_lightblue;
	rk_makerect (textbox->rect, x, y, w, h);
	rk_makerect (textbox->textrect, x + kiss_border,
		y + kiss_border, w - 2 * kiss_border, h - 2 * kiss_border);
	textbox->decorate = decorate;
	textbox->lines = lines;
	textbox->firstline = 0;
	textbox->maxlines = (h - 2 * kiss_border) / textbox->font.lineheight;
	textbox->textwidth = w - 2 * kiss_border;
	textbox->highlightline = -1;
	textbox->selectedline = -1;
	textbox->visible = 0;
	textbox->focus = 0;
	textbox->wdw = wdw;
	return 0;
}

static int textbox_numoflines(RKTextbox *textbox) {
	int numoflines = textbox->maxlines;
	if (((int)r_pvector_len (textbox->lines)) - textbox->firstline < textbox->maxlines) {
		numoflines = ((int)r_pvector_len (textbox->lines)) - textbox->firstline;
	}
	return numoflines;
}

int rk_textbox_event (RKTextbox *textbox, SDL_Event *event, int *draw) {
	int texty, textmaxy, numoflines;

	if (!textbox || !textbox->visible || !event || !textbox->lines || !r_pvector_len (textbox->lines)) {
		return 0;
	}
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED) {
		*draw = 1;
	}
	if (!textbox->focus && (!textbox->wdw || (textbox->wdw && !textbox->wdw->focus))) {
		return 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &textbox->textrect)) {
		numoflines = textbox_numoflines (textbox);
		texty = event->button.y - textbox->textrect.y;
		textmaxy = numoflines * textbox->font.lineheight;
		if (texty < textmaxy) {
			textbox->selectedline = texty / textbox->font.lineheight;
			return 1;
		}
	} else if (event->type == SDL_MOUSEMOTION &&
		rk_pointinrect (event->motion.x, event->motion.y, &textbox->textrect)) {
		numoflines = textbox_numoflines (textbox);
		texty = event->motion.y - textbox->textrect.y;
		textmaxy = numoflines * textbox->font.lineheight;
		textbox->highlightline = -1;
		if (texty < textmaxy) {
			textbox->highlightline = texty / textbox->font.lineheight;
		}
		*draw = 1;
	} else if (event->type == SDL_MOUSEMOTION &&
		!rk_pointinrect (event->motion.x, event->motion.y, &textbox->textrect)) {
		textbox->highlightline = -1;
		*draw = 1;
	}
	return 0;
}

int rk_textbox_draw (RKTextbox *textbox, SDL_Renderer *renderer) {
	SDL_Rect highlightrect;
	char buf[KISS_MAX_LENGTH];
	int numoflines, i;

	if (textbox && textbox->wdw) {
		textbox->visible = textbox->wdw->visible;
	}
	if (!textbox || !textbox->visible || !renderer) {
		return 0;
	}
	rk_fillrect (renderer, &textbox->rect, textbox->bg);
	if (textbox->decorate) {
		rk_decorate (renderer, &textbox->rect, kiss_blue, kiss_edge);
	}
	if (textbox->highlightline >= 0) {
		rk_makerect (highlightrect, textbox->textrect.x,
			textbox->textrect.y + textbox->highlightline * textbox->font.lineheight,
			textbox->textrect.w, textbox->font.lineheight);
		rk_fillrect (renderer, &highlightrect, textbox->hlcolor);
	}
	if (!textbox->lines || !r_pvector_len (textbox->lines)) {
		return 0;
	}
	numoflines = textbox_numoflines (textbox);
	for (i = 0; i < numoflines; i++) {
		rk_string_copy (buf, rk_maxlength (textbox->font, textbox->textwidth,
			(char *)r_pvector_at (textbox->lines, textbox->firstline + i), NULL),
			(char *)r_pvector_at (textbox->lines, textbox->firstline + i), NULL);
		rk_rendertext (renderer, buf, textbox->textrect.x,
			textbox->textrect.y + i * textbox->font.lineheight + textbox->font.spacing / 2,
			textbox->font, textbox->textcolor);
	}
	return 1;
}

int rk_combobox_init (RKCombobox *combobox, RKWindow *wdw,
	char *text, RPVector *a, int x, int y, int w, int h) {
	if (!combobox || !a || !text) {
		return -1;
	}
	if (combobox->combo.magic != KISS_MAGIC) {
		combobox->combo = kiss_combo;
	}
	rk_entry_init (&combobox->entry, wdw, 1, text, x, y, w);
	strcpy (combobox->text, combobox->entry.text);
	rk_window_init (&combobox->window, NULL, 0, x,
		y + combobox->entry.rect.h, w + combobox->vscrollbar.up.w, h);
	if (rk_textbox_init (&combobox->textbox, &combobox->window, 1,
		a, x, y + combobox->entry.rect.h, w, h) == -1) {
		return -1;
	}
	if (rk_vscrollbar_init (&combobox->vscrollbar, &combobox->window, &combobox->window.rect,
		x + combobox->textbox.rect.w, combobox->textbox.rect.y, combobox->textbox.rect.h) == -1) {
		return -1;
	}
	combobox->visible = 0;
	combobox->wdw = wdw;
	combobox->vscrollbar.step = 0.;
	if (((int)r_pvector_len (combobox->textbox.lines)) - combobox->textbox.maxlines > 0) {
		combobox->vscrollbar.step = 1. /
			(((int)r_pvector_len (combobox->textbox.lines)) - combobox->textbox.maxlines);
	}
	return 0;
}

int rk_combobox_event (RKCombobox *combobox, SDL_Event *event, int *draw) {
	int firstline, index;

	if (!combobox || !combobox->visible) {
		return 0;
	}
	if (rk_vscrollbar_event (&combobox->vscrollbar, event, draw) &&
		((int)r_pvector_len (combobox->textbox.lines)) - combobox->textbox.maxlines >= 0) {
		combobox->vscrollbar.step = 0.;
		if (((int)r_pvector_len (combobox->textbox.lines)) - combobox->textbox.maxlines > 0) {
			combobox->vscrollbar.step = 1. /
				(((int)r_pvector_len (combobox->textbox.lines)) - combobox->textbox.maxlines);
		}
		firstline = (int)(((int)r_pvector_len (combobox->textbox.lines) -
			combobox->textbox.maxlines) * combobox->vscrollbar.fraction + 0.5);
		if (firstline >= 0) {
			combobox->textbox.firstline = firstline;
		}
		*draw = 1;
	}
	if (!event) {
		return 0;
	}
	*draw |= (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED);
	if (event->type == SDL_MOUSEBUTTONDOWN &&
		rk_pointinrect (event->button.x, event->button.y, &combobox->entry.rect)) {
		combobox->window.visible = 1;
		*draw = 1;
	}
	if (rk_entry_event (&combobox->entry, event, draw)) {
		combobox->window.visible = 0;
		strcpy (combobox->text, combobox->entry.text);
		*draw = 1;
		SDL_StopTextInput ();
		return 1;
	} else if (rk_textbox_event (&combobox->textbox, event, draw)) {
		combobox->window.visible = 0;
		combobox->entry.active = 0;
		if (combobox->entry.wdw) {
			combobox->entry.wdw->focus = 1;
		}
		combobox->entry.focus = 0;
		index = combobox->textbox.firstline + combobox->textbox.selectedline;
		rk_string_copy (combobox->entry.text,
			rk_maxlength (combobox->textbox.font, combobox->entry.textwidth,
				(char *)r_pvector_at (combobox->textbox.lines, index), NULL),
			(char *)r_pvector_at (combobox->textbox.lines, index), NULL);
		*draw = 1;
		SDL_StopTextInput ();
		return 1;
	}
	return 0;
}

int rk_combobox_draw (RKCombobox *combobox, SDL_Renderer *renderer) {
	if (combobox && combobox->wdw) {
		combobox->visible = combobox->wdw->visible;
	}
	if (!combobox || !combobox->visible || !renderer) {
		return 0;
	}
	rk_renderimage (renderer, combobox->combo, combobox->entry.rect.x + combobox->entry.rect.w,
		combobox->entry.rect.y + combobox->entry.rect.h - combobox->combo.h - kiss_edge, NULL);
	rk_entry_draw (&combobox->entry, renderer);
	rk_window_draw (&combobox->window, renderer);
	rk_vscrollbar_draw (&combobox->vscrollbar, renderer);
	rk_textbox_draw (&combobox->textbox, renderer);
	return 1;
}

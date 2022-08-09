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

#include "kiss_sdl.h"
#include <r_util.h>

static void text_reset(RKTextbox *textbox, RKVScrollbar *vscrollbar) {
	r_pvector_sort (textbox->lines, (RPVectorComparator)strcmp);
	vscrollbar->step = 0.;
	if (((int)r_pvector_len (textbox->lines)) - textbox->maxlines > 0) {
		vscrollbar->step = 1. / (((int)r_pvector_len (textbox->lines)) - textbox->maxlines);
	}
	textbox->firstline = 0;
	textbox->highlightline = -1;
	vscrollbar->fraction = 0.;
}

/* Read directory entries into the textboxes */
static void dirent_read(RKTextbox *textbox1, RKVScrollbar *vscrollbar1,
	RKTextbox *textbox2, RKVScrollbar *vscrollbar2, RKLabel *label_sel) {

	r_pvector_clear (textbox1->lines);
	r_pvector_clear (textbox2->lines);
	char *dir_path = r_sys_getdir ();
	if (!dir_path) {
		return;
	}

	RStrBuf *sbuf = r_strbuf_new (dir_path);
	if (!sbuf) {
		free (dir_path);
		return;
	}
	if (dir_path[0] == '/') {
		if (!r_str_endswith (dir_path, "/")) {
			r_strbuf_append (sbuf, "/");
		}
	} else {
		if (!r_str_endswith (dir_path, "\\")) {
			r_strbuf_append (sbuf, "\\");
		}
	}
	strncpy (label_sel->text, r_strbuf_get (sbuf), (2 * textbox1->rect.w + 2 * kiss_up.w) / kiss_textfont.advance);
	r_strbuf_free (sbuf);

	RList *dir = r_sys_dir (dir_path);
	R_FREE (dir_path);

	while (r_list_length (dir)) {
		char *file = r_list_pop (dir);
		if (r_file_is_directory (file)) {
			r_pvector_push (textbox1->lines, r_str_newf ("%s/", file));
		} else if (r_file_is_regular (file)) {
			r_pvector_push (textbox2->lines, strdup (file));
		}
		dir->free (file);
	}
	r_list_free (dir);
	text_reset (textbox1, vscrollbar1);
	text_reset (textbox2, vscrollbar2);
}

/* The widget arguments are widgets that this widget talks with */
static void textbox1_event(RKTextbox *textbox, SDL_Event *e,
	RKVScrollbar *vscrollbar1, RKTextbox *textbox2,
	RKVScrollbar *vscrollbar2, RKLabel *label_sel, int *draw) {

	if (rk_textbox_event (textbox, e, draw)) {
		int index = textbox->firstline + textbox->selectedline;
		if (strcmp ((char *)r_pvector_at (textbox->lines, index), "")) {
			textbox->selectedline = -1;
			r_sys_chdir ((char *)r_pvector_at (textbox->lines, index));
			dirent_read (textbox, vscrollbar1, textbox2, vscrollbar2, label_sel);
			*draw = 1;
		}
	}
}

static void vscrollbar1_event(RKVScrollbar *vscrollbar, SDL_Event *e, RKTextbox *textbox1, int *draw) {
	int firstline;

	if (rk_vscrollbar_event (vscrollbar, e, draw) &&
		((int)r_pvector_len (textbox1->lines)) - textbox1->maxlines > 0) {
		firstline = (int)((((int)r_pvector_len (textbox1->lines)) -
			textbox1->maxlines) * vscrollbar->fraction + 0.5);
		if (firstline >= 0) {
			textbox1->firstline = firstline;
		}
		*draw = 1;
	}
}

static void textbox2_event(RKTextbox *textbox, SDL_Event *e,
	RKVScrollbar *vscrollbar2, RKEntry *entry, int *draw) {
	int index;

	if (rk_textbox_event (textbox, e, draw)) {
		index = textbox->firstline + textbox->selectedline;
		if (strcmp ((char *)r_pvector_at (textbox->lines, index), "")) {
			rk_string_copy (entry->text, entry->textwidth / kiss_textfont.advance,
				(char *)r_pvector_at (textbox->lines, index), NULL);
			*draw = 1;
		}
	}
}

static void vscrollbar2_event(RKVScrollbar *vscrollbar, SDL_Event *e, RKTextbox *textbox2, int *draw) {
	int firstline;

	if (rk_vscrollbar_event (vscrollbar, e, draw) && r_pvector_len (textbox2->lines)) {
		firstline = (int)((((int)r_pvector_len (textbox2->lines)) -
			textbox2->maxlines) * vscrollbar->fraction + 0.5);
		if (firstline >= 0) {
			textbox2->firstline = firstline;
		}
		*draw = 1;
	}
}

static void button_ok1_event(RKButton *button, SDL_Event *e,
	RKWindow *window1, RKWindow *window2, RKLabel *label_sel,
	RKEntry *entry, RKLabel *label_res,
	RKProgressbar *progressbar, int *draw) {
	char buf[KISS_MAX_LENGTH];

	if (rk_button_event (button, e, draw)) {
		rk_string_copy (buf, rk_maxlength (kiss_textfont,
			window2->rect.w - 2 * kiss_vslider.w, label_sel->text,
			entry->text),label_sel->text, entry->text);
		rk_string_copy (label_res->text, KISS_MAX_LABEL,
			"The following path was selected:\n", buf);
		window2->visible = 1;
		window2->focus = 1;
		window1->focus = 0;
		button->prelight = 0;
		progressbar->fraction = 0.;
		progressbar->run = 1;
		*draw = 1;
	}
}

static void button_cancel_event(RKButton *button, SDL_Event *e,
	int *quit, int *draw) {
	if (rk_button_event (button, e, draw)) {
		*quit = 1;
	}
}

static void button_ok2_event(RKButton *button, SDL_Event *e,
	RKWindow *window1, RKWindow *window2,
	RKProgressbar *progressbar, int *draw) {
	if (rk_button_event (button, e, draw)) {
		window2->visible = 0;
		window1->focus = 1;
		button->prelight = 0;
		progressbar->fraction = 0.;
		progressbar->run = 0;
		*draw = 1;
	}
}

int main (int argc, char **argv) {
	RKWindow window1, window2;
	RKLabel label1 = { 0 }, label2 = { 0 }, label_sel = { 0 }, label_res = { 0 };
	RKButton button_ok1 = { 0 }, button_ok2 = { 0 }, button_cancel = { 0 };
	RKTextbox textbox1 = { 0 }, textbox2 = { 0 };
	RKVScrollbar vscrollbar1 = { 0 }, vscrollbar2 = { 0 };
	RKProgressbar progressbar = { 0 };
	RKEntry entry = { 0 };

	int quit = 0;
	int draw = 1;
	int textbox_width = 250;
	int textbox_height = 250;
	int window2_width = 400;
	int window2_height = 168;
	kiss_array objects;
	SDL_Renderer *renderer = rk_init ("kiss_sdl example 1", &objects, 640, 480);
	if (!renderer) {
		return 1;
	}

	/* Arrange the widgets nicely relative to each other */
	rk_window_init (&window1, NULL, 1, 0, 0, kiss_screen_width,
		kiss_screen_height);
	RPVector *l1 = r_pvector_new (free);
	rk_textbox_init (&textbox1, &window1, 1, l1, kiss_screen_width / 2 - (2 * textbox_width + 2 * kiss_up.w - kiss_edge) / 2,
		3 * kiss_normal.h, textbox_width, textbox_height);
	rk_vscrollbar_init (&vscrollbar1, &window1, NULL, textbox1.rect.x + textbox_width, textbox1.rect.y, textbox_height);
	RPVector *l2 = r_pvector_new (free);
	rk_textbox_init (&textbox2, &window1, 1, l2, vscrollbar1.uprect.x + kiss_up.w, textbox1.rect.y,
		textbox_width, textbox_height);
	SDL_Rect scroll_wheel_rect;
	rk_makerect (scroll_wheel_rect, textbox2.rect.x, textbox2.rect.y, textbox_width + kiss_down.w, textbox_height);
	rk_vscrollbar_init (&vscrollbar2, &window1, &scroll_wheel_rect,
		textbox2.rect.x + textbox_width, vscrollbar1.uprect.y, textbox_height);
	rk_label_init (&label1, &window1, "Folders", textbox1.rect.x + kiss_edge, textbox1.rect.y - kiss_textfont.lineheight);
	rk_label_init (&label2, &window1, "Files", textbox2.rect.x + kiss_edge, textbox1.rect.y - kiss_textfont.lineheight);
	rk_label_init (&label_sel, &window1, "", textbox1.rect.x + kiss_edge, textbox1.rect.y + textbox_height + kiss_normal.h);
	rk_entry_init (&entry, &window1, 1, "kiss", textbox1.rect.x,
		label_sel.rect.y + kiss_textfont.lineheight,
		2 * textbox_width + 2 * kiss_up.w + kiss_edge);
	rk_button_init (&button_cancel, &window1, "Cancel",
		entry.rect.x + entry.rect.w - kiss_edge - kiss_normal.w,
		entry.rect.y + entry.rect.h + kiss_normal.h);
	rk_button_init (&button_ok1, &window1, "OK", button_cancel.rect.x - 2 * kiss_normal.w, button_cancel.rect.y);
	rk_window_init (&window2, NULL, 1, kiss_screen_width / 2 - window2_width / 2,
		kiss_screen_height / 2 - window2_height / 2, window2_width, window2_height);
	rk_label_init (&label_res, &window2, "", window2.rect.x + kiss_up.w, window2.rect.y + kiss_vslider.h);
	label_res.textcolor = kiss_blue;
	rk_progressbar_init (&progressbar, &window2,
		window2.rect.x + kiss_up.w - kiss_edge,
		window2.rect.y + window2.rect.h / 2 - kiss_bar.h / 2 - kiss_border,
		window2.rect.w - 2 * kiss_up.w + 2 * kiss_edge);
	rk_button_init (&button_ok2, &window2, "OK", window2.rect.x + window2.rect.w / 2 - kiss_normal.w / 2,
		progressbar.rect.y + progressbar.rect.h +
			2 * kiss_vslider.h);

	dirent_read (&textbox1, &vscrollbar1, &textbox2, &vscrollbar2, &label_sel);
	/* Do that, and all widgets associated with the window will show */
	window1.visible = 1;

	while (!quit) {

		/* Some code may be written here */

		SDL_Delay (10);
		SDL_Event e;
		while (SDL_PollEvent (&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}

			rk_window_event (&window2, &e, &draw);
			rk_window_event (&window1, &e, &draw);
			textbox1_event (&textbox1, &e, &vscrollbar1,
				&textbox2, &vscrollbar2, &label_sel, &draw);
			vscrollbar1_event (&vscrollbar1, &e, &textbox1, &draw);
			textbox2_event (&textbox2, &e, &vscrollbar2, &entry, &draw);
			vscrollbar2_event (&vscrollbar2, &e, &textbox2, &draw);
			button_ok1_event (&button_ok1, &e, &window1, &window2,
				&label_sel, &entry, &label_res, &progressbar, &draw);
			button_cancel_event (&button_cancel, &e, &quit, &draw);
			rk_entry_event (&entry, &e, &draw);
			button_ok2_event (&button_ok2, &e, &window1, &window2,
				&progressbar, &draw);
		}

		vscrollbar1_event (&vscrollbar1, NULL, &textbox1, &draw);
		vscrollbar2_event (&vscrollbar2, NULL, &textbox2, &draw);
		rk_progressbar_event (&progressbar, NULL, &draw);

		if (!draw) {
			continue;
		}
		SDL_RenderClear (renderer);

		rk_window_draw (&window1, renderer);
		rk_label_draw (&label1, renderer);
		rk_label_draw (&label2, renderer);
		rk_textbox_draw (&textbox1, renderer);
		rk_vscrollbar_draw (&vscrollbar1, renderer);
		rk_textbox_draw (&textbox2, renderer);
		rk_vscrollbar_draw (&vscrollbar2, renderer);
		rk_label_draw (&label_sel, renderer);
		rk_entry_draw (&entry, renderer);
		rk_button_draw (&button_ok1, renderer);
		rk_button_draw (&button_cancel, renderer);
		rk_window_draw (&window2, renderer);
		rk_label_draw (&label_res, renderer);
		rk_progressbar_draw (&progressbar, renderer);
		rk_button_draw (&button_ok2, renderer);

		SDL_RenderPresent (renderer);
		draw = 0;
	}
	rk_fini (&objects);
	r_pvector_free (l1);
	r_pvector_free (l2);
	return 0;
}

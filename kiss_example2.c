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

static struct {
	char name[KISS_MAX_LENGTH];
	int population;
	int area;
} cities[] = {
	{ "Atlanta", 5268860, 8376 },
	{ "Baltimore", 2710489, 2710 },
	{ "Boston", 4552402, 1422 },
	{ "Charlotte", 2217012, 3198 },
	{ "Chicago", 9461105, 10856 },
	{ "Cincinnati", 2130151, 4808 },
	{ "Cleveland", 2077240, 82 },
	{ "Dallas", 6426214, 9286 },
	{ "Denver", 2543482, 155 },
	{ "Detroit", 4290060, 3888 },
	{ "Houston", 5920416, 10062 },
	{ "Kansas City", 2035334, 7952 },
	{ "Los Angeles", 12828837, 4850 },
	{ "Miami", 5564635, 6137 },
	{ "Minneapolis", 3279833, 8120 },
	{ "New York", 19069796, 13318 },
	{ "Orlando", 2134411, 4012 },
	{ "Philadelphia", 5965343, 5118 },
	{ "Phoenix", 4192887, 14598 },
	{ "Pittsburgh", 2356285, 5706 },
	{ "Portland", 2226009, 6684 },
	{ "Sacramento", 2149127, 21429 },
	{ "San Antonio", 2142508, 7387 },
	{ "San Diego", 3095313, 372 },
	{ "San Francisco", 4335391, 6984 },
	{ "Seattle", 4039809, 5872 },
	{ "St. Louis", 2812896, 8458 },
	{ "Tampa", 2783243, 2554 },
	{ "Washington", 5582170, 5564 },
	{ "", 0, 0 }
};

static void select1_event(kiss_selectbutton *select1, SDL_Event *e,
	kiss_selectbutton *select2, int *draw) {
	select2->selected ^= !!rk_selectbutton_event (select1, e, draw);
}

static void select2_event(kiss_selectbutton *select2, SDL_Event *e,
	kiss_selectbutton *select1, int *draw) {
	select1->selected ^= !!rk_selectbutton_event (select2, e, draw);
}

static void combobox_event(kiss_combobox *combobox, SDL_Event *e,
	char *stext, kiss_entry *entry, kiss_selectbutton *select1,
	kiss_selectbutton *select2, kiss_hscrollbar *hscrollbar, int *draw) {
	void **p, *s;
	int i;

	s = combobox->entry.text;
	if (rk_combobox_event (combobox, e, draw)) {
		if ((p = (void **)bsearch (&s, combobox->textbox.array->data,
			     combobox->textbox.array->length, sizeof (void *),
			     rk_string_compare))) {
			i = p - combobox->textbox.array->data;
			if (select1->selected) {
				sprintf (stext, "The population of the "
						"metropolitan area of %s is %d.",
					cities[i].name,
					cities[i].population);
			} else {
				sprintf (stext, "The metropolitan area of "
						"%s is %d square miles.",
					cities[i].name,
					cities[i].area);
			}

		} else {
			strcpy (stext, "Data not found");
		}
		strcpy (entry->text, stext);
		hscrollbar->fraction = 0.;
		hscrollbar->step = 0.;
		if (strlen (stext) - entry->textwidth / kiss_textfont.advance > 0) {
			hscrollbar->step = 1. / (strlen (stext) - entry->textwidth / kiss_textfont.advance);
		}
		entry->text[entry->textwidth / kiss_textfont.advance] = 0;
		*draw = 1;
	}
}

/* This is to show the hscrollbar, only works with ASCII characters */
static void hscrollbar_event(kiss_hscrollbar *hscrollbar, SDL_Event *e,
	char *stext, int *first, kiss_entry *entry, int *draw) {
	char *p = stext;

	if (rk_hscrollbar_event (hscrollbar, e, draw) && strlen (stext) - entry->textwidth / kiss_textfont.advance > 0) {
		*first = (int)((strlen (stext) - entry->textwidth / kiss_textfont.advance) * hscrollbar->fraction + 0.5);
		if (*first >= 0) {
			strcpy (entry->text, p + *first);
			entry->text[entry->textwidth / kiss_textfont.advance] = 0;
			*draw = 1;
		}
	}
}

static void button_ok_event(kiss_button *button_ok, SDL_Event *e,
	int *quit, int *draw) {
	*quit = !!rk_button_event (button_ok, e, draw);
}

int main (int argc, char **argv) {
	RKLabel label1 = { 0 }, label2 = { 0 };
	kiss_button button_ok = { 0 };
	kiss_hscrollbar hscrollbar = { 0 };
	kiss_entry entry = { 0 };
	kiss_combobox combobox = { 0 };
	kiss_selectbutton select1 = { 0 }, select2 = { 0 };
	char stext[KISS_MAX_LENGTH];

	int quit = 0;
	int draw = 1;
	int first = 0;
	strcpy (stext, "");
	/* Combobox textbox width and height */
	int combobox_width = 150;
	int combobox_height = 66;
	int entry_width = 250;
	kiss_array objects;
	SDL_Renderer *renderer = rk_init ("kiss_sdl example 2", &objects, 640, 480);
	if (!renderer) {
		return 1;
	}
	kiss_array a;
	rk_array_init (&a);
	int i;
	for (i = 0; cities[i].population; i++) {
		rk_array_appendstring (&a, 0, cities[i].name, NULL);
	}
	rk_array_append (&objects, ARRAY_TYPE, &a);

	/* Arrange the widgets nicely relative to each other */
	kiss_window window;
	rk_window_init (&window, NULL, 1, 0, 0, kiss_screen_width, kiss_screen_height);
	rk_label_init (&label1, &window, "Population",
		kiss_screen_width / 2 - (combobox_width + kiss_up.w - kiss_edge) / 2 + kiss_edge,
		6 * kiss_textfont.lineheight);
	rk_selectbutton_init (&select1, &window,
		label1.rect.x + combobox_width + kiss_up.w - kiss_edge - kiss_selected.w,
		label1.rect.y + kiss_textfont.ascent - kiss_selected.h);
	rk_label_init (&label2, &window, "Area", label1.rect.x,
		label1.rect.y + 2 * kiss_textfont.lineheight);
	rk_selectbutton_init (&select2, &window, select1.rect.x,
		label2.rect.y + kiss_textfont.ascent - kiss_selected.h);
	rk_combobox_init (&combobox, &window, "none", &a,
		label1.rect.x - kiss_edge, label2.rect.y + 2 * kiss_textfont.lineheight,
		combobox_width, combobox_height);
	rk_entry_init (&entry, &window, 1, "", kiss_screen_width / 2 - entry_width / 2 + kiss_edge,
		combobox.entry.rect.y + combobox.entry.rect.h + 2 * kiss_textfont.lineheight + kiss_border,
		entry_width);
	rk_hscrollbar_init (&hscrollbar, &window, entry.rect.x, entry.rect.y + entry.rect.h, entry.rect.w);
	rk_button_init (&button_ok, &window, "OK", entry.rect.x + entry.rect.w - kiss_edge - kiss_normal.w,
		entry.rect.y + entry.rect.h + kiss_left.h + 2 * kiss_normal.h);

	select1.selected = 1;
	hscrollbar.step = 0.;
	/* Do that, and all widgets associated with the window will show */
	window.visible = 1;

	while (!quit) {

		/* Some code may be written here */

		SDL_Delay (10);
		SDL_Event e;
		while (SDL_PollEvent (&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}

			rk_window_event (&window, &e, &draw);
			select1_event (&select1, &e, &select2, &draw);
			select2_event (&select2, &e, &select1, &draw);
			combobox_event (&combobox, &e, stext, &entry,
				&select1, &select2, &hscrollbar, &draw);
			hscrollbar_event (&hscrollbar, &e, stext, &first,
				&entry, &draw);
			button_ok_event (&button_ok, &e, &quit, &draw);
		}

		rk_combobox_event (&combobox, NULL, &draw);
		hscrollbar_event (&hscrollbar, NULL, stext, &first, &entry,
			&draw);

		if (!draw) {
			continue;
		}
		SDL_RenderClear (renderer);

		rk_window_draw (&window, renderer);
		rk_button_draw (&button_ok, renderer);
		rk_hscrollbar_draw (&hscrollbar, renderer);
		rk_entry_draw (&entry, renderer);
		rk_combobox_draw (&combobox, renderer);
		rk_selectbutton_draw (&select2, renderer);
		rk_label_draw (&label2, renderer);
		rk_selectbutton_draw (&select1, renderer);
		rk_label_draw (&label1, renderer);

		SDL_RenderPresent (renderer);
		draw = 0;
	}
	rk_clean (&objects);
	return 0;
}

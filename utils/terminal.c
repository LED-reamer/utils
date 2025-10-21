#include "terminal.h"

#include <ncurses.h>
bool terminal_isrunning = true;
int terminal_input_value = 0;

static uint32_t terminal_attrib_to_ncurses[] = {
	A_NORMAL,
	A_STANDOUT,
	A_UNDERLINE,
	A_REVERSE,
	A_BLINK,
	A_DIM,
	A_BOLD,
	A_PROTECT,
	A_INVIS,
	A_ALTCHARSET,
	A_CHARTEXT,
};

#define MAX_COLOR_COMBOS 512

typedef struct {
	int32_t fg;
	int32_t bg;
	int32_t pair_id;
} color_pair_entry_t;

static color_pair_entry_t color_pairs[MAX_COLOR_COMBOS];
static int next_pair_id = 1;

// approximation
int32_t rgb_to_256color(uint8_t r, uint8_t g, uint8_t b) {
	int r6 = r * 6 / 256;
	int g6 = g * 6 / 256;
	int b6 = b * 6 / 256;
	return 16 + 36 * r6 + 6 * g6 + b6;
}

int32_t get_color_pair_id(int fg, int bg, bool fg_transparent, bool bg_transparent) {
	int real_fg = fg_transparent ? -1 : fg;
	int real_bg = bg_transparent ? -1 : bg;

	for (int i = 0; i < next_pair_id - 1; i++) {
		if (color_pairs[i].fg == real_fg && color_pairs[i].bg == real_bg) {
			return color_pairs[i].pair_id;
		}
	}
	if (next_pair_id >= COLOR_PAIRS) {
		return 0;
	}

	init_pair(next_pair_id, real_fg, real_bg);
	color_pairs[next_pair_id - 1].fg = real_fg;
	color_pairs[next_pair_id - 1].bg = real_bg;
	color_pairs[next_pair_id - 1].pair_id = next_pair_id;
	return next_pair_id++;
}

void terminal_create() {
	initscr();
	start_color();
	use_default_colors();
	cbreak();
	nodelay(stdscr, TRUE);
	noecho();
	keypad(stdscr, TRUE);

	terminal_hide_cursor(true);
}

void terminal_destroy() {
	endwin();
}

bool terminal_running() {
	terminal_attribute_reset();
	terminal_input_value = getch();
	erase();

	return terminal_isrunning;
}

void terminal_close() {
	terminal_isrunning = false;
}

int terminal_input() {
	return terminal_input_value;
}

void terminal_char(int character, uint32_t x, uint32_t y) {
	mvaddch(y, x, character);
}

void terminal_string(const char* string, uint32_t x, uint32_t y) {
	mvaddstr(y, x, string);
}

void terminal_buffer(char buf[], uint32_t buf_width, uint32_t buf_height, uint32_t x, uint32_t y, bool two_wide_cells) {
	if(two_wide_cells){
		for (uint32_t dy = 0; dy < buf_height; dy++) {
			for (uint32_t dx = 0; dx < buf_width; dx++) {
				terminal_char((int)buf[dx + dy*buf_width], dx*2 + x, dy + y);
			}
		}
		return;
	}
	for (uint32_t row = 0; row < buf_height; row++) {
		char* line = &buf[row * buf_width];
		mvaddnstr(y + row, x, line, buf_width);
	}
}

void terminal_pixel(term_rgb_t pixel_color, uint32_t x, uint32_t y, bool two_wide) {
	terminal_attribute(TERM_REVERSE, (term_color_t){pixel_color.r, pixel_color.g, pixel_color.b, false}, TERM_DEFAULT);
	terminal_char(' ', x, y);
	if (two_wide)
		terminal_char(' ', x + 1, y);
}

void terminal_image(term_rgb_t img[], uint32_t img_width, uint32_t img_height, uint32_t x, uint32_t y, bool two_wide) {
	for (uint32_t dy = 0; dy < img_height; dy++) {
		for (uint32_t dx = 0; dx < img_width; dx++) {
			uint32_t screen_x = x + (two_wide ? dx * 2 : dx);
			term_rgb_t color = img[dy * img_width + dx];
			terminal_pixel(color, screen_x, dy + y, two_wide);
		}
	}

	terminal_attribute_reset();
}

void terminal_box(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	if (width < 2 || height < 2) return;

	// edges
	mvaddch(y, x, ACS_ULCORNER);
	mvaddch(y, x + width - 1, ACS_URCORNER);
	mvaddch(y + height - 1, x, ACS_LLCORNER);
	mvaddch(y + height - 1, x + width - 1, ACS_LRCORNER);

	// horizontal
	mvhline(y, x + 1, ACS_HLINE, width - 2);
	mvhline(y + height - 1, x + 1, ACS_HLINE, width - 2);

	// vertical
	mvvline(y + 1, x, ACS_VLINE, height - 2);
	mvvline(y + 1, x + width - 1, ACS_VLINE, height - 2);
}

uint32_t terminal_width() {
	int x, y;
	(void)y;
	getmaxyx(stdscr, y, x);
	return x;
}

uint32_t terminal_height() {
	int x, y;
	(void)x;
	getmaxyx(stdscr, y, x);
	return y;
}

void terminal_hide_cursor(bool hidden) {
	if (hidden)
		curs_set(0);
	else
		curs_set(1);
}

void terminal_background(term_color_t color) {
	int8_t fg = rgb_to_256color(color.r, color.g, color.b);
	int8_t bg = rgb_to_256color(color.r, color.g, color.b);

	int pair_id = get_color_pair_id(fg, bg, false, false);
	bkgd(COLOR_PAIR(pair_id));
}

void terminal_attribute(terminal_attribute_e attrib, term_color_t front, term_color_t back) {
	terminal_attribute_reset();

	int32_t fg = front.transparent ? 0 : rgb_to_256color(front.r, front.g, front.b);
	int32_t bg = back.transparent ? 0 : rgb_to_256color(back.r, back.g, back.b);

	int pair_id = get_color_pair_id(fg, bg, front.transparent, back.transparent);
	attrset(terminal_attrib_to_ncurses[attrib] | COLOR_PAIR(pair_id));
}

void terminal_attribute_reset() {
	attrset(A_NORMAL);
}

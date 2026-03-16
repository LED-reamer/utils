#include "term.h"
#include "hstd/array.h"
#include "hstd/allocator.h"
#include <ncurses.h>

#define CTRL(x) ((x) & 0x1f)

typedef struct{
	WINDOW* win;
}window_t;
MAKE_ARRAY(windows_t, window_t);

typedef struct{
	windows_t wins;
	bool refresh_triggered;
}context_t;
static context_t ctx = {0};

void term_init(){
	initscr();
	nodelay(stdscr,TRUE);
	noecho();
	raw();
	keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
}

void term_quit(){
	for(size_t i=0; i<ctx.wins.count; i++) delwin(ctx.wins.items[i].win);
	array_free(&ctx.wins);
	endwin();
}

void term_trigger_refresh(){
	ctx.refresh_triggered = true;
}

void term_update(){
	for(size_t i=0; i<ctx.wins.count; i++)
		box(ctx.wins.items[i].win, 0, 0);

	if(ctx.refresh_triggered){
		for(size_t i=0; i<ctx.wins.count; i++){
			wrefresh(ctx.wins.items[i].win);
		}
		refresh();
		ctx.refresh_triggered = false;
	}
}

term_window_t term_window_open(size_t x, size_t y, size_t w, size_t h){
	window_t win = {
		.win = newwin(h, w, y, x),
	};
	array_push(&ctx.wins, win);
	return win.win;
}

void term_window_close(term_window_t window){
	for(size_t i=0; i<ctx.wins.count; i++){
		if(window != ctx.wins.items[i].win) continue;
		
		delwin(ctx.wins.items[i].win);

		//swap and pop
		ctx.wins.items[i] = ctx.wins.items[ctx.wins.count-1];
		array_pop(&ctx.wins);
	}
}

void term_resize_window(term_window_t* window, size_t x, size_t y, size_t w, size_t h){
	mvwin((WINDOW*)window, y, x);
	wresize((WINDOW*)window, (int)h, (int)w);
}

term_window_t* term_get_windows(size_t* count){
	if(count != NULL)
		*count = ctx.wins.count;
	return (term_window_t*)ctx.wins.items;
}

term_event_t term_next_event(){
	return (term_event_t){.key = getch()};
}

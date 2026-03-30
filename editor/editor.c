#include "editor.h"
#include "hstd/logger.h"
#include "terminal/term.h"
#include <stdbool.h>
#include <ncurses.h>

//TODO TEMP
static window_t current_win;//replaced by tiling later

void init(){
	term_init();

	editor_open_file(PATH("test.txt"));
	
	term_next_event();
	term_trigger_refresh(); term_update();
}

void quit(){
	term_quit();
}

void __editor_draw_windows();
void loop(){
	bool running = true;
	while(running){
		__editor_draw_windows();
		int c = term_next_event().key;
		switch(c){
			case 'q': running = false; break;
			case 'B': running = false; break;
		}
		if(c != -1) term_trigger_refresh();
		term_update();
	}
}

void __editor_draw_windows(){
	move(1, 1);
	waddstr(current_win.term_win, current_win.buffer.data);
}

void editor_open_file(path_t path){
	term_trigger_refresh();
	size_t w, h;
	term_get_size(&w, &h);
	current_win.term_win = term_window_open(0, 0, w, h);
	current_win.scroll = 0;
	if(path.name[0] == '\0') goto open_default;
	error_t err = buffer_from_file(&current_win.buffer, path);
	if(err != NULL) goto open_default;

	return;

	char default_file[] = "default file";
open_default:
	err = buffer_copy(&current_win.buffer, default_file, sizeof(default_file));
	if(err != NULL) logger.err("failed to load default file!");
}

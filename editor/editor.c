#include "editor.h"
#include "hstd/logger.h"
#include "terminal/term.h"
#include <stdbool.h>

void init(){
	term_init();
	term_window_open(0, 0, 10, 10);
	term_window_open(10, 2, 30, 10);

	term_next_event();
	term_trigger_refresh(); term_update();
}

void quit(){
	term_quit();
}

void loop(){
	bool running = true;
	while(running){
		int c = term_next_event().key;
		switch(c){
			case 'q': running = false; break;
			case 'B': running = false; break;
		}
		if(c != -1) term_trigger_refresh();
		term_update();
	}
}

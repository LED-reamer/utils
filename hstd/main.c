#include "window.h"
#include "allocator.h"
#include "error.h"
#include <stdio.h>
#include <string.h>

int main(){
	window_t win = window_create("test", 1920/2, 1080/2);

	while(window_open(&win)){
		window_update(&win);
		if(key_just_down(&win, KEY_ESCAPE)) window_close(&win);
	}
	window_destroy(&win);
	
	return 0;
}

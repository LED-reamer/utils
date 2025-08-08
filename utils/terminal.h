#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t r, g, b;
} term_rgb_t;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	bool transparent;
} term_color_t;

void terminal_create();
void terminal_destroy();
bool terminal_running();
void terminal_close();

typedef enum terminal_key_e terminal_key_e;
int terminal_input();

void terminal_char(int character, uint32_t x, uint32_t y);
void terminal_string(const char* string, uint32_t x, uint32_t y);
void terminal_buffer(char buf[], uint32_t buf_width, uint32_t buf_height, uint32_t x, uint32_t y);
void terminal_pixel(term_rgb_t pixel_color, uint32_t x, uint32_t y, bool two_wide);
void terminal_image(term_rgb_t img[], uint32_t img_width, uint32_t img_height, uint32_t x, uint32_t y, bool two_wide);
void terminal_box(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
uint32_t terminal_width();
uint32_t terminal_height();

void terminal_hide_cursor(bool hidden);

typedef enum terminal_attribute_e terminal_attribute_e;
typedef enum terminal_color_e terminal_color_e;

void terminal_background(term_color_t color);
void terminal_attribute(terminal_attribute_e attrib, term_color_t front, term_color_t back);
void terminal_attribute_reset();

// enum definitions

typedef enum terminal_attribute_e {
	TERM_NORMAL,	  // Normal display (no highlight)
	TERM_STANDOUT,	  // Best highlighting mode of the terminal.
	TERM_UNDERLINE,	  // Underlining
	TERM_REVERSE,	  // Reverse video
	TERM_BLINK,		  // Blinking
	TERM_DIM,		  // Half bright
	TERM_BOLD,		  // Extra bright or bold
	TERM_PROTECT,	  // Protected mode
	TERM_INVIS,		  // Invisible or blank mode
	TERM_ALTCHARSET,  // Alternate character set
	TERM_CHARTEXT,	  // Bit-mask to extract a character
} terminal_attribute_e;

#define TERM_DEFAULT \
	(term_color_t) { 0, 0, 0, true }
#define TERM_BLACK \
	(term_color_t) { 0, 0, 0, false }
#define TERM_WHITE \
	(term_color_t) { 255, 255, 255, false }
#define TERM_RED \
	(term_color_t) { 255, 0, 0, false }
#define TERM_GREEN \
	(term_color_t) { 0, 255, 0, false }
#define TERM_YELLOW \
	(term_color_t) { 255, 255, 0, false }
#define TERM_BLUE \
	(term_color_t) { 0, 0, 255, false }
#define TERM_MAGENTA \
	(term_color_t) { 255, 0, 255, false }
#define TERM_CYAN \
	(term_color_t) { 0, 255, 255, false }

typedef enum terminal_key_e {
	TERM_KEY_BREAK = 257,	   // Break key
	TERM_KEY_DOWN = 258,	   // Arrow down
	TERM_KEY_UP = 259,		   // Arrow up
	TERM_KEY_LEFT = 260,	   // Arrow left
	TERM_KEY_RIGHT = 261,	   // Arrow right
	TERM_KEY_HOME = 262,	   // Home key
	TERM_KEY_BACKSPACE = 263,  // Backspace
	TERM_KEY_F0 = 264,		   // Function key zero
	TERM_KEY_DL = 328,		   // Delete line
	TERM_KEY_IL = 329,		   // Insert line
	TERM_KEY_DC = 330,		   // Delete character
	TERM_KEY_IC = 331,		   // Insert char or enter insert mode
	TERM_KEY_EIC = 332,		   // Exit insert char mode
	TERM_KEY_CLEAR = 333,	   // Clear screen
	TERM_KEY_EOS = 334,		   // Clear to end of screen
	TERM_KEY_EOL = 335,		   // Clear to end of line
	TERM_KEY_SF = 336,		   // Scroll 1 line forward
	TERM_KEY_SR = 337,		   // Scroll 1 line backward (reverse)
	TERM_KEY_NPAGE = 338,	   // Next page
	TERM_KEY_PPAGE = 339,	   // Previous page
	TERM_KEY_STAB = 340,	   // Set tab
	TERM_KEY_CTAB = 341,	   // Clear tab
	TERM_KEY_CATAB = 342,	   // Clear all tabs
	TERM_KEY_ENTER = 343,	   // Enter or send
	TERM_KEY_SRESET = 344,	   // Soft (partial) reset
	TERM_KEY_RESET = 345,	   // Reset or hard reset
	TERM_KEY_PRINT = 346,	   // Print or copy
	TERM_KEY_LL = 347,		   // Home down or bottom (lower left)
	TERM_KEY_A1 = 348,		   // Upper left of keypad
	TERM_KEY_A3 = 349,		   // Upper right of keypad
	TERM_KEY_B2 = 350,		   // Center of keypad
	TERM_KEY_C1 = 351,		   // Lower left of keypad
	TERM_KEY_C3 = 352,		   // Lower right of keypad
	TERM_KEY_BTAB = 353,	   // Back tab key
	TERM_KEY_BEG = 354,		   // Beg(inning) key
	TERM_KEY_CANCEL = 355,	   // Cancel key
	TERM_KEY_CLOSE = 356,	   // Close key
	TERM_KEY_COMMAND = 357,	   // Cmd (command) key
	TERM_KEY_COPY = 358,	   // Copy key
	TERM_KEY_CREATE = 359,	   // Create key
	TERM_KEY_END = 360,		   // End key
	TERM_KEY_EXIT = 361,	   // Exit key
	TERM_KEY_FIND = 362,	   // Find key
	TERM_KEY_HELP = 363,	   // Help key
	TERM_KEY_MARK = 364,	   // Mark key
	TERM_KEY_MESSAGE = 365,	   // Message key
	// keypad constants

	TERM_KEY_MOUSE = 409,	   // Mouse event read
	TERM_KEY_MOVE = 366,	   // Move key
	TERM_KEY_NEXT = 367,	   // Next object key
	TERM_KEY_OPEN = 368,	   // Open key
	TERM_KEY_OPTIONS = 369,	   // Options key
	TERM_KEY_PREVIOUS = 370,   // Previous object key
	TERM_KEY_REDO = 371,	   // Redo key
	TERM_KEY_REFERENCE = 372,  // Ref(erence) key
	TERM_KEY_REFRESH = 373,	   // Refresh key
	TERM_KEY_REPLACE = 374,	   // Replace key
	TERM_KEY_RESIZE = 410,	   // Screen resized
	TERM_KEY_RESTART = 375,	   // Restart key
	TERM_KEY_RESUME = 376,	   // Resume key
	TERM_KEY_SAVE = 377,	   // Save key
	TERM_KEY_SBEG = 378,	   // Shifted beginning key
	TERM_KEY_SCANCEL = 379,	   // Shifted cancel key
	TERM_KEY_SCOMMAND = 380,   // Shifted command key
	TERM_KEY_SCOPY = 381,	   // Shifted copy key
	TERM_KEY_SCREATE = 382,	   // Shifted create key
	TERM_KEY_SDC = 383,		   // Shifted delete char key
	TERM_KEY_SDL = 384,		   // Shifted delete line key
	TERM_KEY_SELECT = 385,	   // Select key
	TERM_KEY_SEND = 386,	   // Shifted end key
	TERM_KEY_SEOL = 387,	   // Shifted clear line key
	TERM_KEY_SEXIT = 388,	   // Shifted exit key
	TERM_KEY_SFIND = 389,	   // Shifted find key
	TERM_KEY_SHELP = 390,	   // Shifted help key
	TERM_KEY_SHOME = 391,	   // Shifted home key
	TERM_KEY_SIC = 392,		   // Shifted input key
	TERM_KEY_SLEFT = 393,	   // Shifted left arrow key
	TERM_KEY_SMESSAGE = 394,   // Shifted message key
	TERM_KEY_SMOVE = 395,	   // Shifted move key
	TERM_KEY_SNEXT = 396,	   // Shifted next key
	TERM_KEY_SOPTIONS = 397,   // Shifted options key
	TERM_KEY_SPREVIOUS = 398,  // Shifted prev key
	TERM_KEY_SPRINT = 399,	   // Shifted print key
	TERM_KEY_SREDO = 400,	   // Shifted redo key
	TERM_KEY_SREPLACE = 401,   // Shifted replace key
	TERM_KEY_SRIGHT = 402,	   // Shifted right arrow
	// DOES NOT EXIST: TERM_KEY_SRESUME = 403,	//Shifted resume key
	TERM_KEY_SSAVE = 404,	  // Shifted save key
	TERM_KEY_SSUSPEND = 405,  // Shifted suspend key
	TERM_KEY_SUNDO = 406,	  // Shifted undo key
	TERM_KEY_SUSPEND = 407,	  // Suspend key
	TERM_KEY_UNDO = 408,	  // Undo key
} terminal_key_e;

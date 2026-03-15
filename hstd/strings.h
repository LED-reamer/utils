#ifndef STRINGS_H
#define STRINGS_H

typedef const char* cstring;

typedef struct{
	cstring str;
	unsigned long int len;//u64
}string_t;
#define str(s) (string_t){(const char*)(s), sizeof((s))-1 }
#define str_fmt(s) (int)(s).size, (s).str

//example fmt: printf("%.*s", str_fmt(my_string));


// 		/\ ALL THIS IS actually a string view
//			A NORMAL STRING WOULD BE ALLOCATED OR IS JUST A BUFFER?
#endif

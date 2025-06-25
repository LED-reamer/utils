#include "string.h"
#include <ctype.h>

size_t string_max_len(size_t len1, size_t len2)
{
	if(len1 > len2) return len1;
	return len2;
}

bool string_compare(string_t string1, string_t string2)
{
	if(string1.len != string2.len)
		return false;
	else if(0 == memcmp(string1.buffer, string2.buffer, string_max_len(string1.len, string2.len)))
		return true;
	return false;
}

bool string_compare_cstring(string_t string, const char* cstring)
{
	size_t len = strlen(cstring);
	if(len == 0) return false;
	
	if(string.len != len)
		return false;
	else if(0 == memcmp(string.buffer, cstring, string_max_len(string.len, len)))
		return true;
	return false;
}

bool string_begins_with(string_t string, string_t prefix)
{
	if(prefix.len > string.len)
		return false;
	else if(0 == memcmp(string.buffer, prefix.buffer, prefix.len))
		return true;
	return false;
}

bool string_ends_with(string_t string, string_t suffix)
{
	if(suffix.len > string.len)
		return false;
	else if(0 == memcmp(string.buffer + string.len - suffix.len, suffix.buffer, suffix.len))
		return true;
	return false;
}

int32_t string_find(string_t string, string_t search)
{
	return string_find_from(string, search, 0);
}

int32_t string_find_from(string_t string, string_t search, size_t start_index)
{
	size_t search_index = start_index;
	if(start_index > string.len-1) return -1;
	
	int32_t start_found = -1;
	for(size_t i = 0; i < string.len; i++)
	{
		if(search_index == search.len) return start_found;
		if(string.buffer[i] == search.buffer[search_index])
		{
			search_index++;
		}
		else
		{
			search_index = 0;
			start_found = i+1;
		}
	}
	return -1;
}

bool string_contains(string_t string, string_t search)
{
	size_t search_index = 0;
	for(size_t i = 0; i < string.len; i++)
	{
		if(search_index == search.len) return true;
		if(string.buffer[i] == search.buffer[search_index])
		{
			search_index++;
		}
		else
			search_index = 0;
	}
	return false;
}

bool string_is_int(string_t string)
{
	size_t i = 0;
	if(string.len > 0)
		if(string.buffer[0] == '-') i++;
	
	for(; i < string.len; i++)
	{
		char c = string.buffer[i];
		
		if(!('0' <= c && c <= '9'))
			return false;
	}
	return true;
}

bool string_is_uint(string_t string)
{
	for(size_t i = 0; i < string.len; i++)
	{
		char c = string.buffer[i];
		if(!('0' <= c && c <= '9'))
			return false;
	}
	return true;
}

bool string_is_real(string_t string)
{
	size_t i = 0;
	if(string.len > 0)
		if(string.buffer[0] == '-') i++;
	bool dot_encountered = false;
	for(; i < string.len; i++)
	{
		char c = string.buffer[i];
		
		if(!('0' <= c && c <= '9'))
		{
			if(c == '.' && dot_encountered == false)
			{
				dot_encountered = true;
				continue;
			}
			return false;
		}
	}

	if(dot_encountered && string.len == 1) return false;//special case of "."
	
	return true;
}

string_t string_to_upper(string_t string)
{
	string_t new = string;
	for (size_t i = 0; i < string.len; i++)
	{
		if(islower(string.buffer[i]))
			new.buffer[i] = toupper(string.buffer[i]);
	}

	return new;
}

string_t string_to_lower(string_t string)
{
	string_t new = string;
	for (size_t i = 0; i < string.len; i++)
	{
		if(isupper(string.buffer[i]))
			new.buffer[i] = tolower(string.buffer[i]);
	}

	return new;
}

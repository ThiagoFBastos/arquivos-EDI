#include "cstring.h"

#include <stdlib.h>
#include <string.h>

int readLine(FILE *in, cstring* s) {

	register int ch;
	int i = 0;
	
	s->data = NULL;

	do
	{
		ch = fgetc(in);

		s->data = realloc(s->data, i + 1);
		s->data[i++] = ch;
	
	} while(ch != EOF && ch != '\n');

	s->data[i - 1] = '\0';
	s->size = i - 1;

	return i > 1 ? 1 : ch;
}

void cstring_copy(cstring *s1, cstring s2) {

	s1->size = s2.size;

	if(s1->data)
		free(s1->data);

	s1->data = malloc(s2.size + 1);
	memcpy(s1->data, s2.data, s2.size + 1);
}

void cstring_append(cstring* cs, const char* str) {

	cs->size += strlen(str);
	cs->data = realloc(cs->data, cs->size + 1);
	strcat(cs->data, str);
}

void cstring_add(cstring* dest, cstring src) {

	dest->size += src.size;
	dest->data = realloc(dest->data, dest->size + 1);
	strcat(dest->data, src.data);
}

void cstring_set(cstring* cs, const char* str) {

	cs->size = strlen(str);

	if(cs->data)
		free(cs->data);

	cs->data = malloc(cs->size + 1);
	memcpy(cs->data, str, cs->size + 1);
}

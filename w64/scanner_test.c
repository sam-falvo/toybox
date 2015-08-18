#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "scanner_suite.h"

SCANNER *
scanner_new(void) {
	SCANNER *s;

	s = malloc(sizeof(SCANNER));
	if(s) {
		memset(s, 0, sizeof(SCANNER));
	}
	return s;
}

void
scanner_dispose(SCANNER *s) {
	SCANNERSRC *src;

	if(s) {
		while(1) {
			if(!s->src) break;
			src = s->src;
			s->src = src->next;
			free(src);
		}
		free(s);
	}
}

static int
nybble(char c) {
	int n;

	if(('a' <= c) && (c <= 'z')) c ^= 0x20;
	n = c - '0';
	if(n > 9) n -= 7;
	return n;
}

static int
ishex(char c) {
	return isdigit(c) || (('A' <= c) && (c <= 'F')) || (('a' <= c) && (c <= 'f'));
}

static int
isident0(char c) {
	return isalpha(c) || (c == '_');
}

static int
isident(char c) {
	return isdigit(c) || isident0(c);
}

int
scanner_hex(SCANNER *s) {
	SCANNERSRC *src = s->src;
	s->nval = 0;
	while(1) {
		if(src->pos >= src->length) break;
		if(!ishex(src->buffer[src->pos])) break;
		s->nval *= 16;
		s->nval += nybble(src->buffer[src->pos]);
		src->pos++;
	}
	return 1;
}

int
scanner_try_hex(SCANNER *s) {
	SCANNERSRC *src = s->src;
	char c;

	/*We need at least two chars for the 0x prefix.*/
	if(src->pos >= (src->length - 2)) return 0;

	c = src->buffer[src->pos+1];
	if(src->buffer[src->pos] != '0') return 0;
	if((c != 'x') && (c != 'X')) return 0;

	src->pos += 2;
	return scanner_hex(s);
}

int
scanner_number(SCANNER *s) {
	SCANNERSRC *src = s->src;

	if(scanner_try_hex(s)) return 1;

	s->nval = (src->buffer[src->pos])-'0';
	src->pos++;
	while(1) {
		if(src->pos >= src->length) break;
		if(!isdigit(src->buffer[src->pos])) break;
		s->nval *= 10;
		s->nval += (src->buffer[src->pos])-'0';
		src->pos++;
	}
	s->tok = TOKEN_NUM;
	return 1;
}

int
scanner_ident(SCANNER *s) {
	SCANNERSRC *src = s->src;
	int i = 0;
	char c;

	memset(s->name, 0, MAX_NAME);
	s->tok = TOKEN_IDENT;

	while(1) {
		if(src->pos >= src->length) break;
		c = src->buffer[src->pos];
		if(!isident(c)) break;
		if(i < (MAX_NAME-1)) {
			s->name[i] = c;
			i++;
		}
		src->pos++;
	}
	return 1;
}

void
scanner_skipws(SCANNER *s) {
	SCANNERSRC *src = s->src;
	while(1) {
		if(src->pos >= src->length) break;
		if(!isspace(src->buffer[src->pos])) break;
		src->pos++;
	}
}

int
scanner_next(SCANNER *s) {
	SCANNERSRC *src = s->src;

	if(!src) {
		s->tok = TOKEN_END;
		return 1;
	}

	scanner_skipws(s);

	if(src->pos >= src->length) {
		s->tok = TOKEN_END;
		return 1;
	}

	if(isdigit(src->buffer[src->pos])) return scanner_number(s);
	if(isident0(src->buffer[src->pos])) return scanner_ident(s);

	if(src->buffer[src->pos] == ',') {
		s->tok = TOKEN_COMMA;
		src->pos++;
		return 1;
	}
	if(src->buffer[src->pos] == ':') {
		s->tok = TOKEN_COLON;
		src->pos++;
		if(src->buffer[src->pos] == '=') {
			s->tok = TOKEN_DEF;
			src->pos++;
		}
		return 1;
	}
	if(src->buffer[src->pos] == '=') {
		s->tok = TOKEN_EQ;
		src->pos++;
		return 1;
	}
	return 0;
}

int
scanner_pushString(SCANNER *s, char *buffer) {
	SCANNERSRC *src = malloc(sizeof(SCANNERSRC));
	if(src) {
		memset(src, 0, sizeof(SCANNERSRC));
		src->buffer = buffer;
		src->length = strlen(buffer);
		src->next = s->src;
		s->src = src;
		/* I don't even know what it means for scanner_next to fail at
		 * this point.
		 *
		 * If it fails, does that mean we dispose only of the input
		 * source and return failure?  It seems like for scanner_next
		 * to fail here, things would need to be so wrong that the
		 * entire state of the scanner is in question.	Should we fail
		 * catestrophically?
		 */
		scanner_next(s);
	}
	return src != NULL;
}

int
scanner_token(SCANNER *s) {
	return s->tok;
}

int
scanner_nValue(SCANNER *s) {
	return s->nval;
}

char *
scanner_name(SCANNER *s) {
	return s->name;
}

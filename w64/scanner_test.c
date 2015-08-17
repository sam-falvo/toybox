#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SCANNER			struct Scanner
#define SCANNERSRC		struct ScannerSrc

#define TOKEN_END		999
#define TOKEN_NUM		1000

SCANNER {
	SCANNERSRC *src;	/*Input source stack*/
	int tok;		/*Current input token*/
	int nval;		/*For numeric tokens, their value*/
};

SCANNERSRC {
	/* Describes an input source for the scanner. */
	SCANNERSRC *next;	/*Next input in stack or NULL*/
	char *buffer;		/*String to scan*/
	int length;		/*Length of said string*/
	int pos;		/*Index to next character*/
};

SCANNER *scanner_new(void) {
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
scanner_number(SCANNER *s) {
	SCANNERSRC *src = s->src;
	if(src->pos <= (src->length-2)) {
		if(src->buffer[src->pos] == '0') {
			if(src->buffer[src->pos+1] == 'x') {
				src->pos = src->pos+2;
				return scanner_hex(s);
			}
		}
	}

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
	if(!s->src) {
		s->tok = TOKEN_END;
		return 1;
	}

	scanner_skipws(s);

	if(s->src->pos >= s->src->length) {
		s->tok = TOKEN_END;
		return 1;
	}

	if(isdigit(s->src->buffer[s->src->pos])) return scanner_number(s);
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

int
test_scanner(void) {
	SCANNER *s;
	int r;

	s = scanner_new();
	if(!s) return 0;

	r = scanner_pushString(s, "12345");
	if(!r) goto fail;

	r = scanner_token(s);
	if(r != TOKEN_NUM) {
		r = 0; goto fail;
	}

	r = scanner_nValue(s);
	if(r != 12345) {
		r = 0; goto fail;
	}

	r = scanner_next(s);
	if(!r) goto fail;

	r = scanner_token(s);
	if(r != TOKEN_END) {
		r = 0; goto fail;
	}

fail:
	if(s) scanner_dispose(s);
	return r;
}

int
test_scanner_numbers(void) {
	SCANNER *s;
	int r;

	s = scanner_new();
	if(!s) return 0;

	r = scanner_pushString(s, "12345 0x12345");
	if(!r) goto fail;

	r = scanner_token(s);
	if(r != TOKEN_NUM) {
		r = 0; goto fail;
	}

	r = scanner_nValue(s);
	if(r != 12345) {
		r = 0; goto fail;
	}

	r = scanner_next(s);
	if(!r) goto fail;

	r = scanner_token(s);
	if(r != TOKEN_NUM) {
		r = 0; goto fail;
	}

	r = scanner_nValue(s);
	if(r != 0x12345) {
		r = 0; goto fail;
	}

  r = scanner_next(s);
  if(!r) goto fail;

	r = scanner_token(s);
	if(r != TOKEN_END) {
		r = 0; goto fail;
	}

fail:
	if(s) scanner_dispose(s);
	return r;
}

int
main(int argc, char *argv[]) {
	int r;

	r = test_scanner();
	if(r) printf("Y test_scanner\n");
	else printf("N test_scanner\n");

	r = test_scanner_numbers();
	if(r) printf("Y test_scanner_numbers\n");
	else printf("N test_scanner_numbers\n");

}

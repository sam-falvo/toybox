#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SCANNER			struct Scanner
#define SCANNERSRC		struct ScannerSrc

#define TOKEN_END		999
#define TOKEN_NAT		1000

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
	if(s) {
		free(s);
	}
}

int
scanner_number(SCANNER *s) {
	s->nval = (s->src->buffer[s->src->pos])-'0';
	s->src->pos++;
	while(1) {
		if(s->src->pos >= s->src->length) break;
		if(!isdigit(s->src->buffer[s->src->pos])) break;
		s->nval *= 10;
		s->nval += (s->src->buffer[s->src->pos])-'0';
		s->src->pos++;
	}
	s->tok = TOKEN_NAT;
	return 1;
}

int
scanner_next(SCANNER *s) {
	if(!s->src) {
		s->tok = TOKEN_END;
		return 1;
	}

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
		 * entire state of the scanner is in question.  Should we fail
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
	if(r != TOKEN_NAT) {
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
main(int argc, char *argv[]) {
	int r;

	r = test_scanner();
	if(r) printf("Y test_scanner\n");
	else printf("N test_scanner\n");
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SCANNER			struct Scanner
#define SCANNERSRC		struct ScannerSrc

#define TOKEN_END		999
#define TOKEN_NUM		1000
#define TOKEN_COMMA		1001
#define TOKEN_COLON		1002
#define TOKEN_EQ		1003
#define TOKEN_DEF		1004

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

static int
setup_test_scanner(char *str, SCANNER **ps) {
	SCANNER *s;
	int r;

	*ps = 0;
	s = scanner_new();
	if(!s) return 0;

	r = scanner_pushString(s, str);
	if(!r) goto fail;

	*ps = s;
	return 1;

fail:	if(s) scanner_dispose(s);
	return 0;
}

static int
setup_test_scanner_numbers(char *str, SCANNER **ps) {
	int r;

	r = setup_test_scanner(str, ps);
	if(!r) goto fail;

	r = scanner_token(*ps);
	if(r != TOKEN_NUM) {
		r = 0; goto fail;
	}

	r = scanner_nValue(*ps);
	if(r != 12345) {
		r = 0; goto fail;
	}

	r = scanner_next(*ps);
	if(!r) goto fail;

	return 1;

fail:	if(*ps) scanner_dispose(*ps);
	return 0;
}

int
test_scanner(void) {
	SCANNER *s;
	int r;

	r = setup_test_scanner_numbers("12345", &s);
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

	r = setup_test_scanner_numbers("\t12345 0x12345", &s);
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

#define quantityof(x)	(int)((sizeof(x)/sizeof(x[0])))

int
test_scanner_punct(void) {
	SCANNER *s;
	int r;
	int outputs[] = {TOKEN_COMMA, TOKEN_COLON, TOKEN_EQ, TOKEN_DEF};
	int i;

	r = setup_test_scanner(", : = :=", &s);
	if(!r) goto fail;

	for(i = 0; i < quantityof(outputs); i++) {
		r = scanner_token(s);
		if(r != outputs[i]) {
			printf("Checking for %d; got %d\n", outputs[i], r);
			r = 0; goto fail;
		}
		r = scanner_next(s);
		if(!r) goto fail;
	}
	r = 1;

fail:	if(s) scanner_dispose(s);
	return r;
}

#define TESTDESC struct TestDesc
TESTDESC {
	char *name;
	int (*fn)(void);
};

int
main(int argc, char *argv[]) {
	int r;
	static TESTDESC tds[] = {
		{"test_scanner", test_scanner},
		{"test_scanner_numbers", test_scanner_numbers},
		{"test_scanner_punct", test_scanner_punct},
		{NULL, NULL},
	};
	TESTDESC *td;

	td = &tds[0];
	while(1) {
		if(td->name == NULL) break;
		r = td->fn();
		if(r) printf("Y %s\n", td->name);
		else printf("N %s\n", td->name);
		td++;
	}
}


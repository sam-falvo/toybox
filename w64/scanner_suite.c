#include <stdio.h>
#include <string.h>

#include "scanner_suite.h"
#include "scanner.h"

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

int
test_scanner_ident(void) {
	SCANNER *s;
	int r, i;
	char *outputs[] = {"foo", "b4r", "_baz"};

	r = setup_test_scanner("foo b4r _baz", &s);
	if(!r) goto fail;

	for(i = 0; i < quantityof(outputs); i++) {
		r = scanner_token(s);
		if(r != TOKEN_IDENT) {
			printf("Expected TOKEN_IDENT; got %d\n", r);
			goto fail;
		}

		r = strcmp(scanner_name(s), outputs[i]);
		if(r != 0) {
			printf("Checking for \"%s\"; got \"%s\" (%d).\n", outputs[i], scanner_name(s), r);
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

void
scanner_suite() {
	int r;
	static TESTDESC tds[] = {
		{"test_scanner", test_scanner},
		{"test_scanner_numbers", test_scanner_numbers},
		{"test_scanner_punct", test_scanner_punct},
		{"test_scanner_ident", test_scanner_ident},
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

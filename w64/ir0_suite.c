#include <stdio.h>
#include <string.h>

#include "ir0_suite.h"

/* Hypothetical test structure.  Not yet functional.  Not sure if I want to
 * structure my tests this way or not yet.
 */
static int
test_ir0_ml(void) {
	SCANNER *s = 0;
	IR0 *p = 0;
	int r = 0;

	s = scanner_new();
	if(!s) goto fail;
	r = scanner_pushString(s, "machineCode := 0xDEADBEEFFEEDFACE, 0x1122334455667788");
	if(!r) goto fail;
	r = 0;
	p = ir0_new(s);
	if(!p) goto fail;
	ir0_compile(p);
	if(ir0_rep_size(p) != 3) goto fail;
	if(ir0_rep_at(p, 0) != IR_LABEL) goto fail;
	if(ir0_rep_at(p, 1) != IR_DWORD) goto fail;
	if(ir0_rep_at(p, 2) != IR_DWORD) goto fail;
	if(strcmp(ir0_rparam_at(p, 0), "machineCode")) goto fail;
	if(ir0_rparam_at(p, 1) != 0xDEADBEEFFEEDFACE) goto fail;
	if(ir0_rparam_at(p, 2) != 0x1122334455667788) goto fail;

	r = 1;

fail:	if(p) ir0_dispose(p);
	if(s) scanner_dispose(s);
	return r;	
}

#define TESTDESC struct TestDesc
TESTDESC {
	char *name;
	int (*fn)(void);
};

void
ir0_suite() {
	int r;
	static TESTDESC tds[] = {
		{"test_ir0_ml", test_ir0_ml},
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

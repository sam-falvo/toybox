#ifndef W64_SCANNER_H
#define W64_SCANNER_H

#define MAX_NAME		64

#define SCANNER			struct Scanner
#define SCANNERSRC		struct ScannerSrc

#define TOKEN_END		999
#define TOKEN_NUM		1000
#define TOKEN_COMMA		1001
#define TOKEN_COLON		1002
#define TOKEN_EQ		1003
#define TOKEN_DEF		1004
#define TOKEN_IDENT		1005

SCANNER {
	SCANNERSRC *src;	/*Input source stack*/
	int tok;		/*Current input token*/
	int nval;		/*For numeric tokens, their value*/
	char name[MAX_NAME];	/*Identifier name*/
};

SCANNERSRC {
	/* Describes an input source for the scanner. */
	SCANNERSRC *next;	/*Next input in stack or NULL*/
	char *buffer;		/*String to scan*/
	int length;		/*Length of said string*/
	int pos;		/*Index to next character*/
};

extern SCANNER *scanner_new(void);
extern int scanner_pushString(SCANNER *, char *);
extern void scanner_dispose(SCANNER *);
extern int scanner_token(SCANNER *);
extern int scanner_nValue(SCANNER *);
extern int scanner_next(SCANNER *);
extern char *scanner_name(SCANNER *);

#endif

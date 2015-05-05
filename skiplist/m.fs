VARIABLE randomH

: ropen	S" /dev/urandom" r/o bin open-file throw randomH ! ;
: rclose randomH @ close-file throw ;

CREATE rbuf
	16 allot

VARIABLE c
variable level

CREATE accounts 64 cells allot
: 0ac	accounts 64 cells 0 fill ;
: rand		rbuf 16 randomH @ read-file throw 16 xor abort" 16 bytes req'd" ;
\ VARIABLE next
\ : r		next @ 1103515245 * 12345 + dup next ! 16 rshift 255 AND OVER C! 1+ ;
\ : rand		rbuf r r r r  r r r r   r r r r  r r r r drop ;
: count0	BEGIN DUP 1 AND IF DROP c ! EXIT THEN 1 RSHIFT SWAP 1+ SWAP AGAIN ;
: limit		c @ level @ > IF level @ 1+ DUP c ! level ! THEN ;
: *s		BEGIN DUP WHILE [CHAR] * EMIT 1- REPEAT DROP ;
: count		0 rbuf @ count0 ( limit c @ 1+ *s CR ) ;
: tally		1 c @ CELLS accounts + +!  c OFF ;
: measure	rand count tally ;
: r		DUP S>D <# # # #> TYPE SPACE DUP CELLS accounts + @ . ( *s ) CR 1+ ;
: 8r		r r r r  r r r r ;
: report	0 8r 8r 8r 8r 8r 8r 8r 8r DROP ;
: seed		;
\ : seed		next @ 0= IF 580239238 next ! THEN ;
: init		level off  c off ropen 0ac seed ;
: done		rclose report ;
: m		init 0 begin dup 65536 < while measure 1+ repeat drop done ;


\ Our virtual storage space.

: KB
	1024 * ;

CREATE space
	256 KB ALLOT

\ Since we know that we have a 256KB workspace, and that our nodes are 128
\ bytes long, we use a single bit to determine if any 128-byte chunk of space
\ is in use or not.  This gives us enough room for 2048 btree nodes.

CREATE bam
	256 ALLOT

\ 0 bits indicates that a block is free; 1 bits indicates that a block is in
\ use.

bam 256 0 FILL

VARIABLE bamBlock
VARIABLE bamByte
VARIABLE bamMask

: markHelper ( n -- )
	DUP bamBlock !
	1 OVER 7 AND LSHIFT bamMask !
	3 RSHIFT bamByte ! ;

: markAllocated ( n -- )
	markHelper  bam bamByte @ + DUP C@ bamMask @ OR SWAP C! ;

: markFreed ( n -- )
	markHelper  bam bamByte @ + DUP C@ bamMask @ INVERT AND SWAP C! ;

\ If we're going to allocate a block, we need a way to find an available block
\ first.  Returns the next available free block, or else undefined and a THROW
\ code.
: findFree ( -- n e )
	bamBlock OFF bamByte OFF 1 bamMask !
	BEGIN
		bamByte @ 256 U>= IF
			0 -35 EXIT ( invalid block # error )
		THEN
		bam bamByte @ + C@ bamMask @ AND 0= IF
			bamBlock @ 0 EXIT
		THEN
		bamMask @ 2* DUP bamMask !
		256 = IF
			1 bamMask !
			1 bamByte +!
		THEN
		1 bamBlock +!
	AGAIN ;

\ We need a block allocator.  No need for a separate Free function; just
\ mark the block freed when you're done with it.
: allocBlock ( -- n e )
	findFree DUP IF EXIT THEN
	OVER markAllocated ;

\ B-tree nodes have 4 keys, 5 pointers.  We pad to 128 bytes to make
\ allocation using bitmaps easier (it's a better approximation of the
\ filesystem application which I'm interested in).

0
DUP CONSTANT	node_keyCount CELL+	( how many keys?  [2, 4])
DUP CONSTANT	node_level CELL+	( 0 is leaf, 1+ interior )
DUP CONSTANT	node_key1 CELL+
DUP CONSTANT	node_key2 CELL+
DUP CONSTANT	node_key3 CELL+
DUP CONSTANT	node_key4 CELL+
DUP CONSTANT	node_ptr0 CELL+
DUP CONSTANT	node_ptr1 CELL+
DUP CONSTANT	node_ptr2 CELL+
DUP CONSTANT	node_ptr3 CELL+
DUP CONSTANT	node_ptr4 CELL+
DUP CONSTANT	node_padding 40 +	( pad to 128 bytes )
CONSTANT /node

\ Our "superblock" sits at block 0.  It's just a straight pointer to the
\ root of the tree; nothing fancy.

0 markAllocated

: nod ( n -- a )
	/node * space + ;

: zeroNode ( n -- )
	nod  /node 0 FILL ;

0 zeroNode

\ To insert something into the tree, we use this convenient bunch of
\ variables.
VARIABLE k
VARIABLE v


\ =========================================================================
\ Insert into B-Tree.
\ =========================================================================


VARIABLE n
VARIABLE nn

: searchNode0 ( -- e )
	n @ 0= IF
		-35 EXIT
	THEN
	n @ node_keyCount + @ 0= IF
		-35 EXIT
	THEN

	n @ node_ptr0 + @ v !	( default, just in case )

	\ Search the node from high key to low key.
	\ That way, if we fail the test for key4, but we
	\ pass the test for key3, then we know that k must
	\ reside between key3 <= k < key4.  And so on.
	n @ node_keyCount + @ 1-
	BEGIN DUP 0 >= WHILE
		DUP CELLS n @ node_key1 + + @ k @ <= IF
			CELLS n @ node_ptr1 + + @ v !
			n @ node_level + @ IF	( if not a leaf node, search in child )
				v @ DUP nn ! nod n !
				n @ node_keyCount + @
			ELSE ( return what we have )
				0 EXIT
			THEN
		THEN
		1-
	REPEAT DROP
	\ We didn't find what we're looking for here.
	\ Thus the key k < key1 < ... < key 4.  Take
	\ the default path, but only if we're not a leaf node.
	n @ node_level + @ IF
		v @ DUP nn ! nod n !
		RECURSE EXIT
	THEN

	\ Otherwise, we're really at a leaf, and we definitely
	\ don't have a clue where to find your value.  Sorry, bub.
	\ I hope the default value is what you're looking for.
	n @ node_ptr0 + @ v !  0 ;

: searchNode ( -- e )
	space @ DUP nn ! nod n !  searchNode0 ;


VARIABLE p
VARIABLE pn
VARIABLE q
VARIABLE qn

: insertNode ( -- e )
	( simplest possible case: a brand new tree )
	space @ 0= IF
		allocBlock THROW space !
		space @ nod n !
		n @ /node 0 FILL
		1  n @ node_keyCount + !
		k @  n @ node_key1 + !
		v @  n @ node_ptr1 + !
		0 EXIT
	THEN

	( next simplest: find a node with empty slots )
	v @  searchNode THROW  v !
	n @ node_keyCount + @ 4 < IF
		( Find our insertion point, vacate a slot, then insert. )
		0 BEGIN DUP n @ node_keyCount + @ < WHILE
			DUP CELLS n @ node_key1 + + @ k @ > IF
				( holy shit this is complicated logic. )
				( there has to be an easier way. )
				DUP CELLS n @ node_key1 + +
				OVER 3 SWAP - CELLS >R
				DUP CELL+ R@
				MOVE
				
				DUP CELLS n @ node_ptr1 + +
				DUP CELL+ R> MOVE

				k @ OVER CELLS n @ node_key1 + + !
				v @ SWAP CELLS n @ node_ptr1 + + !
				1 n @ node_keyCount + +!
				0 EXIT
			THEN
			1+
		REPEAT
		( If we're here, we're in the very last slot.  Append. )
		k @ OVER CELLS n @ node_key1 + + !
		v @ SWAP CELLS n @ node_ptr1 + + !
		1 n @ node_keyCount + +!
		0 EXIT
	THEN

	( We've hit a node that is completely full.  We must rebalance! )
	allocBlock THROW DUP p ! nod pn !	( create the new parent node )
	allocBlock THROW DUP q ! nod qn !	( create new child node )
	p @ zeroNode  q @ zeroNode

	( configure parent node to point to our respective children )
	n @ node_level + @  1+  pn @ node_level + !
	n @ node_key3 + @  pn @ node_key1 + !	( the middle key )
	q @  pn @ node_ptr1 + !			( anything >= is in new child )
	nn @ pn @ node_ptr0 + !			( anything < is in original )
	1 pn @ node_keyCount + !		( parent now has 1 key )

	( migrate KVPs from old child to new )
	n @ node_level + @  qn @ node_level + !
	n @ node_key4 + @  qn @ node_key1 + !
	n @ node_ptr4 + @  qn @ node_ptr1 + !
	n @ node_ptr3 + @  qn @ node_ptr0 + !
	1 qn @ node_keyCount + !
	2 n @ node_keyCount + !

	( promote parent node accordingly )
	p @ space !
	RECURSE ;


\ Our virtual storage space.

create supdate 16 cells allot
variable dirty
variable ilevel
variable ivalue
variable p
variable pn
variable prevCyl
variable prevSect
variable q
variable qn
variable readCount
variable sdone
variable seekCount
variable sfound
variable skey
variable slevel
variable svalue
variable writeCount

: sectors	512 * ;
: kb		sectors 2* ;
create space	1440 kb allot

\ Sector buffer management
: wb		1 writeCount +! 1 ms ;
: r		1 readCount +! 1 ms ;
: ?w		dirty @ if wb dirty off then ;
: seek		dup 36 / prevCyl !  1 seekCount +! 18 ms ;
: s		dup prevSect ! dup 36 / prevCyl @ xor if seek then ;
: sector	dup prevSect @ xor if ?w r then s sectors space + ;
: changed	dirty on ;

: try		dup sector @ 0= if $140DE over sector ! changed
		r> drop exit then 1+ ;
: alloc		1 begin dup 2880 < while try repeat ;

variable next
create rbuf 8 allot
: r		next @ 1103515245 * 12345 + dup next ! 16 rshift 255 AND OVER C! 1+ ;
: rand		rbuf r r r r  r r r r drop ;
: count0	begin dup 1 and if drop ilevel ! exit then 1 rshift swap 1+ swap again ;
: cap		ilevel @ 15 min 0 max ilevel ! ;
: count		0 rbuf @ count0 cap ;
: newlevel	rand count ;

\ 0123456789	0123456789	0123456789

0
dup constant	nh_magic	cell+
dup constant	nh_level	cell+
dup constant	nh_key		cell+
dup constant	nh_value	cell+
dup constant	nh_ptrs		16 cells +
constant	nh_sizeof


: x  . ." x :: " .s cr ;

: ss		slevel @ 0< if sfound off sdone on exit then
		pn @  supdate slevel @ cells +  !
		p @ slevel @ cells + nh_ptrs + @ dup qn ! sector q !
		qn @ if
			q @ nh_key + @ skey @ < if
				qn @ pn !
				q @ p !
				exit
			then
		then
		-1 slevel +! ;

: search	0 pn ! 0 sector p !  p @ nh_level + @ slevel !
		sdone off begin sdone @ 0= while ss repeat
		qn @ if q @ nh_key + @ skey @ = if sfound on  q @ nh_value + @ svalue ! then then ;


: rlvl		0 sector nh_level + @ ;
: rlvl!		0 sector nh_level + ! changed ;
: patch		rlvl 1+ begin dup ilevel @ <= while 0 over cells supdate + !  1+ repeat drop ;
: iupdate	ilevel @ rlvl > if patch ilevel @ rlvl! then ;

: s ( s# i -- a )	cells swap sector nh_ptrs + + ;
: supd ( i -- a )	cells supdate + ;
: relink ( i -- )	>R qn @ r@ s r@ supd @ R@ s @ swap ! 
			qn @ r@ supd @ r> s ! changed ;
: insert	search sfound @ if ivalue @ qn @ sector nh_value + ! changed exit then
		newlevel iupdate
		alloc dup qn ! sector q !
		skey @ q @ nh_key + !
		ivalue @ q @ nh_value + !
		ilevel @ q @ nh_level + !
		0 begin dup ilevel @ <= while dup relink 1+ repeat drop ;


: init		79 prevCyl ! dirty off seekCount off writeCount off
		readCount off space 1440 kb 0 fill
		prevSect 4 cells $FF fill
		8734925762345 next !
		$DDDD space nh_magic + !  -1 space nh_level + ! ;


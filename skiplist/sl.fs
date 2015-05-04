\ Our virtual storage space.

variable seekCount
variable writeCount
variable readCount

: sectors	512 * ;
: kb		sectors 2* ;
create space	1440 kb allot

\ Sector buffer management

variable	prevCyl
variable	prevSect
variable	dirty

: wb		1 writeCount +! ;
: r		1 readCount +! ;
: ?w		dirty @ if wb dirty off then ;
: seek		dup 36 / prevCyl !  1 seekCount +! ;
: s		dup prevSect ! dup 36 / prevCyl @ xor if seek then ;
: sector	dup prevSect @ xor if ?w r then s sectors space + ;
: changed	dirty on ;


\ 0123456789	0123456789	0123456789

0
dup constant	nh_magic	cell+
dup constant	nh_level	cell+
dup constant	nh_key		cell+
dup constant	nh_value	cell+
dup constant	nh_ptrs		16 cells +
constant	nh_sizeof


variable pn
variable p
variable slevel
variable qn
variable q
variable sfound
variable skey
variable svalue
variable sdone


: ss		slevel @ 0< if sfound off sdone on exit then
		p @ slevel @ cells + nh_ptrs + @ dup qn !
		qn @ if
			qn @ sector q !
			q @ nh_key + @ skey @ < if
				qn @ pn !
				q @ p !
				exit
			then
			q @ nh_key + @ skey @ = if
				q @ nh_value + @ svalue !
				sfound on sdone on
				exit
			then
		then
		-1 slevel +! ;

: search	0 pn ! 0 sector p !  p @ nh_level + @ slevel !
		sdone off begin sdone @ 0= while ss repeat ;


: init		seekCount off 79 prevCyl ! dirty off seekCount off
		writeCount off readCount off space 1440 kb 0 fill
		-1 space nh_level + ! ;


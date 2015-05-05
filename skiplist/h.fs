init
: ii		dup skey ! 10 * ivalue !  insert ;
: iii		10 begin dup 1000 < while dup ii 10 + repeat drop ;
iii


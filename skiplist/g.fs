0 sector constant hd

1 sector constant a	\ 1
2 sector constant b	\ 2
3 sector constant c	\ 3
4 sector constant d	\ 2
5 sector constant e	\ 1


$4EADE2 constant h_magic
$0140DE constant l_magic


h_magic hd nh_magic + !
l_magic  a nh_magic + !
l_magic  b nh_magic + !
l_magic  c nh_magic + !
l_magic  d nh_magic + !
l_magic  e nh_magic + !

2 hd nh_level + !
0  a nh_level + !
1  b nh_level + !
2  c nh_level + !
1  d nh_level + !
0  e nh_level + !

10 a nh_key + !
20 b nh_key + !
30 c nh_key + !
40 d nh_key + !
50 e nh_key + !

100 a nh_value + !
200 b nh_value + !
300 c nh_value + !
400 d nh_value + !
500 e nh_value + !

1 hd nh_ptrs + !
2  a nh_ptrs + !
3  b nh_ptrs + !
4  c nh_ptrs + !
5  d nh_ptrs + !


2 hd nh_ptrs + cell+ !
3  b nh_ptrs + cell+ !
4  c nh_ptrs + cell+ !

3 hd nh_ptrs + 2 cells + !


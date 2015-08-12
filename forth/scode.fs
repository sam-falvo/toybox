\ We compile S16X4-compatible machine forth code into a vector of SCODE
\ structures.
\ 
\ SCODE structures are 4-byte entities, that consist of an 8-bit opcode, and
\ a 24-bit parameter field.  The parameter field isn't always used.

: sx32
  \ sign-extend 32-bit value for 64-bit or higher systems.
  dup $80000000 and 2* negate or ;

: opcodeFromScode
  255 and ;

: paramFromScode
  sx32 256 / ;

: makeScode ( opc param -- scode )
  256 * swap 255 and or ;


\ SCODEs are always written to memory in little-endian format.
\ RISC-V is little-endian, as is x86.  This allows for an efficient way to
\ store SCODEs on these platforms.

: 8! ( value addr -- addr+1 )
  2dup c!  1+ swap 256 / swap ;

: 8@ ( addr value -- addr+1 value' )
  8 lshift over c@ 255 and or swap 1- swap ;

: le32! ( value addr -- )
  8! 8! 8! 8! 2drop ;

: le64! ( value addr -- )
  8! 8! 8! 8! le32! ;

: le32@ ( addr -- value )
  3 + 0 8@ 8@ 8@ 8@ nip ;


\ Each S16X4 primitive has a corresponding SCODE.

0 0 makeScode constant NopCode
1 0 makeScode constant LitCode

2 0 makeScode constant FwmCode
3 0 makeScode constant SwmCode
4 0 makeScode constant FbmCode
5 0 makeScode constant SbmCode

6 0 makeScode constant AddCode
7 0 makeScode constant AndCode
8 0 makeScode constant XorCode

9 0 makeScode constant JeqCode
10 0 makeScode constant JneCode
11 0 makeScode constant CallCode
12 0 makeScode constant JmpCode
13 0 makeScode constant RfsCode

\ Labels to support multi-pass compilation

14 0 makeScode constant LabelCode
15 0 makeScode constant FwgCode		\ fetch word relative to globals

\ Some additional operations not supported natively by the S16X4.

16 0 makeScode constant DupCode
17 1 makeScode constant DropCode
18 0 makeScode constant SwapCode
19 0 makeScode constant RotCode
20 0 makeScode constant PushCode
21 0 makeScode constant PopCode
22 1 makeScode constant RDropCode
23 1 makeScode constant RFetchCode

\ Miscellaneous

24 0 makeScode constant LoadGPCode

\ First pass of compiling Forth code is to write SCODEs into a buffer.

variable sbp
create scodeBuffer    256 4 * allot
variable labelsUsedSoFar
variable sp		\ slot pointer
variable pp		\ pass pointer
: scodeBuffer0
  scodeBuffer 256 4 * $cc fill
  scodeBuffer sbp !  scodeBuffer sp !
  0 labelsUsedSoFar ! ;
scodeBuffer0

: reserve ( x -- i )
  \ Reserve a dword slot at the beginning of the buffer.
  \ Initialize the slot.  Then, return an offset for the
  \ slot.
  sp @ dup 8 + sbp @ sp @ - move
  sp @ le64!
  sp @ scodeBuffer -
  8 sp +!  8 sbp +!  8 pp +! ;

: getlab  14 labelsUsedSoFar @ makeScode 1 labelsUsedSoFar +! ;

: sb,     sbp @ le32!  4 sbp +! ;
: nop,    NopCode sb, ;
: small     dup -2048 2048 within ;
: biglit,   reserve 15 swap makeSCode sb, ;
: lit,    small if 1 swap makeScode sb, exit then biglit, ;
: fwm,    FwmCode sb, ;
: swm,    SwmCode sb, ;
: fbm,    FbmCode sb, ;
: sbm,    SbmCode sb, ;
: add,    AddCode sb, ;
: and,    AndCode sb, ;
: xor,    XorCode sb, ;
: if,     getlab 9 over paramFromScode makeSCode sb, ;
: then,   sb, ;
: call,   11 swap makeScode sb, ;
: proc      create sbp @ scodeBuffer - , does> @ call, ;
: extern    32 word count type ."  at $" sbp @ scodeBuffer - s>d hex <# # # # # #> decimal type cr ;
: :,      >IN @ extern >IN ! proc ;
: ;,      RfsCode sb, ;
: begin,  getlab dup sb, ;
: while,  if, swap ;
: again,  12 swap paramFromScode makeSCode sb, ;
: repeat, again, then, ;


: dup,    DupCode sb, ;
: drop,   DropCode sb, ;
: swap,   SwapCode sb, ;
: rot,    RotCode sb, ;
: push,   PushCode sb, ;
: pop,    PopCode sb, ;
: r@,     RFetchCode sb, ;

: ext:    create , does> @ call, ;

\ Diagnostic disassembly of SCODEs.

: opname  S" NOP   LIT   FWM   SWM   FBM   SBM   ADD   AND   XOR   JEQ   JNE   JSR   JMP   RFS   LABEL FWG   DUP   DROP  SWAP  ROT   PUSH  POP   RDRP  R     LDGP  " drop ;
: opc     255 and 6 * opname + 6 type ;
: param   256 / . ;
: h.      hex s>d <# # # # # # # # # #> type ."  " decimal ;
: insn    dup scodeBuffer - s>d <# # # # # # #> type ."  " le32@ dup h. dup opc param cr ;
: dis     cr scodeBuffer begin dup sbp @ < while dup insn 4 + repeat drop ;


\ During compilation, we distinguish between literals
\ that fit in an ADDI instruction, and those that do not.
\ For those that require fetching from RAM, we use the
\ FWG instruction.  This instruction, however, requires
\ that we establish the value of our global pointer (GP)
\ register.  Thus, we must establish the GP register
\ before the first use of FWG, both from the beginning
\ of the compiled code, and since a subroutine might
\ alter the GP register for its own use, after any
\ CallCode instructions as well.

variable gpvalid

: loadGPpass
  sp @ pp !
  gpvalid off
  begin pp @ sbp @ < while
    pp @ le32@ opcodeFromScode 15 = gpvalid @ 0= and if
      pp @ dup 4 + sbp @ pp @ - move
      LoadGPCode pp @ le32!
      4 pp +!  4 sbp +!
      gpvalid on
    else pp @ le32@ opcodeFromScode 11 = if
      gpvalid off
    then then
    4 pp +!
  repeat ;


\ PASS: Remove labels, create label table

variable scp
variable labelptr

: collapse
  scp @ dup 4 + swap over labelptr @ swap - .s cr move ;

: removeLabel
  \ Eat the LabelCode pseudo-instruction.
  \ Move all subsequent code up, freeing up four bytes at the end.
  \ Point labelptr there, and store the label table entry there.
  \ 
  \     +-------+------+----------------------+
  \     | label | 0000 |       offset         |
  \     +-------+------+----------------------+
  \     3      2 2    2 1
  \     1      4 3    0 9                    0
  \ 
  \ label is between 0 and 255 inclusive.
  \ offset is the BYTE offset into the currently compiled code buffer.
  \ 
  \ Returns true if a label was found.  False otherwise.
  scodeBuffer scp !
  begin scp @ labelptr @ < while
    scp @ le32@ opcodeFromScode 14 = if
      scp @ le32@ paramFromScode 24 lshift
      scp @ scodeBuffer - $FFFFF and or >r
      collapse
      -4 labelptr +!  r> labelptr @ le32!
      -1 exit
    then
    4 scp +!
  repeat 0 ;

: removeLabels
  \ This is the main entry point to the label removal step.
  sbp @ labelptr !
  begin
    removeLabel 0= if exit then
  again ;

: .label
  le32@ dup 24 rshift 255 and . ."  $" $FFFFF and hex s>d <# # # # # # #> type decimal cr ;

: .labels
  labelptr @ cr
  begin dup sbp @ < while dup .label 4 + repeat drop ;


\ Once we have our local label table, we can now apply fix-up the SCODEs
\ that target those labels.

: address
  >r labelptr @
  begin dup sbp @ < while
    dup le32@ 24 rshift 255 and r@ = if
      le32@ $FFFFF and r> drop exit
    then
    4 +
  repeat -1 abort" Undefined label; cannot resolve!" ;

: !fixup
  dup le32@ paramFromScode address
  over le32@ opcodeFromScode swap makeScode over le32! ;

: ?fixup
  dup le32@ opcodeFromScode 9 = if !fixup exit then
  dup le32@ opcodeFromScode 10 = if !fixup exit then
  dup le32@ opcodeFromScode 12 = if !fixup exit then ;

: fixupBranches
  scodeBuffer
  begin dup labelptr @ < while ?fixup 4 + repeat drop ;

\ The two previous passes are designed to go together.

: fixupLabels
  removeLabels fixupBranches ;


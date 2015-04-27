\ Applications can provide callbacks for a variety of system events.  However,
\ if an application needs to be terminated for any reason, we need a simple
\ way for the kernel to restore known-good event handlers.
\ 
\ The approach Eden takes is to express callbacks in a data structure which an
\ application can provide to the kernel.  This structure, expressible with a
\ single pointer, contains references to callbacks for appropriate event
\ handlers.

0
DUP CONSTANT  vt_shellInit ( - )                CELL+
DUP CONSTANT  vt_keyHandler ( ch - )            CELL+
DUP CONSTANT  vt_1sTick ( - )                   CELL+
DUP CONSTANT  vt_prompt ( - )                   CELL+
DUP CONSTANT  vt_shellLineHandler ( caddr u - ) CELL+
CONSTANT      vt_sizeof

\ Applications set their custom event handlers by adjusting this variable.
\ Use 0 for any handler you're not interested in.
VARIABLE current-vt

\ This initializes the system shell for the Eden operating system.  It should
\ render the initial display for the user, so the user knows the program is
\ running.  It should also establish new handlers for various events generated
\ by the system.
: ^shellInit ( -- a )
        current-vt @ vt_shellInit + ;

\ This handler deals with individual key presses as they arrive.  By default,
\ this vector points to the shell's line input handler.  This is generally
\ what you want for most CLI applications.
: ^keyHandler ( -- a )
        current-vt @ vt_keyHandler + ;

\ This handler is called after every second elapses.
: ^1sTick ( -- a )
        current-vt @ vt_1sTick + ;

\ Prints the prompt for the current command-line program.  Note that Eden does
\ not use this vector; it's called by the shell.
: ^prompt ( -- a )
        current-vt @ vt_prompt + ;

\ When the user submits a line of text for interpretation, this vector will be
\ invoked.
: ^shellLineHandler ( -- a )
        current-vt @ vt_shellLineHandler + ;

\ Applications, when they're "done", call this vector.  Note that this is a
\ case where the application calls back *into* the OS, not the other way
\ around.  Hence, this vector is initialized by the OS, and doesn't need to be
\ multiplexed among different applications.
VARIABLE ^shellDone ( -- )


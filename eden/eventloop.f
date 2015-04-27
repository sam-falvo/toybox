VARIABLE previousSecond


\ For jobs that are scheduled, Eden needs to maintain a queue of things to do.

CREATE taskQueue
        q_sizeof ALLOT
        taskQueue q_sizeof 0 FILL


\ When Eden's "kernel" (if you can call this a kernel) first boots, it's not
\ running any application at all.  Therefore, default all event handlers to
\ no-operations.

: doNothing ( -- )
        ;

CREATE default-vectors
        ' doNothing ,   ( shellInit )
        ' doNothing ,   ( keyHandler )
        ' doNothing ,   ( 1sTick )
        ' doNothing ,   ( prompt )
        ' doNothing ,   ( shellLineHandler )

default-vectors current-vt !


\ Call the keyboard handler if a key has been pressed.

: pollKeyboard ( -- )
        KEY? IF KEY ^keyHandler @ EXECUTE THEN ;


\ Call the 1sTick event handler when the Forth clock says a second has
\ elapsed.

: seconds
        TIME&DATE 2DROP 2DROP DROP ;

: 1s-passed? ( -- f )
        seconds previousSecond @ = NOT ;

: pollTimer ( -- )
        1s-passed? IF seconds previousSecond !   ^1sTick @ EXECUTE THEN ;


\ Dispatch the next pending callback.  The caller must make sure one is
\ available to call back.

: runScheduledTask ( -- )
        queueGetHead queueRemHead EXECUTE ;


\ Schedule an arbitrary Forth word.  The word MUST take no parameters and MUST
\ not deliver results on any stacks.  Ergo, it must take its information from
\ other sources, such as global variables.  In a real-world asynchronous OS,
\ we'd want to also include some kind of context-sensitive parameter as well,
\ so as to emulate closure-like behavior.  That'd allow us to at least re-use
\ event handlers in different contexts.
\ 
\ Still, for this quick and dirty demonstration, it's proven to be rather
\ useful as-is.

: schedule ( xt -- )
        taskQueue current-queue !  queueAddTail ;


\ This is the main event loop of the Eden pseudo-operating-system.  We start
\ out by initializing the "very first program," which is assumed to be a shell.
\ Like any async program, it establishes its default set of event handlers,
\ kicks off the initial screen update for the user's benefit, and then drops
\ into an endless event loop.  At least with SwiftForth, use CTRL-C to exit.

: eventLoop ( -- )
        taskQueue current-queue !  queueReset
        ^shellInit @ EXECUTE
        BEGIN   BEGIN   taskQueue current-queue ! ( a handler could have changed it )
                        queueEmpty? NOT
                WHILE   runScheduledTask
                REPEAT

                BEGIN   taskQueue current-queue !  queueEmpty?
                WHILE   pollKeyboard pollTimer
                REPEAT
        AGAIN ;

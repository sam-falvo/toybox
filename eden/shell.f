\ Forth is strictly a "define before you use" language, being a strictly early
\ bound language.  So to read the program's comments like a narrative, read
\ from the bottom up.

VARIABLE nChars
80 CONSTANT maxChars
CREATE shellBuffer   maxChars ALLOT

\ The keyboard handler accumulates text into a line input buffer.  This is
\ a brutally simple handler: no line editing facilities at all (not even
\ backspace!).  If you wanted, though, here you'd implement a gap-buffer editor
\ which would allow for basic line editing.

: shellKH ( ch - )
      DUP $0D = OVER $0A = OR IF
        DROP CR
        shellBuffer nChars @ ^shellLineHandler @ EXECUTE
        nChars OFF
        ^prompt @ EXECUTE
        EXIT
      THEN

      nChars @ maxChars < IF
        DUP EMIT
        shellBuffer nChars @ + C!
        1 nChars +!
      THEN ;


\ The shell's prompt handler .... prints a prompt.

: shellPrompt ( - )
      ." Eden> " ;

\ When handling a line of text, we should look through the user's path for a
\ program of the anticipated name, load it, and then execute it.  That's too
\ much work for a simple prototype like Eden, so I just brute-force it by
\ launching built-in applications instead.

: shellLH ( caddr u - )
      2DUP S" clock" COMPARE 0= IF 2DROP doClock EXIT THEN
      ." Unknown command: " TYPE CR ;


\ When an application "terminates," it must restore its use of the
\ current-vectors variable, then invoke the ^shellDone event handler, which the
\ shell implements.  In our case, we simply print the prompt and return.  No
\ further action is needed at this time.

: shellDone ( - )
      ^prompt @ EXECUTE ;


\ When booting Eden, the shell is the first "application" it ever runs.  Its
\ initialization is therefore somewhat special compared to more "normal" apps
\ you might invoke.
\ 
\ First, the shellInit callback needs to be set.  We can't just put it ahead
\ of the callback vectors data structure because we'll need to make a forward
\ reference to it later.  So we instead "defer" its implementation.  This
\ creates a surrogate definition which we'll patch to the real implementation
\ later.

DEFER shellInit


\ We tell Eden that we want our own keyboard handler and prompting handlers.
\ We, the shell, actually invoke the line handler, not the kernel.  However,
\ we have our own line handler too, because duhh, we're a shell interface.  :)

CREATE shellHandlers
      ' shellInit   ,   ( shellInit )
      ' shellKH     ,   ( keyHandler )
      ' doNothing   ,   ( 1sTick )
      ' shellPrompt ,   ( prompt )
      ' shellLH     ,   ( shellLineHandler )


\ On initialization, we clear the screen and announce.  We print a prompt, then
\ wait for the user to type a legitimate command, which is really what we're
\ interested in.

: (shellInit) ( - )
      PAGE ." Eden Shell 1.0" CR
      shellPrompt
      shellHandlers current-vt !
      ['] shellDone ^shellDone !
      ;


\ This is the special part.  Since Eden cannot call the shell's initialization
\ vector above at run-time (not without scanning through memory for a signature
\ telling Eden where it sits in memory), we have to monkey-patch the
\ default-vectors variable at compile-time.  Thus, the shell, despite being a
\ completely normal program in every other way, must be "compiled into" the
\ kernel for this to work.

' (shellInit) IS shellInit
' shellInit  default-vectors !


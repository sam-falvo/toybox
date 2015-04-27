\ Forth is strictly a "define before you use" language, being a strictly early
\ bound language.  So to read the program's comments like a narrative, read
\ from the bottom up.

VARIABLE old_vt
VARIABLE seconds

\ Upon quitting, restore the OS-default handler vectors, and tell the shell
\ that we're done.

: clockQuit ( - )
      old_vt @ current-vt !
      ^shellDone @ EXECUTE ;

\ The user can press Q to exit the application, or S to switch to a new line.
\ S is useful for measuring the delay between two events.  Just press S when
\ said event of interest happens, and now you have a record of when it happened
\ emblazened on your console display.
\ 
\ Case is sensitive.  Q and S, not q and s.

: clockKH ( ch - )
      [char] Q OVER = IF DROP clockQuit EXIT THEN
      [char] S = IF ['] CR schedule EXIT THEN ;

\ On a clock tick, we just increment our counter and display the number of
\ seconds elapsed since running the application.

: clockTick ( - )
      seconds @ . 5 SPACES 13 EMIT
      1 seconds +! ;

\ When launching the clock, we announce our presence to the user, then
\ acquire keyboard and timer tick callbacks.  Since we want to give the user
\ the appearance of a synchronous application, we also consume prompt display
\ events, where we just do nothing with them.

CREATE clockHandlers
      0             ,   ( shellInit )
      ' clockKH     ,   ( keyHandler )
      ' clockTick   ,   ( 1sTick )
      ' doNothing   ,   ( prompt )
      ' doNothing   ,   ( shellLineHandler )

: doClock ( - )
      ." Press Q to quit." CR
      seconds OFF
      current-vt @ old_vt !
      clockHandlers current-vt ! ;


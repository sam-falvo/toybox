\ Queues are simply double-ended arrays, allocated with ALLOCATE, and freed
\ with FREE.  This is a very, very naive implementation, designed to get the
\ job done, not necessarily quickly, or in the smallest memory possible.


VARIABLE current-queue

0
DUP CONSTANT q_vbase                    CELL+
DUP CONSTANT q_vcap                     CELL+
DUP CONSTANT q_qhead                    CELL+
DUP CONSTANT q_qtail                    CELL+
CONSTANT q_sizeof


\ Vector base address
: vbase ( -- a )
        current-queue @ q_vbase + ;

\ Vector capacity, in elements
: vcap ( -- a )
        current-queue @ q_vcap + ;

\ Index of head element
: qhead ( -- a )
        current-queue @ q_qhead + ;

\ Index just past the tail element
: qtail ( -- a )
        current-queue @ q_qtail + ;


\ Empty the queue.  To get things started, we assume a vector of 16 cells.

: queueReset ( -- )
        vbase @ IF vbase @ FREE THROW THEN
        16 CELLS ALLOCATE THROW vbase !
        16 vcap !
        0 qhead !   0 qtail ! ;


\ Is the queue empty?  Fairly obvious.

: queueEmpty? ( -- f )
        qhead @ qtail @ = ;


\ When we exceed one of the bounds of the vector backing the queue, we need to
\ expand it in the appropriate direction.  You can expand it in the "front",
\ or in the "back", depending on whether you're adding elements to the front
\ or to the back of the vector.

VARIABLE tmpbase
VARIABLE tmpcap
VARIABLE tmphead
VARIABLE tmptail

: queueVectorExpandFront ( -- )
        vcap @ 2* tmpcap !
        tmpcap @ CELLS ALLOCATE THROW tmpbase !
        qhead @ vcap @ + tmphead !
        qtail @ vcap @ + tmptail !

        vbase qhead @ CELLS +
          tmpbase @ qhead @ vcap @ + CELLS +
          qtail @ qhead @ - CELLS
          MOVE

        vbase @ FREE THROW

        tmptail @ qtail !
        tmphead @ qhead !
        tmpbase @ vbase !
        tmpcap @ vcap ! ;

: queueVectorExpandBack ( -- )
        vcap @ 2* tmpcap !
        tmpcap @ CELLS ALLOCATE THROW tmpbase !

        vbase @ tmpbase @ qtail @ CELLS MOVE
        vbase @ FREE THROW

        tmpbase @ vbase !
        tmpcap @ vcap ! ;


\ These routines let you add elements to the head-end ("front") of the queue,
\ inspect what's there, and to pop the head element off again.

: queueAddHead ( x -- )
        qhead @ IF                                      ( 0 < qhead )
                -1 qhead +!                             ( 0 <= qhead )
                qhead @ CELLS vbase @ + !
                EXIT
        THEN
        ( 0 = qhead ) queueVectorExpandFront ( 0 < qhead ) RECURSE ;

: queueGetHead ( -- x )
        vbase @ qhead @ CELLS + @ ;

: queueRemHead ( -- )
        qhead @ qtail @ < IF
                1 qhead +! ( qhead <= qtail )
        THEN ( qhead = qtail ) ;


\ These routines let you add elements to the tail-end ("back") of the queue,
\ inspect what's there, and to pop the tail element off again.

: queueAddTail ( x -- )
        qtail @ vcap @ < IF
                vbase @ qtail @ CELLS + !       ( qtail < vcap )
                1 qtail +!                      ( qtail <= vcap )
                EXIT
        THEN
        ( qtail = vcap ) queueVectorExpandBack ( qtail < vcap ) RECURSE ;

: queueGetTail ( -- x )
        vbase @ qtail @ 1- CELLS + @ ;

: queueRemTail ( -- )
        qhead @ qtail @ < IF
                -1 qtail +!     ( qhead <= qtail )
        THEN ( qhead = qtail ) ;


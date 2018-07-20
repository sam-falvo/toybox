# Experimental Interface Manager

XIM is intended to cut all the fat of my previous GUI attempts (which tried to combine various aspects of GEOS and GEM together, in various proportions),
and just focuses on offering a GEOS-like environment.

This means:

* No workstations, no video drivers.  You get the screen you're dealt with.  You can *query* its size and attributes, but that's it.
* The XIM *executable* provides the linkage to the host operating system; your "applications" are what's written as shared objects.
* FIXME: More that I can't remember; it's 11PM and almost bed time.

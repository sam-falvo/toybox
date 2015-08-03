This is a spike GUI implementation.

I use SDL 2.0 as a raw frame buffer, at least to the greatest extent that I can.  It takes inspiration from [GEM](https://en.wikipedia.org/wiki/Graphical_Environment_Manager) and from [GEOS](https://en.wikipedia.org/wiki/GEOS_%288-bit_operating_system%29) in about equal parts.  Every now and again, you might find some Intuition influence, but overall, I'm attempting to model a minimalistic GUI environment --- the smallest, simplest thing that could possibly work and remain convenient and comfortable to code for.

To build it, you'll want to use the [qo](https://github.com/andlabs/qo) build tool.  Just cd into this directory and type `qo`.  You should get an executable named `gui`.  Enjoy.

## methodology

Write software top-down.  Let the needs (and my gut feel) dictate how to evolve the software.  Evolve lower-level code based on higher-level needs.

Note: there's no unit testing here.  I've coded myself into a corner more than once on this project.  That's OK --- this is just a spike.  Expect things to change drastically from revision to revision, as I explore implementation techniques.

## why

Eventually, this code will inform how I develop the GUI for the [Kestrel-3](https://github.com/sam-falvo/kestrel).


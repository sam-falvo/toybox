This is an attempt to see how far I can get in replicating the [W programming language](http://www.vttoth.com/CMS/index.php/projects/49), but for the RISC-V instruction set architecture.

For pragmatic reasons, some things will need to differ from the original W definition.

The biggest thing you'll notice is that words are now 64, not 16, bits wide.  Hence the change in the name of the language: W64, versus just plain W.


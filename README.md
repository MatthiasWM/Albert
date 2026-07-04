
  A L B E R T
 =============

A modular disassembler and decompiler for the NewtonOS ROM.

Create a new directory for your adventure. Create the directories
`input`, `cache`, and `output`. Copy all the required files into `input`
(see `input/README.md` in the git repository).

Run `albert` in the new directory from the commend line.


Options
-------

Command line operators are run in the order they are specified. If operations
depend on other operators, those are run first. Input operators are generally
only executed a single time. Output operators can run multiple times with
different arguments.

Argument are alway optional with a sensible fallback.

```
--aif file          load aif ROM file and extract the binary ROM data
                    defaults to "input/Senior CirrusNoDebug image"
--rex file          load the ROM extensionand copy the binary data
                    defaults to "input/Senior CirrusNoDebug high"
--out-asm file      generate an annotated ARM32 disassembly
                    defaults to "output/newtonos.s"
--out-bin file      generate a binary dump of the ROM data
                    defaults to "output/717006.bin"
-h: print help text
```
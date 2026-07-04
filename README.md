
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
-h                  print help text
```

Planning
--------

- create a minimal assembler output (`.word` for unknown, `.fill` for unused)
- read symbols and labels from aif
    - add symbols and labels map
    - add op to read symbols and labels from aif and store in map
    - add labels to the assembler output
    - debug out put for duplicate symbols, and for duplicate addresses
    - fix duplicates
- introduce disassembler output
    - add ARM32 disassembler
    - add ARM32 attribute
    - test with predefined ROM section
- introduce static control flow analysis
    - recursively follow code execution and set attributes
    - update label type for jump and call targets
    - find other pointer to function (vtables, patch table, protocols)
- introduce static analysis of NewtonScript database
- analyze REx0 and add ARM code and NewtonScript from REx packages
- analyze symbols and nuild a type database
- run clang over include files and improve type database
- create a function and method database
- analyse data flow inside every function and mark local data use
- output single function, get AI to create C++ code, compare Norcroft compiler
  result to assembler input
- ongoing verification that the generate ROM is valid
- find and remove magic values as much as possible

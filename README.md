
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
        - symbol maps added
    - add op to read symbols and labels from aif and store in map
        - all symbols are read from the aif file
    - add labels to the assembler output
        - done - looks funny
    - debug output for duplicate symbols, and for duplicate addresses
        - we do have a bunch of addresses with multiple symbols, but that's ok
        - we do *not* have duplicate labels, which is great!
    - fix duplicates
        - done
- analyze labels and symbols and build a type database
    - find C++ labels
        - mostly done
        - TODO: need to handle labels starting with `__` like
          constructor, destructor, operators, etc.
    - extract those that are function pointers and set the call target attribute
        - done
    - extract the class and add it to the types
        - done
    - extract all arguments and build a function type
        - done
    - add type and name as a symbol, linked from the original label
        - ACTIVE: half way implemented, verify by printing all symbols
        - TODO: How can I find the owning class from the symbol?
        - TODO: How can I list each argument to the function?
    - read the clang generated json file, compiling all headers
    - merge clang types and aif types
    - add type information to absolute addresses and class memeber offsets
- find arm call targets
    - from vtables
    - from classinfos
    - from the patch table
- introduce static control flow analysis
    - recursively follow code execution and set attributes
    - update label type for jump and call targets
- introduce disassembler output
    - add ARM32 disassembler to arm code marked by attributes
- introduce static analysis of NewtonScript database
- analyze REx0 and add ARM code and NewtonScript from REx packages
- run clang over include files and improve type database
- create a function and method database
- analyse data flow inside every function and mark local data use
- output single function, get AI to create C++ code, compare Norcroft compiler
  result to assembler input
- ongoing verification that the generate ROM is valid
- find and remove magic values as much as possible

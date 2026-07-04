
The `input` Directory
=====================

Albert generates all output by reading and analysing resources in the
`input` directory. The following files should e copied to this directory
by the user:

- `Senior CirrusNoDebug image` - This file contains the binary part of the
  operating system. It's store in ARM Image Format (aif) and carries many
  symbols for debugging. It was originally distributed as part of the
  Driver Developer Kit (DDK).
- `Senior CirrusNoDebug high` - This fil holds a binary copy of the NewtonOS
  2.0 ROM Extension (REx). It was originally distributed as part of the
  Driver Developer Kit (DDK).
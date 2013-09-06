# libzdl

## Overview

libdl "replacement" for ZeroVM. Implements dlopen(), dlclose(), dlsym(), 
dlerror(), dlvsym(), dladdr().

ZeroVM currently doesn't support dynamic linking, thus no dynamic linker is
present. Libzdl is a workaround to get dlsym() work.

## Limitations

1. We could use dlopen/dlsym only for executable itself ("" as filename, see 
man dlopen). 

  Due to static linking everuthing is already linked in executable, so we
  could get address to any symbol we need.

2. Untrusted code can't read .nexe ELF header, so we use "/proc/self" hack.
Instead of reading own ELF sections, we map nexe to /dev/self file. 

3. Using BFD (Binary File Descriptor, GNU binutils) as ELF reader and parser.

## Static linking issues

Actually, with static linking linker includes only referenced object files.
So, some symbols (which is not referenced by our source code) are not linked 
in final executable. Currently, there are two ways of dealing with it:

1. Dummy code which uses all library symbols somebody may need.
2. Use --whole-archive/--no-whole-archive linker switches to make linker
include whole static library.

  Unfortunately, latter case could lead to new undefined references and multiple
  definitions. We could handle them with
  * --allow-multiple-definition
  * --unresolved-symbols=ignore-in-object-files

## Usage

    git clone https://github.com/rampage644/zerovm-llvm
    cd zerovm-llvm
    cd libzdl
    make

Next, link -lzdl to your project instead (or before) -ldl.

## Dependencies

*BFD* is used for ELF sections reading and symbol search. You have to link
your executable with:

1. zdl
1. bfd
1. iberty

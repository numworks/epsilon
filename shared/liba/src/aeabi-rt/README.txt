We're using the ARM Embedded ABI (AEABI) to build our code for the Cortex-M.
That ABI specifies a lot of things: how procedures are called, how ELF files
should be structured, and last but not least, a run-time.

That run-time consists in a list of helper functions that the compiler is free
to call anywhere : unaligned memory accesses, integer division functions,
memory copying/clearing/setting, etc...

Since we're telling our compiler to build an AEABI binary, it may decide to use
those symbols, and so we have to provide an implementation for them. Refer to
the "Run-time ABI for the ARM Architecture" for a full list of functions.
http://infocenter.arm.com/help/topic/com.arm.doc.ihi0043d/IHI0043D_rtabi.pdf

Note that this is not formally the job of a libc implementation. Similar code is
often shipped alongside a compiler (LLVM calls it compiler-rt, GCC libgcc). But
since we're using so few symbols in practice, it makes sense to glue it in. Also
some libc do also implement this (newlib, for example).

liba is an adhoc libc.

Warning: you must call "liba_init()" before using liba.

We need a very small subset of the functionality provided by the standard C
library. We could use an available libc implementation, but those are usually
way too large and may have problematic licenses.

It wouldn't even be fair to call liba a libc at all since it doesn't come close
to implementing a significant portion of the standard. However, we do need some
functionality usually provided by libc, so instead of reinventing the wheel we
just implement the same entities (types, functions, etc…).

. = ALIGN(4);
_init_array_start = .;
KEEP (*(.init_array*))
_init_array_end = .;
ASSERT(ASSERTIONS || (_init_array_start == _init_array_end), ".init_array not empty, there are static initialized objects");

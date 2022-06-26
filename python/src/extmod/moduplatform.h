// This file is needed, because MicroPython includes it in the modsys.c file.
// https://github.com/micropython/micropython/commit/402df833fe6da5233c83c58421e81493cda54f67#diff-99822946f0f35edf3fa262e9a3b213da739cfd30f5c8c0c44ef0eb67d7d7b4b0R39
// It is just a hack to make it work.
// It will be needed if the sys module is enabled.
#if MICROPY_PY_SYS
#warning "To build the sys module, you need to use the real moduplatform module from MicroPython."
#endif

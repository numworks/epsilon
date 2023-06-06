#include <extapp_api.h>

#if defined _FXCG || defined NSPIRE_NEWLIB
// On the port, we use the Built-in file manager to import files.
void host_filemanager();
void extapp_main() {
  host_filemanager();
}
#else
// Elsewhere, just draw a rectangle to test the extapp API.
void extapp_main() {
  extapp_pushRectUniform(10, 10, LCD_WIDTH-20, LCD_HEIGHT-20, 0);
  extapp_msleep(1000);
}
#endif

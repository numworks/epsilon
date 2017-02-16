#include <ion.h>
#include <ion/c.h>

void ion_log_print(const char * message) {
  Ion::Log::print(message);
}

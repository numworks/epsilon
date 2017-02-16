#include <ion.h>
#include <ion/c.h>

void ion_log_print_integer(uint32_t i) {
  Ion::Log::print(i);
}

void ion_log_print_string(const char * message) {
  Ion::Log::print(message);
}

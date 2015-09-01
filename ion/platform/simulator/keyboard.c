#include <ion.h>
#include <stdio.h>

char ion_getchar() {
  printf("GETCHAR\n");
  ion_sleep();
  char c = getchar();
  printf("Returning %c\n", c);
  return c;
}

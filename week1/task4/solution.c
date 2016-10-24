#include <string.h>
#include <stddef.h>

int stringStat(const char *string, size_t multiplier, int *count) {
  if (count != NULL) {
    ++*count;
  }

  return (int) (strlen(string) * multiplier);
}
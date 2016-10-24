#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
  extern int stringStat(const char *, size_t, int *);
  int cnt = 6;
  const char *inp = "Hello";
  int result = stringStat(inp, 5, &cnt);

  assert(cnt == 7);
  assert(strlen(inp) * 5 == result);
  printf("%d, %d\n", result, cnt);

  return 0;
}
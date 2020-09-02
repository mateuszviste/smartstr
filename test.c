/*
 * test file for smartstr.
 * Copyright (C) 2020 Mateusz Viste
 */

#include <stdio.h>
#include "smartstr.h"

int main(void) {
  smartstr_t *s = NULL;

  smartstr_adds(&s, "Hello, World");
  smartstr_addc(&s, '!');

  if (strcmp(smartstr_ptr(s), "Hello, World!") != 0) {
    fprintf(stderr, "error @%d\n", __LINE__);
    goto QUIT;
  }

  smartstr_truncate(&s, 5);
  if (strcmp(smartstr_ptr(s), "Hello") != 0) {
    fprintf(stderr, "error @%d\n", __LINE__);
    goto QUIT;
  }

  smartstr_set(&s, "Mateusz");
  if (strcmp(smartstr_ptr(s), "Mateusz") != 0) {
    fprintf(stderr, "error @%d\n", __LINE__);
    goto QUIT;
  }

  smartstr_truncate(&s, 0);
  if ((smartstr_len(s) != 0) || (smartstr_ptr(s)[0] != 0)) {
    fprintf(stderr, "error @%d\n", __LINE__);
    goto QUIT;
  }

  smartstr_free(&s);
  if (s != NULL) fprintf(stderr, "error @%d\n", __LINE__);

  /* if here, then all tests passed */
  printf("all good.\n");

  QUIT:
  smartstr_free(&s);

  return(0);
}

/*
 * This is a small library that provides support for "smart" strings in
 * ANSI C. It defines a new type for strings: smartstr_t, as well as a set of
 * functions that deal with this type. The library features automatic
 * allocation and resizing to protect the programmer against the most common
 * errors when working with variable-size strings in C.
 *
 * https://github.com/mateuszviste/smartstr
 *
 * Copyright (C) 2019-2020 Mateusz Viste
 * Published under the terms of the MIT license.
 */

#include <stdarg.h>   /* va_start() */
#include <stdlib.h>   /* calloc() */
#include <stdio.h>    /* snprintf() */
#include <string.h>   /* memcpy(), strlen() */

#include "smartstr.h"


/* initial size (in bytes) for new smartstr allocations */
#define SMARTSTR_INITSZ 32

/* define the internal struct (opaque to the user) */
struct _smartstr_t {
  unsigned long allocsz;
  unsigned long curlen;
  char s[1];
};

/* reallocs **s to be able to hold a string of at least newsz bytes.
 * reallocation may both increase or decrease *s alloc size
 * this function will never truncate existing content */
static int smartstr_realloc(smartstr_t **s, unsigned long newsz) {
  smartstr_t *new;
  unsigned long newalloc;
  if (newsz < (*s)->curlen) newsz = (*s)->curlen + 1;
  if (newsz < SMARTSTR_INITSZ) newsz = SMARTSTR_INITSZ;
  /* find out the size of the new allocation */
  newalloc = (*s)->allocsz;
  while (newsz > newalloc) newalloc *= 2;
  while (newsz < (newalloc / 2)) newalloc /= 2;

  if (newalloc == (*s)->allocsz) return(0); /* no job to do */

  /* ready to do the mem job now */
  new = realloc(*s, sizeof(smartstr_t) + newsz);
  if (new == NULL) return(-1);
  new->allocsz = newsz;
  *s = new;
  return(0);
}


/****************************************************************************
 *** PUBLIC INTERFACE *******************************************************
 ****************************************************************************/

int smartstr_addf(smartstr_t **out, const char *fmt, ...) {
  int res = 0;
  va_list ap;
  unsigned long availspace;
  long reslen;
  if (out == NULL) return(-2);
  if (*out == NULL) {
    *out = smartstr_new();
    if (*out == NULL) return(-1);
  }

  RETRY:
  availspace = (*out)->allocsz - (*out)->curlen;
  va_start(ap, fmt);
  /* I need to disable the "string is not a literal" warning here so clang does not whine about it */
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
  reslen = vsnprintf((*out)->s + (*out)->curlen, availspace, fmt, ap);
  #pragma clang diagnostic pop
  va_end(ap);

  /* if failed, then realloc and try again */
  if (reslen >= (long)availspace) {
    (*out)->s[(*out)->curlen] = 0; /* trim the result of the snprintf() attempt */
    if (smartstr_realloc(out, (*out)->curlen + (unsigned long)reslen + 1) == 0) goto RETRY;
    res = -1;  /* otherwise we are out of memory! */
  } else if (reslen < 0) { /* snprintf() error! */
    (*out)->s[(*out)->curlen] = 0;
    res = -4;
  } else {
    (*out)->curlen += (unsigned long)reslen;
  }

  return(res);
}


int smartstr_adds(smartstr_t **out, const char *s) {
  unsigned long availspace;
  size_t slen;
  if (out == NULL) return(-2);

  if (*out == NULL) {
    *out = smartstr_new();
    if (*out == NULL) return(-1);
  }

  if ((s == NULL) || (s[0] == 0)) return(0); /* appending an empty string is super fast */

  slen = strlen(s);

  /* compute avail storage space */
  availspace = (*out)->allocsz - (*out)->curlen;

  /* increase storage if needed */
  if (availspace <= slen) {
    if (smartstr_realloc(out, (*out)->curlen + slen + 1) != 0) return(-1);
  }

  /* copy string and update length */
  memcpy((*out)->s + (*out)->curlen, s, slen + 1); /* +1 because I'm interested in the terminator as well */
  (*out)->curlen += slen;

  return(0);
}


int smartstr_addc(smartstr_t **out, const char c) {
  if (out == NULL) return(-1);

  if (*out == NULL) {
    *out = smartstr_new();
    if (*out == NULL) return(-1);
  }

  /* realloc if needed */
  if ((*out)->curlen + 1 >= (*out)->allocsz) {
    if (smartstr_realloc(out, (*out)->curlen + 1) != 0) return(-2);
  }
  /* append char */
  (*out)->s[(*out)->curlen++] = c;
  (*out)->s[(*out)->curlen] = 0; /* I'm a friend of Sarah Connor - is she here? */
  /* */
  return(0);
}


int smartstr_addhead(smartstr_t **out, const char *head, size_t hlen) {

  if (hlen == 0) return(0); /* adding an empty string is easy */

  if (*out == NULL) {
    *out = smartstr_new();
    if (*out == NULL) return(-1);
  }

  /* realloc space if needed */
  if (smartstr_realloc(out, (*out)->curlen + hlen) != 0) return(-1);

  /* move current content right by hlen offset */
  memmove((*out)->s + hlen, (*out)->s, (*out)->curlen + 1);
  /* insert head */
  memcpy((*out)->s, head, hlen);
  /* update curlen */
  (*out)->curlen += hlen;

  return(0);
}


void smartstr_truncate(smartstr_t **s, size_t maxlen) {
  /* do I need to do anything at all? */
  if ((s == NULL) || (*s == NULL)) return;
  if (maxlen >= (*s)->curlen) return;
  /* curlen > maxlen, let's truncate */
  (*s)->curlen = maxlen;
  (*s)->s[maxlen] = 0;
  /* adjust memory allocation, perhaps some bytes can be saved */
  smartstr_realloc(s, 0);
}


int smartstr_cmp(const smartstr_t *s1, const smartstr_t *s2) {
  /* watch out for NULL situations */
  if ((s1 == NULL) && (s2 == NULL)) return(0);
  if ((s1 == NULL) || (s2 == NULL)) return(1);
  /* cmp string lengths */
  if (s1->curlen != s2->curlen) return(1);
  /* same length */
  if (s1->curlen == 0) return(0); /* two empty strings are always equal */
  return(memcmp(s1->s, s2->s, s1->curlen));
}


int smartstr_set(smartstr_t **out, const char *init) {
  unsigned long initlen;
  if (out == NULL) return(-1);

  if (*out == NULL) {
    *out = smartstr_new();
    if (*out == NULL) return(-1);
  }

  /* */
  if (init == NULL) {
    initlen = 0;
  } else {
    initlen = strlen(init);
  }
  if (smartstr_realloc(out, initlen + 1) != 0) {
    return(-1);
  }
  if (init != NULL) memcpy((*out)->s, init, initlen + 1);
  (*out)->curlen = initlen;
  return(0);
}


int smartstr_cat(smartstr_t **s1, const smartstr_t *s2) {
  unsigned long targetsz;
  if (s1 == NULL) return(-1);
  if (s2 == NULL) return(0);

  if (*s1 == NULL) {
    *s1 = smartstr_new();
    if (*s1 == NULL) return(-1);
  }

  /* make sure that s1 is big enough */
  targetsz = (*s1)->curlen + s2->curlen + 1;
  /* do a realloc if needed */
  if (smartstr_realloc(s1, targetsz) != 0) return(-2);
  /* now we good to do the actual copying */
  memcpy((*s1)->s + (*s1)->curlen, s2->s, s2->curlen + 1); /* +1 to catch s2's NULL terminator */
  (*s1)->curlen += s2->curlen;
  return(0);
}


size_t smartstr_len(const smartstr_t *s) {
  if (s == NULL) return(0);
  return(s->curlen);
}


smartstr_t *smartstr_new(void) {
  smartstr_t *s;
  s = calloc(1, sizeof(smartstr_t) + SMARTSTR_INITSZ);
  if (s == NULL) return(NULL);
  s->allocsz = SMARTSTR_INITSZ;
  return(s);
}


void smartstr_free(smartstr_t **s) {
  free(*s);
  *s = NULL;
}


const char *smartstr_ptr(const smartstr_t *s) {
  if (s == NULL) return(NULL);
  return(s->s);
}

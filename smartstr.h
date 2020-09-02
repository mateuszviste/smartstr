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

#ifndef SMARTSTR_H
#define SMARTSTR_H

#include <string.h>   /* size_t definition there */

/* opaque declaration of the data type */
struct _smartstr_t;
typedef struct _smartstr_t smartstr_t;

/* library version (long int) */
#define smartstr_version 20200902l

/* appends a formatted string to the tail of a smart string. accepts same
 * arguments as sprintf. returns 0 on success.
 * if *out is NULL, then a new smartstr is allocated. */
int smartstr_addf(smartstr_t **out, const char *fmt, ...);

/* appends a null-terminated string to the tail of a smart string.
 * returns 0 on success. if *out is NULL, then a new smartstr is allocated. */
int smartstr_adds(smartstr_t **out, const char *s);

/* appends a single character to the tail of out. returns 0 on success.
 * if *out is NULL, then a new smartstr is allocated. */
int smartstr_addc(smartstr_t **out, const char c);

/* adds a head prefix of length hlen to out. returns 0 on success.
 * if *out is NULL, then a new smartstr is allocated. */
int smartstr_addhead(smartstr_t **out, const char *head, size_t hlen);

/* compare two strings - returns 0 if equal, non-zero otherwise.
 * s1 and/or s2 may be NULL (two NULLs are equal) */
int smartstr_cmp(const smartstr_t *s1, const smartstr_t *s2);

/* truncates string s to maxlen bytes */
void smartstr_truncate(smartstr_t **s, size_t maxlen);

/* sets out with the content of the init string. init may be NULL - then out
 * is simply cleared. returns 0 on success. previous out content is lost.
 * if *out is NULL, then a new smartstr is allocated. */
int smartstr_set(smartstr_t **out, const char *init);

/* concatenates contenant of s2 to end of *s1. returns 0 on success.
 * on error, *s1 is left untouched.
 * if *s1 is NULL, then a new smartstr is allocated.
 * s2 may be NULL as well - it is considered like an empty string */
int smartstr_cat(smartstr_t **s1, const smartstr_t *s2);

/* returns length (in bytes) of string s */
size_t smartstr_len(const smartstr_t *s);

/* creates (allocates) a new smartstr_t object
 * that needs to be freed with smartstr_free() */
smartstr_t *smartstr_new(void);

/* deallocates memory utilized by **s, and sets *s to NULL
 * nothing is done if s or *s are NULL */
void smartstr_free(smartstr_t **s);

/* returns a pointer to the actual NULL-terminated C string stored in s.
 * returns NULL if s is NULL */
const char *smartstr_ptr(const smartstr_t *s);

#endif

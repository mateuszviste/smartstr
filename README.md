# smartstr: a convenience ANSI C (C89) library for easy strings

I am an old time ANSI C programmer. I despise C++ with all its nifty features and milions of extensions. I believe that a programming language ought to be simple. ANSI C is very simple. So simple, that it misses a "string" type - instead, the programmer has to compose with arrays of characters. This may often lead to bugs: buffer overflows because the string outgrew its allocated space, segmentation faults when something or someone forgot to end the char array with a null terminator, memory leaks, etc.

Because I was tired of fighting with stupid strings all the time, I created smartstr.

## How?

smartstr is a tiny library that strives to protect the programmer against aforementionned issues. It does so by providing a new, opaque data type called 'smartstr_t', as well as a set of functions that deals with it. No longer the programmer has to worry about memory (re)allocations, fiddling with terminators or wasting cpu cycles just to figure out how to append one string to another.

When using smartstr, you can no longer tinker with the char array (that's the entire point of this library, after all), but you may still access a standard, null-terminated (read-only) version of your string.

smartstr tries not only to make handling strings easy for the programmer, it also makes effort to do so in an efficient way.

## Examples

````
/* declare the string - no need to "create" it explicitely if you init it to NULL */
smartstr_t *mystring = NULL;

/* set the string to some initial words */
smartstr_set(&mystring, "Hello, World");

/* append a single character to the string */
smartstr_addc(&mystring, '!');

/* how long is my string? */
size_t slen = smartstr_len(mystring);

/* get a NULL-terminated pointer to your string */
printf("My smart string is: %s\n", smartstr_ptr(mystring));

/* don't forget to free you string once you're done with it */
smartstr_free(&mystring);

/* smartstr_free() sets the pointer back to NULL, so you can reuse it right away if needed */
smartstr_addf(&mystring, "Last night, I counted %d sheep.", rand());
````

Interested? The documentation of all available functions is present in smartstr.h.

This library is published by Mateusz Viste under the terms of the MIT license.

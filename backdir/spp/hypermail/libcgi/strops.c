/*
 * This file is part of the LIBCGI library
 *
 * Copyright 1994 by Enterprise Integration Technologies Corporation
 *
 * This is freeware with commercialization rights reserved; see the
 * LICENSE included in the distribution for specifics.
 */

#include <stdio.h>

char *trim(s)
     char *s;
 {
  char *t=s;
  
  while (*t) t++;
  while (t > s && *--t == ' ') *t = 0;
  return s;
}

char *sanitize(buf, s)
     char *buf;
     char *s;
 {
  char *t;

  for (t=buf;*s; s++)
    if (*s == ' ') *t++ = '+';
    else if (isalnum(*s)) *t++ = *s;
    else {
      sprintf(t, "%%%2X", *s);
      t += 3;
    }
  *t = '\0';
  return buf;
}

char *strmaxcpy(dest, src, n)
     char *dest;
     char *src;
     int n;
 {
  char *d=dest;

  if (n < 1) return NULL;
  while (--n && *src) *d++ = *src++;
  *d = 0;
  return dest;
}

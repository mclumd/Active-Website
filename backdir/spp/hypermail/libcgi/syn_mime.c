/*
 * This file is part of the LIBCGI library
 *
 * Copyright 1994 by Enterprise Integration Technologies Corporation
 *
 * This is freeware with commercialization rights reserved; see the
 * LICENSE included in the distribution for specifics.
 */

#include <stdio.h>
#include "cgi.h"

int syn_mimeheader(buf, ct)
     char *buf;
     char *ct;
{
  int x;

  if (buf && ct) {
    x = (int) sprintf(buf, "Content-Type: %s\n\n", ct);
    return (x && x != EOF);
  } else return 0;
}

int print_mimeheader(ct)
     char *ct;
{
  return (ct && (printf("Content-Type: %s\n\n", ct) != EOF));
}

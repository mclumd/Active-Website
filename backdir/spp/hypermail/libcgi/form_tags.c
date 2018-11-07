/*
 * This file is part of the LIBCGI library
 *
 * Copyright 1994 by Enterprise Integration Technologies Corporation
 *
 * This is freeware with commercialization rights reserved; see the
 * LICENSE included in the distribution for specifics.
 */

#include <stdio.h>

void print_sel_list(tname, opts, init)
     char *tname;
     char **opts;
     char *init;
 {
  printf("<select name=\"%s\">\n", tname);
  while (*opts) {
    printf("<option%s>%s\n", init && !strcasecmp(*opts, init) ? " selected" : "",
	   *opts);
    opts++;
  }
  fputs("</select>", stdout);
}

void print_input_blank(tname, size, maxlength, init)
     char *tname;
     unsigned int size;
     unsigned int maxlength;
     char *init;
 {
  printf("<input name=\"%s\"", tname);
  if (size) printf(" size=%d", size);
  if (maxlength) printf(" maxlength=%d", maxlength);
  if (init && *init) printf(" value=\"%s\"", init);
  fputs(">", stdout);
}

void print_submit(label)
     char *label;
 {
  printf("<input type=submit");
  if (label && *label) printf(" value=\"%s\"", label);
  fputs(">", stdout);
}

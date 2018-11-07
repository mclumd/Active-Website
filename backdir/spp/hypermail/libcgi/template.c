/*
 * This file is part of the LIBCGI library
 *
 * Copyright 1994 by Enterprise Integration Technologies Corporation
 *
 * This is freeware with commercialization rights reserved; see the
 * LICENSE included in the distribution for specifics.
 */

#include<stdio.h>
#include "cgi.h"

cgi_main(ci)
     cgi_info *ci;
 {
  char *parmval();
  form_entry *parms, *p;
  form_entry *get_form_entries();
  char *foo, *bar;

  print_mimeheader("text/html");

  puts("<title>Your Title Here</title>");
  puts("<h1>Your heading here</h1>");

  parms = get_form_entries(ci);
  if (parms) {
    /* extract specific form parameters */
    for(p=parms; p; p = p->next) {
      if (strcasecmp(p->name, "foo")) foo = p->val;
      else if (strcasecmp(p->name, "bar")) bar = p->val;
    }
  }

  switch(mcode(ci)) {

  case MCODE_HEAD:
    return;

  case MCODE_GET:    
    puts("Your GET response here");
    printf("based on foo=%s and bar=%s.\n", foo, bar);
    break;

  case MCODE_POST:
    puts("Your POST response here");
    printf("based on foo=%s and bar=%s.\n", foo, bar);
    break;

  default:
    printf("Unrecognized method '%s'.\n", ci->request_method);
  }

  free_form_entries(parms);
}

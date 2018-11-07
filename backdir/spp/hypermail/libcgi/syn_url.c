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

int syn_base_url(buf, ci)
     char *buf;
     cgi_info *ci;
{
  int x;

  if (ci && buf) {
    x = (int) sprintf(buf, "http://%s:%s%s", ci->server_name, ci->server_port,
	    ci->script_name);
    return (x && x != EOF);
  } else return 0;
}

int print_base_url(ci)
     cgi_info *ci;
{
  return (ci &&
          (printf("http://%s:%s%s", ci->server_name, ci->server_port,
	    ci->script_name) != EOF));
}

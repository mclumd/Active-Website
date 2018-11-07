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

int mcode(ci)
     cgi_info *ci;
 {
  if (ci->request_method == NULL) return 0;
  else if (!strncasecmp(ci->request_method, "GET", 3)) return MCODE_GET;
  else if (!strncasecmp(ci->request_method, "POST", 4)) return MCODE_POST;
  else if (!strncasecmp(ci->request_method, "PUT", 3)) return MCODE_PUT;
  else if (!strncasecmp(ci->request_method, "HEAD", 4)) return MCODE_HEAD;
  else return 0;
}


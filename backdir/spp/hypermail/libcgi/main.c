/*
 * This file is part of the LIBCGI library
 *
 * Copyright 1994 by Enterprise Integration Technologies Corporation
 *
 * This is freeware with commercialization rights reserved; see the
 * LICENSE included in the distribution for specifics.
 */

#include "cgi.h"

main()
 {

  cgi_info ci;

  get_cgi_info(&ci);
  cgi_main(&ci);
}

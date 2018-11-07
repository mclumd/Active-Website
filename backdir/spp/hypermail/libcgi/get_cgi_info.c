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

int get_cgi_info(ci)
     cgi_info *ci;
 {
  char *getenv(), *s;

  ci->content_length = (s = getenv("CONTENT_LENGTH")) ? atoi(s) : 0;
  ci->content_type = getenv("CONTENT_TYPE");
  ci->server_software = getenv("SERVER_SOFTWARE");
  ci->gateway_interface = getenv("GATEWAY_INTERFACE");
  ci->server_protocol = getenv("SERVER_PROTOCOL");
  ci->server_port = getenv("SERVER_PORT");
  ci->request_method = getenv("REQUEST_METHOD");
  ci->http_accept = getenv("HTTP_ACCEPT");
  ci->path_info = getenv("PATH_INFO");
  ci->path_translated = getenv("PATH_TRANSLATED");
  ci->script_name = getenv("SCRIPT_NAME");
  ci->query_string = getenv("QUERY_STRING");
  ci->remote_host = getenv("REMOTE_HOST");
  ci->remote_addr = getenv("REMOTE_ADDR");
  ci->remote_user = getenv("REMOTE_USER");
  ci->auth_type = getenv("AUTH_TYPE");
  ci->remote_user = getenv("REMOTE_USER");
  ci->remote_ident = getenv("REMOTE_IDENT") ;
  return (ci->server_name = getenv("SERVER_NAME")) != NULL;
}

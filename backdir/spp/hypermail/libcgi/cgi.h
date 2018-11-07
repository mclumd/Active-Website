/*
 * This file is part of the LIBCGI library
 *
 * Copyright 1994 by Enterprise Integration Technologies Corporation
 *
 * This is freeware with commercialization rights reserved; see the
 * LICENSE included in the distribution for specifics.
 */

#ifndef CGI_H
#define CGI_H

#define MCODE_GET 1
#define MCODE_POST 2
#define MCODE_PUT 3
#define MCODE_HEAD 4

typedef struct
{
  char *server_software;
  char *server_name;
  char *gateway_interface;
  char *server_protocol;
  char *server_port;
  char *request_method;
  char *http_accept;
  char *path_info;
  char *path_translated;
  char *script_name;
  char *query_string;
  char *remote_host;
  char *remote_addr;
  char *remote_user;
  char *auth_type;
  char *remote_ident;
  char *content_type;
  int content_length;
} cgi_info;

typedef struct festruct
{
  char *name;
  char *val;
  struct festruct *next;
} form_entry;




/* Prototypes */
/* ---------- */
char *parmval();
form_entry *get_form_entries();
void print_input_blank();
void print_sel_list();
void print_submit();
void print_doc_begin();
char *trim();
char *sanitize();
char *strmaxcpy();
int get_cgi_info();
int syn_mimeheader();
int print_mimeheader();
int syn_base_url();
int print_base_url();
int mcode();

#endif

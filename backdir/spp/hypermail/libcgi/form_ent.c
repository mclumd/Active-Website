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

#define LF 10
#define CR 13

#define STARTSIZE 8

form_entry *get_form_entries(ci)
     cgi_info *ci;
{
  form_entry *get_fes_from_stream(), *get_fes_from_string();

  if (ci && ci->request_method && !strncasecmp(ci->request_method, "POST", 4) &&
      ci->content_type &&
      !strncasecmp(ci->content_type, "application/x-www-form-urlencoded", 33))
    return get_fes_from_stream(ci->content_length, stdin);
  else if (ci && ci->request_method && !strncasecmp(ci->request_method, "GET", 3))
    return get_fes_from_string(ci->query_string);
  else
    return NULL;
}

void free_form_entries(fe)
     form_entry *fe;
 {
  form_entry *tempfe;
  while (fe) {
    if (fe->name) free(fe->name);
    if (fe->val) free(fe->val);
    tempfe = fe->next;
    free(fe);
    fe = tempfe;
  }
}

char *parmval(fe, s)
     form_entry *fe;
     char *s;
 {
  while (fe) {
    if (!strcasecmp(fe->name, s)) return fe->val;
    else fe = fe->next;
  }
  return NULL;
}

form_entry *get_fes_from_string(s)
     char *s;
 {
  form_entry *fe;
  int asize;
  int i;
  char *malloc(), *realloc();
  unsigned char dd2c();

  if (s == NULL) return NULL;
  while (isspace(*s) || *s == '&') s++; /* some cases that shouldn't happen */
  if (*s == '\0') return NULL;
  fe = (form_entry *) malloc(sizeof(form_entry));
  if (fe == NULL) return NULL;
  fe->name = malloc((asize=STARTSIZE*sizeof(char)));
  if (fe->name == NULL) {
    free(fe);
    return NULL;
  }
  /* get form field name */
  for (i=0; *s && *s != '&' && *s != '='; s++,i++) {
    switch (*s) {
    case '+':
      fe->name[i] = ' ';
      break;
    case '%':
      fe->name[i] = dd2c(s[1], s[2]);
      s += 2;
      break;
    default:
      fe->name[i] = *s;
    }
    if (i+1 >= asize) { /* try to double the buffer */
      fe->name = realloc(fe->name, (asize*=2));
      if (fe->name == NULL) return NULL;
    }
  }
  fe->name[i] = '\0';
  switch (*s++) {
  case '&':
    fe->val = NULL;
    break;
  case '=':
    fe->val = malloc((asize=STARTSIZE*sizeof(char)));
    if (fe->val == NULL) break;
    for (i=0; *s && *s != '&'; s++,i++) {
      switch (*s) {
      case '+':
	fe->val[i] = ' ';
	break;
      case '%':
	fe->val[i] = dd2c(s[1], s[2]);
	s += 2;
	break;
      default:
	fe->val[i] = *s;
      }
      if (i+1 >= asize) { /* try to double the buffer */
	fe->val = realloc(fe->val, (asize*=2));
	if (fe->val == NULL) return NULL;
      }
    }
    fe->val[i] = '\0';
    switch (*s++) {
    case '&':
      fe->next = get_fes_from_string(s);
      break;
    case '\0':
    default:
      fe->next = NULL;
    }
    break;
  case '\0':
  default:
    fe->val = NULL;
    fe->next = NULL;
  }
  return fe;
}

#define getccl(s, l) (l-- ? getc(s) : EOF)

form_entry *get_fes_from_stream(length, stream)
     int length;
     FILE *stream;
 {
  form_entry *fe;
  int asize;
  int i;
  int c;
  char *malloc(), *realloc();
  unsigned char dd2c();

  while (isspace(c=getccl(stream,length)) || c == '&');
  if (c == EOF) return NULL;
  fe = (form_entry *) malloc(sizeof(form_entry));
  if (fe == NULL) return NULL;
  fe->name = malloc((asize=STARTSIZE*sizeof(char)));
  if (fe->name == NULL) {
    free(fe);
    return NULL;
  }
  /* get form field name */
  for (i=0; c != EOF && c != '&' && c != '='; c=getccl(stream,length),i++) {
    switch (c) {
    case '+':
      fe->name[i] = ' ';
      break;
    case '%':
      fe->name[i] = dd2c(getccl(stream,length), getccl(stream,length));
      break;
    default:
      fe->name[i] = c;
    }
    if (i+1 >= asize) { /* try to double the buffer */
      fe->name = realloc(fe->name, (asize*=2));
      if (fe->name == NULL) return NULL;
    }
  }
  fe->name[i] = '\0';
  if (c == EOF) {
    fe->val = NULL;
    fe->next = NULL;
  }
  else switch (c) {
  case '&':
    fe->val = NULL;
    break;
  case '=':
    fe->val = malloc((asize=STARTSIZE*sizeof(char)));
    for (i=0, c=getccl(stream,length); c != EOF && c != '&'; c=getccl(stream,length),i++) {
      switch (c) {
      case '+':
	fe->val[i] = ' ';
	break;
      case '%':
	fe->val[i] = dd2c(getccl(stream,length), getccl(stream,length));
	break;
      default:
	fe->val[i] = c;
      }
      if (i+1 >= asize) { /* try to double the buffer */
	fe->val = realloc(fe->val, (asize*=2));
	if (fe->val == NULL) return NULL;
      }
    }
    fe->val[i] = '\0';
    if (c == '&') {
      fe->next = get_fes_from_stream(length, stream);
    }
    else fe->next = NULL;
  }
  return fe;
}

unsigned char dd2c(d1, d2)
     char d1;
     char d2;
 {
    register unsigned char digit;

    digit = (d1 >= 'A' ? ((d1 & 0xdf) - 'A')+10 : (d1 - '0'));
    digit *= 16;
    digit += (d2 >= 'A' ? ((d2 & 0xdf) - 'A')+10 : (d2 - '0'));
    return(digit);
}

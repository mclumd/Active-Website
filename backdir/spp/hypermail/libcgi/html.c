/*
 * This file is part of the LIBCGI library
 *
 * Copyright 1994 by Enterprise Integration Technologies Corporation
 *
 * This is freeware with commercialization rights reserved; see the
 * LICENSE included in the distribution for specifics.
 */

void print_doc_begin(title)
     char *title;
 {
  printf("<head><title>%s</title></head>\n", title);
  printf("<h1>%s</h1>\n<img src=http://www.eit.com/graphics/stripe.bottom.gif alt=\"\"><br>", title);
}

void print_doc_end(text)
     char *text;
 {
  char *w, *getenv();

  puts("<p><hr>");
  if (text && *text) puts(text);
  else if (w = getenv("WEBMASTER")) printf("<address>%s</address>\n", w);
}

void print_logo()
 {
  printf("<a href=http://www.eit.com/><img src=http://www.eit.com/graphics/eit.small.gif alt=\"EIT\" align=bottom></a>");
}

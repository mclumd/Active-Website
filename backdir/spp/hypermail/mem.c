/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/14/94
*/

#include "hypermail.h"
#include "mem.h"

/* Just a tiny malloc() error checker!
*/

void *emalloc(i)
     int i;
{
	void *p;

	if ((p = (void *) malloc(i)) == NULL)
		progerr("Ran out of memory!");
	return p;
}

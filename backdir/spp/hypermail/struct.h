/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/18/94
*/

unsigned hash();
void addhash();
int hashreplylookup();
int hashreplynumlookup();
struct body *hashnumlookup();
struct body *addbody();
struct reply *addreply();
struct printed *markasprinted();
int wasprinted();
struct header *addheader();
int rmlastlines();

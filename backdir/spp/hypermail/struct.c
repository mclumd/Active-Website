/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/21/94
*/

#include "hypermail.h"
#include "struct.h"

/* Ladies and gentlemen - the hash function.
*/

unsigned hash(s)
     char *s;
{
	unsigned hashval;

	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

/* The structure most of everything else depends on.
** Hashes a message - header info, pointer to a list of body lines -
** by number, message ID, date, and subject, so an article can be
** handily looked up and retrieved using any of these criteria.
*/

void addhash(num, date, name, email, msgid, subject, inreply, fromdate, bp)
     int num;
     char *date;
     char *name;
     char *email;
     char *msgid;
     char *subject;
     char *inreply;
     char *fromdate;
     struct body *bp;
{
	struct email *ep;
	struct body *sp;
	unsigned hashval;
	char numstr[NUMSTRLEN];

	sp = bp;

	ep = (struct email *) emalloc(sizeof(*ep));
	ep->msgnum = num;
	ep->name = (char *) strdup(name);
	ep->emailaddr = (char *) strdup(email);
	ep->fromdatestr = (char *) strdup(fromdate);
	ep->datestr = (char *) strdup(date);
	ep->msgid = (char *) strdup(msgid);
	ep->subject = (char *) strdup(subject);
	ep->inreplyto = (char *) strdup(inreply);
	ep->bodylist = sp;

	hashval = hash(stripzone(date));
	ep->next = etable[hashval];
	etable[hashval] = ep;

	ep = (struct email *) emalloc(sizeof(*ep));
	ep->msgnum = num;
	ep->name = (char *) strdup(name);
	ep->emailaddr = (char *) strdup(email);
	ep->fromdatestr = (char *) strdup(fromdate);
	ep->datestr = (char *) strdup(date);
	ep->msgid = (char *) strdup(msgid);
	ep->subject = (char *) strdup(subject);
	ep->inreplyto = (char *) strdup(inreply);
	ep->bodylist = sp;

	hashval = hash(msgid);
	ep->next = etable[hashval];
	etable[hashval] = ep;

	ep = (struct email *) emalloc(sizeof(*ep));
	ep->msgnum = num;
	ep->name = (char *) strdup(name);
	ep->emailaddr = (char *) strdup(email);
	ep->fromdatestr = (char *) strdup(fromdate);
	ep->datestr = (char *) strdup(date);
	ep->msgid = (char *) strdup(msgid);
	ep->subject = (char *) strdup(subject);
	ep->inreplyto = (char *) strdup(inreply);
	ep->bodylist = sp;

	hashval = hash(subject);
	ep->next = etable[hashval];
	etable[hashval] = ep;

	ep = (struct email *) emalloc(sizeof(*ep));
	ep->msgnum = num;
	ep->name = (char *) strdup(name);
	ep->emailaddr = (char *) strdup(email);
	ep->fromdatestr = (char *) strdup(fromdate);
	ep->datestr = (char *) strdup(date);
	ep->msgid = (char *) strdup(msgid);
	ep->subject = (char *) strdup(subject);
	ep->inreplyto = (char *) strdup(inreply);
	ep->bodylist = sp;

	sprintf(numstr, "%d", num);
	hashval = hash(numstr);
	ep->next = etable[hashval];
	etable[hashval] = ep;
}

/* Given an "in-reply-to:" field, this tries to retrieve information
** about the article that is a reply. If all else fails but a reply is
** found by comparing subjects, issubjmatch is set to 1.
*/

int hashreplylookup(inreply, name, subject, issubjmatch)
     char *inreply;
     char *name;
     char *subject;
     int *issubjmatch;
{
	struct email *ep;

	*issubjmatch = 0;
	if (numstrchr(inreply, ':') == 2)
		ep = etable[hash(stripzone(inreply))];
	else
		ep = etable[hash(inreply)];
	while (ep != NULL) {
		if (strcmp(inreply, ep->msgid) == 0) {
			strcpy(name, ep->name);
			strcpy(subject, ep->subject);
			return ep->msgnum;
		}
		ep = ep->next;
	}

	if (numstrchr(inreply, ':') == 2)
		ep = etable[hash(stripzone(inreply))];
	else
		ep = etable[hash(inreply)];
	while (ep != NULL) {
		if (strstr(inreply, stripzone(ep->datestr))) {
			strcpy(name, ep->name);
			strcpy(subject, ep->subject);
			return ep->msgnum;
		}
		ep = ep->next;
	}

	if (numstrchr(inreply, ':') == 2)
		ep = etable[hash(stripzone(inreply))];
	else
		ep = etable[hash(inreply)];
	while (ep != NULL) {
		if (strcmp(inreply, ep->subject) == 0) {
			strcpy(name, ep->name);
			strcpy(subject, ep->subject);
			*issubjmatch = 1;
			return ep->msgnum;
		}
		ep = ep->next;
	}

	return -1;
}

/* Same as the above function, but only returns the article number.
*/

int hashreplynumlookup(inreply, maybereply)
     int *maybereply;
     char *inreply;
{
	struct email *ep;

	*maybereply = 0;

	if (numstrchr(inreply, ':') == 2)
		ep = etable[hash(stripzone(inreply))];
	else
		ep = etable[hash(inreply)];
	while (ep != NULL) {
		if (strcmp(inreply, ep->msgid) == 0)
			return ep->msgnum;
		ep = ep->next;
	}

	if (numstrchr(inreply, ':') == 2)
		ep = etable[hash(stripzone(inreply))];
	else
		ep = etable[hash(inreply)];
	while (ep != NULL) {
		if (strstr(inreply, stripzone(ep->datestr)))
			return ep->msgnum;
		ep = ep->next;
	}

	if (numstrchr(inreply, ':') == 2)
		ep = etable[hash(stripzone(inreply))];
	else
		ep = etable[hash(inreply)];
	while (ep != NULL) {
		if (strcmp(inreply, ep->subject) == 0) {
			*maybereply = 1;
			return ep->msgnum;
		}
		ep = ep->next;
	}

	return -1;
}

/* From an article's number, retrieve all information associated with
** that article.
*/

struct body *hashnumlookup(num, name, email, subject, reply, date, fromdate,
msgid)
     int num;
     char *name;
     char *email;
     char *subject;
     char *reply;
     char *date;
     char *fromdate;
     char *msgid;
{
	struct email *ep;
	char numstr[NUMSTRLEN];

	sprintf(numstr, "%d", num);
	for (ep = etable[hash(numstr)]; ep != NULL; ep = ep->next)
		if (num == ep->msgnum) {
			strcpy(name, ep->name);
			strcpy(email, ep->emailaddr);
			strcpy(subject, ep->subject);
			strcpy(reply, ep->inreplyto);
			strcpy(date, ep->datestr);
			strcpy(fromdate, ep->fromdatestr);
			strcpy(msgid, ep->msgid);
			return ep->bodylist;
		}
	return NULL;
}

/* Add a line to a linked list that makes up an article's body.
*/

struct body *addbody(bp, line)
     struct body *bp;
     char *line;
{
        struct body *tempnode, *newnode;

        newnode = (struct body *) emalloc(sizeof(struct body));
	newnode->line = (char *) strdup(line);
        newnode->next = NULL;

        if (bp == NULL)
                bp = newnode;
        else {
                for (tempnode = bp; tempnode->next != NULL; tempnode =
                tempnode->next)
                        ;
                tempnode->next = newnode;
        }

        return bp;
}

/* Remove the last empty lines, if any, from an article body's linked list.
*/

int rmlastlines(bp)
     struct body *bp;
{
	struct body *tempnode;

	for (tempnode = bp; tempnode->next != NULL &&
	(tempnode->next->line)[0] != '\0'; tempnode = tempnode->next)
		;
	if ((tempnode->line)[0] == '\n') {
		(tempnode->line)[0] = '\0';
		return 1;
	}
	return 0;
}

/* If a message is a reply to another, that message's number and the
** number of the message it may be referring to is put in this list.
*/

struct reply *addreply(rp, num, fromnum, name, subject, maybereply)
     struct reply *rp;
     int num;
     int fromnum;
     char *name;
     char *subject;
     int maybereply;
{
	struct reply *tempnode, *newnode;

        newnode = (struct reply *) emalloc(sizeof(struct reply));
        newnode->msgnum = num;
        newnode->frommsgnum = fromnum;
        newnode->name = (char *) strdup(name);
        newnode->subject = (char *) strdup(subject);
	newnode->maybereply = maybereply;
        newnode->next = NULL;

        if (rp == NULL)
                rp = newnode;
        else {
                for (tempnode = rp; tempnode->next != NULL; tempnode =
                tempnode->next)
                        ;
                tempnode->next = newnode;
        }

        return rp;
}

/* Mark an article number as having been printed.
*/

struct printed *markasprinted(pp, num)
     struct printed *pp;
     int num;
{
	struct printed *tempnode, *newnode;

        newnode = (struct printed *) emalloc(sizeof(struct printed));
        newnode->msgnum = num;
        newnode->next = NULL;

        if (pp == NULL)
                pp = newnode;
        else {
                for (tempnode = pp; tempnode->next != NULL; tempnode =
                tempnode->next)
                        ;
                tempnode->next = newnode;
        }

        return pp;
}

/* Has an article already been printed?
*/

int wasprinted(list, num)
     struct printed *list;
     int num;
{
	struct printed *pp;

	for (pp = list; pp != NULL; pp = pp->next)
		if (pp->msgnum == num)
			return 1;
	return 0;
}

/* Add article header information to a binary tree and sort by date,
** subject, or author. This is necessary for printing the index files.
*/

struct header *addheader(hp, num, name, subject, date, sorttype)
     struct header *hp;
     int num;
     char *name;
     char *subject;
     char *date;
     int sorttype;
{
        int isbigger, yearsecs;

        isbigger = 0;
        if (hp == NULL) {
                hp = (struct header *) emalloc(sizeof(struct header));
                hp->msgnum = num;
		hp->name = (char *) strdup(name);
		hp->subject = (char *) strdup(subject);
		hp->datestr = (char *) strdup(date);
		if (sorttype == 2) {
			yearsecs = convtoyearsecs(date);
			if (!yearsecs)
				yearsecs = getlocaltime();
			hp->datenum = yearsecs;
			if (!firstdatenum || yearsecs < firstdatenum)
				firstdatenum = yearsecs;
			if (yearsecs > lastdatenum)
				lastdatenum = yearsecs;
		}
		else
			hp->datenum = 0;
                hp->left = hp->right = NULL;
                return hp;
        }

	if (sorttype == 1)
	        isbigger = (strcasecmp(name, hp->name) > 0) ? 0 : 1;
	else if (sorttype == 0)
	        isbigger = (strcasecmp(subject, hp->subject) > 0) ? 0 : 1;
	else if (sorttype == 2) {
		yearsecs = convtoyearsecs(date);
		if (!yearsecs)
			yearsecs = getlocaltime();
		if (reverse)
		        isbigger = (yearsecs < hp->datenum) ? 0 : 1;
		else
		        isbigger = (yearsecs >= hp->datenum) ? 0 : 1;
		if (!firstdatenum || yearsecs < firstdatenum)
			firstdatenum = yearsecs;
		if (yearsecs > lastdatenum)
			lastdatenum = yearsecs;
	}

        if (isbigger)
                hp->left = addheader(hp->left, num, name, subject, date,
		sorttype);
        else
                hp->right = addheader(hp->right, num, name, subject, date,
		sorttype);

        return hp;
}

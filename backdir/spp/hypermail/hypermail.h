/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.	      
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com
** 7/31/94
*/

#include "config.h"

#define VERSION    "1.02"
#define PROGNAME   "hypermail"
#define HMURL      "http://www.eit.com/software/hypermail/hypermail.html"
#define DIRNAME    "archive"
#define INDEXNAME  "index.html"
#define DATENAME   "date.html"
#define THRDNAME   "thread.html"
#define SUBJNAME   "subject.html"
#define AUTHNAME   "author.html"
#define NONAME     "(no name)"
#define NODATE     "(no date)"
#define NOEMAIL    "(no email)"
#define NOSUBJECT  "(no subject)"
#define MAXLINE	   1000
#define MAXFILELEN 100
#define NAMESTRLEN 80
#define NUMSTRLEN  10
#define MAILSTRLEN 80
#define DATESTRLEN 80
#define MSGDSTRLEN 80
#define REPYSTRLEN 240
#define SUBJSTRLEN 100
#define URLSTRLEN  100
#define HASHSIZE   101

#define SHORTDATELEN   9
#define TIMEZONELEN    10
#define YEARLEN        5
#define CENTURY        1900
#define BASEYEAR       1970
#define DAYSPERYEAR    365
#define SECSPERMIN     60
#define SECSPERHOUR    3600
#define SECSPERDAY     86400
#define IS_LEAP(y) (y > 1752 && (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)))

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>

#ifndef MAIN_FILE
#define VAR extern
#else
#define VAR
#endif

struct reply {
	int msgnum;
	int frommsgnum;
	char *name;
	char *subject;
	int maybereply;
	struct reply *next;
};

struct body {
	char *line;
	struct body *next;
};

struct printed {
	int msgnum;
	struct printed *next;
};

struct email {
	int msgnum;
	char *name;
	char *emailaddr;
	char *fromdatestr;
	char *datestr;
	char *msgid;
	char *subject;
	char *inreplyto;
	struct body *bodylist;
	struct email *next;
};

struct header {
	int msgnum;
	char *name;
	char *subject;
	char *datestr;
	int datenum;
	struct header *left;
	struct header *right;
};

VAR struct header *subjectlist;
VAR struct header *authorlist;
VAR struct header *datelist;
VAR struct reply *replylist;
VAR struct reply *threadlist;
VAR struct printed *printedlist;
VAR struct printed *printedthreadlist;
VAR struct email *etable[HASHSIZE];
VAR char timezonestr[TIMEZONELEN];
VAR char thisyear[YEARLEN];
VAR char datename[NAMESTRLEN];
VAR char thrdname[NAMESTRLEN];
VAR char subjname[NAMESTRLEN];
VAR char authname[NAMESTRLEN];
VAR char errmsg[MAXLINE];
VAR int firstdatenum;
VAR int lastdatenum;
VAR int bignum;
VAR int showprogress;
VAR int reverse;
VAR int showheaders;
VAR int showhtml;
VAR int thrdlevels;
VAR int dirmode;
VAR int filemode;

#ifdef MAIN_FILE
char *urls[] = { "http://", "gopher://", "file://", "ftp://",
	"wais://", "telnet://", "news:", "mailto:", NULL };
char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
        "Aug", "Sep", "Oct", "Nov", "Dec" };
char *days[] = { "Sun ", "Mon ", "Tue ", "Wed ", "Thu ", "Fri ", "Sat ", NULL };
char monthnums[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '0', '1', '2' };
int monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 30, 30, 31 };
#else
extern char *urls[], *months[], *days[];
extern char monthnums[];
extern int monthdays[];
#endif

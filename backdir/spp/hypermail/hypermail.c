/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 8/2/94
*/

#define MAIN_FILE
#include "hypermail.h"

int main(argc, argv)
     int argc;
     char **argv;
{
	extern char *optarg;
	extern int optind;
	int i, overwrite, use_stdin, use_mbox, increment;
	char mbox[MAXLINE], label[MAXLINE], dir[MAXLINE],
		archives[MAXLINE], about[MAXLINE], configfile[MAXLINE],
		defaultindex[MAXLINE];

/* Load the hard-wired defaults, then the environment variables.
*/

	strcpy(mbox, MBOX);
	if (getenv("HM_MBOX") != NULL)
		strcpy(mbox, getenv("HM_MBOX"));
	strcpy(archives, ARCHIVES);
	if (getenv("HM_ARCHIVES") != NULL)
		strcpy(archives, getenv("HM_ARCHIVES"));
	strcpy(about, ABOUT);
	if (getenv("HM_ABOUT") != NULL)
		strcpy(about, getenv("HM_ABOUT"));
	strcpy(label, LABEL);
	if (getenv("HM_LABEL") != NULL)
		strcpy(label, getenv("HM_LABEL"));
	strcpy(dir, DIR);
	if (getenv("HM_DIR") != NULL)
		strcpy(dir, getenv("HM_DIR"));
	strcpy(defaultindex, DEFAULTINDEX);
	if (getenv("HM_DEFAULTINDEX") != NULL)
		strcpy(defaultindex, getenv("HM_DEFAULTINDEX"));

	showprogress = PROGRESS;
	if (getenv("HM_PROGRESS") != NULL)
		showprogress = atoi(getenv("HM_PROGRESS"));
	overwrite = OVERWRITE;
	if (getenv("HM_OVERWRITE") != NULL)
		overwrite = atoi(getenv("HM_OVERWRITE"));
	increment = INCREMENT;
	if (getenv("HM_INCREMENT") != NULL)
		increment = atoi(getenv("HM_INCREMENT"));
	reverse = REVERSE;
	if (getenv("HM_REVERSE") != NULL)
		reverse = atoi(getenv("HM_REVERSE"));
	showheaders = SHOWHEADERS;
	if (getenv("HM_SHOWHEADERS") != NULL)
		showheaders = atoi(getenv("HM_SHOWHEADERS"));
	showhtml = SHOWHTML;
	if (getenv("HM_SHOWHTML") != NULL)
		showhtml = atoi(getenv("HM_SHOWHTML"));
	thrdlevels = THRDLEVELS;
	if (getenv("HM_THRDLEVELS") != NULL)
		thrdlevels = atoi(getenv("HM_THRDLEVELS"));
	dirmode = DIRMODE;
	if (getenv("HM_DIRMODE") != NULL)
		dirmode = strtol(getenv("HM_DIRMODE"), (char **) NULL, 0);
	filemode = FILEMODE;
	if (getenv("HM_FILEMODE") != NULL)
		filemode = strtol(getenv("HM_FILEMODE"), (char **) NULL, 0);

	strcpy(configfile, CONFIGFILE);
	if (getenv("HM_CONFIGFILE") != NULL)
		strcpy(configfile, getenv("HM_CONFIGFILE"));

	if (!strcmp(mbox, "NONE"))
		use_stdin = 1;
	else
		use_stdin = 0;

/* ...then use the command-line options...
*/

	use_mbox = 0;
	firstdatenum = lastdatenum = 0;
	while ((i = (int) getopt(argc, argv, "?zhixupm:l:d:a:b:c:")) != -1)
		if ((char) i == 'm') {
			strcpy(mbox, optarg);
			use_mbox = 1;
		}
		else if ((char) i == 'a')
			strcpy(archives, optarg);
		else if ((char) i == 'b')
			strcpy(about, optarg);
		else if ((char) i == 'l')
			strcpy(label, optarg);
		else if ((char) i == 'd')
			strcpy(dir, optarg);
		else if ((char) i == 'c')
			strcpy(configfile, optarg);
		else if ((char) i == 'x')
			overwrite = 1;
		else if ((char) i == 'i')
			use_stdin = 1;
		else if ((char) i == 'p')
			showprogress = 1;
		else if ((char) i == 'u')
			increment = 1;
		else if ((char) i == 'z' || (char) i == 'h' ||
		(char) i == '?')
			usage();
		else
			usage();

/* ...then read the configuration file.
*/

	readconfigs(configfile, mbox, label, dir, archives, about, &overwrite,
		&increment, defaultindex);

/* Default names for directories and labels need to be figured out.
*/

	if (use_stdin && !strcmp(dir, "NONE"))
		strcpy(dir, DIRNAME);
	if (!strcmp(dir, "NONE"))
		strcpy(dir, (strrchr(mbox, '/')) ? (char *) strrchr(mbox, '/')
		+ 1 : mbox);
	if (!strcmp(label, "NONE"))
		strcpy(label, (strrchr(mbox, '/')) ? (char *) strrchr(mbox, '/')
		+ 1 : mbox);

/* Which index file will be called "index.html"?
*/

	strcpy(datename, (!strcmp(defaultindex, "date")) ? INDEXNAME :
	DATENAME);
	strcpy(thrdname, (!strcmp(defaultindex, "thread")) ? INDEXNAME :
	THRDNAME);
	strcpy(subjname, (!strcmp(defaultindex, "subject")) ? INDEXNAME :
	SUBJNAME);
	strcpy(authname, (!strcmp(defaultindex, "author")) ? INDEXNAME :
	AUTHNAME);

	if (use_mbox && use_stdin)
		progerr("Can't read from file and stdin at once.");

	gettimezone();
	getthisyear();
	if (increment) {
		loadoldheaders(dir);
		loadheaders(mbox, use_stdin, 1);
		checkdir(dir);
		writearticles(dir, label, overwrite, bignum);
		fixnextheader(dir, bignum - 1);
#ifdef SHOWREPLIES
		fixreplyheader(dir, bignum);
#endif
		fixthreadheader(dir, bignum);
	}
	else {
		loadheaders(mbox, use_stdin, 0);
		checkdir(dir);
		writearticles(dir, label, overwrite, 0);
	}

/* Always write the index files
*/

	writedates(dir, label, archives, about);
	writethreads(dir, label, archives, about);
	writesubjects(dir, label, archives, about);
	writeauthors(dir, label, archives, about);

	exit(0);
}

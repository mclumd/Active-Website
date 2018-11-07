/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/25/94
*/

#include "hypermail.h"
#include "file.h"

/* Is a file a directory?
*/

int isdirectory(path)
     char *path;
{
	struct stat stbuf;

	if (stat(path, &stbuf))
		return 0;
	return ((stbuf.st_mode & S_IFMT) == S_IFDIR) ? 1 : 0;
}

/* Does a file exist?
*/

int isfile(path)
     char *path;
{
	struct stat stbuf;

	if (stat(path, &stbuf))
		return 0;
	return ((stbuf.st_mode & S_IFMT) == S_IFREG) ? 1 : 0;
}

/* This tries to create and chmod a directory.
*/

void checkdir(dir)
     char *dir;
{
	if (!isdirectory(dir)) {
		if (mkdir(dir, dirmode) == -1) {
			sprintf(errmsg,
			"Couldn't create archive directory \"%s\".", dir);
			progerr(NULL);
		}
		else if (showprogress)
			printf("Creating directory \"%s\", mode %o.\n",
			dir, dirmode);
		if (chmod(dir, dirmode) == -1) {
			sprintf(errmsg, "Couldn't chmod \"%s\" to %o.",
			dir, dirmode);
			progerr(NULL);
		}
	}
}

/* Reads a configuration file if it exists and puts all the right
** values into the right variables.
*/

void readconfigs(path, mbox, label, dir, archives, about, overwrite, increment,
defaultindex)
     char *path;
     char *mbox;
     char *label;
     char *dir;
     char *archives;
     char *about;
     int *overwrite;
     int *increment;
     char *defaultindex;
{
	char tmppath[MAXFILELEN], line[MAXLINE], value[MAXLINE];
	struct passwd *pp;
	FILE *fp;

	if (!strcmp(path, "NONE"))
		return;
	if (path[0] == '~') {
		pp = getpwuid(getuid());
		sprintf(tmppath, "%s%s", pp->pw_dir, path + 1);
		if ((fp = fopen(tmppath, "r")) == NULL)
			return;
	}
	else {
		if ((fp = fopen(path, "r")) == NULL)
			return;
		printf("path: %s\n", path);
	}
	while (fgets(line, MAXLINE, fp) != NULL) {
		if (line[0] == '#' || line[0] == '\n')
			continue;
		if (getconfvalue(line, "hm_mbox", value) != NULL)
			strcpy(mbox, value);
		if (getconfvalue(line, "hm_label", value) != NULL)
			strcpy(label, value);
		if (getconfvalue(line, "hm_archives", value) != NULL)
			strcpy(archives, value);
		if (getconfvalue(line, "hm_about", value) != NULL)
			strcpy(about, value);
		if (getconfvalue(line, "hm_dir", value) != NULL)
			strcpy(dir, value);
		if (getconfvalue(line, "hm_defaultindex", value) != NULL)
			strcpy(defaultindex, value);

		if (getconfvalue(line, "hm_progress", value) != NULL)
			showprogress = atoi(value);
		if (getconfvalue(line, "hm_overwrite", value) != NULL)
			*overwrite = atoi(value);
		if (getconfvalue(line, "hm_increment", value) != NULL)
			*increment = atoi(value);
		if (getconfvalue(line, "hm_reverse", value) != NULL)
			reverse = atoi(value);
		if (getconfvalue(line, "hm_showheaders", value) != NULL)
			showheaders = atoi(value);
		if (getconfvalue(line, "hm_showhtml", value) != NULL)
			showhtml = atoi(value);
		if (getconfvalue(line, "hm_thrdlevels", value) != NULL)
			thrdlevels = atoi(value);
		if (getconfvalue(line, "hm_dirmode", value) != NULL)
			dirmode = strtol(value, (char **) NULL, 0);
		if (getconfvalue(line, "hm_filemode", value) != NULL)
			filemode = strtol(value, (char **) NULL, 0);
	}
	fclose(fp);
}

/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/31/94
*/

/* All the nasty string functions live here.
*/

#include "hypermail.h"
#include "string.h"

/* Malloc() out a string, give it to whoever asked for it.
*/

char *strdup(s)
     char *s;
{
	char *p;

	p = (char *) emalloc(strlen(s) + 1);
	strcpy(p, s);
	return p;
}

/* Strips the timezone information from long date strings, so more correct
** comparisons can be made between dates when looking for article replies.
*/

char *stripzone(date)
     char *date;
{
	int num;
	static char tmpdate[DATESTRLEN];

	if (!strcmp(date, NODATE))
		return date;
	strcpy(tmpdate, date);
	num = strlen(tmpdate);
	while (tmpdate[num] != ':')
		num--;
	num += 3;
	while (tmpdate[num])
		tmpdate[num++] = '\0';

	return tmpdate;
}

/* How many times does the character c appear in string s?
*/

int numstrchr(s, c)
     char *s;
     char c;
{
	int i;

	for (i = 0; *s != '\0'; s++)
		if (*s == c)
			i++;
	return i;
}

/* This is pretty cruddy and needs to be cleaned up and fixed!
** Converts URLs, email addresses, and message IDs in a line to links,
** mail commands, and articles respectively.
*/

char *convurls(line, mailid, mailsubject)
     char *line;
     char *mailid;
     char *mailsubject;
{
	int i, j, k, limit, status, subjmatch;
	char *c, anchor[URLSTRLEN], inreply[REPYSTRLEN], name[NAMESTRLEN],
		subject[SUBJSTRLEN];
	static char tmpline[MAXLINE], tmpline2[MAXLINE], tmpline3[MAXLINE],
		tmpline4[MAXLINE];

	if (!strncmp(line, "In-Reply-To:", 12) ||
	!strncmp(line, "References:", 11) ||
	((!strncmp(line, "    ", 4) || line[0] == '\t')
	&& strchr(line, '<') && strchr(line, '>') && strchr(line, '@'))) {
		strcpy(inreply, (char *) getreply(line));
		strcpy(tmpline4, convchars(line));
		if (inreply[0] != '\0') {
			status = hashreplylookup(inreply, name, subject,
			&subjmatch);
			if (status != -1) {
				for (i = j = 0; tmpline4[i] != ' ' &&
				j < MAXLINE && tmpline4[i] != '\t'; i++)
					tmpline3[j++] = tmpline4[i];
				while (isspace(tmpline4[i])) {
					tmpline3[j++] = ' ';
					i++;
				}
				tmpline3[j] = '\0';
				sprintf(tmpline3, "%s<a href=\"%.3d.html\">",
				tmpline3, status);
				for (j = strlen(tmpline3); tmpline4[i] &&
				tmpline4[i] != '\n' && j < MAXLINE; i++)
					tmpline3[j++] = tmpline4[i];
				tmpline3[j] = '\0';
				strcat(tmpline3, "</a>\n");
				return tmpline3;
			}
		}
		return tmpline4;
	}

	strcpy(tmpline3, convchars(line));

	for (i = 0; urls[i] != NULL; i++)
		if ((c = (char *) strstr(tmpline3, urls[i])) != NULL) {
			limit = c - tmpline3;
			for (j = k = 0; j < limit && j < MAXLINE; j++)
				tmpline[k++] = tmpline3[j];
			tmpline[k] = '\0';
			j = 0;
			while (*c && *c != ' ' && *c != ')' &&
			*c != '>' && *c != '&' && *c != '\n' &&
			*c != '\"'  && *c != ',' && *c != ']' &&
			*c != '[' && *c != '\t' && *c != '\\' &&
			j < URLSTRLEN) {
				anchor[j++] = *c;
				c++;
			}
			if (anchor[j - 1] == '.' ||
			anchor[j - 1] == '*') {
				anchor[j - 1] = '\0';
				c--;
			}
			anchor[j] = '\0';
			sprintf(tmpline2, "%s<a href=\"%s\">%s</a>",
			tmpline, anchor, anchor);
			k = strlen(tmpline2);
			while (*c && k < MAXLINE) {
				tmpline2[k++] = *c;
				c++;
			}
			tmpline2[k] = '\0';
			strcpy(tmpline3, tmpline2);
			if (!strncmp(urls[i], "mailto:", 7))
				return tmpline3;
		}

#ifdef MAILCOMMAND
	if (strstr(tmpline3, "message") || strstr(tmpline3, "-Id"))
		return tmpline3;

	if ((c = (char *) strchr(tmpline3, '@')) != NULL) {
		while (c != tmpline3 && *c != ' ' && *c != ','
		&& *c != '<' && *c != ';' && *c != '(' &&
		*c != '\t' && *c != '=')
			c--;
		if (c != tmpline3 || (c == tmpline3 && *c == '\t'))
			c++;
		limit = c - tmpline3;
		for (j = k = 0; j < limit && j < MAXLINE; j++)
			tmpline[k++] = tmpline3[j];
		tmpline[k] = '\0';
		j = 0;
		while (*c && *c != ' ' && *c != ')' &&
		*c != '>' && *c != '&' && *c != '\n' &&
		*c != '\"'  && *c != ',' && *c != ']' &&
		*c != '[' && *c != '=' && *c != '\t' &&
		j < URLSTRLEN) {
			anchor[j++] = *c;
			c++;
		}
		if (anchor[j - 1] == '.' || anchor[j - 1] == '*') {
			anchor[j - 1] = '\0';
			c--;
		}
		anchor[j] = '\0';
		if (anchor[0] == '@' || anchor[j - 1] == '@' ||
		!strchr(anchor, '.'))
			return tmpline3;
		sprintf(tmpline2, "%s<a href=\"%s\">%s</a>",
			tmpline, makemailcommand(MAILCOMMAND, anchor,
			mailid, mailsubject), anchor);
		k = strlen(tmpline2);
		while (*c && k < MAXLINE) {
			tmpline2[k++] = *c;
			c++;
		}
		tmpline2[k] = '\0';
		strcpy(tmpline3, tmpline2);
	}
#endif

	return tmpline3;
}

/* Converts <, >, and & to &lt;, &gt; and &amp;.
** It's ugly because it's fast.
*/

char *convchars(line)
     char *line;
{
	int i, j;
	static char tmpline[MAXLINE];

	for (i = j = 0; line[i] && i < MAXLINE; i++) {
		if (line[i] == '<') {
			tmpline[j++] = '&';
			tmpline[j++] = 'l';
			tmpline[j++] = 't';
			tmpline[j++] = ';';
		}
		else if (line[i] == '>') {
			tmpline[j++] = '&';
			tmpline[j++] = 'g';
			tmpline[j++] = 't';
			tmpline[j++] = ';';
		}
		else if (line[i] == '&') {
			tmpline[j++] = '&';
			tmpline[j++] = 'a';
			tmpline[j++] = 'm';
			tmpline[j++] = 'p';
			tmpline[j++] = ';';
		}
		else
			tmpline[j++] = line[i];
	}
	tmpline[j] = '\0';

	return tmpline;
}

/* Just the opposite of convchars().
** It's ugly because it's also fast.
*/

char *unconvchars(line)
     char *line;
{
	static char tmpline[MAXLINE];
	int i, j;

	for (i = j = 0; line[i]; i++)
		if (line[i] == '&') {
			if (line[i + 1] && line[i + 1] == 'l') {
				if (line[i + 2] && line[i + 2] == 't')
					if (line[i + 3] && line[i + 3] == ';') {
						tmpline[j++] = '<';
						i += 3;
					}
			}
			else if (line[i + 1] && line[i + 1] == 'g') {
				if (line[i + 2] && line[i + 2] == 't')
					if (line[i + 3] && line[i + 3] == ';') {
						tmpline[j++] = '>';
						i += 3;
					}
			}
			else if (line[i + 1] && line[i + 1] == 'a') {
				if (line[i + 2] && line[i + 2] == 'm')
					if (line[i + 3] && line[i + 3] == 'p')
					if (line[i + 4] && line[i + 4] == ';') {
						tmpline[j++] = '&';
						i += 4;
					}
			}
			else
				tmpline[j++] = line[i];
		}
		else
			tmpline[j++] = line[i];
	tmpline[j] = '\0';

	return tmpline;
}

/* Grabs whatever happens to be between the outermost double quotes in a line.
** This is for grabbing the values in comments.
*/

char *getvalue(line)
     char *line;
{
	int i;
	char *c, *d;
	static char tmpline[MAXLINE];

	tmpline[0] = '\0';
	c = (char *) strchr(line, '\"');
	d = (char *) strrchr(line, '\"');
	if (c == NULL)
		return tmpline;
	for (c++, i = 0; *c && c != d && i < MAXLINE; c++)
		tmpline[i++] = *c;
	tmpline[i] = '\0';

	return tmpline;
}

/* Grabs the value for a particular variable in a line - really,
** just about anything after an equals sign.
*/

char *getconfvalue(line, var, value)
     char *line;
     char *var;
     char *value;
{
	int i;
	char *c;
	static char tmpvalue[MAXLINE];

	if (strstr(line, var) != NULL) {
		if ((c = (char *) strchr(line, '=')) == NULL)
			return NULL;
		c++;
		while (isspace(*c) || *c == '\"')
			c++;
		if (*c == '\0')
			return NULL;
		for (i = 0; *c != '\0' && *c != '\"' && *c != '\n' &&
		i < MAXLINE; c++)
			tmpvalue[i++] = *c;
		tmpvalue[i] = '\0';
		strcpy(value, tmpvalue);
		return tmpvalue;
	}
	else
		return NULL;
}

/* Recursively gets rid of re:'s in a subject and strips spaces at the
** end of subjects. Makes the subject index much less cluttered...
*/

char *unre(subject)
    char *subject;
{
	int i;
	char *c;
	static char tmpsubject[SUBJSTRLEN], tmpsubject2[SUBJSTRLEN];

	if (!strstr(subject, "Re:") && !strstr(subject, "re:"))
		return subject;

	c = (char *) strstr(subject, "Re:");
	if (c == NULL)
		c = (char *) strstr(subject, "re:");
	c += 3;
	while (isspace(*c))
		c++;
	i = 0;
	while (*c && i < SUBJSTRLEN) {
		tmpsubject[i++] = *c;
		c++;
	}
	i--;
	while (isspace(tmpsubject[i]) || tmpsubject[i] == '\n') {
		tmpsubject[i] = '\0';
		i--;
	}
	i++;
	tmpsubject[i] = '\0';

	while (strstr(tmpsubject, "Re:") || strstr(tmpsubject, "re:")) {
		strcpy(tmpsubject2, unre(tmpsubject));
		strcpy(tmpsubject, tmpsubject2);
	}

	return tmpsubject;
}

/* Only gets rid of one re: in a subject, so messages the subject is a reply to
** can be guessed.
*/

void oneunre(inreply, subject)
     char *inreply;
     char *subject;
{
        int i;
        char *c;

        if (strstr(subject, "Re: ") || strstr(subject, "re: ")) {
                c = (char *) strchr(subject, ':') + 2;
                if (*c && *c == ' ')
                        c++;
                for (i = 0; *c && *c != '\n' && i < MAXLINE; c++)
                        inreply[i++] = *c;
                inreply[i] = '\0';
        }
}

/* Removes the last carriage return from a line.
*/

char *rmcr(line)
     char *line;
{
	int i;
	static char tmpline[MAXLINE];

	i = 0;
	while (*line && i < MAXLINE) {
		if (*line == '\n')
			*line = '\0';
		tmpline[i++] = *line;
		line++;
	}
	tmpline[i] = '\0';

	return tmpline;
}

/* Is a line in an article body part of a quoted passage?
*/

int isquote(line)
     char *line;
{
	while (*line && isspace(*line)) {
		line++;
	}
	if (!(*line))
		return 0;
	if (*line == '|' || *line == ']' || *line == '>')
		return 1;

	return 0;
}

/* Generates the mail command to use from the default mail command,
** the current recipient's email address, the current ID of the
** message, and the current subject.
*/

char *makemailcommand(mailcommand, email, id, subject)
     char *mailcommand;
     char *email;
     char *id;
     char *subject;
{
	int hasre;
	static char tmpsubject[SUBJSTRLEN], tmpstring[MAXLINE];

	if (!strncmp(subject, "Re:", 3) || !strncmp(subject, "re:", 3))
		hasre = 1;
	else
		hasre = 0;

	sprintf(tmpsubject, "%s%s", (hasre) ? "" : "Re: ", subject);

	strcpy(tmpstring, replace(mailcommand, "$TO", email));
	strcpy(tmpstring, replace(tmpstring, "$ID", id));
	strcpy(tmpstring, replace(tmpstring, "$SUBJECT", tmpsubject));
	strcpy(tmpstring, replace(tmpstring, " ", "%20"));
	strcpy(tmpstring, replace(tmpstring, "+", "%22"));

	return tmpstring;
}

/* Given a string, replaces all instances of "oldpiece" with "newpiece".
*/

char *replace(string, oldpiece, newpiece)
     char *string;
     char *oldpiece;
     char *newpiece;
{
	int i, j, limit;
	char *c;
	char beforestring[MAXLINE], afterstring[MAXLINE];
	static char newstring[MAXLINE];

	if ((c = (char *) strstr(string, oldpiece)) == NULL)
		return string;
	limit = c - string;

	for (i = 0; i < limit; i++)
		beforestring[i] = string[i];
	beforestring[i] = '\0';

	i += strlen(oldpiece);

	for (j = 0; string[i] != '\0'; i++)
		afterstring[j++] = string[i];
	afterstring[j] = '\0';

	sprintf(newstring, "%s%s%s", beforestring, newpiece, afterstring);

	while (strstr(newstring, oldpiece))
		strcpy(newstring, replace(newstring, oldpiece, newpiece));

	return newstring;
}

/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/31/94
*/

#include "hypermail.h"
#include "parse.h"

/* Parsing...the heart of Hypermail!
** This loads in the articles from stdin or a mailbox, adding the right
** field variables to the right structures. If readone is set, it will
** think anything it reads in is one article only.
*/

void loadheaders(mbox, use_stdin, readone)
     char *mbox;
     int use_stdin;
     int readone;
{
	FILE *fp;
	char name[NAMESTRLEN], email[MAILSTRLEN], date[DATESTRLEN],
		msgid[MSGDSTRLEN], subject[SUBJSTRLEN], inreply[REPYSTRLEN],
		line[MAXLINE], tmpfrom[MAXLINE], fromdate[DATESTRLEN],
		oldline[MAXLINE];
	int num, isinheader, hassubject, hasdate, wasinreply;
	struct body *bp;

	if (!strcmp(mbox, "NONE") || use_stdin)
		fp = stdin;
	else {
		if ((fp = fopen(mbox, "r")) == NULL) {
			sprintf(errmsg, "Couldn't open mail archive \"%s\".",
			mbox);
			progerr(NULL);
		}
	}

	if (readone)
		num = bignum;
	else
		num = 0;
	hassubject = 0;
	hasdate = 0;
	wasinreply = 0;
	isinheader = 1;
	inreply[0] = '\0';
	tmpfrom[0] = '\0';
	oldline[0] = '\0';
	bp = NULL;
	if (!readone) {
		replylist = NULL;
		subjectlist = NULL;
		authorlist = NULL;
		datelist = NULL;
	}
	if (showprogress && readone)
		printf("Reading new header...\n");
	if (showprogress && !readone) {
		if (!strcmp(mbox, "NONE") || use_stdin)
			printf("Loading mailbox...    ");
		else
			printf("Loading mailbox \"%s\"...    ", mbox);
	}
	while (fgets(line, MAXLINE, fp) != NULL) {
		if (isinheader) {
			if (!strncmp(line, "Received:", 9) ||
			!strncmp(line, "Return-Path:", 12) ||
			!strncmp(line, "Cc:", 3) ||
			!strncmp(line, "X-", 2) ||
			!strncmp(line, "Flags:", 6))
				continue;
			if (!strncmp(line, "From ", 5))
				strcpy(fromdate, (char *) getfromdate(line));
			else if (!strncmp(line, "Date:", 5)) {
				bp = (struct body *) addbody(bp, line);
				strcpy(date, (char *) getmaildate(line));
				hasdate = 1;
			}
			else if (!strncmp(line, "From:", 5)) {
				bp = (struct body *) addbody(bp, line);
				getname(line, name, email);
			}
			else if (!strncmp(line, "Message-Id:", 11)) {
				bp = (struct body *) addbody(bp, line);
				strcpy(msgid, (char *) getid(line));
			}
			else if (!strncmp(line, "Subject:", 8)) {
				bp = (struct body *) addbody(bp, line);
				strcpy(subject, (char *) getsubject(line));
				hassubject = 1;
			}
			else if (!strncmp(line, "In-Reply-To:", 12)) {
				bp = (struct body *) addbody(bp, line);
				strcpy(inreply, (char *) getreply(line));
				wasinreply = 1;
			}
			else if ((!strncmp(line, "    ", 4) ||
			line[0] == '\t') &&
			strchr(line, '<') && strchr(line, '>') &&
			strchr(line, '@') && wasinreply) {
				bp = (struct body *) addbody(bp, line);
				strcpy(inreply, (char *) getreply(line));
				wasinreply = 0;
			}
			else if (!strncmp(line, "To:", 3))
				bp = (struct body *) addbody(bp, line);
			else if (line[0] == '\n') {
				bp = (struct body *) addbody(bp, line);
				isinheader = 0;
			}
		}
		else {
			if (!strncmp(line, "From ", 5)) {
				if (readone)
					continue;
				if (strstr(oldline,
				"Forwarded message:")) {
					oldline[0] = '\0';
					continue;
				}
				isinheader = 1;
				wasinreply = 0;
				if (!hassubject)
					strcpy(subject, NOSUBJECT);
				else
					hassubject = 1;
				if (!hasdate)
					strcpy(date, NODATE);
				else
					hasdate = 1;
				if (inreply[0] == '\0')
					oneunre(inreply, subject);
				while (rmlastlines(bp));
				addhash(num, date, name, email, msgid, subject,
				inreply, fromdate, bp);
				authorlist = (struct header *)
				addheader(authorlist, num, name, subject,
				date, 1);
				subjectlist = (struct header *)
				addheader(subjectlist, num, name, unre(subject),
				date, 0);
				datelist = (struct header *)
				addheader(datelist, num, name, subject,
				fromdate, 2);
				strcpy(fromdate, (char *) getfromdate(line));

				bp = NULL;
				num++;
				if (!(num % 10) && showprogress && !readone) {
					printf("\b\b\b\b%4d", num);
					fflush(stdout);
				}
				inreply[0] = '\0';
			}
			else
				bp = (struct body *) addbody(bp, line);
		}
		strcpy(oldline, line);
	}
	if (!isinheader) {
		if (!hassubject)
			strcpy(subject, NOSUBJECT);
		if (!hasdate)
			strcpy(date, NODATE);
		if (inreply[0] == '\0')
			oneunre(inreply, subject);
		while (rmlastlines(bp));
		addhash(num, date, name, email, msgid, subject, inreply,
		fromdate, bp);
		authorlist = (struct header *)
		addheader(authorlist, num, name, subject, date, 1);
		subjectlist = (struct header *)
		addheader(subjectlist, num, name, unre(subject), date, 0);
		datelist = (struct header *) addheader(datelist, num, name,
		subject, fromdate, 2);
		num++;
	}
	if (showprogress && !readone)
		printf("\b\b\b\b%4d articles.\n", num);

	if (!readone)
		bignum = num - 1;
	fclose(fp);

	crossindex();

	threadlist = NULL;
	printedthreadlist = NULL;
	crossindexthread1(datelist);
}

/* All this does is get all the relevant header information from the
** comment fields in existing archive files. Everything is loaded into
** structures in the exact same way as if articles were being read from
** stdin or a mailbox.
*/

void loadoldheaders(dir)
     char *dir;
{
	FILE *fp;
	char name[NAMESTRLEN], email[MAILSTRLEN], date[DATESTRLEN],
		msgid[MSGDSTRLEN], subject[SUBJSTRLEN], inreply[REPYSTRLEN],
		line[MAXLINE], fromdate[DATESTRLEN], filename[MAXFILELEN];
	int num;
	struct body *bp;

	num = 0;
	sprintf(filename, "%s%s%.4d.html", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", num);

	bp = NULL;
	bp = (struct body *) addbody(bp, "\0");

	authorlist = subjectlist = datelist = NULL;

	if (showprogress)
		printf("Reading old headers...    ");
	while ((fp = fopen(filename, "r")) != NULL) {

		fgets(line, MAXLINE, fp);
		strcpy(fromdate, (char *) getvalue(line));
		fgets(line, MAXLINE, fp);
		strcpy(date, (char *) getvalue(line));
		fgets(line, MAXLINE, fp);
		strcpy(name, (char *) getvalue(line));
		fgets(line, MAXLINE, fp);
		strcpy(email, (char *) getvalue(line));
		fgets(line, MAXLINE, fp);
		strcpy(subject, (char *) unconvchars(getvalue(line)));
		fgets(line, MAXLINE, fp);
		strcpy(msgid, (char *) getvalue(line));
		fgets(line, MAXLINE, fp);
		strcpy(inreply, (char *) unconvchars(getvalue(line)));

		fclose(fp);

		addhash(num, date, name, email, msgid, subject,
		inreply, fromdate, bp);
		authorlist = (struct header *)
		addheader(authorlist, num, name, subject, date, 1);
		subjectlist = (struct header *)
		addheader(subjectlist, num, name, unre(subject), date, 0);
		datelist = (struct header *)
		addheader(datelist, num, name, subject, fromdate, 2);

		num++;
		if (!(num % 10) && showprogress) {
			printf("\b\b\b\b%4d", num);
			fflush(stdout);
		}

		sprintf(filename, "%s%s%.4d.html", dir,
		(dir[strlen(dir) - 1] == '/') ? "" : "/", num);
	}
	if (showprogress)
		printf("\b\b\b\b%4d articles.\n", num);

	if (!num)
		bignum = 0;
	else
		bignum = num;
}

/* Adds a "Next:" link in the proper article, after the archive has been
** incrementally updated.
*/

void fixnextheader(dir, num)
     char *dir;
     int num;
{
	char filename[MAXFILELEN], line[MAXLINE], name[NAMESTRLEN],
		email[MAILSTRLEN], subject[SUBJSTRLEN], inreply[REPYSTRLEN],
		date[DATESTRLEN], fromdate[DATESTRLEN], msgid[MSGDSTRLEN];
	struct body *bp, *status;
	FILE *fp;

	sprintf(filename, "%s%s%.4d.html", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", num);

	bp = NULL;
	if ((fp = fopen(filename, "r")) != NULL) {
		while ((fgets(line, MAXLINE, fp)) != NULL)
			bp = (struct body *) addbody(bp, line);
	}
	else
		return;
	fclose(fp);

	if ((fp = fopen(filename, "w+")) != NULL)
		while (bp != NULL) {
			fprintf(fp, "%s", bp->line);
			if (!strncmp(bp->line, "<!-- next=", 10)) {

				status = (struct body *) hashnumlookup(num + 1,
				name, email, subject, inreply, date, fromdate,
				msgid);
				if (status != NULL) {
				fprintf(fp, "<li> <b>Next message:</b> ");
				fprintf(fp,
				"<a href=\"%.4d.html\">%s: \"%s\"</a>\n",
				num + 1, name, convchars(subject));
				}

			}
			bp = bp->next;
		}
	fclose(fp);
}

/* Adds a "Reply:" link in the proper article, after the archive has been
** incrementally updated.
*/

void fixreplyheader(dir, num)
     char *dir;
     int num;
{
	int replynum, subjmatch;
	char filename[MAXFILELEN], line[MAXLINE], name[NAMESTRLEN],
		email[MAILSTRLEN], subject[SUBJSTRLEN], inreply[REPYSTRLEN],
		date[DATESTRLEN], fromdate[DATESTRLEN], msgid[MSGDSTRLEN],
		name2[NAMESTRLEN], subject2[SUBJSTRLEN];
	struct body *bp, *status;
	FILE *fp;

	status = (struct body *) hashnumlookup(num,
	name, email, subject, inreply, date, fromdate, msgid);
	if (status == NULL || inreply[0] == '\0')
		return;
	if (inreply[0] != '\0') {
		replynum = hashreplylookup(inreply, name2, subject2,
		&subjmatch);
		if (replynum == -1)
			return;
	}

	sprintf(filename, "%s%s%.4d.html", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", replynum);

	bp = NULL;
	if ((fp = fopen(filename, "r")) != NULL) {
		while ((fgets(line, MAXLINE, fp)) != NULL)
			bp = (struct body *) addbody(bp, line);
	}
	else
		return;
	fclose(fp);

	if ((fp = fopen(filename, "w+")) != NULL)
		while (bp != NULL) {
			if (!strncmp(bp->line, "<!-- reply", 10)) {
				fprintf(fp, "<li> <b>Reply:</b> ");
				fprintf(fp, "<a href=\"%.4d.html\">", num);
				fprintf(fp, "%s: \"%s\"</a>\n",
				name, convchars(subject));
			}
			fprintf(fp, "%s", bp->line);
			bp = bp->next;
		}
	fclose(fp);
}

/* Adds a "Next in thread:" link in the proper article, after the archive
** has been incrementally updated.
*/

void fixthreadheader(dir, num)
     char *dir;
     int num;
{
	int threadnum;
	char filename[MAXFILELEN], line[MAXLINE], name[NAMESTRLEN],
		subject[SUBJSTRLEN];
	struct body *bp;
	struct reply *rp;
	FILE *fp;

	for (rp = threadlist; rp != NULL; rp = rp->next)
		if (rp->next != NULL && rp->next->msgnum == num &&
		rp->msgnum != -1) {
			threadnum = rp->msgnum;
			strcpy(name, rp->next->name);
			strcpy(subject, rp->next->subject);
			break;
		}
	if (rp == NULL)
		return;

	sprintf(filename, "%s%s%.4d.html", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", threadnum);

	bp = NULL;
	if ((fp = fopen(filename, "r")) != NULL) {
		while ((fgets(line, MAXLINE, fp)) != NULL)
			bp = (struct body *) addbody(bp, line);
	}
	else
		return;
	fclose(fp);

	if ((fp = fopen(filename, "w+")) != NULL)
		while (bp != NULL) {
			fprintf(fp, "%s", bp->line);
			if (!strncmp(bp->line, "<!-- nextthr", 12)) {
				fprintf(fp, "<li> <b>Next in thread:</b> ");
				fprintf(fp, "<a href=\"%.4d.html\">", num);
				fprintf(fp, "%s: \"%s\"</a>\n",
				name, convchars(subject));
			}
			bp = bp->next;
		}
	fclose(fp);
}

/* Cross-indexes - adds to a list of replies. If a message is a reply to
** another, the number of the messge it's replying to is added to the list.
** This list is searched upon printing.
*/

void crossindex()
{
	int num, status, maybereply;
	char name[NAMESTRLEN], subject[SUBJSTRLEN], email[MAILSTRLEN],
		inreply[REPYSTRLEN], date[DATESTRLEN], fromdate[DATESTRLEN],
		msgid[MSGDSTRLEN];

	num = 0;
	replylist = NULL;

	while (hashnumlookup(num, name, email, subject, inreply,
	date, fromdate, msgid) != NULL) {
		if (inreply[0] != '\0') {
			status = hashreplynumlookup(inreply, &maybereply);
			if (status != -1)
				replylist = (struct reply *)
				addreply(replylist, status, num, name, subject,
				maybereply);
		}
		num++;
	}
}

/* First, print out the threads in order by date...
** Each message number is appended to a thread list. Threads and individual
** messages are separated by a -1.
*/

void crossindexthread1(hp)
     struct header *hp;
{
        int hasreply;
        struct reply *rp;

        if (hp != NULL) {
                crossindexthread1(hp->left);

                for (hasreply = 0, rp = replylist; rp != NULL; rp = rp->next)
                        if (rp->frommsgnum == hp->msgnum) {
                                hasreply = 1;
                                break;
                        }

                if (!hasreply && !wasprinted(printedthreadlist, hp->msgnum)) {
			threadlist = (struct reply *)
			addreply(threadlist, hp->msgnum, hp->msgnum,
			hp->name, hp->subject, 0);
                        crossindexthread2(hp->msgnum);
			threadlist = (struct reply *)
			addreply(threadlist, -1, -1, " ", " ", 0);
		}

                crossindexthread1(hp->right);
        }
}

/* Recursively checks for replies to replies to a message, etc.
** Replies are added to the thread list.
*/

void crossindexthread2(num)
     int num;
{
        struct reply *rp;

        for (rp = replylist; rp != NULL; rp = rp->next)
                if (rp->msgnum == num) {
			threadlist = (struct reply *)
			addreply(threadlist, rp->frommsgnum, num,
			rp->name, rp->subject, 0);
                        printedlist = (struct printed *)
                        markasprinted(printedthreadlist, rp->frommsgnum);
                        crossindexthread2(rp->frommsgnum);
                }
}

/* Grabs the date string from a Date: header.
*/

char *getmaildate(line)
     char *line;
{
	int i;
	char *c;
	static char date[DATESTRLEN];

	c = (char *) strchr(line, ':');
	if ((*(c + 1) && *(c + 1) == '\n') ||
	(*(c + 2) && *(c + 2) == '\n')) {
		strcpy(date, NODATE);
		return date;
	}
	c += 2;
	while (isspace(*c))
		c++;
	for (i = 0; *c && *c != '\n' && i < DATESTRLEN; c++)
		date[i++] = *c;
	date[i] = '\0';

	return date;
}

/* Grabs the date string from a From article separator.
*/

char *getfromdate(line)
     char *line;
{
	int i;
	char *c;
	static char tmpdate[DATESTRLEN];

	for (i = 0; days[i] != NULL && (c = (char *) strstr(line, days[i]))
	== NULL; i++)
		;
	if (days[i] == NULL)
		tmpdate[0] = '\0';
	else {
		for (i = 0; *c && *c != '\n' && i < DATESTRLEN; c++)
			tmpdate[i++] = *c;
		tmpdate[i] = '\0';
		if (tmpdate[16] != ':') {
			tmpdate[16] = ':';
			tmpdate[17] = '0';
			tmpdate[18] = '0';
			tmpdate[19] = ' ';
			tmpdate[20] = thisyear[0];
			tmpdate[21] = thisyear[1];
			tmpdate[22] = thisyear[2];
			tmpdate[23] = thisyear[3];
			tmpdate[24] = '\0';
		}
		sprintf(tmpdate, "%s %s", tmpdate, timezonestr);
		return tmpdate;
	}

	return tmpdate;
}

/* Grabs the name and email address from a From: header.
** This could get tricky; I've tried to keep it simple.
*/

void getname(line, name, email)
     char *line;
     char *name;
     char *email;
{
	int i;
	char *c;

	if ((c = (char *) strchr(line, '@')) == NULL) {
		if (strchr(line, '(')) {
			c = (char *) strchr(line, ':') + 1;
			while (isspace(*c))
				c++;
			for (i = 0; *c && *c != '(' && *c != ' ' &&
			*c != '\n' && i < MAILSTRLEN; c++)
				email[i++] = *c;
			email[i] = '\0';
		}
		else
			strcpy(email, NOEMAIL);
	}
	else {
		while (*c != ' ' && *c != '<')
			c--;
		c++;
		for (i = 0; *c && *c != '>' && *c != ' ' && *c != '\n' &&
		i < MAILSTRLEN;
		c++)
			email[i++] = *c;
		email[i] = '\0';
	}

	if (strchr(line, '<')) {
		c = (char *) strchr(line, ':') + 1;
		while (isspace(*c) || *c == '\"')
			c++;
	}
	else if (strchr(line, '('))
		c = (char *) strchr(line, '(') + 1;
	else {
		if (strcmp(email, NOEMAIL))
			strcpy(name, email);
		else
			strcpy(name, NONAME);
		return;
	}

	for (i = 0; *c && *c != '<' && *c != '\"' && *c != ')' &&
	*c != '(' && *c != '\n' && i < NAMESTRLEN; c++)
		name[i++] = *c;
	if (*c == '<' || *c == '(')
		name[--i] = '\0';
	else
		name[i] = '\0';
}

/* Grabs the message ID, like <...> from the Message-ID: header.
*/

char *getid(line)
     char *line;
{
	int i;
	char *c;
	static char msgid[MSGDSTRLEN];

	c = (char *) strchr(line, '<') + 1;
	for (i = 0; *c && *c != '>' && *c != '\n' && i < MSGDSTRLEN; c++) {
		if (*c == '\\')
			continue;
		msgid[i++] = *c;
	}
	msgid[i] = '\0';

	return msgid;
}

/* Grabs the subject from the Subject: header.
*/

char *getsubject(line)
     char *line;
{
	int i;
	char *c;
	static char subject[SUBJSTRLEN];

	c = (char *) strchr(line, ':') + 2;
	while (isspace(*c))
		c++;
	for (i = 0; *c && *c != '\n' && i < SUBJSTRLEN; c++)
		subject[i++] = *c;
	subject[i] = '\0';
	for (i--; i >= 0 && isspace(subject[i]); i--)
		subject[i] = '\0';

	if (subject[0] == NULL || isspace(subject[0]) || subject[0] == '\n' ||
	!strcasecmp(subject, "Re:"))
		strcpy(subject, NOSUBJECT);

	return subject;
}

/* Grabs the message ID, or date, from the In-reply-to: header.
** Who knows what other formats are out there...
*/

char *getreply(line)
     char *line;
{
	int i;
	char *c;
	static char reply[REPYSTRLEN];

	if ((c = (char *) strstr(line, "dated: ")) != NULL) {
		c += 7;
		for (i = 0; *c && *c != '.' && *c != '\n' && i < REPYSTRLEN;
		c++)
			reply[i++] = *c;
		reply[i] = '\0';
		return reply;
	}

	if ((c = (char *) strchr(line, '<')) != NULL ) {
		c++;
		for (i = 0; *c && *c != '>' && *c != '\n' && i < MSGDSTRLEN;
		c++) {
			if (*c == '\\')
				continue;
			reply[i++] = *c;
		}
		reply[i] = '\0';
		return reply;
	}

	if ((c = (char *) strstr(line, "sage of ")) != NULL) {
		c += 8;
		if (*c == '\"')
			c++;
		for (i = 0; *c && *c != '.' && *c != '\n' && *c != 'f' &&
		i < REPYSTRLEN; c++)
			reply[i++] = *c;
		reply[i] = '\0';
		if (*c == 'f')
			reply[--i] = '\0';
		return reply;
	}

	reply[0] = '\0';
	return reply;
}

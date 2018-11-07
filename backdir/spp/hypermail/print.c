/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/31/94
*/

#include "hypermail.h"
#include "print.h"

/* Printing...the other main part of this program!
** This writes out the articles, beginning with the number startnum.
*/

void writearticles(dir, label, overwrite, startnum)
     char *dir;
     char *label;
     int overwrite;
     int startnum;
{
	int num, tmpnum, statusnum, skip, subjmatch, newfile;
	char name[NAMESTRLEN], email[MAILSTRLEN], subject[SUBJSTRLEN],
		inreply[REPYSTRLEN], date[DATESTRLEN], fromdate[DATESTRLEN],
		msgid[MSGDSTRLEN];
	char currentemail[MAILSTRLEN], currentid[MSGDSTRLEN],
		currentsubject[SUBJSTRLEN];
	char name2[NAMESTRLEN], email2[MAILSTRLEN], subject2[SUBJSTRLEN],
		inreply2[REPYSTRLEN], date2[DATESTRLEN], fromdate2[DATESTRLEN],
		msgid2[MSGDSTRLEN];
	char filename[MAXFILELEN];
	struct body *bp, *status;
	struct reply *rp;
	FILE *fp;

	num = startnum;
	skip = 0;

	if (showprogress)
		printf("Writing articles to \"%s\"...    ", dir);

	while ((bp = (struct body *) hashnumlookup(num, name, email,
	subject, inreply, date, fromdate, msgid)) != NULL) {

		strcpy(currentemail, email);
		strcpy(currentid, msgid);
		strcpy(currentsubject, subject);

		sprintf(filename, "%s%s%.4d.html", dir,
		(dir[strlen(dir) - 1] == '/') ? "" : "/", num);

/* Determine to overwrite files or not
*/
		if (isfile(filename))
			newfile = 0;
		else
			newfile = 1;

		if (!newfile && !overwrite) {
			skip = 1;
			num++;
			continue;
		}
		else {
			if ((fp = fopen(filename, "w")) == NULL) {
				sprintf(errmsg, "Couldn't write \"%s\".",
				filename);
				progerr(NULL);
			}
		}

/* Create the comment fields necessary for incremental updating
*/

		printcomment(fp, "received", fromdate);
		printcomment(fp, "sent", date);
		printcomment(fp, "name", name);
		printcomment(fp, "email", email);
		printcomment(fp, "subject", convchars(subject));
		printcomment(fp, "id", msgid);
		printcomment(fp, "inreplyto", convchars(inreply));

		fprintf(fp, "<title>%s: %s</title>\n", label,
		convchars(subject));
		fprintf(fp, "<h1>%s</h1>\n", convchars(subject));
		if (!strcmp(name, email)) {
#ifdef MAILCOMMAND
			fprintf(fp, "<a href=\"%s\">",
			makemailcommand(MAILCOMMAND, email, currentid,
			currentsubject));
			fprintf(fp, "<i>%s</i></a><br>\n", name);
#else
			fprintf(fp, "<i>%s</i><br>\n", name);
#endif
		}
		else {
#ifdef MAILCOMMAND
			fprintf(fp, "<b>%s</b> (<a href=\"%s\">", name,
			makemailcommand(MAILCOMMAND, email, currentid,
			currentsubject));
			fprintf(fp, "<i>%s</i></a>)<br>\n", email);
#else
			fprintf(fp, "%s (<i>%s</i>)<br>\n", name, email);
#endif
		}
		fprintf(fp, "<i>%s</i>\n<p>\n", date);

		fprintf(fp, "<ul>\n");

		fprintf(fp, "<li> <b>Messages sorted by:</b> ");
		fprintf(fp, "<a href=\"%s#%d\">[ date ]</a>", datename, num);
		fprintf(fp, "<a href=\"%s#%d\">[ thread ]</a>", thrdname, num);
		fprintf(fp, "<a href=\"%s#%d\">[ subject ]</a>", subjname, num);
		fprintf(fp, "<a href=\"%s#%d\">[ author ]</a>\n", authname,
		num);

		printcomment(fp, "next", "start");

/* Is there a next message?
*/

		status = (struct body *) hashnumlookup(num + 1, name2,
		email2, subject2, inreply2, date2, fromdate2, msgid2);
		if (status != NULL) {
			fprintf(fp, "<li> <b>Next message:</b> ");
			fprintf(fp, "<a href=\"%.4d.html\">%s: \"%s\"</a>\n",
			num + 1, name2, convchars(subject2));
		}

/* Is there a previous message?
*/

		status = (struct body *) hashnumlookup(num - 1, name2,
		email2, subject2, inreply2, date2, fromdate2, msgid2);
		if (status != NULL) {
			fprintf(fp, "<li> <b>Previous message:</b> ");
			fprintf(fp, "<a href=\"%.4d.html\">%s: \"%s\"</a>\n",
			num - 1, name2, convchars(subject2));
		}

/* Is this message a reply to another?
*/

		if (inreply[0] != '\0') {
			statusnum = hashreplylookup(inreply, name, subject,
			&subjmatch);
			if (statusnum != -1) {
				if (subjmatch)
					fprintf(fp,
					"<li> <b>Maybe in reply to:</b>");
				else
					fprintf(fp,
					"<li> <b>In reply to:</b>");
				fprintf(fp, " <a href=\"%.4d.html\">",
				statusnum);
				fprintf(fp, "%s: \"%s\"</a>\n",
				name, convchars(subject));
			}
		}

/* Is there a message next in the thread?
*/

		printcomment(fp, "nextthread", "start");

		for (rp = threadlist; rp != NULL; rp = rp->next)
			if (rp->msgnum == num && rp->next != NULL &&
			rp->next->msgnum != -1) {
				fprintf(fp, "<li> <b>Next in thread:</b> ");
				fprintf(fp, "<a href=\"%.4d.html\">",
				rp->next->msgnum);
				fprintf(fp, "%s: \"%s\"</a>\n",
				rp->next->name, convchars(rp->next->subject));
			}

/* Does this message have replies? If so, print them all!
*/

#ifdef SHOWREPLIES
		for (rp = replylist; rp != NULL; rp = rp->next)
			if (rp->msgnum == num) {
				if (rp->maybereply)
					fprintf(fp, "<li> <b>Maybe reply:</b>");
				else
					fprintf(fp, "<li> <b>Reply:</b>");
				fprintf(fp, " <a href=\"%.4d.html\">",
				rp->frommsgnum);
				fprintf(fp, "%s: \"%s\"</a>\n",
				rp->name, convchars(rp->subject));
			}
		printcomment(fp, "reply", "end");
#endif

		fprintf(fp, "</ul>\n");

/* Finally...print the body!
*/
		printbody(fp, bp, currentid, currentsubject);

		fprintf(fp, "<p>\n<ul>\n");

		printcomment(fp, "next", "start");

		status = (struct body *) hashnumlookup(num + 1, name2,
		email2, subject2, inreply2, date2, fromdate2, msgid2);
		if (status != NULL) {
			fprintf(fp, "<li> <b>Next message:</b> ");
			fprintf(fp, "<a href=\"%.4d.html\">%s: \"%s\"</a>\n",
			num + 1, name2, convchars(subject2));
		}

		status = (struct body *) hashnumlookup(num - 1, name2,
		email2, subject2, inreply2, date2, fromdate2, msgid2);
		if (status != NULL) {
			fprintf(fp, "<li> <b>Previous message:</b> ");
			fprintf(fp, "<a href=\"%.4d.html\">%s: \"%s\"</a>\n",
			num - 1, name2, convchars(subject2));
		}

		if (inreply[0] != '\0') {
			statusnum = hashreplylookup(inreply, name, subject,
			&subjmatch);
			if (statusnum != -1) {
				if (subjmatch)
					fprintf(fp,
					"<li> <b>Maybe in reply to:</b>");
				else
					fprintf(fp,
					"<li> <b>In reply to:</b>");
				fprintf(fp, " <a href=\"%.4d.html\">",
				statusnum);
				fprintf(fp, "%s: \"%s\"</a>\n",
				name, convchars(subject));
			}
		}

		printcomment(fp, "nextthread", "start");

		for (rp = threadlist; rp != NULL; rp = rp->next)
			if (rp->msgnum == num && rp->next != NULL &&
			rp->next->msgnum != -1) {
				fprintf(fp, "<li> <b>Next in thread:</b> ");
				fprintf(fp, "<a href=\"%.4d.html\">",
				rp->next->msgnum);
				fprintf(fp, "%s: \"%s\"</a>\n",
				rp->next->name, convchars(rp->next->subject));
			}

#ifdef SHOWREPLIES
		for (rp = replylist; rp != NULL; rp = rp->next)
			if (rp->msgnum == num) {
				if (rp->maybereply)
					fprintf(fp, "<li> <b>Maybe reply:</b>");
				else
					fprintf(fp, "<li> <b>Reply:</b>");
				fprintf(fp, " <a href=\"%.4d.html\">",
				rp->frommsgnum);
				fprintf(fp, "%s: \"%s\"</a>\n",
				rp->name, convchars(rp->subject));
			}
		printcomment(fp, "reply", "end");
#endif

		fprintf(fp, "</ul>\n");

		fclose(fp);

		if (newfile && chmod(filename, filemode) == -1) {
			sprintf(errmsg, "Couldn't chmod \"%s\" to %o.",
			filename, filemode);
			progerr(NULL);
		}

		if (!(num % 5) && showprogress && !skip) {
			printf("\b\b\b\b%3.0f%c", ((float) num / (float) bignum)
			* 100, '%');
			fflush(stdout);
		}

		num++;
	}
	if (showprogress)
		printf("\b\b\b\b    \n");
	if (!overwrite)
		bignum = num - 1;
}

void printbody(fp, bp, id, subject)
     FILE *fp;
     struct body *bp;
     char *id;
     char *subject;
{

	int inheader, insig, inblank, inhtml;

#ifdef SHOWHR
	fprintf(fp, "<hr>\n");
#endif
	printcomment(fp, "body", "start");

	if (!showhtml || (showhtml && showheaders))
		fprintf(fp, "<pre>\n");

	inheader = 1;
	inblank = 1;
	inhtml = 0;
	insig = 0;

	while (bp != NULL) {

		if ((bp->line)[0] == '\n') {
			if (inheader && showhtml && showheaders)
				fprintf(fp, "</pre>\n");
			inheader = 0;
		}
		else if (inheader && !showheaders) {
			bp = bp->next;
			continue;
		}

		if ((bp->line)[0] == '\n' && inblank) {
			bp = bp->next;
			continue;
		}
		else
			inblank = 0;

		if (!strncmp(bp->line, "<html>", 6) ||
		!strncmp(bp->line, "<HTML>", 6)) {
			inhtml = 1;
			if (!showhtml)
				fprintf(fp, "</pre>\n");
		}
		else if (!strncmp(bp->line, "</html>", 7) ||
		!strncmp(bp->line, "</HTML>", 7)) {
			inhtml = 0;
			fprintf(fp, "%s", bp->line);
			if (!showhtml)
				fprintf(fp, "<pre>\n");
			bp = bp->next;
			continue;
		}

		if (inhtml) {
			fprintf(fp, "%s", bp->line);
			bp = bp->next;
			continue;
		}

		if (showhtml) {
			if (!strcmp(bp->line, "--\n") ||
			!strcmp(bp->line, "-- \n") ||
			!strcmp(bp->line, "---\n")) {
				insig = 1;
				fprintf(fp, "<pre>\n");
			}

			if ((bp->line)[0] == '\n')
				fprintf(fp, "<p>\n");
			else {
				if (insig)
					fprintf(fp, "%s",
					(char *) convurls(bp->line,
					id, subject));
				else if (isquote(bp->line))
#ifdef IQUOTES
					fprintf(fp, "<i>%s</i><br>\n",
					(char *) convurls(rmcr(bp->line),
					id, subject));
#else
					fprintf(fp, "%s<br>\n",
					(char *) convurls(rmcr(bp->line),
					id, subject));
#endif
				else if ((bp->line)[0] != '\0')
#ifdef SHOWBR
					fprintf(fp, "%s<br>\n",
					(char *) convurls(rmcr(bp->line),
					id, subject));
#else
					fprintf(fp, "%s\n",
					(char *) convurls(rmcr(bp->line),
					id, subject));
#endif
			}
		}
		else if ((bp->line)[0] != '\0')
			fprintf(fp, "%s", (char *) convurls(bp->line,
			id, subject));

		bp = bp->next;

	}

	if (!showhtml)
		fprintf(fp, "</pre>\n");
	else {
		if (insig)
			fprintf(fp, "</pre>\n");
	}

	printcomment(fp, "body", "end");
#ifdef SHOWHR
	fprintf(fp, "<hr>\n");
#endif
}

/* Write the date index...
*/

void writedates(dir, label, archives, about)
     char *dir;
     char *label;
     char *archives;
     char *about;
{
	int num, newfile;
	char name[NAMESTRLEN], email[MAILSTRLEN], subject[SUBJSTRLEN],
		inreply[REPYSTRLEN];
	char filename[MAXFILELEN];
	struct body *bp, *status;
	FILE *fp;

	sprintf(filename, "%s%s%s", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", datename);
	if (isfile(filename))
		newfile = 0;
	else
		newfile = 1;
	if ((fp = fopen(filename, "w")) == NULL) {
		sprintf(errmsg, "Couldn't write \"%s\".", filename);
		progerr(NULL);
	}

	if (showprogress)
		printf("Writing date index to \"%s\"...", filename);

	fprintf(fp, "<title>%s by date</title>\n", label);
	fprintf(fp, "<a name=\"start\"><h1>%s by date</h1></a>\n", label);
	fprintf(fp, "<ul>\n");
	if (strcmp(about, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">About this ", about);
		fprintf(fp, "archive</a></b>\n");
	}
	if (!reverse)
		fprintf(fp,
		"<li> <b><a href=\"#end\">Most recent messages</a></b>\n");
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ thread ]</a>", thrdname);
	fprintf(fp, "<a href=\"%s#start\">[ subject ]</a>", subjname);
	fprintf(fp, "<a href=\"%s#start\">[ author ]</a>\n", authname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</b>\n</ul>\n<p>\n");
	fprintf(fp, "<b>Starting:</b> <i>%s</i><br>\n",
	getdatestr(firstdatenum));
	fprintf(fp, "<b>Ending:</b> <i>%s</i><br>\n", getdatestr(lastdatenum));
	fprintf(fp, "<b>Messages:</b> %d\n<p>\n", bignum + 1);
	fprintf(fp, "<ul>\n");

	printdates(fp, datelist);

	fprintf(fp, "</ul>\n<p>\n");
	fprintf(fp, "<a name=\"end\"><b>Last message date:</b></a> ");
	fprintf(fp, "<i>%s</i><br>\n", getdatestr(lastdatenum));
	fprintf(fp, "<b>Archived on:</b> <i>%s</i><p>\n", getlocaltime());
	fprintf(fp, "<ul>\n");
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ thread ]</a>", thrdname);
	fprintf(fp, "<a href=\"%s#start\">[ subject ]</a>", subjname);
	fprintf(fp, "<a href=\"%s#start\">[ author ]</a>\n", authname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</a></b>\n</ul>\n<p>\n");
	printfooter(fp);
	fclose(fp);

	if (newfile && chmod(filename, filemode) == -1) {
		sprintf(errmsg, "Couldn't chmod \"%s\" to %o.", filename,
		filemode);
		progerr(NULL);
	}

	if (showprogress)
		putchar('\n');
}

/* Write the thread index...
*/

void writethreads(dir, label, archives, about)
     char *dir;
     char *label;
     char *archives;
     char *about;
{
	int newfile;
	char filename[MAXFILELEN];
	FILE *fp;

	printedlist = NULL;

	sprintf(filename, "%s%s%s", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", thrdname);
	if (isfile(filename))
		newfile = 0;
	else
		newfile = 1;
	if ((fp = fopen(filename, "w")) == NULL) {
		sprintf(errmsg, "Couldn't write \"%s\".", filename);
		progerr(NULL);
	}

	if (showprogress)
		printf("Writing thread index to \"%s\"...", filename);

	fprintf(fp, "<title>%s by thread</title>\n", label);
	fprintf(fp, "<a name=\"start\"><h1>%s by thread</h1></a>\n", label);
	fprintf(fp, "<ul>\n");
	if (strcmp(about, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">About this ", about);
		fprintf(fp, "archive</a></b>\n");
	}
	if (!reverse)
		fprintf(fp,
		"<li> <b><a href=\"#end\">Most recent messages</a></b>\n");
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ date ]</a>", datename);
	fprintf(fp, "<a href=\"%s#start\">[ subject ]</a>", subjname);
	fprintf(fp, "<a href=\"%s#start\">[ author ]</a>\n", authname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</ul>\n<p>\n");
	fprintf(fp, "<b>Starting:</b> <i>%s</i><br>\n",
	getdatestr(firstdatenum));
	fprintf(fp, "<b>Ending:</b> <i>%s</i><br>\n", getdatestr(lastdatenum));
	fprintf(fp, "<b>Messages:</b> %d\n<p>\n", bignum + 1);
	fprintf(fp, "<ul>\n");

	printthreads(fp, datelist);

	fprintf(fp, "</ul>\n<p>\n");
	fprintf(fp, "<a name=\"end\"><b>Last message date:</b></a> ");
	fprintf(fp, "<i>%s</i><br>\n", getdatestr(lastdatenum));
	fprintf(fp, "<b>Archived on:</b> <i>%s</i><p>\n", getlocaltime());
	fprintf(fp, "<ul>\n");
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ date ]</a>", datename);
	fprintf(fp, "<a href=\"%s#start\">[ subject ]</a>", subjname);
	fprintf(fp, "<a href=\"%s#start\">[ author ]</a>\n", authname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</ul>\n<p>\n");
	printfooter(fp);
	fclose(fp);

	if (newfile && chmod(filename, filemode) == -1) {
		sprintf(errmsg, "Couldn't chmod \"%s\" to %o.", filename,
		filemode);
		progerr(NULL);
	}

	if (showprogress)
		putchar('\n');
}

/* Prints the article pointers in the thread index by date.
*/

void printthreads(fp, hp)
     FILE *fp;
     struct header *hp;
{
	int hasreply;
	struct reply *rp;

	if (hp != NULL) {
		printthreads(fp, hp->left);

		for (hasreply = 0, rp = replylist; rp != NULL; rp = rp->next)
			if (rp->frommsgnum == hp->msgnum) {
				hasreply = 1;
				break;
			}

		if (!hasreply && !wasprinted(printedlist, hp->msgnum)) {
			fprintf(fp, "<li> <a href=\"%.4d.html\"><b>%s</b></a> ",
			hp->msgnum, convchars(hp->subject));
			fprintf(fp, "<a name=\"%d\"><i>%s</i></a>\n",
			hp->msgnum, hp->name);
			printedlist = (struct printed *)
			markasprinted(printedlist, hp->msgnum);

			checkreplies(fp, hp->msgnum, 1);
		}

		printthreads(fp, hp->right);
	}
}

/* Prints the subject index.
*/

void writesubjects(dir, label, archives, about)
     char *dir;
     char *label;
     char *archives;
     char *about;
{
	int newfile;
	char filename[MAXFILELEN];
	FILE *fp;

	sprintf(filename, "%s%s%s", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", subjname);
	if (isfile(filename))
		newfile = 0;
	else
		newfile = 1;
	if ((fp = fopen(filename, "w")) == NULL) {
		sprintf(errmsg, "Couldn't write \"%s\".", filename);
		progerr(NULL);
	}

	if (showprogress)
		printf("Writing subject index to \"%s\"...", filename);

	fprintf(fp, "<title>%s by subject</title>\n", label);
	fprintf(fp, "<a name=\"start\"><h1>%s by subject</h1></a>\n", label);
	fprintf(fp, "<ul>\n");
	if (strcmp(about, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">About this ", about);
		fprintf(fp, "archive</a></b>\n");
	}
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ date ]</a>", datename);
	fprintf(fp, "<a href=\"%s#start\">[ thread ]</a>", thrdname);
	fprintf(fp, "<a href=\"%s#start\">[ author ]</a>\n", authname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</ul>\n<p>\n");
	fprintf(fp, "<b>Starting:</b> <i>%s</i><br>\n",
	getdatestr(firstdatenum));
	fprintf(fp, "<b>Ending:</b> <i>%s</i><br>\n", getdatestr(lastdatenum));
	fprintf(fp, "<b>Messages:</b> %d\n<p>\n", bignum + 1);
	fprintf(fp, "<ul>\n");

	printsubjects(fp, subjectlist);

	fprintf(fp, "</ul>\n<p>\n");
	fprintf(fp, "<b>Last message date:</b> <i>%s</i><br>\n",
	getdatestr(lastdatenum));
	fprintf(fp, "<b>Archived on:</b> <i>%s</i><p>\n", getlocaltime());
	fprintf(fp, "<ul>\n");
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ date ]</a>", datename);
	fprintf(fp, "<a href=\"%s#start\">[ thread ]</a>", thrdname);
	fprintf(fp, "<a href=\"%s#start\">[ author ]</a>\n", authname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</ul>\n<p>\n");
	printfooter(fp);
	fclose(fp);

	if (newfile && chmod(filename, filemode) == -1) {
		sprintf(errmsg, "Couldn't chmod \"%s\" to %o.", filename,
		filemode);
		progerr(NULL);
	}

	if (showprogress)
		putchar('\n');
}

/* Pretty-prints the dates in the index files.
*/

void printdates(fp, hp)
     FILE *fp;
     struct header *hp;
{
	if (hp != NULL) {
		printdates(fp, hp->left);
		fprintf(fp, "<li> <a href=\"%.4d.html\"><b>%s</b></a> ",
		hp->msgnum, convchars(hp->subject));
		fprintf(fp, "<a name=\"%d\"><i>%s</i></a>\n", hp->msgnum,
		hp->name);
		printdates(fp, hp->right);
	}
}

/* Print the subject index pointers alphabetically.
*/

void printsubjects(fp, hp)
     FILE *fp;
     struct header *hp;
{
	static char oldsubject[SUBJSTRLEN];

	if (hp != NULL) {
		printsubjects(fp, hp->left);
		if (strcasecmp(hp->subject, oldsubject))
			fprintf(fp, "<li> <b>%s</b>\n",
			convchars(hp->subject));
		fprintf(fp, "<ul>\n");
		fprintf(fp, "<li> <a href=\"%.4d.html\">%s</a> ",
		hp->msgnum, hp->name);
		fprintf(fp, "<a name=\"%d\"><i>%s</i></a>\n", hp->msgnum,
		hp->datestr);
		fprintf(fp, "</ul>\n");
		strcpy(oldsubject, hp->subject);
		printsubjects(fp, hp->right);
	}
}

/* Prints the author index file.
*/

void writeauthors(dir, label, archives, about)
     char *dir;
     char *label;
     char *archives;
     char *about;
{
	int newfile;
	char filename[MAXFILELEN];
	FILE *fp;

	sprintf(filename, "%s%s%s", dir,
	(dir[strlen(dir) - 1] == '/') ? "" : "/", authname);
	if (isfile(filename))
		newfile = 0;
	else
		newfile = 1;
	if ((fp = fopen(filename, "w")) == NULL) {
		sprintf(errmsg, "Couldn't write \"%s\".", filename);
		progerr(NULL);
	}

	if (showprogress)
		printf("Writing author index to \"%s\"...", filename);

	fprintf(fp, "<title>%s by author</title>\n", label);
	fprintf(fp, "<a name=\"start\"><h1>%s by author</h1></a>\n", label);
	fprintf(fp, "<ul>\n");
	if (strcmp(about, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">About this ", about);
		fprintf(fp, "archive</a></b>\n");
	}
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ date ]</a>", datename);
	fprintf(fp, "<a href=\"%s#start\">[ thread ]</a>", thrdname);
	fprintf(fp, "<a href=\"%s#start\">[ subject ]</a>\n", subjname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</ul>\n<p>\n");
	fprintf(fp, "<b>Starting:</b> <i>%s</i><br>\n",
	getdatestr(firstdatenum));
	fprintf(fp, "<b>Ending:</b> <i>%s</i><br>\n", getdatestr(lastdatenum));
	fprintf(fp, "<b>Messages:</b> %d\n<p>\n", bignum + 1);
	fprintf(fp, "<ul>\n");

	printauthors(fp, authorlist);

	fprintf(fp, "</ul>\n<p>\n");
	fprintf(fp, "<b>Last message date:</b> <i>%s</i><br>\n",
	getdatestr(lastdatenum));
	fprintf(fp, "<b>Archived on:</b> <i>%s</i><p>\n", getlocaltime());
	fprintf(fp, "<ul>\n");
	fprintf(fp, "<li> <b>Messages sorted by:</b> ");
	fprintf(fp, "<a href=\"%s#start\">[ date ]</a>", datename);
	fprintf(fp, "<a href=\"%s#start\">[ thread ]</a>", thrdname);
	fprintf(fp, "<a href=\"%s#start\">[ subject ]</a>\n", subjname);
	if (strcmp(archives, "NONE")) {
		fprintf(fp, "<li> <b><a href=\"%s\">Other mail ", archives);
		fprintf(fp, "archives</a></b>\n");
	}
	fprintf(fp, "</ul>\n<p>\n");
	printfooter(fp);
	fclose(fp);

	if (newfile && chmod(filename, filemode) == -1) {
		sprintf(errmsg, "Couldn't chmod \"%s\" to %o.", filename,
		filemode);
		progerr(NULL);
	}

	if (showprogress)
		putchar('\n');
}

/* Prints the author index links sorted alphabetically.
*/

void printauthors(fp, hp)
     FILE *fp;
     struct header *hp;
{
	static char oldname[SUBJSTRLEN];

	if (hp != NULL) {
		printauthors(fp, hp->left);
		if (strcasecmp(hp->name, oldname))
			fprintf(fp, "<li> <b>%s</b>\n", hp->name);
		fprintf(fp, "<ul>\n");
		fprintf(fp, "<li> <a href=\"%.4d.html\">%s</a> ",
		hp->msgnum, convchars(hp->subject));
		fprintf(fp, "<a name=\"%d\"><i>%s</i></a>\n", hp->msgnum,
		hp->datestr);
		fprintf(fp, "</ul>\n");
		strcpy(oldname, hp->name);
		printauthors(fp, hp->right);
	}
}

/* While printing the thread index, prints any replies to replies.
*/

void checkreplies(fp, num, level)
     FILE *fp;
     int num;
     int level;
{
	struct reply *rp;

	for (rp = replylist; rp != NULL; rp = rp->next)
		if (rp->msgnum == num) {
			if (level < thrdlevels)
				fprintf(fp, "<ul>\n");
			fprintf(fp, "<li> <a href=\"%.4d.html\">",
			rp->frommsgnum);
			printedlist = (struct printed *)
			markasprinted(printedlist, rp->frommsgnum);
			fprintf(fp, "<b>%s </b></a> ",
			convchars(rp->subject));
			fprintf(fp, "<a name=\"%d\"><i>%s</i></a>\n",
			rp->frommsgnum, rp->name);
			checkreplies(fp, rp->frommsgnum, level + 1);
			if (level < thrdlevels)
				fprintf(fp, "</ul>\n");
		}
}

/* Prints a comment in the file.
*/

void printcomment(fp, label, value)
     FILE *fp;
     char *label;
     char *value;
{
	fprintf(fp, "<!-- %s=\"%s\" -->\n", label, value);
}

/* Prints the footer and pointer to the docs in the index files.
*/

void printfooter(fp)
     FILE *fp;
{
	fprintf(fp, "<hr>\n<i>This archive was generated by ");
	fprintf(fp, "<a href=\"%s\">%s %s</a>.</i>\n",
	HMURL, PROGNAME, VERSION);
}

/* Prints a program error string and stops. If errorstr is NULL,
** this prints whatever happens to be in errstr[] at the time.
*/

void progerr(errorstr)
     char *errorstr;
{
	if (errorstr != NULL)
	        fprintf(stderr, "%s: %s\n", PROGNAME, errorstr);
	else
	        fprintf(stderr, "%s: %s\n", PROGNAME, errmsg);
        fprintf(stderr, "%s: type \"%s -z\" for options.\n", PROGNAME,
        PROGNAME);
        exit(-1);
}

/* Prints the usage.
*/

void usage()
{
	printf("  usage: %s -upix [-m \"mbox\"] [-l \"label\"] ",
	PROGNAME);
	printf("[-d \"dir\"]\n");
	printf("         [-a \"URL\"] [-b \"URL\"] [-c \"file\"]\n\n");
	printf("options: -u : Update archive by one article\n");
	printf("         -p : Show progress\n");
	printf("         -i : Read messages from standard input\n");
	printf("         -x : Overwrite previous messages\n");
	printf("         -m : Mail archive to read in\n");
	printf("         -l : What to name the output archive\n");
	printf("         -d : The directory to save HTML files in\n");
	printf("         -a : URL to other archives\n");
	printf("         -b : URL to archive information\n");
	printf("         -c : Configuration file to read in\n\n");
	printf("version: %s\n", VERSION);
	printf("   docs: %s\n", HMURL);

	exit(1);
}

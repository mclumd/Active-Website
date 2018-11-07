/*
** mail.c 1.0
** This tiny CGI program constructs an email form (GET) and
** sends the contents (POST).
**
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Jay Weber, weber@eit.com
** Kevin Hughes, kevinh@eit.com 
** 7/28/94
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libcgi/cgi.h"

#define CGINAME  "/cgi-bin/mail"
#define SENDMAIL "/usr/lib/sendmail"
#define ROWS     25
#define COLS     60
#define MAXLEN   80

void printform();
char *strdup();
void progerr();
char *lookupnumaddr();

cgi_main(ci)
     cgi_info *ci;
{
	form_entry *parms, *p;
	form_entry *get_form_entries();
	char *from, *to, *subject, *replyto, *body, *host;
	char tmpstr[MAXLEN];
	FILE *f;

	print_mimeheader("text/html");
 
	from = to = subject = body = replyto = "";
	parms = get_form_entries(ci);

	if (parms) {
		for (p = parms; p; p = p->next) {
			if (!strcasecmp(p->name, "from"))
				from = p->val;
			else if (!strcasecmp(p->name, "to"))
				to = p->val;
			else if (!strcasecmp(p->name, "subject"))
				subject = p->val;
			else if (!strcasecmp(p->name, "body"))
				body = p->val;
			else if (!strcasecmp(p->name, "replyto"))
				replyto = p->val;
		}
	}

	sprintf(tmpstr, lookupnumaddr(ci->remote_addr));
	if (strchr(tmpstr, '.'))
		host = (char *) strdup(tmpstr);
	else
		host = (char *) strdup(ci->remote_addr);

	sprintf(tmpstr, "%s@%s", (ci->remote_user && *(ci->remote_user)) ?
	ci->remote_user : "", host);
	if (from == NULL || from[0] == '\0')
		from = (char *) strdup(tmpstr);

	switch (mcode(ci)) {

	case MCODE_HEAD:
		return;

	case MCODE_GET:
		printform(to, from, subject, body, replyto);
		break;
    
	case MCODE_POST:
		if (from == NULL || from[0] == '\0' || from[0] == '@' ||
		from[strlen(from) - 1] == '@' || !strchr(from, '@'))
			progerr("Invalid <b>From:</b> address.");
		if (to == NULL || to[0] == '\0' || to[0] == '@' ||
		to[strlen(to) - 1] == '@' || !strchr(to, '@'))
			progerr("Invalid <b>To:</b> address.");
		if (subject == NULL || subject[0] == '\0')
			progerr("The subject field is blank.");
		if (body == NULL || body[0] == '\0')
			progerr("No message has been written.");

		sprintf(tmpstr, "%s -t", SENDMAIL);
		if (f = popen(tmpstr, "w")) {
			fprintf(f, "From: %s\nTo: %s\n", from, to);
			if (replyto != NULL && replyto[0] != '\0')
				fprintf(f, "In-Reply-To: <%s>\n", replyto);
			fprintf(f, "Subject: %s\nX-Sender: %s\n\n%s",
			subject, host, body);
			pclose(f);

			printf("<title>Email Gateway Response</title>\n");
			printf("<h1>Your message has been sent</h1>\n");
			printf("The following message has been sent to ");
			printf("<b>%s</b>:\n<p>\n<pre>\n<hr>\n\n", to);

			printf("From: %s\n", from);
			printf("To: %s\n", to);
			if (replyto != NULL && replyto[0] != '\0')
				printf("In-Reply-To: <%s>\n", replyto);
			printf("Subject: %s\n", subject);
			printf("X-Sender: %s\n\n", host);
			printf("%s\n", body);

			printf("\n<hr></pre>\n");
		}
		else
			progerr("The message was not sent - sendmail error.");
		break;

		default:
			sprintf(tmpstr, "Unrecognized method used: \"%s\".",
			ci->request_method);
			progerr(tmpstr);
	}

	free_form_entries(parms);
}

void printform(to, from, subject, body, replyto)
     char *to;
     char *from;
     char *subject;
     char *body;
     char *replyto;
{
	printf("<title>Email Gateway</title>\n");
	printf("<h1>Email Gateway</h1>\n");
	printf("<i>Send an email message by filling in the form below.</i>\n");
	printf("<p>\n");

	printf("<form method=POST action=\"%s\">\n", CGINAME);
	printf("<pre>\n");
	printf("<b>         To:</b> ");
	printf("<input size=45 name=\"to\" value=\"%s\">\n", to);
	printf("<b>       From:</b> ");
	printf("<input size=45 name=\"from\" value=\"%s\">\n", from);
	printf("<b>    Subject:</b> ");
	printf("<input size=45 name=\"subject\" value=\"%s\">\n", subject);
	if (replyto != NULL && replyto[0] != '\0') {
		printf("<b>In reply to:</b> ");
		printf("<input size=45 name=\"replyto\" value=\"%s\">\n",
		replyto);
	}
	printf("\n");

	printf("<textarea name=\"body\" value=\"\" ");
	printf("rows=%d cols=%d>%s\n</textarea>\n\n", ROWS, COLS, body);

	printf("               ");
	printf("<input type=submit value=\"    Send Email    \">     ");
	printf("<input type=reset value=\"    Reset Form    \">");
	printf("</pre>\n</form>\n");
}

char *strdup(s)
     char *s;
{
	char *p;
 
	p = (char *) malloc(strlen(s) + 1);
	strcpy(p, s);
	return p;
}

void progerr(s)
     char *s;
{
	printf("<title>Email Gateway Error</title>\n");
	printf("<h1>Email Gateway Error</h1>\n");

	printf("This gateway program encountered an error:\n<p>\n");
	printf("<code>%s</code>\n", s);

	exit(0);
}

char *lookupnumaddr(numaddress)
     char *numaddress;
{
        unsigned long addr;
        struct hostent *he;

        addr = inet_addr(numaddress);
        if (addr == -1)
                return numaddress;

        he = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
        if (he)
                return he->h_name;
        else
                return numaddress;
}

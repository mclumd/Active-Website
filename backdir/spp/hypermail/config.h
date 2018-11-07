/*
** Configuration file for Hypermail (C version)
** Kevin Hughes, kevinh@eit.com
** 8/1/94
**
** Copyright (C) 1994, Enterprise Integration Technologies Corp.	      
** All Rights Reserved.
*/

/* User-definable options are below */

#ifndef MBOX
#define MBOX "mbox"
#endif

/* Define MBOX as "NONE" to read from standard input.
*/

#ifndef ARCHIVES
#define ARCHIVES ".."
#endif

/* This should be a URL of some type. Define ARCHIVES as "NONE" to omit
** a link to other archives.
*/

#ifndef ABOUT
#define ABOUT "NONE"
#endif

/* This should be a URL of some type. Define ABOUT as "NONE" to omit
** a link to archive information.
*/

#ifndef REVERSE
#define REVERSE 0
#endif

/* Define as 0 if you want latest message on the bottom for date and
** thread indexes. Else define as 1 to put recent articles on top.
*/

#ifndef SHOWHEADERS
#define SHOWHEADERS 0
#endif

/* Define as 0 if you want to hide mail headers in articles.
** Else define as 1.
*/

#ifndef SHOWHTML
#define SHOWHTML 1
#endif

/* Define as 0 if you want articles to be shown in a proportional font.
** Else define as 1.
*/

#ifndef SHOWBR
#define SHOWBR
#endif

/* Define if you want article lines to end with the <br> tag.
** Else uncomment to have non-quoted lines word-wrap. Only takes effect
** if SHOWHTML is defined.
*/

#ifndef IQUOTES
#define IQUOTES
#endif

/* Define if you want quoted lines to be shown in italics. Only
** take effect if SHOWHTML is defined.
*/

#ifndef SHOWHR
/* #define SHOWHR */
#endif

/* Define if you want horizontal rules before and after articles.
*/

#ifndef LABEL
#define LABEL "NONE"
#endif

/* Define as the default title you want to call your archives.
** Define as "NONE" to use the name of the input mailbox.
*/

#ifndef DIR
#define DIR "NONE"
#endif

/* This is the default directory that new files are put into and
** that existing files are read from. If defined as "NONE", this
** will be the name of the input mailbox.
*/

#ifndef OVERWRITE
#define OVERWRITE 0
#endif

/* Define as 0 to always overwrite existing files. Define as 1 to
** only write files that are new.
*/

#ifndef INCREMENT
#define INCREMENT 0
#endif

/* Define as 1 to read in one article only and update existing archives.
** Define as 0 for normal operation.
*/

#ifndef PROGRESS
#define PROGRESS 0
#endif

/* Define as 1 to show progress as Hypermail works. Define as 0 for silent
** operation.
*/

#ifndef DIRMODE
#define DIRMODE 0777
#endif

/* Define as the permissions mode to change any created directories to.
** This must be an octal number.
*/

#ifndef FILEMODE
#define FILEMODE 0666
#endif

/* Define as the permissions mode to change any created HTML files to.
** This must be an octal number.
*/

#ifndef THRDLEVELS
#define THRDLEVELS 2
#endif

/* Define as the number of thread level indents to show in the thread
** index file. The normal value is 2.
*/

#ifndef CONFIGFILE
#define CONFIGFILE "~/.hmrc"
#endif

/* Define as the configuration file to read settings from. ~ as the first
** character represents the current user's home directory. Define as
** "NONE" for no configuration file.
*/

#ifndef DEFAULTINDEX
#define DEFAULTINDEX "thread"
#endif

/* Define as the default main index that users see when the archive
** is first accessed. Possible values are "date", "subject", "author", and
** "thread".
*/

#ifndef EURODATE
/* #define EURODATE */
#endif

/* Define if you want dates in the index files as "DD MM YYYY" instead of
** "MM DD YYYY".
*/

#ifndef SHOWREPLIES
/* #define SHOWREPLIES */
#endif

/* Define if you want links to article replies in messages.
*/

#ifndef MAILCOMMAND
/*
#define MAILCOMMAND "/cgi-bin/mail?to=$TO&replyto=$ID&subject=$SUBJECT"
#define MAILCOMMAND "mailto:$TO"
*/
#endif

/* This is the mail command that email links go to, for instance
** "mailto:$TO" or "/cgi-bin/mail?to=$TO".
**
** In constructing this command, you can specify variables:
**
** $TO : the email address of the person you're sending mail to.
** $ID : the ID of the message you're replying to.
** $SUBJECT: the subject you're replying to.
*/

/* End of user-definable options */

/*
** Copyright (C) 1994, Enterprise Integration Technologies Corp.        
** All Rights Reserved.
** Kevin Hughes, kevinh@eit.com 
** 7/14/94
*/

/* Date stuff. Everything here is my own code, with the exception
** of getnweekday(), which is in the public domain. -- Kevin
*/

#include "hypermail.h"
#include "date.h"

/* This converts a long date string into the form D/M/Y.
*/

void convtoshortdate(date, shortdate)
     char *date;
     char *shortdate;
{
        char c, d, e;
        int i, month;

        c = date[4];
        d = date[5];
        e = date[6];

        for (i = 0; !(c == (months[i])[0] && d == (months[i])[1] &&
        e == (months[i])[2]); i++)
                ;
        month = i + 1;

        shortdate[0] = (month > 9) ? '1' : '0';
        shortdate[1] = monthnums[month];
        shortdate[2] = '/';
        shortdate[3] = (date[8] == ' ') ? '0' : date[8];
        shortdate[4] = date[9];
        shortdate[5] = '/';
        shortdate[6] = date[22];
        shortdate[7] = date[23];
        shortdate[8] = '\0';
}

/* Splits a short date (M/D/Y) into month, day, and year components.
** It looks ugly because it should be super-fast.
*/

void splitshortdate(shortdate, month, day, year)
     char *shortdate;
     int *month;
     int *day;
     int *year;
{
        if (shortdate[0] == ' ' || shortdate[0] == '0')
                *month = (shortdate[1] - '0');
        else if (shortdate[1] == '/')
                *month = (shortdate[0] - '0');
        else if (shortdate[2] == '/')
                *month = (shortdate[0] - '0') * 10 + (shortdate[1] - '0');

        if (shortdate[2] =='/') {
                if (shortdate[5] == '/') {
                        *day = (shortdate[3] - '0') * 10 +
                        (shortdate[4] - '0');
                        *year = (shortdate[6] - '0') * 10 +
                        (shortdate[7] - '0');
                }
                else if (shortdate[4] == '/') {
                        *day = (shortdate[3] - '0');
                        *year = (shortdate[5] - '0') * 10
                        + (shortdate[6] - '0');
                }
        }
        else if (shortdate[1] =='/') {
                if (shortdate[4] == '/') {
                        *day = (shortdate[2] - '0') * 10
                        + (shortdate[3] - '0');
                        *year = (shortdate[5] - '0') * 10
                        + (shortdate[6] - '0');
                }
                else if (shortdate[3] == '/') {
                        *day = (shortdate[2] - '0');
                        *year = (shortdate[4] - '0') * 10
                        + (shortdate[5] - '0');
                }
        }
}

/* Given a short date (M/D/Y), it returns the number of seconds
** since BASEYEAR.
*/

long getyearsecs(shortdate)
     char *shortdate;
{
        int i, yearday, yearsecs, prevyeardays;
        int month, day, year;

        splitshortdate(shortdate, &month, &day, &year);
        year += CENTURY;

        for (yearday = i = 0; i < month - 1; i++) {
                if (i == 1 && IS_LEAP(year))
                        yearday++;
                yearday += monthdays[i];
        }
        yearday += day;

        prevyeardays = 0;
        for (i = BASEYEAR; i != year; i++) {
                if (IS_LEAP(i))
                        prevyeardays++;
                prevyeardays += DAYSPERYEAR;
        }

        yearsecs = (yearday + prevyeardays) * SECSPERDAY;
 
        return yearsecs;
}

/* Given a long date string, it returns the number of seconds
** since BASEYEAR.
*/

int convtoyearsecs(date)
     char *date;
{
        char hourstr[3], minstr[3], secstr[3], shortdate[SHORTDATELEN];
        int hours, minutes, seconds;
        long yearsecs;

        convtoshortdate(date, shortdate);
        yearsecs = getyearsecs(shortdate);

        sprintf(hourstr, "%c%c", date[11], date[12]);
        sprintf(minstr, "%c%c", date[14], date[15]);
        sprintf(secstr, "%c%c", date[17], date[18]);

        hours = atoi(hourstr);
        minutes = atoi(minstr);
        seconds = atoi(secstr);

        return (int) (yearsecs + (hours * SECSPERHOUR) +
        (minutes * SECSPERMIN) + seconds);
}

/* Gets the local time and returns it formatted.
*/

char *getlocaltime()
{
        static char s[DATESTRLEN];
        time_t tp;

        time(&tp);
#ifdef EURODATE
        strftime(s, DATESTRLEN, "%a %d %b %Y - %H:%M:%S", localtime(&tp));
#else
        strftime(s, DATESTRLEN, "%a %b %d %Y - %H:%M:%S", localtime(&tp));
#endif
	sprintf(s, "%s %s", s, timezonestr);

        return s;
}

/* Gets the local time zone.
*/

void gettimezone()
{
        time_t tp;

        time(&tp);
        strftime(timezonestr, TIMEZONELEN, "%Z", localtime(&tp));
}

/* Gets the current year.
*/

void getthisyear()
{
        time_t tp;

        time(&tp);
        strftime(thisyear, YEARLEN, "%Y", localtime(&tp));
}

/* From the number of seconds since BASEYEAR, this pretty-prints
** a date for you.
*/

char *getdatestr(yearsecs)
     long yearsecs;
{
        register int day, year, month, hours, minutes;
        static char date[DATESTRLEN];

        for (day = 0; yearsecs > SECSPERDAY; day++)
                yearsecs -= SECSPERDAY;

        for (year = BASEYEAR; day > DAYSPERYEAR; year++) {
                if (IS_LEAP(year))
                        day--;
                day -= DAYSPERYEAR;
        }

        if (IS_LEAP(year) && day > (monthdays[0] + monthdays[1])) {
                day--;
                yearsecs -= SECSPERDAY;
        }

        for (month = 0; day > monthdays[month]; month++)
                day -= monthdays[month];

        for (hours = 0; yearsecs > SECSPERHOUR; hours++)
                yearsecs -= SECSPERHOUR;

        for (minutes = 0; yearsecs > SECSPERMIN; minutes++)
                yearsecs -= SECSPERMIN;

#ifdef EURODATE
        sprintf(date, "%s%s %02d %d - %02d:%02d:%02d %s",
        days[getnweekday(month + 1, day, year)], months[month],
        day, year, hours, minutes, yearsecs, timezonestr);
#else
        sprintf(date, "%s%02d %s %d - %02d:%02d:%02d %s",
        days[getnweekday(month + 1, day, year)], day, months[month],
        year, hours, minutes, yearsecs, timezonestr);
#endif

	return date;
}

/* Returns the day of the year.
*/

int getnweekday(mn, dy, yr)
     int mn;
     int dy;
     int yr;
{
        int n1, n2;

        if (mn < 3) {
                mn += 12;
                yr -= 1;
        }
        n1 = (26 * (mn + 1)) / 10;
        n2 = (int) ((125 * (long) yr) / 100);

        return ((dy + n1 + n2 - (yr / 100) + (yr / 400) + -1) % 7);
}

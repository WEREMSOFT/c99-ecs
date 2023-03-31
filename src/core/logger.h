#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <time.h>
#include <string.h>
#include <stdio.h>

void currentDateTimeToString(char *destinationString)
{
	time_t t = time(NULL);
  	struct tm tm = *localtime(&t);
  	snprintf(destinationString, 200, "%d/%02d/%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void loggerLog(char* message)
{
	char dateTime[50];
	currentDateTimeToString(dateTime);

	printf("\033[32mLOG: [%s] %s\n", dateTime, message);
}

void loggerError(char* message)
{
	char dateTime[50];
	currentDateTimeToString(dateTime);

	printf("\033[31mERR: [%s] %s\n", dateTime, message);
}
#endif
#include "log_util.h"
#include "TMP_CONF.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

const char ANSI_RESET[] = "";
const char ANSI_RED[] = "";
const char ANSI_GREEN[] = "";
const char ANSI_YELLOW[] = "";
const char ANSI_BLUE[] = "";
const char ANSI_MAGENTA[] = "";
const char ANSI_CYAN[] = "";
const char ANSI_WHITE[] = "";

FILE* LOGFILE = NULL;
logfunc_t LOGFUNCTION = logstring_default;
unsigned logpos = 0;

void log_open_file(char *filename, char* msg)
{
	time_t now = time(NULL);
	memset(msg, 0, 512);

	if (filename == NULL)
		LOGFILE = fopen(LOG_FILE_NAME, "a+");
	else
		LOGFILE = fopen(filename, "a+");

	if (LOGFILE)
	{
		sprintf(msg, "%s log start: %s", filename, ctime(&now));
		fprintf(LOGFILE, "%s log start: %s", filename, ctime(&now));
	}
	else 
	{
		sprintf(msg, "Could not open %s for output. Proceeding without a log file.\n", LOG_FILE_NAME);
		fprintf(stderr, "Could not open %s for output. Proceeding without a log file.\n", LOG_FILE_NAME);
	}	
}

void log_close_file(void)
{
	time_t now = time(NULL);

	if (LOGFILE)
	{
		fprintf(LOGFILE, "log ends: %s\n", ctime(&now));
		fclose(LOGFILE);
	}

	LOGFILE = NULL;
}

void logstring_default(int level, const char *str)
{
	if (level == LOG_DEBUG)
		fprintf(stderr, "%s", str);
	if (level == LOG_INFO)
		fprintf(stderr, "%s%s", ANSI_GREEN, str);
	if (level == LOG_WARNING)
		fprintf(stderr, "%s%s", ANSI_MAGENTA, str);
	if (level == LOG_ERROR)
		fprintf(stderr, "%s%s", ANSI_RED, str);
	fprintf(stderr, "%s", ANSI_RESET);
}

int log_printf(int level, const char *format, ...)
{
	va_list al;
	char *buf = NULL;
	unsigned len_buf;

	va_start(al, format);
	len_buf = vsnprintf(buf, 0, format, al);
	va_end(al);
	buf = (char *)malloc(len_buf + 24);
	if (level == LOG_DEBUG)
		sprintf(buf, "%04i DEBUG   ", logpos++);
	else if (level == LOG_INFO)
		sprintf(buf, "%04i INFO    ", logpos++);
	else if (level == LOG_WARNING)
		sprintf(buf, "%04i WARNING ", logpos++);
	else if (level == LOG_ERROR)
		sprintf(buf, "%04i ERROR   ", logpos++);

	va_start(al, format);
	vsprintf(buf + strlen(buf), format, al);
	va_end(al);
	strcat(buf, "\n");
	if (LOGFUNCTION)
		LOGFUNCTION(level, buf);
	if (LOGFILE)
		fprintf(LOGFILE, "%s", buf);
	free(buf);

	return len_buf;
}
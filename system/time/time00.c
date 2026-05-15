/*
 * Note: We need to define _XOPEN_SOURCE feature test macro
 * to get strptime function
 */
#include <stdlib.h>
#define _XOPEN_SOURCE

#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(void) {
  // seconds since epoch
  time_t t = time(NULL); // seconds since epoch

  printf("The Number of seconds since epoch: %ld\n", t);

  // print date and time in human readable format
  printf("Time in readable format %s", ctime(&t));

  // local time
  struct tm *ltime = localtime(&t);
  printf("Local time is %s", asctime(ltime));

  // gm time
  struct tm *gtime = gmtime(&t);
  printf("The greenwich mean time is %s", asctime(gtime));

  // gmtime in seconds
  time_t st = mktime(gtime);
  printf("The greenwich mean time in seconds since epoch: %ld\n", st);

  // Parsing time
  // Note: strptime is only available if _XOPEN_SOURCE feature test macro is
  // defined
  struct tm ptime;
  char *result = strptime("2025-01-20 15:30:00", "%Y-%m-%d %H:%M:%S", &ptime);
  if (result != NULL) {
    printf("Parsed time : Year %d, Month %d, Day %d, Hour %d, Minute %d, "
           "Seconds %d\n",
           ptime.tm_year + 1900, ptime.tm_mon + 1, ptime.tm_wday, ptime.tm_hour,
           ptime.tm_min, ptime.tm_sec);
  }

  // formatting time
  char mytime[50];
  time_t time3;
  struct tm *tmptime = localtime(&time3);

  strftime(mytime, sizeof(mytime), "%x - %I:%M %p", tmptime);

  printf("Formatted date & time : %s\n", mytime);

  // timezone
  setenv("TZ", "America/Los_Angeles", 1); // Define TZ environment variable
  tzset();                                // Updat
  time_t rawtime;
  struct tm *tinfo = localtime(&rawtime);
  printf("local time: %s", asctime(tinfo));
  printf("timezone: %s -- %f\n", tinfo->tm_zone,
         (double)(tinfo->tm_gmtoff) / 3600);

  // Difference between time in seconds
  time_t time1, time2;
  time(&time1);
  for (int sec = 1; sec <= 3; sec++) {
    sleep(1);
  }
  time(&time2);

  printf("Difference between time is %.2f seconds\n", difftime(time2, time1));

  return 0;
}


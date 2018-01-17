/*==========================================================================
==========================================================================*/

#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <getopt.h>

#define M2 (12.42 * 3600)

#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE 
#define TRUE 1
#endif
#ifndef FALSE 
#define FALSE 0 
#endif

#define BASE_FORMAT "%d %m %Y %H:%M"
#define BASE_FORMAT_DISPLAY "DD MM YYYY HH:MM"

/*==========================================================================
 show_times 
==========================================================================*/
void show_times (time_t time_base, int day, int month, int year, BOOL tfh,
    BOOL diurnal)
  {
  struct tm tm_midnight;
  memset (&tm_midnight, 0, sizeof (struct tm));
  tm_midnight.tm_sec = 0;
  tm_midnight.tm_min = 0;
  tm_midnight.tm_hour = 0;
  tm_midnight.tm_mday = day;
  tm_midnight.tm_mon = month - 1;
  tm_midnight.tm_year = year - 1900;
  tm_midnight.tm_isdst = -1; 
  
  struct tm tm_next_midnight;
  memset (&tm_next_midnight, 0, sizeof (struct tm));
  tm_next_midnight.tm_sec = 59;
  tm_next_midnight.tm_min = 59;
  tm_next_midnight.tm_hour = 23;
  tm_next_midnight.tm_mday = day;
  tm_next_midnight.tm_mon = month - 1;
  tm_next_midnight.tm_year = year - 1900;
  tm_next_midnight.tm_isdst = -1; 

  time_t time_midnight = mktime (&tm_midnight); 
  time_t time_next_midnight = mktime (&tm_next_midnight); 
  
  //printf ("midnight=%s\n", ctime (&time_midnight));
  //printf ("next_midnight=%s\n", ctime (&time_next_midnight));
  //printf ("base=%s\n", ctime (&time_base));

  time_t next = time_base;
  int high = 1;
  while (next < time_next_midnight)
    {
    if (next > time_midnight)
      {
      char buff[30];


      struct tm *tm = localtime (&next);
      if (tfh)
        strftime (buff, sizeof (buff) - 1, "%a %b %d, %H:%M", tm); 
      else
        strftime (buff, sizeof (buff) - 1, "%a %b %d, %I:%M %P", tm); 


      printf ("%s %s\n", high?"High":"Low ", buff);
      high = !high;
      } 
    if (diurnal)
      next += M2;
    else
      next += M2 / 2;
    }
  }

/*==========================================================================
get_rc_file
==========================================================================*/
char *get_rc_file (void)
  {
  char *ret = NULL;
  asprintf (&ret, "%s/.qdtide.rc", getenv ("HOME"));
  return ret;
  }

/*==========================================================================
read_base_time
==========================================================================*/
time_t read_base_time (char **error)
  {
  time_t ret = 0;
  char *rc_file = get_rc_file();
  FILE *f = fopen (rc_file, "r");
  if (f)
    {
    int got_base = 0;
    char buff[256];
    while (fgets (buff, sizeof (buff), f))
      {
      char *p = strchr (buff, '=');
      if (p)
        {
        *p = 0;
        if (strcmp (buff, "base") == 0)
          {
          got_base = 1;
          struct tm tm_base;
          time_t now = time (NULL);
          memcpy (&tm_base, localtime (&now), sizeof (struct tm));
          if (strptime (p + 1, BASE_FORMAT, &tm_base))
            {
            tm_base.tm_sec = 0;
            time_t time_base = mktime (&tm_base); 
            ret = time_base;
            }
          else
            {
            asprintf (error, 
              "Unparseable base time in rc file %s", 
               rc_file);
            }
          }
        }
      }

    if (got_base)
      {
      // Already done
      }
    else
      {
      asprintf (error, 
        "No base tide in rc file %s -- has the program been calibrated?", 
         rc_file);
      }
    fclose (f);
    }
  else
    {
    asprintf (error, 
     "Can't read rc file %s -- has the program been calibrated?", 
     rc_file);
    }

  free (rc_file);
  return ret;
  }


/*==========================================================================
 main
==========================================================================*/
void parse_and_set_base (const char *base, char **error)
  {
  struct tm tm_base;
  if (strptime (base, BASE_FORMAT, &tm_base))
     {
     char *rc_file = get_rc_file();
     FILE *f = fopen (rc_file, "w");
     if (f)
       {
       fprintf (f, "base=%s\n", base);
       fclose (f);
       }
     else
       {
       asprintf (error, 
         "Can't write rc file %s", 
       rc_file);
       }
     free (rc_file);
     }
  else
     {
     *error = strdup ("Base time format should be " BASE_FORMAT_DISPLAY);
     }
  }

/*==========================================================================
 main
==========================================================================*/
int main (int argc, char **argv)
  {
  static BOOL show_version = FALSE;
  static BOOL show_usage = FALSE;
  static BOOL tfh = FALSE;
  static BOOL diurnal = FALSE;
  char *base = NULL;

  static struct option long_options[] = 
   {
     {"version", no_argument, &show_version, 'v'},
     {"help", no_argument, &show_usage, '?'},
     {"diurnal", no_argument, &diurnal, 'd'},
     {"tfh", no_argument, &tfh, 't'},
     {"base", required_argument, NULL, 'b'},
     {0, 0, 0, 0}
   };

  int opt;
  while (1)
   {
   int option_index = 0;
   opt = getopt_long (argc, argv, "?dtvb:",
     long_options, &option_index);

   if (opt == -1) break;

   switch (opt)
     {
     case 0:
        if (strcmp (long_options[option_index].name, "version") == 0)
          show_version = TRUE;
        else if (strcmp (long_options[option_index].name, "help") == 0)
          show_usage = TRUE;
        else if (strcmp (long_options[option_index].name, "tfh") == 0)
          tfh = TRUE;
        else if (strcmp (long_options[option_index].name, "diurnal") == 0)
          diurnal = TRUE;
        else if (strcmp (long_options[option_index].name, "base") == 0)
          base = strdup (optarg); 
        else
          exit (-1);
        break;

     case 'v': show_version = TRUE; break;
     case '?': show_usage = TRUE; break;
     case 'b': base = strdup (optarg); break;
     case 't': tfh = TRUE; break; 
     case 'd': diurnal = TRUE; break; 
     default:  exit(-1);
     }
   }

  if (show_usage)
    {
    printf ("Usage %s [options]\n", argv[0]);
    printf ("  -b, --base {MM DD YYYY HH:MM} set base high tide\n");
    printf ("  -d, --diurnal                 assume two tides a day\n");
    printf ("  -t, --tfh                     twenty-four hour times\n");
    printf ("  -v, --version                 show version information\n");
    printf ("  -?                            show this message\n");
    exit (0);
    }
 
  if (show_version)
    {
    printf (NAME " " VERSION "\n");
    printf ("A quick-and-dirty tide clock\n");
    printf ("Copyright (c)2017 Kevin Boone\n");
    printf ("Distributed according to the terms of the GPL, v3.0\n");
    exit (0);
    }

  int ret = 0;

  BOOL proceed = TRUE;

  if (base)
    {
    char *error = NULL;
    parse_and_set_base (base, &error);
    if (error)
      {
      proceed = FALSE;
      printf ("%s: %s\n", argv[0], error);
      free (error);
      }  
    }

  if (proceed)
    {
    int day = -1;
    int month = -1;
    int year = -1;

    if (optind == argc)
      {
      // No day/month arguments
      }
    else if (optind == argc - 1)
      {
      // just the day 
      day = atoi (argv[optind]);
      }
    else if (optind == argc - 2)
      {
      // just the day 
      day = atoi (argv[optind]);
      month = atoi (argv[optind + 1]);
      }
    else if (optind == argc - 3)
      {
      // just the day 
      day = atoi (argv[optind]);
      month = atoi (argv[optind + 1]);
      year = atoi (argv[optind + 2]);
      }

    time_t now = time (NULL);
    char *error = NULL;
    time_t time_base = read_base_time (&error);
    if (time_base > 0)
      {

      struct tm tm_now;
      memcpy (&tm_now, localtime (&now), sizeof (struct tm));
      
      if (year == -1) year = tm_now.tm_year + 1900;
      if (month == -1) month = tm_now.tm_mon + 1;
      if (day == -1) day = tm_now.tm_mday;

      show_times (time_base, day, month, year, tfh, diurnal);
      }
    else
      {
      printf ("%s: %s\n", argv[0], error);
      free (error);
      }
    }

  if (base) free (base);
 
  return ret;
  }




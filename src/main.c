/*============================================================================
 * libebookinfo
 * main.c
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <getopt.h>
#include <sys/stat.h>
#include <ebookinfo/ebookinfo.h>

int main (int argc, char **argv)
  {
  static BOOL show_version = FALSE;
  static BOOL show_usage = FALSE;
  static BOOL show_comment = FALSE;
  static BOOL html2text = FALSE;

  static struct option long_options[] = 
   {
     {"version", no_argument, &show_version, 'v'},
     {"comment", no_argument, &show_comment, 'c'},
     {"html2text", no_argument, &html2text, 'h'},
     {"help", no_argument, &show_usage, '?'},
     {0, 0, 0, 0}
   };

  int opt;
  while (1)
   {
   int option_index = 0;
   opt = getopt_long (argc, argv, "?vch",
     long_options, &option_index);

   if (opt == -1) break;

   switch (opt)
     {
     case 0:
        if (strcmp (long_options[option_index].name, "version") == 0)
          show_version = TRUE;
        else if (strcmp (long_options[option_index].name, "help") == 0)
          show_usage = TRUE;
        break;

     case 'c': show_comment = TRUE; break;
     case 'v': show_version = TRUE; break;
     case 'h': html2text = TRUE; break;
     case '?': show_usage = TRUE; break;
     default:  exit(-1);
     }
   }

  if (show_usage)
    {
    printf ("Usage %s [options] {files}\n", argv[0]);
    printf ("  -c, --show            show comment/description\n");
    printf ("  -h, --html2text       format with html2text\n");
    printf ("  -v, --version         show version information\n");
    printf ("  -?                    show this message\n");
    exit (0);
    }
 
  if (show_version)
    {
    printf ("ebookinfo " VERSION "\n");
    printf ("Copyright (c)2017 Kevin Boone\n");
    printf ("Distributed according to the terms of the GPL, v3.0\n");
    exit (0);
    }
 
  int i;
  for (i = optind; i < argc; i++)
    {
    const char *filename = argv[i];

    if (argc - optind > 1)
      printf ("file: %s\n", filename);

    char *error = NULL;
    EBook *ebook = ebook_open (filename, &error);
    if (ebook)
      {
      int type = ebook_get_type (ebook);
      switch (type)
        {
        case EBOOK_TYPE_EPUB: 
          printf ("type: EPUB\n");
          break;
        case EBOOK_TYPE_MOBI: 
          printf ("type: MOBI\n");
          break;
        case EBOOK_TYPE_RTF: 
          printf ("type: RTF\n");
          break;
        default: 
          printf ("type: unknown\n");
        } 
      EBookMetadata *metadata = ebook_get_metadata (ebook, &error); 
      if (metadata)
	{
        const char *title = ebookmetadata_get_title (metadata);
        const char *author = ebookmetadata_get_author (metadata);
        const char *genre = ebookmetadata_get_genre (metadata);
        const char *comment = ebookmetadata_get_comment (metadata);
        const char *year = ebookmetadata_get_year (metadata);
        if (title)
	  printf ("title: %s\n", title); 
        if (author)
	  printf ("author: %s\n", author); 
        if (genre)
	  printf ("genre: %s\n", genre); 
        if (year)
	  printf ("year: %s\n", year); 
        if (comment && show_comment)
          {
          if (html2text)
            {
            FILE *f = popen ("html2text", "w");
	    fprintf (f, comment); 
            fclose (f);
            }
          else
            {
            printf (comment);
            printf ("\n");
            }
          }

	ebookmetadata_destroy (metadata);
	}
      else
	{
	fprintf (stderr, "Can't read metadata: %s\n", error);
	free (error);
	}

      ebook_close (ebook);
      }
    else
      {
      fprintf (stderr, "Can't open e-book file %s: %s\n", filename, error);
      free (error);
      }
    }
  
  return 0;
  }


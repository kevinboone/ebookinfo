/*============================================================================
 * libebookinfo
 * rtf.c
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <pcre.h>
#include <ebookinfo/ebook.h>
#include <ebookinfo/constants.h>
#include "epub.h" 


typedef struct _RTF
  {
  char *filename;
  EBookMetadata *cached_metadata;
  } RTF;


static pcre *re_title, *re_author, *re_genre, *re_year, *re_comment;


/*============================================================================
init_re
============================================================================*/
static void init_re (void)
  {
  const char *pcreErrorStr;
  int pcreErrorOffset = 0;

  re_title = pcre_compile ("{\\\\title\\s+(.*?)}", PCRE_EXTENDED, 
    &pcreErrorStr, &pcreErrorOffset, NULL);
  re_author = pcre_compile ("{\\\\author\\s+(.*?)}", PCRE_EXTENDED, 
    &pcreErrorStr, &pcreErrorOffset, NULL);
  re_genre = pcre_compile ("{\\\\subject\\s+(.*?)}", PCRE_EXTENDED, 
    &pcreErrorStr, &pcreErrorOffset, NULL);
  re_year = pcre_compile ("revtim\\\\yr(.*?)\\\\", PCRE_EXTENDED, 
    &pcreErrorStr, &pcreErrorOffset, NULL);
  re_comment = pcre_compile ("{\\\\doccomm\\s+(.*?)}", PCRE_EXTENDED, 
    &pcreErrorStr, &pcreErrorOffset, NULL);
  }

/*============================================================================
init_re
============================================================================*/
static void cleanup_re (void)
  {
  if (re_title)
    pcre_free (re_title);
  if (re_author)
    pcre_free (re_author);
  if (re_genre)
    pcre_free (re_genre);
  if (re_year)
    pcre_free (re_year);
  if (re_comment)
    pcre_free (re_comment);
  }


/*============================================================================
rtf_recognize
============================================================================*/
BOOL rtf_recognize (const char *filename)
  {
  BOOL ret = FALSE;
  int f = open (filename, O_RDONLY);
  if (f)
    {
    unsigned char buff[20];
    int n;
    n = read (f, buff, sizeof (buff));
    if (n == sizeof (buff))
      {
      if (strncmp ((const char *)buff, "{\\rtf", 5) == 0)
        {
        ret = TRUE;
        }
      }
    close (f);
    }
  return ret;
  }


/*============================================================================
rtf_close
============================================================================*/
void rtf_close (EBook *self)
  {
  if (self)
    {
    RTF *rtf = (RTF *) ebook_get_data (self);
    if (rtf)
      {
      if (rtf->filename)
        {
        free (rtf->filename);
        }
      if (rtf->cached_metadata)
        {
        ebookmetadata_destroy (rtf->cached_metadata);
        }
      free (rtf);
      }
    }
  }


/*============================================================================
rtf_open
============================================================================*/
BOOL rtf_open (EBook *self, const char *filename, char **error)
  {
  BOOL ret = TRUE;

  RTF *rtf = malloc (sizeof (RTF));
  memset (rtf, 0, sizeof (RTF));
  rtf->filename = strdup (filename);
  ebook_set_data (self, rtf);
  
  return ret;
  }



/*============================================================================
rtf_get_metadata
============================================================================*/
EBookMetadata *rtf_get_metadata (const EBook *ebook, char **error)
  {
  EBookMetadata *ret = NULL;

  char *title = NULL, *author=NULL, *year=NULL, *genre=NULL, *comment=NULL;

  RTF *rtf = (RTF *) ebook_get_data (ebook);

  if (rtf->cached_metadata) 
    return ebookmetadata_clone (rtf->cached_metadata);

  init_re();

  const char *filename = rtf->filename;
  int f = open (filename, O_RDONLY);
  if (f)
    {
    BOOL ok = TRUE;

    int len = 500000; // If it ain't in the first 50-k, tough
    char *buff = malloc (len);
    memset (buff, 0, len);
    read (f, buff, len);

    int vec[10];

    int count = pcre_exec (re_title, NULL, buff, len,  
       0, 0, vec, 10);         
    if (count == 2)
     {
     int m = vec[3] - vec[2] + 1;
     title = malloc (m);
     strncpy (title, buff + vec[2], vec[3] - vec[2]);
     title[vec[3] - vec[2]]=0;
     }

    count = pcre_exec (re_author, NULL, buff, len,  
       0, 0, vec, 10);         
    if (count == 2)
     {
     int m = vec[3] - vec[2] + 1;
     author = malloc (m);
     strncpy (author, buff + vec[2], vec[3] - vec[2]);
     author[vec[3] - vec[2]]=0;
     }

    count = pcre_exec (re_genre, NULL, buff, len,  
       0, 0, vec, 10);         
    if (count == 2)
     {
     int m = vec[3] - vec[2] + 1;
     genre = malloc (m);
     strncpy (genre, buff + vec[2], vec[3] - vec[2]);
     genre[vec[3] - vec[2]]=0;
     }

    count = pcre_exec (re_year, NULL, buff, len,  
       0, 0, vec, 10);         
    if (count == 2)
     {
     int m = vec[3] - vec[2] + 1;
     year = malloc (m);
     strncpy (year, buff + vec[2], vec[3] - vec[2]);
     year[vec[3] - vec[2]]=0;
     }

    count = pcre_exec (re_comment, NULL, buff, len,  
       0, 0, vec, 10);         
    if (count == 2)
     {
     int m = vec[3] - vec[2] + 1;
     comment = malloc (m);
     strncpy (comment, buff + vec[2], vec[3] - vec[2]);
     comment[vec[3] - vec[2]]=0;
     }

    if (ok)
      {
      ret = ebookmetadata_create (title, 
        author, year, genre, comment); 

      rtf->cached_metadata = ebookmetadata_clone (ret);
      }


    free (buff);
    close (f);
    }

  cleanup_re();
  
  if (title) free (title);
  if (author) free (author);
  if (year) free (year);
  if (genre) free (genre);
  if (comment) free (comment);

  return ret;
  }




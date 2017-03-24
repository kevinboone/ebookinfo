/*============================================================================
 * libebookinfo
 * ebook.c
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <ebookinfo/ebook.h>
#include <ebookinfo/ebookmetadata.h>
#include "mobi.h" 
#include "epub.h" 
#include "rtf.h" 

/*============================================================================
private struct ebook
============================================================================*/
struct _EBook
  {
  int type;
  void *data;
  };

/*============================================================================
ebook_open
============================================================================*/
EBook *ebook_open (const char *filename, char **error)
  {
  EBook *self = NULL;

  if (access (filename, R_OK) == 0)
    {
    if (mobi_recognize (filename))
      {
      self = malloc (sizeof (EBook));
      memset (self, 0, sizeof (EBook));
      self->type = EBOOK_TYPE_MOBI;
      if (mobi_open (self, filename, error))
        {
        }
      else
        {
        mobi_close (self);
        free (self);
        self = NULL;
        }
      } 
    else if (epub_recognize (filename))
      {
      self = malloc (sizeof (EBook));
      memset (self, 0, sizeof (EBook));
      self->type = EBOOK_TYPE_EPUB;
      if (epub_open (self, filename, error))
        {
        }
      else
        {
        epub_close (self);
        free (self);
        self = NULL;
        }
      }
    else if (rtf_recognize (filename))
      {
      self = malloc (sizeof (EBook));
      memset (self, 0, sizeof (EBook));
      self->type = EBOOK_TYPE_RTF;
      if (rtf_open (self, filename, error))
        {
        }
      else
        {
        rtf_close (self);
        free (self);
        self = NULL;
        }
      }
    else
      {
      asprintf (error, "Book format not recognized"); 
      }
   
    }  
  else
    {
    asprintf (error, strerror (errno)); 
    }
  return self;
  }


/*============================================================================
ebook_close
============================================================================*/
void ebook_close (EBook *self)
  {
  if (self)
    {
    switch (self->type)
      {
      case EBOOK_TYPE_EPUB:
        epub_close (self);
        break;
      case EBOOK_TYPE_MOBI:
        mobi_close (self);
        break;
      case EBOOK_TYPE_RTF:
        rtf_close (self);
        break;
      }
    free (self);
    }
  }


/*============================================================================
ebook_get_type
============================================================================*/
int ebook_get_type (const EBook *self)
  {
  return self->type;
  }


/*============================================================================
ebook_get_data
============================================================================*/
void *ebook_get_data (const EBook *self)
  {
  return self->data;
  }


/*============================================================================
ebook_set_data
============================================================================*/
void ebook_set_data (EBook *self, void *data)
  {
  self->data = data;
  }


/*============================================================================
ebook_get_metadata
============================================================================*/
EBookMetadata *ebook_get_metadata (const EBook *self, char **error)
  {
  EBookMetadata *ret = NULL;

  if (self)
    {
    switch (self->type)
      {
      case EBOOK_TYPE_EPUB:
        ret = epub_get_metadata (self, error);
        break;
      case EBOOK_TYPE_MOBI:
        ret = mobi_get_metadata (self, error);
        break;
      case EBOOK_TYPE_RTF:
        ret = rtf_get_metadata (self, error);
        break;
      default:
        asprintf (error, 
           "Internal error: unknown book type %d", self->type);
      }
    }
  else
    {
    asprintf (error, "Internal error: get_metadata called with a null self");
    }
  return ret;
  }





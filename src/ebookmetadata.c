/*============================================================================
 * libebookinfo
 * ebookmetadata.c
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
private struct ebookmetadata
============================================================================*/
struct _EBookMetadata
  {
  char *title;
  char *author;
  char *year;
  char *genre;
  char *comment;
  };


/*============================================================================
create
============================================================================*/
EBookMetadata *ebookmetadata_create (const char *title, const char *author,
                 const char *year, const char *genre, const char *comment)
  {
  EBookMetadata *self = malloc (sizeof (EBookMetadata));
  memset (self, 0, sizeof (EBookMetadata));
  if (title) ebookmetadata_set_title (self, title);
  if (author) ebookmetadata_set_author (self, author);
  if (year) ebookmetadata_set_year (self, year);
  if (genre) ebookmetadata_set_genre (self, genre);
  if (comment) ebookmetadata_set_comment (self, comment);
  return self;
  }


/*============================================================================
clone
============================================================================*/
EBookMetadata *ebookmetadata_clone (const EBookMetadata *self)
  {
  return ebookmetadata_create (
    ebookmetadata_get_title (self),
    ebookmetadata_get_author (self),
    ebookmetadata_get_year (self),
    ebookmetadata_get_genre (self),
    ebookmetadata_get_comment (self));
  }


/*============================================================================
get_author
============================================================================*/
const char *ebookmetadata_get_author (const EBookMetadata *self)
  {
  if (self)
    return self->author;
  else 
    return NULL;
  }


/*============================================================================
set_author
============================================================================*/
void ebookmetadata_set_author (EBookMetadata *self, const char *author)
  {
  if (self->author) free (self->author);
  self->author = strdup (author);
  }


/*============================================================================
get_title
============================================================================*/
const char *ebookmetadata_get_title (const EBookMetadata *self)
  {
  if (self)
    return self->title;
  else 
    return NULL;
  }


/*============================================================================
set_title
============================================================================*/
void ebookmetadata_set_title (EBookMetadata *self, const char *title)
  {
  if (self->title) free (self->title);
  self->title = strdup (title);
  }


/*============================================================================
get_year
============================================================================*/
const char *ebookmetadata_get_year (const EBookMetadata *self)
  {
  if (self)
    return self->year;
  else 
    return NULL;
  }


/*============================================================================
set_year
============================================================================*/
void ebookmetadata_set_year (EBookMetadata *self, const char *year)
  {
  if (self->year) free (self->year);
  self->year = strdup (year);
  }


/*============================================================================
get_genre
============================================================================*/
const char *ebookmetadata_get_genre (const EBookMetadata *self)
  {
  if (self)
    return self->genre;
  else 
    return NULL;
  }


/*============================================================================
set_genre
============================================================================*/
void ebookmetadata_set_genre (EBookMetadata *self, const char *genre)
  {
  if (self->genre) free (self->genre);
  self->genre = strdup (genre);
  }


/*============================================================================
get_comment
============================================================================*/
const char *ebookmetadata_get_comment (const EBookMetadata *self)
  {
  if (self)
    return self->comment;
  else 
    return NULL;
  }


/*============================================================================
set_comment
============================================================================*/
void ebookmetadata_set_comment (EBookMetadata *self, const char *comment)
  {
  if (self->comment) free (self->comment);
  self->comment = strdup (comment);
  }


/*============================================================================
destroy
============================================================================*/
void ebookmetadata_destroy (EBookMetadata *self)
  {
  if (self)
    {
    if (self->title) free (self->title);
    if (self->author) free (self->author);
    if (self->year) free (self->year);
    if (self->genre) free (self->genre);
    if (self->comment) free (self->comment);
    free (self);
    }
  }






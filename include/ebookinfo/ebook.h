/*============================================================================
 * libebookinfo
 * ebook.h
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/

#pragma once

#include <ebookinfo/ebookmetadata.h>

struct _EBook;
typedef struct _EBook EBook;

#define EBOOK_TYPE_EPUB 0
#define EBOOK_TYPE_MOBI 1
#define EBOOK_TYPE_RTF  2

#ifdef __CPLUSPLUS
extern "C" {
#endif

EBook         *ebook_open (const char *filename, char **error);
void          ebook_close (EBook *self);
int           ebook_get_type (const EBook *self);
EBookMetadata *ebook_get_metadata (const EBook *self, char **error);

// Internal use only
void     *ebook_get_data (const EBook *self);
void     ebook_set_data (EBook *self, void *data);
#ifdef __CPLUSPLUS
}
#endif



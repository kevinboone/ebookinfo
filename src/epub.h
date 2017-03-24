/*============================================================================
 * libebookinfo
 * epub.h
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/

#pragma once

#include <ebookinfo/constants.h> 
#include <ebookinfo/ebookmetadata.h> 

#ifdef __CPLUSPLUS
extern "C" {
#endif

BOOL           epub_recognize (const char *filename);
void           epub_close (EBook *self);
BOOL           epub_open (EBook *self, const char *filename, char **error);
EBookMetadata *epub_get_metadata (const EBook *ebook, char **error);

#ifdef __CPLUSPLUS
}
#endif




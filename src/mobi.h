/*============================================================================
 * libebookinfo
 * mobi.h
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/

#pragma once

#include <ebookinfo/constants.h> 
#include <ebookinfo/ebookmetadata.h> 

#ifdef __CPLUSPLUS
extern "C" {
#endif

BOOL           mobi_recognize (const char *filename);
void           mobi_close (EBook *self);
BOOL           mobi_open (EBook *self, const char *filename, char **error);
EBookMetadata *mobi_get_metadata (const EBook *ebook, char **error);

#ifdef __CPLUSPLUS
}
#endif




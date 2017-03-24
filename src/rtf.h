/*============================================================================
 * libebookinfo
 * rtf.h
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/

#pragma once

#include <ebookinfo/constants.h> 
#include <ebookinfo/ebookinfo.h> 

#ifdef __CPLUSPLUS
extern "C" {
#endif

BOOL           rtf_recognize (const char *filename);
BOOL           rtf_open (EBook *self, const char *filename, char **error);
void           rtf_close (EBook *self);
EBookMetadata *rtf_get_metadata (const EBook *ebook, char **error);

#ifdef __CPLUSPLUS
}
#endif




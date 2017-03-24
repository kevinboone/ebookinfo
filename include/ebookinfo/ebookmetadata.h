/*============================================================================
 * libebookinfo
 * ebook.h
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/

#pragma once

struct _EBookMetadata;
typedef struct _EBookMetadata EBookMetadata;

#ifdef __CPLUSPLUS
extern "C" {
#endif

EBookMetadata *ebookmetadata_create (const char *title, const char *author,
                 const char *year, const char *genre, const char *comment);
EBookMetadata *ebookmetadata_clone (const EBookMetadata *self);
void           ebookmetadata_destroy (EBookMetadata *self);

const char    *ebookmetadata_get_title (const EBookMetadata *self);
void           ebookmetadata_set_title (EBookMetadata *self, const char *title);
const char    *ebookmetadata_get_author (const EBookMetadata *self);
void           ebookmetadata_set_author (EBookMetadata *self, const char *author);
const char    *ebookmetadata_get_year (const EBookMetadata *self);
void           ebookmetadata_set_year (EBookMetadata *self, const char *year);
const char    *ebookmetadata_get_genre (const EBookMetadata *self);
void           ebookmetadata_set_genre (EBookMetadata *self, const char *genre);
const char    *ebookmetadata_get_comment (const EBookMetadata *self);
void           ebookmetadata_set_comment (EBookMetadata *self, const char *comment);
 
#ifdef __CPLUSPLUS
}
#endif




/*==========================================================================
kmediascanner
ebistring.h
Copyright (c)2017 Kevin Boone, GPLv3.0
*==========================================================================*/

#pragma once

struct _EBIString;
typedef struct _EBIString EBIString;

#ifdef __cplusplus 
extern "C" {
#endif

EBIString    *ebistring_create_empty (void);
EBIString    *ebistring_create (const char *s);
EBIString    *ebistring_clone (const EBIString *self);
int          ebistring_find (const EBIString *self, const char *search);
void         ebistring_destroy (EBIString *self);
const char   *ebistring_cstr (const EBIString *self);
const char   *ebistring_cstr_safe (const EBIString *self);
void         ebistring_append_printf (EBIString *self, const char *fmt,...);
void         ebistring_append (EBIString *self, const char *s);
void         ebistring_prepend (EBIString *self, const char *s);
int          ebistring_length (const EBIString *self);
EBIString    *ebistring_substitute_all (const EBIString *self, 
                const char *search, const char *replace);
void         ebistring_delete (EBIString *self, const int pos, 
                const int len);
void         ebistring_insert (EBIString *self, const int pos, 
                const char *replace);
BOOL         ebistring_create_from_utf8_file (const char *filename, 
                EBIString **result, char **error);
EBIString    *ebistring_encode_url (const char *s);

#ifdef __cplusplus 
}
#endif





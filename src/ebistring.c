/*==========================================================================
kmediascanner
ebistring.c
Copyright (c)2017 Kevin Boone, GPLv3.0
*==========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <pthread.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <ebookinfo/constants.h>
#include "ebistring.h"

struct _EBIString
  {
  char *str;
  }; 


/*==========================================================================
ebistring_create_empty 
*==========================================================================*/
EBIString *ebistring_create_empty (void)
  {
  return ebistring_create ("");
  }


/*==========================================================================
ebistring_create
*==========================================================================*/
EBIString *ebistring_create (const char *s)
  {
  EBIString *self = malloc (sizeof (EBIString));
  self->str = strdup (s);
  return self;
  }


/*==========================================================================
ebistring_destroy
*==========================================================================*/
void ebistring_destroy (EBIString *self)
  {
  if (self)
    {
    if (self->str) free (self->str);
    }
  free (self);
  }


/*==========================================================================
ebistring_cstr
*==========================================================================*/
const char *ebistring_cstr (const EBIString *self)
  {
  return self->str;
  }


/*==========================================================================
ebistring_cstr_safe
*==========================================================================*/
const char *ebistring_cstr_safe (const EBIString *self)
  {
  if (self)
    {
    if (self->str) 
      return self->str;
    else
      return "";
    }
  else
    return "";
  }


/*==========================================================================
ebistring_append
*==========================================================================*/
void ebistring_append (EBIString *self, const char *s) 
  {
  if (!s) return;
  if (self->str == NULL) self->str = strdup ("");
  int newlen = strlen (self->str) + strlen (s) + 2;
  self->str = realloc (self->str, newlen);
  strcat (self->str, s);
  }


/*==========================================================================
ebistring_prepend
*==========================================================================*/
void ebistring_prepend (EBIString *self, const char *s) 
  {
  if (!s) return;
  if (self->str == NULL) self->str = strdup ("");
  int newlen = strlen (self->str) + strlen (s) + 2;
  char *temp = strdup (self->str); 
  free (self->str);
  self->str = malloc (newlen);
  strcpy (self->str, s);
  strcat (self->str, temp);
  free (temp);
  }


/*==========================================================================
ebistring_append_printf
*==========================================================================*/
void ebistring_append_printf (EBIString *self, const char *fmt,...) 
  {
  if (self->str == NULL) self->str = strdup ("");
  va_list ap;
  va_start (ap, fmt);
  char *s;
  vasprintf (&s, fmt, ap);
  ebistring_append (self, s);
  free (s);
  va_end (ap);
  }


/*==========================================================================
ebistring_length
*==========================================================================*/
int ebistring_length (const EBIString *self)
  {
  if (self == NULL) return 0;
  if (self->str == NULL) return 0;
  return strlen (self->str);
  }


/*==========================================================================
ebistring_clone
*==========================================================================*/
EBIString *ebistring_clone (const EBIString *self)
  {
  if (!self) return NULL;
  if (!self->str) return ebistring_create_empty();
  return ebistring_create (ebistring_cstr (self));
  }


/*==========================================================================
ebistring_find
*==========================================================================*/
int ebistring_find (const EBIString *self, const char *search)
  {
  if (!self) return -1;
  if (!self->str) return -1;
  const char *p = strstr (self->str, search);
  if (p)
    return p - self->str;
  else
    return -1;
  }


/*==========================================================================
ebistring_delete
*==========================================================================*/
void ebistring_delete (EBIString *self, const int pos, const int len)
  {
  char *str = self->str;
  if (pos + len > strlen (str))
    ebistring_delete (self, pos, strlen(str) - len);
  else
    {
    char *buff = malloc (strlen (str) - len + 2);
    strncpy (buff, str, pos); 
    strcpy (buff + pos, str + pos + len);
    free (self->str);
    self->str = buff;
    }
  }


/*==========================================================================
ebistring_insert
*==========================================================================*/
void ebistring_insert (EBIString *self, const int pos, 
    const char *replace)
  {
  char *buff = malloc (strlen (self->str) + strlen (replace) + 2);
  char *str = self->str;
  strncpy (buff, str, pos);
  buff[pos] = 0;
  strcat (buff, replace);
  strcat (buff, str + pos); 
  free (self->str);
  self->str = buff;
  }



/*==========================================================================
ebistring_substitute_all
*==========================================================================*/
EBIString *ebistring_substitute_all (const EBIString *self, 
    const char *search, const char *replace)
  {
  EBIString *working = ebistring_clone (self);
  BOOL cont = TRUE;
  while (cont)
    {
    int i = ebistring_find (working, search);
    if (i >= 0)
      {
      ebistring_delete (working, i, strlen (search));
      ebistring_insert (working, i, replace);
      }
    else
      cont = FALSE;
    }
  return working;
  }


/*==========================================================================
  ebistring_create_from_utf8_file 
*==========================================================================*/
BOOL ebistring_create_from_utf8_file (const char *filename, 
    EBIString **result, char **error)
  {
  EBIString *self = NULL;
  BOOL ok = FALSE; 
  int f = open (filename, O_RDONLY);
  if (f > 0)
    {
    self = malloc (sizeof (EBIString));
    struct stat sb;
    fstat (f, &sb);
    int64_t size = sb.st_size;
    char *buff = malloc (size + 2);
    read (f, buff, size);
    self->str = buff; 
    self->str[size] = 0;
    *result = self;
    ok = TRUE;
    }
  else
    {
    asprintf (error, "Can't open file '%s' for reading: %s", 
      filename, strerror (errno));
    ok = FALSE;
    }

  return ok;
  }


/*==========================================================================
  ebistring_encode_url
*==========================================================================*/
static char to_hex(char code)
  {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
  }


EBIString *ebistring_encode_url (const char *str)
  {
  if (!str) return ebistring_create_empty();;
  const char *pstr = str; 
  char *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr)
    {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_'
      || *pstr == '.' || *pstr == '~')
      *pbuf++ = *pstr;
    else if (*pstr == ' ')
      *pbuf++ = '+';
    else
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4),
         *pbuf++ = to_hex(*pstr & 15);
    pstr++;
    }
  *pbuf = '\0';
  EBIString *result = ebistring_create (buf);
  free (buf);
  return (result);
  }





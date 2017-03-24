/*============================================================================
 * libebookinfo
 * epub.c
 * Copyright (c)2017 Kevin Boone. GPLv3.0
============================================================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pcre.h>
#include <ebookinfo/ebook.h>
#include <ebookinfo/constants.h>
#include "sxmlc.h" 
#include "epub.h" 
#include "ebistring.h" 

typedef struct _EPUB
  {
  char *tempdir;

  } EPUB;

static pcre *re_entity;

/*============================================================================
epub_recognize
============================================================================*/
BOOL epub_recognize (const char *filename)
  {
  BOOL ret = FALSE;
  int f = open (filename, O_RDONLY);
  if (f >= 0)
    {
    char buff[2];
    if (read (f, buff, 2) == 2)
      {
      if (buff[0] == 'P' && buff[1] == 'K')
        ret = TRUE;
      }
    close (f);
    }

  return ret;
  }

/*============================================================================
epub_close
============================================================================*/
void epub_close (EBook *self)
  {
  if (self)
    {
    EPUB *epub = (EPUB *) ebook_get_data (self);
    if (epub)
      {
      if (epub->tempdir)
        {
        char *cmd;
        asprintf (&cmd, "rm -rf \"%s\"", epub->tempdir);
        system (cmd);
        free (cmd);
        }
      free (epub->tempdir);
      free (epub);
      }
    }
  }


/*============================================================================
epub_open
============================================================================*/
BOOL epub_open (EBook *self, const char *filename, char **error)
  {
  BOOL ret = FALSE;

  EPUB *epub = malloc (sizeof (EPUB));
  memset (epub, 0, sizeof (EPUB));

  const char *tempbase = getenv("TMP");
  if (!tempbase) tempbase = "/tmp";
  char *tempdir;
  asprintf (&tempdir, "%s/ebookinfo%d", tempbase, getpid());

  char *cmd;
  asprintf (&cmd, "mkdir -p \"%s\"", tempdir);
  system (cmd);
  free (cmd);

  epub->tempdir = tempdir;
 
  asprintf (&cmd, "unzip -o -qq \"%s\" -d \"%s\"", filename, tempdir);
  system (cmd);
  free (cmd);

  asprintf (&cmd, "chmod -R 744 \"%s\"", tempdir);
  system (cmd);
  free (cmd);

  ebook_set_data (self, epub);
  ret = TRUE;
  
  return ret;
  }


/*===========================================================================
unescape
We might need more than this, but <, >, and & are the only ones I've
seen in the description field in practice
===========================================================================*/
const char *decode_entity (const char *e)
  {
  if (strcasecmp (e, "lt") == 0) return "<";
  if (strcasecmp (e, "gt") == 0) return ">";
  if (strcasecmp (e, "amp") == 0) return "&";
  return e;
  }

/*===========================================================================
unescape
===========================================================================*/
char *unescape (const char *_input)
  {
  char *input = strdup (_input);
  BOOL done = FALSE;
  EBIString *s = ebistring_create_empty ();

  while (!done)
    {
    int vec[10];
    int count = pcre_exec (re_entity, NULL, input, strlen (input),  
       0, 0, vec, 10);         

    if (count != 1) done = TRUE;
    if (!done)
      {
      char *temp = strdup (input);
      temp[vec[0]] = 0;
      ebistring_append (s, temp);
      free (temp);
      char *subs = strdup (input+ vec[0]+1);
      subs [vec[1] - vec[0] - 2] = 0;
      ebistring_append (s, decode_entity (subs));
      free (subs);
      temp = strdup (input + vec[1]);
      free (temp);
      memmove (input, input + vec[1], strlen (input) - vec[1] + 1);
      }
    }

  ebistring_append (s, input);

  char *t = strdup (ebistring_cstr(s));
  free (input);
  ebistring_destroy (s);
  return t;
  }


/*===========================================================================
parse_content
===========================================================================*/
static BOOL parse_content (const char *filename, 
    char **title, char **creator, char **year, 
    char **genre, char **comment, char **error)
  {
  BOOL ok = TRUE;

  // Note that all hrefs in this file are relative to this file's
  // location, not to the location of the manifest.
  char *filename2 = strdup (filename);
  
  FILE *f = fopen (filename, "r");
  if (f)
    {
    XMLDoc *xmldoc = malloc (sizeof (XMLDoc));
    XMLDoc_init (xmldoc);
    if (XMLDoc_parse_file_DOM (filename, xmldoc))
      {
      XMLNode *root = XMLDoc_root (xmldoc);
      int i, l = root->n_children;
      for (i = 0; i < l; i++)
        {
        XMLNode *r1 = root->children[i];
        if (strcasecmp (r1->tag, "metadata") == 0)
          {
          int i, l = r1->n_children;
          for (i = 0; i < l; i++)
            {
            XMLNode *m = r1->children[i];
            if (strcasestr (m->tag, "creator"))
              {
              if (m->text && creator) *creator = strdup (m->text);
              }
            else if (strcasestr (m->tag, "description"))
              {
              if (m->text && comment) *comment = unescape (m->text);
              }
            else if (strcasestr (m->tag, "title"))
              {
              if (m->text && title) *title = strdup (m->text);
              }
            else if (m->text && strcasestr (m->tag, "date"))
              {
              char *y = strdup (m->text);
              char *p = strchr (y, '-');
              if (p)
                {
                *p = 0; 
                if (year) *year = strdup (y);
                }
              free (y);
              }
            else if (strcasestr (m->tag, "subject"))
              {
              if (m->text && genre) 
               {
               if (*genre)
                 {
                 *genre = realloc (*genre, strlen (*genre) + strlen (m->text) + 5);
                 strcat (*genre, ",");
                 strcat (*genre, m->text);
                 }
               else
                 *genre = strdup (m->text);
               }
              }
            }
          }
        else if (strcasecmp (r1->tag, "manifest") == 0)
          {
          }
        }
      }
    else
      {
      asprintf (error, "parsing EPUB: Can't parse content file %s\n", filename);
      }
    fclose (f);
    XMLDoc_free (xmldoc);
    free (xmldoc);
    }
  else
    {
    asprintf (error, "parseing EPUB: Can't open content file %s\n", filename);
    }
  free (filename2);
  return ok;
  }

 

/*===========================================================================
_get_epub_metadata
===========================================================================*/
static BOOL _epub_get_metadata (const char *tempdir, 
     char **title, char **creator, char **year, char **genre, char **comment,
     char **error) 
  {
  BOOL ok = TRUE;

  char *opf;
  asprintf (&opf, "%s/META-INF/container.xml", tempdir);
  FILE *f = fopen (opf, "r");
  if (f)
    {
    XMLDoc *xmldoc = malloc (sizeof (XMLDoc));
    XMLDoc_init (xmldoc);
    if (XMLDoc_parse_file_DOM (opf, xmldoc))
      {
      XMLNode *root = XMLDoc_root (xmldoc);
      int i, l = root->n_children;
      for (i = 0; i < l; i++)
        {
        XMLNode *r1 = root->children[i];
        if (strcmp (r1->tag, "rootfiles") == 0)
          {
          XMLNode *rootfiles = r1;
          int i, l = rootfiles->n_children;
          for (i = 0; i < l; i++)
            {
            XMLNode *r1 = rootfiles->children[i];
            if (strcmp (r1->tag, "rootfile") == 0)
              {
              int k, nattrs = r1->n_attributes;
              for (k = 0; k < nattrs; k++)
                {
                char *name = r1->attributes[k].name;
                char *value = r1->attributes[k].value;
                if (strcmp (name, "full-path") == 0)
                  {
                  char *c;
                  asprintf (&c, "%s/%s", tempdir, value);
                  ok = parse_content (c, 
                    title, creator, year, genre, comment, error);
                  free (c);
                  }
                }
              }
            }
          }
        }
      }
    else
      {
      asprintf (error, "parsing EPUB: can't parse container.xml\n");
      ok = FALSE;
      }

    XMLDoc_free (xmldoc);
    free (xmldoc);
    fclose (f);
    }
  else
    {
    asprintf (error, "parsing EPUB: can't open file %s\n", opf);
    ok = FALSE;
    }

  free (opf);

  return ok;
  }


/*============================================================================
init_re
============================================================================*/
static void init_re (void)
  {
  const char *pcreErrorStr;
  int pcreErrorOffset = 0;

  re_entity = pcre_compile ("&.*?;", PCRE_EXTENDED, 
    &pcreErrorStr, &pcreErrorOffset, NULL);
  }

/*============================================================================
init_re
============================================================================*/
static void cleanup_re (void)
  {
  if (re_entity)
    pcre_free (re_entity);
  }

/*============================================================================
epub_get_metadata
============================================================================*/
EBookMetadata *epub_get_metadata (const EBook *ebook, char **error)
  {
  EBookMetadata *ret = NULL;

  init_re();

  EPUB *epub = (EPUB *) ebook_get_data (ebook);
  const char *tempdir = epub->tempdir;

  char *title = NULL, *author=NULL, *year=NULL, *genre=NULL, *comment=NULL;

  BOOL ok = _epub_get_metadata (tempdir, 
     &title, &author, &year, &genre, &comment,
     error);

  if (ok)
    {
    ret  = ebookmetadata_create (title, 
      author, year, genre, comment); 
  
    if (title) free (title);
    if (author) free (author);
    if (year) free (year);
    if (genre) free (genre);
    if (comment) free (comment);
    }

  cleanup_re();
  
  return ret;
  }





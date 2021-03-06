char version[12] = "2018-01-17";

/*  Copyright (C) 2014-18 R. D. Tennent School of Computing,
 *  Queen's University, rdt@cs.queensu.ca
 *
 *  This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General
 *  Public License along with this program; if not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street,
 *  Fifth Floor, Boston, MA 02110-1301, USA.
 */

/*  autosp - preprocessor to generate note-spacing commands for MusiXTeX scores */

/*  Usage: autosp [-v | --version | -h | --help]
 *         autosp [-d | --dotted] [-l | --log] infile[.aspc] [outfile[.tex]]
 *
 *  Options 
 *    --dotted (-d) suppresses extra spacing for dotted beam notes.
 *    --log (-l) generate a log file
 *
 *
 *
 *  Key functions:
 *
 *    process_command, which processes commands
 *      \instrumentnumber
 *      \nbinstrument
 *      \setstaffs
 *      \startpiece
 *      \TransformNotes
 *      \def\vnotes
 *      \def\atnextbar
 *      \anotes etc.
 *      \bar
 *      \endpiece etc.
 *
 *    analyze_notes, which determines pointers notes[i] and 
 *      characters terminator[i] for each staff; '$' is used
 *      as the terminator for the last staff.
 *
 *    generate_notes, which repeatedly uses spacing_note (i) to
 *      determine spacings[i] of the next spacing note in 
 *      each staff; the current spacing is then the smallest
 *      of these.  If necessary, the current notes command
 *      is terminated by terminate_notes() and a new notes 
 *      command initiated by initialize_notes(). The current global 
 *      skip and note commands for each staff are then copied to 
 *      outstrings[i] using output_notes (i).  When necessary, 
 *      vspacing[i] is set to generate subsequent skips.
 *      Appoggiaturas and xtuplets are treated separately.
 *
 *    spacing_note (i), which searches for the next spacing note
 *      in the i'th staff, unless a "virtual note" (skip) is
 *      needed.
 *  
 *    output_notes (i), which copies a global_skip string and 
 *    outstrings[i] to output, using filter_output (i) to discard
 *    commas and global and large skips, and insert collective-coding 
 *    prefixes and closing braces.  
 *
 *    terminate_notes, which copies outstrings[i] to output,
 *    in the order determined by TransformNotes2.
 *
 *  Spacing values are expressed as sums of binary multiples of 
 *    SMALL_NOTE (by default, 1/256th); for example, half-note
 *    spacing = SP(2) = SMALL_NOTE/2. Special small or large 
 *    values are used for appoggiaturas and bar-rests.
 *      
 */

# include "utils.h"

#ifdef HAVE_CONFIG_H   /* for TeXLive */
#include <config.h>
#endif

# ifdef KPATHSEA
# include <kpathsea/getopt.h>
# else
# include <getopt.h>
# endif

# ifdef HAVE_STDBOOL_H
# include <stdbool.h>
# else
# ifndef HAVE__BOOL
# define _Bool signed char
# endif
# define bool    _Bool
# define true    1
# define false   0
# endif

# define PRIVATE static

# define SHORT_LEN 256
# define MAX_STAFFS 9

# define SMALL_NOTE 512
# define SP(note) (SMALL_NOTE/note)  
/* note = { 256 | 128 | 64 | 32 | 16 | 8 | 4 | 2 | 1 }  */
# define MAX_SPACING 2*SP(1)
# define APPOGG_SPACING SP(64)+SP(256)  /* not a legitimate spacing */
# define notespp "\\vnotes2.95\\elemskip"
# define Notespp "\\vnotes3.95\\elemskip"
# define NOtespp "\\vnotes4.95\\elemskip"
# define NOTespp "\\vnotes6.95\\elemskip"
# define NOTEsp  "\\vnotes9.52\\elemskip"
# define APPOGG_NOTES "\\vnotes1.45\\elemskip"

PRIVATE bool debug = false;
PRIVATE char infilename[SHORT_LEN];
PRIVATE char *infilename_n = infilename;
PRIVATE char outfilename[SHORT_LEN];
PRIVATE char *outfilename_n = outfilename;
PRIVATE char logfilename[SHORT_LEN];
PRIVATE char *logfilename_n = logfilename;


PRIVATE FILE *infile, *outfile, *logfile;

PRIVATE char line[LINE_LEN];            /* line of input                      */

PRIVATE int ninstr = 1;                 /* number of instruments              */
PRIVATE int staffs[MAX_STAFFS] = {0,1}; /* number of staffs for ith instrument*/
PRIVATE int nstaffs = 1;                /* number of staffs                   */
PRIVATE int nastaffs = 1;               /* number of active staffs;           */
PRIVATE char terminator[MAX_STAFFS];    /* one of '&' "|', '$'                */
PRIVATE bool active[MAX_STAFFS];        /* is staff i active?                 */

PRIVATE char *notes[MAX_STAFFS];        /* note segment for ith staff         */
PRIVATE char *current[MAX_STAFFS];

PRIVATE int spacings[MAX_STAFFS];       /* spacing for ith staff              */
PRIVATE int spacing = MAX_SPACING;      /* spacing for current notes          */
PRIVATE int old_spacing = MAX_SPACING;

PRIVATE int vspacing[MAX_STAFFS];       /* virtual-note (skip) spacing        */
PRIVATE bool vspacing_active[MAX_STAFFS]; /* virtual-note spacing active?       */
                      /* used to preclude unnecessary pre-accidental skips    */

PRIVATE bool nonvirtual_notes;          
                           /* used to preclude output of *only* virtual notes */

PRIVATE int cspacing[MAX_STAFFS];       /* nominal collective-note spacing    */
PRIVATE char collective[MAX_STAFFS][SHORT_LEN];
                                    /* prefixes for collective note sequences */
PRIVATE bool first_collective[MAX_STAFFS];

PRIVATE int restbars = 0;
PRIVATE char deferred_bar[SHORT_LEN];   /* deferred \bar (or \endpiece etc.)  */
PRIVATE bool Changeclefs = false; /* output \Changeclefs after \def\atnextbar */

PRIVATE int beaming[MAX_STAFFS];        /* spacing for beamed notes           */
PRIVATE int new_beaming;
PRIVATE int semiauto_beam_notes[MAX_STAFFS]; /* semi-automatic beam notes     */

/* save-restore state for a staff; used in process_xtuplet */
PRIVATE int beamingi;  
PRIVATE char *currenti;
PRIVATE int cspacingi;
PRIVATE int vspacingi;
PRIVATE char collectivei[SHORT_LEN];
PRIVATE bool first_collectivei;

PRIVATE int xtuplet[MAX_STAFFS];        /* x for xtuplet in staff i          */

PRIVATE bool appoggiatura;
PRIVATE bool dottedbeamnotes = false;   /* dotted beam notes ignored?        */

PRIVATE bool bar_rest[MAX_STAFFS];

PRIVATE char outstrings[MAX_STAFFS][LINE_LEN];  
                                     /* accumulate commands to be output    */
PRIVATE char *n_outstrings[MAX_STAFFS];

PRIVATE int global_skip;  
   /* = 1, 2, 3, or 4 for (non-standard) commands \QQsk \HQsk \TQsk \Qsk    */
   /* = 5 for five commas and double-flat accidental spacing                */
   /* = 6 for six commas                                                    */
PRIVATE char global_skip_str[7][16]   
= { "","\\qqsk","\\hqsk","\\tqsk","\\qsk","\\qsk\\qqsk","\\qsk\\hqsk" }; 
                                   /* all based on \elemskip, not \noteskip */

PRIVATE char TransformNotes2[SHORT_LEN] = {'\0'};      
                                      /* 2nd argument of \TransformNotes    */
PRIVATE bool TransformNotesDefined = false;

PRIVATE void
usage ()
{
  printf ("Usage: autosp [-v | --version | -h | --help]\n");
  printf ("       autosp [-d | --dotted] [-l | --log] infile[.aspc] [outfile[.tex]]\n");
  printf ("       autosp [-d | --dotted] [-l | --log] infile.tex outfile[.tex]\n");
}


PRIVATE
char *ps(int spacing)
/* convert spacing value to note value(s)  */
{ char *s = malloc(SHORT_LEN);
  char *s_n;
  if (s == NULL) error ("Not enough memory");
  s_n = s; 
  *s = '\0';
  if (spacing == MAX_SPACING) 
    sprintf(s, "MAX_SPACING");
  else if (spacing == 0)
    sprintf(s, "0");
  else 
  { int n = 1;
    while (true)
    { if (spacing >= SMALL_NOTE/n)
      { s_n = s_n + sprintf (s_n, "SP(%d)", n);
        spacing = spacing - SMALL_NOTE/n;
        if (spacing > 0) 
          s_n = s_n + sprintf (s_n, "+");
        else break;
      }
      n = 2*n;
    }
  }
  return s;
}

PRIVATE void 
note_segment (char *s)
{ char *t;
  t = strpbrk (s, "|&$");
  if (t == NULL) t = s + strlen (s);
  while (s < t)
  { fputc (*s, logfile); s++; }
  fputc ('\n', logfile);
}

PRIVATE void
status (int i)
{ 
  if (active[i])
  { fprintf (logfile, "notes[%d]=", i); note_segment (notes[i]); 
    fprintf (logfile, "current[%d]=", i); note_segment (current[i]); 
    fprintf (logfile, "spacings[%d]=%s\n", i, ps(spacings[i]));
    fprintf (logfile, "vspacing[%d]=%s\n", i, ps(vspacing[i]));
    fprintf (logfile, "outstrings[%d]=%s\n", i, outstrings[i]);
    if (xtuplet[i] > 1)
      fprintf (logfile, "xtuplet[%d]=%d\n", i, xtuplet[i]);
      
  }
}

PRIVATE void
status_spacing (void)
{
  fprintf (logfile, "spacing=%s\n", ps(spacing));
  fprintf (logfile, "old_spacing=%s\n", ps(old_spacing));
}

PRIVATE void
status_all (void)
{ int i;
  for (i=1; i <= nstaffs; i++)
    if (active[i]) status (i);
  status_spacing ();
  fprintf (logfile, "nastaffs=%d\n", nastaffs);
  if (dottedbeamnotes) 
    fprintf (logfile, "dottedbeamnotes == true\n");
}

PRIVATE void
status_collective (int i)
{ 
  if (active[i])
  { fprintf (logfile, "cspacing[%d]=%s\n", i, ps(cspacing[i]));
    fprintf (logfile, "collective[%d]=%s\n", i, collective[i]);
    fprintf (logfile, "first_collective[%d]=%d\n", i, first_collective[i]);
  }
}

PRIVATE void
status_beam (int i)
{ 
  if (active[i])
  { fprintf (logfile, "beaming[%d]=%s\n", i, ps(beaming[i])); }
}

PRIVATE
void analyze_notes (char **ln) 
/* divide material from *ln to \en into notes[i] segments
   and initialize terminator[i] etc.                          */
{
  int i; char *s; char *t;  
  int newlines = 0;
  s = *ln+1;  /* skip "/"  */
  while (isalpha(*s)) {s++;}  /* skip rest of the initial command  */
  while (true)
  { /* look for \en */
    t = strstr(s, "\\en");
    if (t != NULL) break;
    /* replace EOL by a blank and append another line of input */
    { char new_line[LINE_LEN];  
      char *nl;
      if (fgets (new_line, LINE_LEN, infile) == NULL)
        error ("Unexpected EOF.");
      nl = new_line;
      while (*nl == ' ') nl++;  /* avoid spaces */
      t = strpbrk (s, "\n%");
      if (t == NULL) error ("Missing EOL.");
      if (*t == '\n') 
      { *t = ' '; t++;}
      *t = '\0';
      if (append (line, &t, nl, LINE_LEN) >= LINE_LEN)
        error ("Line too long.");
      newlines++;
    }
  }
  /* t -> "\en"  */
  if ( prefix("\\enotes", t) ) *ln = t+7;
  else *ln = t+3;
  *t = '$'; /* replace '\' by '$' (a distinctive terminating character) */
  for (i=1; i <= nstaffs; i++)      
  { char *tt;
    while (*s == ' ') s++;  /* skip spaces  */
    if (active[i])
    { notes[i] = s; 
      current[i] = s; 
    }
    tt = strpbrk (s, "|&$");
    if (tt == NULL) error ("can't parse note-spacing command.");
    s = tt; 
    terminator[i] = *s;
    if (*s != '$') s++;
  }
  lineno = lineno + newlines;

  /* initialize: */
  spacing = MAX_SPACING;
  old_spacing = MAX_SPACING;
  appoggiatura = false;
  global_skip = 0;
  for (i=1; i <= nstaffs; i++)
  {
    if (active[i])
    { n_outstrings[i] = outstrings[i];
      *n_outstrings[i] = '\0';
      vspacing[i] = 0;  
      vspacing_active[i] = false;  
      collective[i][0] = '\0'; 
      cspacing[i] = MAX_SPACING; 
      first_collective[i] = false;
      xtuplet[i] = 1;
    }
  }
  if (debug) 
  { fprintf (logfile, "\nAfter analyze_notes:\n");
    status_all ();
  }
}

PRIVATE
void checkc (char *s, char c)
{ if (*s != c) 
  {
    printf ("Error on line %d: Expected %c but found %d:\n%s\n", lineno, c, *s, line);
    exit (EXIT_FAILURE);
  }
}

PRIVATE
void checkn (char *s)
{ if (strpbrk (s, "0123456789") != s) 
  {
    printf ("Error on line %d: Expected digit but found %c:\n%s\n", lineno, *s, line);
    exit (EXIT_FAILURE);
  }
}

PRIVATE
void filter_output (int i)
{ /* discard \sk \bsk \Qsk \TQsk \HQsk \QQsk and \Cpause */
  char *s = notes[i];
  while (s < current[i])
  { char *t;
    t = strpbrk (s+1, "\\&|$");
    if (t == NULL || t > current[i]) t = current[i];
    if (!prefix ("\\sk", s)
     && !prefix ("\\bsk", s)
     && !prefix ("\\Cpause", s) 
     && !prefix ("\\Qsk", s) 
     && !prefix ("\\HQsk", s) 
     && !prefix ("\\TQsk", s) 
     && !prefix ("\\QQsk", s) 
     && !prefix ("\\Triolet", s)
     && !prefix ("\\Xtuplet", s) )
    {
      while (s < t) 
      { while (*s == ',') s++; /* global skips */
        *n_outstrings[i] = *s; 
        n_outstrings[i]++; s++; 
      }
      *(n_outstrings[i]) = '\0';
    }
    s = t;
  }
  notes[i] = s;
}

PRIVATE
void output_notes (int i)
/* append from notes[i] up to current[i] to outstrings[i] */
{ if (debug) 
  { fprintf (logfile, "\nEntering output_notes:\n");
    status (i);
  }
  if (debug)
  {
    fprintf (logfile, "\nAppending %s for global_skip=%i\n", global_skip_str[global_skip], global_skip);
  }
  append (outstrings[i], &(n_outstrings[i]), global_skip_str[global_skip], LINE_LEN);
  if (vspacing[i] > 0)
  { 
    if (nonvirtual_notes) 
      append (outstrings[i], &(n_outstrings[i]), "\\sk", LINE_LEN); 
    vspacing[i] = vspacing[i] - spacing;
  }
  else if (collective[i][0])
  { 
    if (debug) 
    { fprintf (logfile, "\nBefore outputting collective note:\n");
      status (i);
      status_collective (i);
    }
    if (!first_collective[i])
      append (outstrings[i], &(n_outstrings[i]), collective[i], LINE_LEN);
    first_collective[i] = false;
    filter_output (i);
    append (outstrings[i], &(n_outstrings[i]), "}", LINE_LEN);
    if (*notes[i] == '}')
    { collective[i][0] = '\0';
      cspacing[i] = 0;
      notes[i]++;
    }
    if (*notes[i] == '}')  /* close of {\tinynotesize..{}}? */
    { append (outstrings[i], &(n_outstrings[i]), "}", LINE_LEN);
      notes[i]++;
    }
    if (debug)
    { fprintf (logfile, "\nAfter outputting collective note:\n");
      status (i);
      status_collective (i);
    }
  }
  else
    filter_output (i);
  if (debug)
  { fprintf (logfile, "\nAfter output_notes:\n");
    status (i);
    status_spacing ();
  }
}

PRIVATE
void pseudo_output_notes (int i)
{ /* process notes like output_notes but without outputting  */
  if (vspacing[i]  > 0) 
    vspacing[i] = vspacing[i] - spacing;
  else if (collective[i][0])
  {
    first_collective[i] = false;
    if (*current[i] == '}')
    { collective[i][0] = '\0';
      current[i]++;
    }
  }
}

PRIVATE
void update_global_skip (int n)
/* global skips may be used in more than one staff */
{  
  if (n>6) 
  { warning ("Too many commas."); n = 6; }
  if (global_skip < n) global_skip = n; 
}

PRIVATE
int collective_note (int i)
{ char *s = current[i];
  int spacing = cspacing[i];
  if (debug)
  { fprintf (logfile, "\nEntering collective_note:\n");
    status (i);
    status_collective (i);
  }
  while (true) /* search for alphabetic or numeric note (or asterisk) */
  { int n = 0;
    while (*s == ',') 
    { n++; s++; } /* global skips */
    update_global_skip (n);
      /* commas will be discarded by filter_output (i) */
    if (*s == '.' && new_beaming == 0 && !dottedbeamnotes) 
      spacing = spacing * 1.50; 
    else if ( (*s == '^' || *s == '_' || *s == '=' || *s == '>') 
              && !vspacing_active[i]  /* is additional spacing needed? */
            )
    /* leave space for normal accidentals */
    { update_global_skip (3) ;
      if (debug)
      { fprintf (logfile, "\nLeave space for accidental\n");
        status (i);
        status_beam (i);
        status_collective (i);
      }
    }
    else if ( (*s == '<') /* double-flat */ 
              && !vspacing_active[i]  /* is additional spacing needed? */
            )
    { update_global_skip (5);
      if (debug)
      { fprintf (logfile, "\nLeave space for double-flat\n");
        status (i);
        status_beam (i);
        status_collective (i);
      }
    }
    else if (isalnum (*s) || *s == '*')
    {  
      s++; 
      while (*s == '\'' || *s == '`' || *s == '!') 
      /* transposition characters */
        s++;
      current[i] = s;
      if (debug)
      { fprintf (logfile, "\nAfter collective_note:\n");
        status (i);
        status_collective (i);
      }
      vspacing_active[i] = false;
      return spacing; 
    }
    s++;
  }
}

PRIVATE
void beam_initiation (char *s, int i)
{ 
  if ( prefix ("\\ibbbbbb", s)
    || prefix ("\\Ibbbbbb", s)
    || prefix ("\\nbbbbbb", s) )
    beaming[i] = SP(256); 
  else if ( prefix ("\\ibbbbb", s)
    || prefix ("\\Ibbbbb", s)
    || prefix ("\\nbbbbb", s) )
    beaming[i] = SP(128); 
  else if ( prefix ("\\ibbbb", s)
    || prefix ("\\Ibbbb", s)
    || prefix ("\\nbbbb", s) )
    beaming[i] = SP(64); 
  else if ( prefix ("\\ibbb", s) 
    || prefix ("\\Ibbb", s)
    || prefix ("\\nbbb", s) ) 
    beaming[i] = SP(32); 
  else if ( prefix ("\\ibb", s) 
    || prefix ("\\Ibb", s)
    || prefix ("\\nbb", s) )
    beaming[i] = SP(16); 
  else if ( prefix ("\\ib", s) 
    || prefix ("\\Ib", s) )
    beaming[i] = SP(8); 
  if (debug)
  { fprintf (logfile, "\nAfter beam_initiation:\n");
    status (i);
    status_beam (i);
  }
}

PRIVATE
void beam_termination (char *s, int i)
{
  if (debug)
  { fprintf (logfile, "\nEntering beam_termination:\n");
    status (i);
    status_beam (i);
  }
  if ( prefix ("\\tbbbbbb", s) )
  { 
    new_beaming = SP(128);
    if (beaming[i] > SP(256))
      beaming[i] = SP(256);
  }
  else if ( prefix ("\\tbbbbb", s) )
  { 
    new_beaming = SP(64);
    if (beaming[i] > SP(128))
      beaming[i] = SP(128);
  }
  else if ( prefix ("\\tbbbb", s) )
  { 
    new_beaming = SP(32);
    if (beaming[i] > SP(64))
      beaming[i] = SP(64);
  }
  else if ( prefix ("\\tbbb", s) )
  { 
    new_beaming = SP(16);
    if (beaming[i] > SP(32))
      beaming[i] = SP(32);
  }
  else if ( prefix ("\\tbb", s) ) 
  { 
    new_beaming = SP(8);
    if (beaming[i] > SP(16))
      beaming[i] = SP(16);
  }
  else if ( prefix ("\\tb", s) )
  {
    new_beaming = 0; 
  }
  if (debug)
  { fprintf (logfile, "\nAfter beam_termination:\n");
    status (i);
    status_beam (i);
  }
}

PRIVATE
char *skip_balanced_text (char *s)
{
  char *t;
  do
  {
    t = strpbrk (s, "{}");
    if (t == NULL) error ("Expected '}'");
    if (*t == '{') t = skip_balanced_text (t+1);
  }
  while (*t != '}');
  return t+1;
}

PRIVATE 
char *skip_arg (char *s)
{ 
  while (*s == ' ') s++;
  if (*s == '{')
  { s++;
    skip_balanced_text (s);
  }
  else  
    s++;
  return s;
}

PRIVATE
int spacing_note (int i)
/*  Search from current[i] for the next spacing note in the ith staff.  
 *  Returns the spacing value and updates current[i].
 */
{ 
  int spacing = MAX_SPACING;
  char *s, *t;
  bool tinynotesize = false;
  bool doubledotted = false;
  bool dotted = false;
  new_beaming = 0;
  if (!active[i]) return spacing;
  if (vspacing[i] > 0) 
  { /* virtual note (i.e., a skip) */
    return vspacing[i]; 
  } 
  if (collective[i][0])
  { 
    nonvirtual_notes = true;
    return collective_note (i); 
  }
  if (semiauto_beam_notes[i] > 0)
  { if (debug) 
      fprintf (logfile, "\nsemiauto_beam_notes reduced\n");
    semiauto_beam_notes[i]--;
    current[i]++;
    if (debug) status (i);
    nonvirtual_notes = true;
    return beaming[i];
  }
  s = strpbrk (current[i], "\\&|$");
  if (*s != '\\') 
  { return spacing; }
  if (debug)
  { fprintf (logfile, "\nIn spacing_note:\n");
    status (i);
  }
  while (true)
  { 
    if ( prefix ("\\wh", s)
      || prefix ("\\breve", s) 
      || prefix ("\\pause", s) 
      || prefix ("\\wq", s) )
    { spacing = SP(1); break; }
    
    if ( prefix ("\\ha", s)
      || (prefix ("\\hl", s)  && !prefix ( "\\hloff", s) )
         
      || prefix ("\\hu", s)
      || prefix ("\\hp", s)
      || prefix ("\\hpause", s) )
    { spacing = SP(2); break;}

    if ( prefix ("\\qa", s)
      || prefix ("\\ql", s)
      || prefix ("\\qu", s)
      || prefix ("\\qp", s) )
    { spacing = SP(4); break; }

    if ( ( prefix ("\\ca", s) 
        || prefix ("\\cl", s)
        || prefix ("\\cu", s) )
      && !prefix ("\\caesura", s ) )
    { spacing = SP(8); break; }

    if ( prefix ("\\ds", s ) 
       && !prefix ("\\dsh", s) ) 
    { spacing = SP(8); break; }

    if ( prefix ("\\cccc", s) ) 
    { spacing = SP(64); break; }

    if ( prefix ("\\ccc", s) ) 
    { spacing = SP(32); break; }

    if ( prefix ("\\cc", s) 
       && !prefix ("\\ccn", s) 
       && !prefix ("\\cchar", s) ) 
    { spacing = SP(16); break; }

    if ( prefix ("\\qs", s)
       && !prefix ("\\qsk", s) 
       && !prefix ("\\qspace", s) )
    { spacing = SP(16); break; }

    if ( prefix ("\\hs", s) 
       && !prefix ("\\hsk", s)  
       && !prefix ("\\hsp", s) ) 
    { spacing = SP(32); break;}

    if ( prefix ("\\qqs", s )
       && !prefix ("\\qqsk", s) )
    { spacing = SP(64); break; }

    if ( prefix ("\\qb", s))  /* beam note */
    { spacing = beaming[i]; 
      if (new_beaming != 0) /* set by preceding \tb... */
      { beaming[i] = new_beaming; 
        new_beaming = 0; 
      }
      if (debug)
      { fprintf (logfile, "\nBeam note:\n");
        status (i);
        status_beam (i);
      }
      break; 
    }
    
    if ( prefix ("\\Cpause", s) )
    { /* bar-centered rest */
      bar_rest[i] = true;
      spacing = MAX_SPACING;
      break;
    }

    if (prefix("\\tqqq", s) ||
        prefix("\\nqqq", s) )
    { if (beaming[i] > SP(32)) 
        beaming[i] = SP(32);
      spacing = beaming[i];
      beaming[i] = SP(16);
      new_beaming = 0;
      if (debug)
      { fprintf (logfile, "\nAfter beam completion:\n");
        status (i);
        status_beam (i);
      }
      break;
    }

    if (prefix("\\tqq", s) ||
        prefix("\\nqq", s) )
    { if (beaming[i] > SP(16)) 
        beaming[i] = SP(16);
      spacing = beaming[i];
      beaming[i] = SP(8);
      new_beaming = 0;
      if (debug)
      { fprintf (logfile, "\nAfter beam completion:\n");
        status (i);
        status_beam (i);
      }
      break;
    }
    if ( prefix("\\tq", s) 
      && !prefix("\\tqsk", s) )
    { spacing = beaming[i];
      new_beaming = 0;
      if (debug)
      { fprintf (logfile, "\nAfter beam completion:\n");
        status (i); 
        status_beam (i);
      }
      break; 
    }


/*  non-spacing commands:  */

    if ( prefix ("\\ib", s) 
      || prefix ("\\Ib", s)
      || prefix ("\\nb", s) )
      beam_initiation (s, i);

    else if ( prefix("\\tb", s) )
      beam_termination (s, i);

    else if ( prefix("\\ztq", s) )
    /* non-spacing beam termination */
      new_beaming = 0; 

    else if ( prefix("\\xtuplet", s) ||
              prefix("\\xxtuplet", s)||
              prefix("\\Xtuplet", s) )
    { char *t = s+1;
      while (!isdigit(*t)) t++;
      xtuplet[i] = atoi(t);
    }
    else 
    if ( prefix("\\triolet", s)
      || prefix("\\Triolet", s)
      || prefix("\\uptrio", s)
      || prefix("\\downtrio", s)
      || prefix("\\uptuplet", s)
      || prefix("\\downtuplet", s) )
    { 
      xtuplet[i] = 3;
    }

    else if ( prefix("\\zchar", s) 
       || prefix("\\lchar", s)
       || prefix("\\cchar", s) 
       || prefix("\\zcn", s)
       || prefix("\\lcn", s)
       || prefix("\\ccn", s) )
    { /* need to skip two arguments, possibly with embedded commands */
      char *t;
      t  = strpbrk (s+1, " {");
      if (t == NULL) error ("Argument expected");
      t = skip_arg (t);
      t = skip_arg (t);
    }

    else if ( prefix("\\tinynotesize", s) )
    { 
       tinynotesize = true;
       appoggiatura = true;
    }

    else if (prefix("\\ppt", s) 
       || prefix("\\pppt", s) )
    { 
      doubledotted = true;  /* triple-dotted spaced as double-dotted */
    }
    else if (prefix("\\pt", s) && !prefix("\\ptr", s)) 
    {
      dotted = true;
    }
    else if ( prefix ("\\Dqbb", s) )
    { semiauto_beam_notes[i] = 1;
      beaming[i] = SP(16);
      current[i] = current[i] + 8;
      nonvirtual_notes = true;
      return beaming[i];
    }
    else if ( prefix ("\\Dqb", s) )
    { if (debug) fprintf (logfile, "\nFound \\Dqb\n");
      semiauto_beam_notes[i] = 1;
      beaming[i] = SP(8);
      current[i] = current[i] + 7;
      nonvirtual_notes = true;
      if (debug) status (i);
      return beaming[i];
    }
    else if ( prefix ("\\Tqbb", s) )
    { semiauto_beam_notes[i] = 2;
      beaming[i] = SP(16);
      current[i] = current[i] + 8;
      nonvirtual_notes = true;
      return beaming[i];
    }
    else if ( prefix ("\\Tqb", s) )
    { semiauto_beam_notes[i] = 2;
      beaming[i] = SP(8);
      current[i] = current[i] + 7;
      nonvirtual_notes = true;
      return beaming[i];
    }
    else if ( prefix ("\\Qqbb", s) )
    { semiauto_beam_notes[i] = 3;
      beaming[i] = SP(16);
      current[i] = current[i] + 8;
      nonvirtual_notes = true;
      return beaming[i];
    }
    else if ( prefix ("\\Qqb", s) )
    { semiauto_beam_notes[i] = 3;
      beaming[i] = SP(8);
      current[i] = current[i] + 7;
      nonvirtual_notes = true;
      return beaming[i];
    }
    else if (prefix ("\\rlap", s) )
    /*  skip the argument, as it must be regarded as non-spacing  */
    { char *t;
      t  = strpbrk (s+1, " {\\");
      if (t == NULL) error ("Argument expected");
      if (*t == '\\') 
        s = t+1;
      else 
        s = skip_arg(t); 
    }
    else if (prefix ("\\Hsk", s) )
      update_global_skip (5); 
    else if (prefix ("\\Qsk", s) )
      update_global_skip (4); 
    else if (prefix ("\\TQsk", s) )
      update_global_skip (3); 
    else if (prefix ("\\HQsk", s) )
      update_global_skip (2); 
    else if (prefix ("\\QQsk", s) )
      update_global_skip (1);

    /* Command is non-spacing.         */
    /* Skip ahead to the next command. */
    s = strpbrk (s+1, "\\&|$");

    if ( *s != '\\')
    { current[i] = s; 
      if (debug)
      { fprintf (logfile, "\nAfter spacing_note:\n");
        status (i);
      }
      return spacing; 
    }
  } /* end of while (true) loop  */

  if (spacing < MAX_SPACING) 
    nonvirtual_notes = true;
  if (prefix ("\\whpp", s)
   || prefix ("\\hupp", s)
   || prefix ("\\hlpp", s)
   || prefix ("\\happ", s)
   || prefix ("\\hspp", s)
   || prefix ("\\hppp", s)
   || prefix ("\\hpausepp", s)
   || prefix ("\\qupp", s)
   || prefix ("\\qlpp", s)
   || prefix ("\\qapp", s)
   || prefix ("\\qspp", s)
   || prefix ("\\qppp", s)
   || prefix ("\\qqspp", s)
   || prefix ("\\cupp", s)
   || prefix ("\\clpp", s)
   || prefix ("\\ccupp", s)
   || prefix ("\\cclpp", s)
   || prefix ("\\cccupp", s)
   || prefix ("\\ccclpp", s)
   || prefix ("\\qbpp", s) 
   || prefix ("\\dspp", s) 
   || doubledotted  )
  { spacing *= 1.75; doubledotted = false;}
  else 
  if (prefix ("\\whp", s)
   || prefix ("\\hup", s)
   || prefix ("\\hlp", s)
   || prefix ("\\hap", s)
   || prefix ("\\hsp", s)
   || prefix ("\\hpp", s)
   || prefix ("\\hpausep", s)
   || prefix ("\\qup", s)
   || prefix ("\\qlp", s)
   || prefix ("\\qap", s)
   || prefix ("\\qsp", s)
   || prefix ("\\qqsp", s)
   || prefix ("\\qpp", s)
   || prefix ("\\cup", s)
   || prefix ("\\clp", s)
   || prefix ("\\ccup", s)
   || prefix ("\\cclp", s)
   || prefix ("\\cccup", s)
   || prefix ("\\ccclp", s)
   || prefix ("\\qbp", s) 
   || prefix ("\\dsp", s) 
    || dotted  ) 
  { spacing *= 1.5; dotted = false; }

  t = strpbrk (s+1, "{\\&|$"); /* collective coding?  */
  if (*t == '{')  /*  {...}  */
  { /* Save prefix in collective[i].
     * It will be output for every note in the collective */
    char *ss = s; 
    char *tt = collective[i];
    while (ss <= t)
    { *tt = *ss; tt++, ss++; }
    *tt = '\0';
    current[i] = t+1;
    if (tinynotesize) spacing = APPOGG_SPACING;
    cspacing[i] = spacing;
    first_collective[i] = true;
    return collective_note (i);
  } 
  if (tinynotesize) spacing = APPOGG_SPACING; 
  current[i] = strpbrk (s+1, "\\&|$");
  if (debug)
  { fprintf (logfile, "\nAfter spacing_note:\n");
    status (i);
  }
  vspacing_active[i] = false;
  return spacing;
}

PRIVATE void
output_rests (void)
{ /* outputs a multi-bar rest and the deferred_bar command */
  int i;
  fprintf ( outfile, "\\NOTes\\sk\\en%%\n" );
  fprintf (outfile, "\\def\\atnextbar{\\znotes");
  if (restbars == 1)
     fprintf ( outfile, "\\centerpause" );
  else  
     fprintf ( outfile, "\\centerHpause{%d}", restbars );
  fprintf (outfile, "\\en}%%\n");
  if (Changeclefs) /* \Changeclefs has to be output after \def\atnextbar...  */
  {
    fprintf ( outfile, "\\Changeclefs%%\n");
    Changeclefs = false;
  }
  if (restbars > 1) 
    fprintf ( outfile, "\\advance\\barno%d%%\n", restbars-1 );
  restbars = 0; 
  for (i=1; i <= nstaffs; i++) bar_rest[i] = false;
  fprintf ( outfile, "%s%%\n", deferred_bar); 
  deferred_bar[0] = '\0';
}

void initialize_notes ()
{ int i;
  if (debug)
  { fprintf (logfile, "\nEntering initialize_notes\n");
    status_all ();
  }
  if ( nastaffs == 1 && spacing != MAX_SPACING && restbars > 0) 
    output_rests ();

  if (spacing == MAX_SPACING)
    fprintf (outfile, "\\znotes");
  else if (spacing == SP(1)+SP(2) || spacing == SP(1)+SP(2)+SP(4))
    fprintf (outfile, NOTEsp); 
  else if (spacing == SP(1))   
    fprintf (outfile, "\\NOTEs"); 
  else if (spacing == SP(2)+SP(4)+SP(8))
    fprintf (outfile, "%s", NOTespp);
  else if (spacing == SP(2)+SP(4) )
    fprintf (outfile, "\\NOTesp"); 
  else if (spacing == SP(2))
    fprintf (outfile, "\\NOTes"); 
  else if (spacing == SP(4)+SP(8) )
    fprintf (outfile, "\\NOtesp"); 
  else if (spacing == SP(4)+SP(8)+SP(16) )
    fprintf (outfile, "%s", NOtespp);
  else if (spacing == SP(4))
    fprintf (outfile, "\\NOtes"); 
  else if (spacing == SP(8)+SP(16) )
    fprintf (outfile, "\\Notesp"); 
  else if (spacing == SP(8)+SP(16)+SP(32))
    fprintf (outfile, "%s", Notespp);
  else if (spacing == SP(8))
    fprintf (outfile, "\\Notes"); 
  else if (spacing == SP(16)+SP(32)+SP(64))
    fprintf (outfile, notespp);
  else if (spacing == SP(16)+SP(32) ) 
    fprintf (outfile, "\\notesp"); 
  else if (spacing == SP(16) || spacing == SP(32)+SP(64) || spacing == SP(32)+SP(64)+SP(128))
    fprintf (outfile, "\\notes"); 
  else if (spacing == SP(32) || spacing == SP(64)+SP(128) || spacing == SP(64)+SP(128)+SP(256))
    fprintf (outfile, "\\nnotes");  
  else if (spacing == SP(64) )
    fprintf (outfile, "\\nnnotes"); 
  else if (spacing == APPOGG_SPACING)
    fprintf (outfile, "%s", APPOGG_NOTES); 
  else 
  { printf ("Error on line %d: spacing %s not recognized.\n", lineno, ps(spacing));
    exit (EXIT_FAILURE);
  }
  if (debug) 
  {  fprintf (logfile, "\noutputting \\Notes command for spacing=%s.\n", ps(spacing));
     status_all();
  }
  for (i=1; i <= nstaffs; i++) 
  { if (active[i])
    { n_outstrings[i] = outstrings[i];
    *n_outstrings[i] = '\0';
    }
  }
}

PRIVATE
void terminate_notes ()
{ int i;
  char *s, *t;
  if (debug)
  { fprintf (logfile, "\nEntering terminate_notes:\n");
    status_all ();
  }
  t = TransformNotes2;
  while (true)
  { s = strchr (t, '#');
    if (s == NULL) 
      break;
    while (t < s)  /* output any initial \transpose etc. */
    { putc (*t, outfile); t++; }
    t++; /* skip # */
    i = atoi (t) -1; t++;
    if (spacing == MAX_SPACING)
    { /* output any commands left in notes[i] */
      filter_output (i);
    }
    fprintf (outfile, "%s", outstrings[i]);
    if (debug) fprintf (logfile, "\noutputting %s from outstrings[%d].\n", outstrings[i], i);
    outstrings[i][0] = '\0';
    n_outstrings[i] = outstrings[i];
    if (spacing < MAX_SPACING && spacing > 2 * old_spacing)
    { /* add extra space *before* much longer notes */
      fprintf (outfile, "\\hqsk");
      if (debug) fprintf (logfile, "\nExtra half-notehead space before longer notes.\n");
    }
    if (*t != '\0') 
    { putc (*t, outfile); t++; }  /* terminator */
  }
  if (spacing == MAX_SPACING && old_spacing < SP(8) )
  { /* add extra space before \en */
    fprintf (outfile, "\\hqsk");
    if (debug) fprintf (logfile, "\nExtra half-notehead space before \\en.\n");
  }
  fprintf (outfile, "\\en"); 
  if (debug)
  { fprintf (logfile, "\noutputting \\en\n"); 
    fprintf (logfile, "\nAfter terminate_notes:\n");
    status_all ();
  }
}

PRIVATE
void process_appogg (void)
{ int i;
  if (debug)
  { fprintf (logfile, "\nEntering process_appogg:\n");
    status_all();
  }

  if (old_spacing < MAX_SPACING) 
  { char *s, *t;
    if (debug) fprintf (logfile, "Terminate current notes command:\n");
    t = TransformNotes2;
    while (true)
    { s = strchr (t, '#');
      if (s == NULL) break;
      while (t < s)  /* output any initial \transpose etc. */
      { putc (*t, outfile); t++; }
      t++; /* skip # */
      i = atoi (t) - 1; t++;
      fprintf (outfile, "%s", outstrings[i]);
      if (debug) fprintf (logfile, "\noutputting %s from outstrings[%d].\n", outstrings[i], i);
      outstrings[i][0] = '\0';
      n_outstrings[i] = outstrings[i];
      if (*t != '\0') 
      { putc (*t, outfile); t++; }  /* terminator */
    }
    fprintf (outfile, "\\en");
    if (debug)
    { fprintf (logfile, "\\en");
      fprintf (logfile, "\nAfter termination of notes command:\n");
      status_all ();
    }
  }

  if (debug) fprintf (logfile, "\nCreate a new notes command for the appoggiatura:\n");
  spacing = APPOGG_SPACING;
  initialize_notes ();
  spacing = MAX_SPACING;
  for (i=1; i <= nstaffs; i++)
    if (active[i])
    { 
      if (spacings[i] == APPOGG_SPACING )
      {
        output_notes (i); 
        spacings[i] = spacing_note (i);
      }
      if (spacings[i] < spacing)
        spacing = spacings[i];
    }
  appoggiatura = false;
  nonvirtual_notes = true;
  old_spacing = APPOGG_SPACING;
  if (debug)
  { fprintf (logfile, "\nAfter process_appogg:\n");
    status_all ();
  }
  return;
}

PRIVATE
void save_state (int i)
/* used in process_xtuplet */
{
  beamingi = beaming[i];
  currenti = current[i];
  cspacingi = cspacing[i];
  vspacingi = vspacing[i];
  first_collectivei = first_collective[i];
  collectivei[0] = '\0';
  append (collectivei, NULL, collective[i], SHORT_LEN);
}

PRIVATE
void restore_state (int i)
/* used in process_xtuplet */
{
  beaming[i] = beamingi;
  current[i] = currenti;
  cspacing[i] = cspacingi;
  vspacing[i] = vspacingi;
  first_collective[i] = first_collectivei;
  collective[i][0] = '\0';
  append (collective[i], NULL, collectivei, SHORT_LEN);
}


PRIVATE
void process_xtuplet (void)
{ 
  int i, xi=0;
  int xspacing = MAX_SPACING;         /* xtuplet total spacing              */
  int normalized_xspacing;        /* (xspacing / xtuplet) * (xtuplet - 1);  */
  int xsp;
  double multnoteskip;

  if (debug)
  { fprintf (logfile, "\nEntering process_xtuplet:\n");
    for (i=1; i <= nstaffs; i++)
      fprintf (logfile, "i=%d xtuplet[i]=%d\n", i, xtuplet[i]);
  }

  if (old_spacing < MAX_SPACING) 
  { char *s, *t;
    if (debug) fprintf (logfile, "Terminate current notes command:\n");
    t = TransformNotes2;
    while (true)
    { s = strchr (t, '#');
      if (s == NULL) break;
      while (t < s)  /* output any initial \transpose etc. */
      { putc (*t, outfile); t++; }
      t++; /* skip # */
      i = atoi (t) - 1; t++;
      fprintf (outfile, "%s", outstrings[i]);
      if (debug) fprintf (logfile, "\noutputting %s from outstrings[%d].\n", outstrings[i], i);
      outstrings[i][0] = '\0';
      n_outstrings[i] = outstrings[i];
      if (*t != '\0') 
      { putc (*t, outfile); t++; }  /* terminator */
    }
    fprintf (outfile, "\\en");
    if (debug)
    { fprintf (logfile, "\\en");
      fprintf (logfile, "\nAfter termination of notes command:\n");
      status_all ();
    }
  }

  if (debug) 
  { fprintf (logfile, "\nDetermine xtuplet duration:\n");
    fflush (logfile);
  }
  for (i=1; i <= nstaffs; i++)
    if (xtuplet[i] > 1)
    {
      xi = i;
      save_state (xi);
      pseudo_output_notes (xi);
      xspacing = spacings[xi];
      while (xspacing % xtuplet[xi] != 0)
      { 
        xspacing +=  spacing_note (xi);
        pseudo_output_notes (xi);
        if (xspacing >= MAX_SPACING) 
          error ("Can't determine xtuplet duration.");
      }
      restore_state (xi);
      break;
    }
  if (debug) fprintf (logfile, "\nxspacing=%s\n", ps(xspacing));
  if (debug) fprintf (logfile, "xi=%d  xtuplet[xi]=%d\n", xi, xtuplet[xi]);
  normalized_xspacing = (xspacing / xtuplet[xi]) * (xtuplet[xi] - 1); 
  if (debug) 
  { fprintf (logfile, "normalized_xspacing=%s\n", ps(normalized_xspacing));
  }
  spacing = xspacing / xtuplet[xi];
  if (debug) 
  { 
    fprintf (logfile, "\nDetermine minimal spacing over all active staffs:\n");
  }
  for (i=1; i <= nstaffs; i++)
    if (active[i] && xtuplet[i] == 1)
    { save_state (i);
      pseudo_output_notes (i);
      xsp = spacings[i]; 
      if (spacings[i] < spacing) 
        spacing = spacings[i];
      while (xsp < normalized_xspacing) 
      { int spi = spacing_note (i);
        pseudo_output_notes (i);
        xsp += spi;
        if (spi < spacing)
        { spacing = spi;
          if (debug) fprintf (logfile, "for i=%d  spi=%s\n", i, ps(spi));
        }
      }
      restore_state (i);
    }
  if (debug) fprintf (logfile, "\nxspacing=%s\n", ps(spacing));

  if (debug) fprintf (logfile, "\nCreate a new notes command for the xtuplet:\n");
  initialize_notes ();


  if (debug) fprintf (logfile, "\nProcess non-xtuplet staffs:\n");
  for (i=1; i <= nstaffs; i++)
    vspacing[i] = 0;
  xsp = 0;
  while (true)
  {
    for (i=1; i <= nstaffs; i++)
      if (active[i] && xtuplet[i] == 1)
        output_notes (i);
    xsp += spacing;
    for (i=1; i <= nstaffs; i++)
    {
      /* virtual notes needed?  */
      if (active[i] && xtuplet[i] == 1 && spacings[i] != 0 && spacings[i] != spacing && vspacing[i] == 0)
      {
        vspacing[i] = spacings[i] - spacing;
      } 
    }
    if (xsp >= normalized_xspacing) break;
    global_skip = 0;
    for (i=1; i <= nstaffs; i++)
      if (active[i] && xtuplet[i] == 1) 
        spacings[i] = spacing_note(i);
  }
  if (debug)
  { fprintf (logfile, "\nAfter processing non-xtuplet staffs:\n");
    status_all ();
  }
  if (debug)
  {
    fprintf (logfile, "Generate \\multnoteskip factors.\n");
    fprintf (logfile, "xi=%d xtuplet[xi]=%d\n", xi, xtuplet[xi]);
  }
  multnoteskip = (double)  (xtuplet[xi] - 1) / xtuplet[xi]; 
  for (i=1; i <= nstaffs; i++)
    if (xtuplet[i] > 1)
    {
      n_outstrings[i] += sprintf (n_outstrings[i], "\\multnoteskip{%5.3f}", multnoteskip); 
      if (debug) 
      { fprintf (logfile, "\noutstrings[%d]=", i);
        note_segment (outstrings[i]); 
      }
    }
  if (debug) fprintf (logfile, "\nRe-process xtuplet staffs:\n");
  for (i=1; i <= nstaffs; i++)
    if (xtuplet[i] > 1)
    {
      xsp = 0;
      while (true)
      { output_notes (i);
        xsp += spacing;
        if (spacings[i] != spacing && vspacing[i] == 0)
          vspacing[i] = spacings[i] - spacing;
        if (xsp >= xspacing) break;
        global_skip = 0;
        spacings[i] = spacing_note (i);
      }
    }

  /* Restore normal \noteskip in the xtuplet staffs. */
  if (debug)
  {
    fprintf (logfile, "Restore \\multnoteskip factors.\n");
    fprintf (logfile, "xi=%d xtuplet[xi]=%d\n", xi, xtuplet[xi]);
  }
  multnoteskip = (double)  xtuplet[xi] / (xtuplet[xi]-1);
  for (i=1; i <= nstaffs; i++)
    if (xtuplet[i] > 1)
    {
      n_outstrings[i] += sprintf (n_outstrings[i], "\\multnoteskip{%5.3f}", multnoteskip); 
      xtuplet[i] = 1;
    }
  nonvirtual_notes = true;
  if (debug)
  { fprintf (logfile, "\nAfter process_xtuplet:\n");
    status_all ();
  }
  return;
}

PRIVATE
void generate_notes ()
{ int i;
  bool xtuplet_flag;
  while (true)
  { old_spacing = spacing;
    spacing = MAX_SPACING;
    global_skip = 0;
    nonvirtual_notes = false;
    if (debug)
    { fprintf (logfile, "\nIn generate_notes:\n");
      status_all ();
    }
    for (i=1; i <= nstaffs; i++) 
      if (active[i])
      {
        spacings[i] = spacing_note (i);
        if (spacings[i] < spacing)
          spacing = spacings[i];
      }
    if (appoggiatura)
    {
      process_appogg ();
    }
    xtuplet_flag = false;
    for (i=1; i <= nstaffs; i++)
      if (xtuplet[i] > 1) xtuplet_flag = true;
    if (xtuplet_flag)
    {
      process_xtuplet ();
      continue;
    }
    if (spacing != old_spacing || spacing == MAX_SPACING)  
    { if (old_spacing < MAX_SPACING) 
        terminate_notes ();
      if (spacing == MAX_SPACING || nonvirtual_notes == false) 
      {
        if (debug)
        { fprintf (logfile, "\nAfter generate_notes:\n");
          status_all ();
        }
        return;
      }
      if (old_spacing < MAX_SPACING) putc ('\n', outfile);

      initialize_notes ();
    }

    for (i=1; i <= nstaffs; i++)  /* append current notes to outstrings */
      if (active[i]) 
        output_notes (i);
    for (i=1; i <= nstaffs; i++)
    {
      /* virtual notes needed?  */
      if (active[i] && spacings[i] != MAX_SPACING && spacings[i] != spacing && vspacing[i] == 0 )
      {
        vspacing[i] = spacings[i];
        vspacing_active[i] = true;
        vspacing[i] = vspacing[i] - spacing;
        if (debug)
        { fprintf (logfile, "\nAfter vspacing initialization:\n");
          status (i);
        }
      } 
    }
  }
}


PRIVATE
void process_command (char **ln)
{ char *s, *t;
  if (debug)
  { fprintf (logfile, "\nProcessing command:%s\n", *ln);
    fflush (logfile);
  }
  if ( prefix("\\instrumentnumber", *ln) )
  { 
    s = strpbrk (*ln, "123456789");
    if ( s == NULL ) error ("\\instrumentnumber command unreadable.");
    ninstr = atoi (s);
    while (*ln <= s) { putc (**ln, outfile); (*ln)++;}
  }

  if ( prefix("\\def\\nbinstrument", *ln) )
  { 
    s = strpbrk (*ln, "123456789");
    if ( s == NULL ) error ("\\def\\nbinstrument command unreadable.");
    ninstr = atoi (s);
    while (*ln <= s) { putc (**ln, outfile); (*ln)++;}
  }

  else if ( prefix("\\setstaffs", *ln) )
  { /* for now, just record in staffs[i]  */
    int n, p;
    s = strpbrk (*ln, "123456789");
    if ( s == NULL ) error ("\\setstaffs command unreadable.");
    n = (int)(*s) - (int)('0'); /* instrument number  */
    s = strpbrk (s+1, "123456789");
    if ( s == NULL ) error ("\\setstaffs command unreadable.");
    p = (int)(*s) - (int)('0'); /* number of staffs */
    staffs[n] = p;
    while (*ln <= s) { putc (**ln, outfile); (*ln)++;}
  }

  else if ( prefix("\\startpiece", *ln) )
  { 
    if (!TransformNotesDefined) /* create default TransformNotes2:  */
    { int i, j;
      t = TransformNotes2;
      nstaffs = 1;
      sprintf (t, "#%1i", nstaffs+1); t = t+2; 
      for (j=2; j <= staffs[1]; j++)
      {  nstaffs++; sprintf (t, "|#%1i", nstaffs+1); t = t+3; 
         active[nstaffs] = true;
      }
      for (i=2; i <= ninstr; i++) 
      { nstaffs++; sprintf (t, "&#%1i", nstaffs+1); t = t+3; 
        for (j=2; j <= staffs[i]; j++)
        {  nstaffs++; sprintf (t, "|#%1i", nstaffs+1); t = t+3; 
           active[nstaffs] = true;
        }
      }   
      nastaffs = nstaffs;
      if (debug)
        fprintf (logfile, "default TransformNotes2=%s\n", TransformNotes2);
    }
    if (nstaffs == 1) fprintf (outfile, "\\nostartrule\n");
    if (debug)
    {
      int j;
      fprintf (logfile, "ninstr=%d nstaffs=%d nastaffs=%d\n", ninstr, nstaffs, nastaffs);
      for (j=1; j <= nstaffs; j++)
        fprintf (logfile, "active[%d]=%d\n", j, active[j]);
    }
    fprintf (outfile, "\\startpiece");
    t = strpbrk (*ln+1, "\\%\n");
    *ln = t;
  }

  else if ( prefix("\\startextract", *ln) )
  { 
    if (!TransformNotesDefined) /* create default TransformNotes2:  */
    { int i, j;
      t = TransformNotes2;
      nstaffs = 1;
      sprintf (t, "#%1i", nstaffs+1); t = t+2; 
      for (j=2; j <= staffs[1]; j++)
      {  nstaffs++; sprintf (t, "|#%1i", nstaffs+1); t = t+3; 
         active[nstaffs] = true;
      }
      for (i=2; i <= ninstr; i++) 
      { nstaffs++; sprintf (t, "&#%1i", nstaffs+1); t = t+3; 
        for (j=2; j <= staffs[i]; j++)
        {  nstaffs++; sprintf (t, "|#%1i", nstaffs+1); t = t+3; 
           active[nstaffs] = true;
        }
      }   
      nastaffs = nstaffs;
      if (debug)
        fprintf (logfile, "default TransformNotes2=%s\n", TransformNotes2);
    }
    if (nstaffs == 1) fprintf (outfile, "\\nostartrule\n");
    fprintf (outfile, "\\startextract");
    t = strpbrk (*ln+1, "\\%\n");
    *ln = t;
  }

  else if ( prefix("\\TransformNotes", *ln) ) 
  { /* determine ninstr, nstaffs, nastaffs, staffs[i], and active[i] */
    int i;
    s = *ln + 16;  /* first parameter  */
    ninstr = 0;
    nstaffs = 0;
    while (true)
    { ninstr++; nstaffs++;
      staffs[ninstr] = 1;
      checkc (s, '#'); s++;
      checkn (s); s++;
      while (*s == '|')
      { staffs[ninstr]++; nstaffs++;
        s++;
        checkc (s, '#'); s++;
        checkn (s); s++; 
      }
      if (*s != '&') break;
      s++;
    }
    checkc (s, '}'); s++;
    s = strchr (s, '{');
    if (s == NULL) error ("Can't parse \\TransformNotes");
    s++;
    /*  determine TransformNotes2:  */
    t = TransformNotes2;
    do { *t = *s; t++; s++; }
    while (*s != '}');
    *t = '\0';  /* terminate TransformNotes2 */
    TransformNotesDefined = true;
    if (debug)
      fprintf (logfile, "defined TransformNotes2=%s\n", TransformNotes2);
    
    /* determine active staffs:  */
    for (i=1; i <= nstaffs; i++) active[i] = false;
    nastaffs = 0;
    t = strpbrk (TransformNotes2, "#}"); /* may have \transpose etc. before # */
    if (t == NULL) error ("Can't parse second argument of \\TransformNotes");
    while (*t == '#')
    { if (sscanf (t, "#%d", &i) != 1) 
        error ("sscanf for argument number fails");
      active[i-1] = true;  /* parameters start at 2 */
      nastaffs++;
      t = strpbrk (t+1, "#}"); 
      if (t == NULL) break;
    }
    if (debug)
    {
      int j;
      fprintf (logfile, "ninstr=%d nstaffs=%d nastaffs=%d\n", ninstr, nstaffs, nastaffs);
      for (j=1; j <= nstaffs; j++)
        fprintf (logfile, "active[%d]=%d\n", j, active[j]);
    }

    /* output \TransformNotes...  as a comment:  */
    putc ('%', outfile);  
    while (*ln <= s) 
    { putc (**ln, outfile); (*ln)++; }
  }

  else if (prefix("\\def\\atnextbar{\\znotes", *ln))
  { /*  whole-bar or multi-bar rest? */
    int i;
    bool centerpause = false;
    *ln = *ln + 15;  /*  skip "\def\atnextbar{"  */
    analyze_notes(ln);
    for (i=1; i <= nstaffs; i++)
    { 
      if (active[i]) 
      { char *t; 
        char *s; 
        t = strpbrk (notes[i], "&|$");
        if (t == NULL) t = notes[i] + strlen (notes[i]);
        s = strstr (notes[i], "\\centerpause");
        if (s != NULL && s < t && nastaffs == 1)
        {
          bar_rest[i] = true;
          centerpause = true;
        }
        break;
      }
    }
    if (!centerpause) /* generate transformed \znotes ... \en */
    { 
      fprintf ( outfile, "\\def\\atnextbar{\\znotes");
      t = TransformNotes2;
      while (true)
      {
        /*  output TransformNotes2 prefix (\transpose etc.):  */
        s = strchr (t, '#');
        if (s == NULL) break;
        while (t < s)  
        { putc (*t, outfile); t++; }
        t++;  /* skip '#' */
        /* output notes: */
        i = atoi (t) - 1; t++;  
        s = notes[i];
        while (*s != '&' && *s != '|' && *s != '$')
        { putc (*s, outfile); s++; }
        if (*t != '\0') 
        { putc (*t, outfile); t++; } /* output terminator */
      }
      fprintf (outfile, "\\en}%%\n");
    }
    t = strchr (*ln, '}');
    if (t == NULL) error ("Can't find }.");
    *ln = t+1;
    t = strpbrk (*ln, "%\\");
    if (t == NULL ) t = *ln + strlen(*ln);
    *ln = t;
  }

  else if ( prefix("\\def", *ln) )  /* copy to output */
  { t = *ln + strlen(*ln);
    while (*ln < t)
    { fputc (**ln, outfile); 
      (*ln)++;
    }
  }

  else if ( prefix("\\anotes", *ln) )
  { if (debug)
    { fprintf (logfile, "\nProcessing %s", *ln);
      fprintf (logfile, "lineno=%d\n",  lineno);
      fflush (logfile);
    }
    analyze_notes (ln);
    if (debug)
    { fprintf (logfile, "\nBefore generate_notes *ln=%s\n", *ln);
      fflush (logfile);
    }
    generate_notes (); 
    if (debug)
    { fprintf (logfile, "\nAfter generate_notes *ln=%s\n", *ln);
      fflush (logfile);
    }
    t = strpbrk (*ln, "%\\\n");
    if (t == NULL) t = *ln + strlen (*ln);
    if (*t == '\n') putc ('%', outfile);
    *ln = t;
  }
  else if ( prefix("\\nnnotes", *ln) ||
            prefix("\\nnnotes", *ln) ||
            prefix("\\nnotes", *ln)  ||
            prefix("\\notes", *ln)   || 
            prefix("\\Notes", *ln)   ||
            prefix("\\NOtes", *ln)   ||
            prefix("\\NOTes", *ln)   ||
            prefix("\\NOTEs", *ln)   || 
            prefix("\\znotes", *ln)  )
  { 
    char *s, *t;
    int nstaff;
    if (debug)
    { fprintf (logfile, "\nProcessing %s", *ln);
      fprintf (logfile, "lineno=%d\n",  lineno);
      fflush (logfile);
    }
    t = *ln+1;
    while ( isalpha (*t) ) t++;
    s = *ln;
    while (s < t) { putc (*s, outfile); s++; }

    analyze_notes (ln);

    t = TransformNotes2;
    while (true)
    {
      /*  output TransformNotes2 prefix (\transpose etc.):  */
      s = strchr (t, '#');
      if (s == NULL) break;
      while (t < s)  
      { putc (*t, outfile); t++; }

      t++;  /* skip '#' */

      /* output notes: */
      nstaff = atoi (t) - 1; t++;  
      s = notes[nstaff];
      while (*s != '&' && *s != '|' && *s != '$')
      { putc (*s, outfile); s++; }
      if (*t != '\0') 
      { putc (*t, outfile); t++; } /* output terminator */
    }
    fprintf (outfile, "\\en\n");

    t = strpbrk (*ln, "%\\\n");
    if (t == NULL) t = *ln + strlen (*ln);
    if (*t == '\n') putc ('%', outfile);
    *ln = t;
  }

  else if (prefix("\\vnotes", *ln) )
  {
    char *s, *t;
    int nstaff;
    if (debug)
    { fprintf (logfile, "\nProcessing %s", *ln);
      fprintf (logfile, "lineno=%d\n",  lineno);
      fflush (logfile);
    }
    s = *ln;
    t = strchr (*ln+1, '\\');  /* find \elemskip  */
    *ln = t;
    t = t + 9;
    while (s < t) { putc (*s, outfile); s++; }

    
    analyze_notes (ln);

    t = TransformNotes2;
    while (true)
    {
      /*  output TransformNotes2 prefix (\transpose etc.):  */
      s = strchr (t, '#');
      if (s == NULL) break;
      while (t < s)  
      { putc (*t, outfile); t++; }

      t++;  /* skip '#' */

      /* output notes: */
      nstaff = atoi (t) - 1; t++;  
      s = notes[nstaff];
      while (*s != '&' && *s != '|' && *s != '$')
      { putc (*s, outfile); s++; }
      if (*t != '\0') 
      { putc (*t, outfile); t++; } /* output terminator */
    }
    fprintf (outfile, "\\en\n");

    t = strpbrk (*ln, "%\\\n");
    if (t == NULL) t = *ln + strlen (*ln);
    if (*t == '\n') putc ('%', outfile);
    *ln = t;
  }

  else if ( prefix ("\\bar", *ln) && !prefix ("\\barno", *ln))
  { int i;
    char *s, *t;
    bool atnextbar = false; 
    for (i=1; i <= nstaffs; i++)
      if (active[i] && bar_rest[i]) 
      { atnextbar = true; break; }
    if (nastaffs == 1 && atnextbar)
    { restbars++;
      sprintf (deferred_bar, "\\bar");
      if (debug) fprintf (logfile, "\nrestbars increased to %d\n", restbars);
      for (i=1; i<= nstaffs; i++)
        bar_rest[i] = false;
      *ln = *ln+4;
      t = strpbrk (*ln, "\\\n");
      if (t == NULL) t = *ln + strlen (*ln);
      if (*t == '\n') putc ('%', outfile);
      *ln = t;
    }
    else
    { if (atnextbar)
      { fprintf (outfile, "\\def\\atnextbar{\\znotes");
        t = TransformNotes2;
        while (true)
        {
          s = strchr (t, '#');
          if (s == NULL) 
            break;
          while (t < s)  /* output any initial \transpose etc. */
          { putc (*t, outfile); t++; }
          t++; /* skip # */
          i = atoi (t) -1; t++;
          if (active[i])
          {
            if (bar_rest[i])
              fprintf (outfile, "\\centerpause");
            bar_rest[i] = false;
          }
          if (*t != '\0') 
          { putc (*t, outfile); t++; }  /* terminator */
        }
        fprintf (outfile, "\\en}%%\n");
      }
      if (Changeclefs) /* \Changeclefs has to be output after \def\atnextbar...  */
      {
        fprintf ( outfile, "\\Changeclefs%%\n");
        Changeclefs = false;
      }
      t = strpbrk (*ln+1, "%\\\n");
      while (*ln < t)
      { fputc (**ln, outfile);
        (*ln)++;
      }
      if (*t == '\n') putc ('%', outfile);
      *ln = t;
    }
    if (debug)
    { fprintf (logfile, "\nAfter \\bar processing, *ln=%s\n", *ln);
      fflush (logfile);
    }
  }

  else if ( prefix ("\\endpiece", *ln)
         || prefix ("\\Endpiece", *ln)
         || prefix ("\\endextract", *ln)
         || prefix ("\\stoppiece", *ln)
         || prefix ("\\raggedstoppiece", *ln)
         || prefix ("\\rightrepeat", *ln)
         || prefix ("\\leftrepeat", *ln)
         || prefix ("\\alaligne", *ln)
         || prefix ("\\alapage", *ln)
         || prefix ("\\changecontext", *ln)
         || prefix ("\\Changecontext", *ln)
         || prefix ("\\zchangecontext", *ln)
         || prefix ("\\zalaligne", *ln)
         || prefix ("\\zalapage", *ln) )
  { int i;
    bool atnextbar = false; 
    for (i=1; i <= nstaffs; i++)
      if (active[i] && bar_rest[i]) 
      { atnextbar = true; break; }
    if (nastaffs == 1 && atnextbar)
    { restbars++;
      sprintf (deferred_bar, "%s", *ln);
      output_rests ();
      t = strpbrk (*ln+1, "%\\\n");
      if (*t == '\n') putc ('%', outfile);
      *ln = t;
    }
    else if (atnextbar)
    {
      fprintf (outfile, "\\def\\atnextbar{\\znotes");
      for (i=1; i <= nstaffs; i++)
      {
        if (active[i])
        {
          if (bar_rest[i])
            fprintf (outfile, "\\centerpause");
          bar_rest[i] = false;
        }
        if ( terminator[i] != '$') putc (terminator[i], outfile);
      }
      fprintf (outfile, "\\en}%%\n");
      if (Changeclefs) /* \Changeclefs has to be output after \def\atnextbar...  */
      {
        fprintf ( outfile, "\\Changeclefs%%\n");
        Changeclefs = false;
      }
      t = strpbrk (*ln+1, "%\\\n");
      while (*ln < t)
      { fputc (**ln, outfile);
        (*ln)++;
      }
      if (*t == '\n') putc ('%', outfile);
      *ln = t;
    }
    else
    { if (nastaffs == 1 && restbars > 0)
        output_rests ();
      t = strpbrk (*ln+1, "%\\\n");
      while (*ln < t)
      { fputc (**ln, outfile);
        (*ln)++;
      }
      if (*t == '\n') putc ('%', outfile);
      *ln = t;
    }
    if (debug)
    { fprintf (logfile, "\nAfter \\bar processing, *ln=%s\n", *ln);
      fflush (logfile);
    }
  }

  else if  ( prefix ("\\znotes", *ln) )
  {
    if (nastaffs == 1 && restbars > 0)
      output_rests ();
    fputs (*ln, outfile);
    *ln = *ln + strlen(*ln);
  }
 
  else if ( prefix ("\\Changeclefs", *ln) )
  {  /* defer till after \def\atnextbar ... */
    Changeclefs = true;
    *ln = *ln + strlen(*ln);
  }

  else if ( prefix ("\\documentclass", *ln) )
  {
    
    if ( suffix( ".tex", outfilename) )
    { char newoutfilename[SHORT_LEN];
      char *newoutfilename_n = newoutfilename;
      append (newoutfilename, &newoutfilename_n, infilename, sizeof (newoutfilename));
      newoutfilename_n -= 4;
      *newoutfilename_n = '\0';
      append (newoutfilename, &newoutfilename_n, "ltx", sizeof (newoutfilename));
      if (rename ( outfilename, newoutfilename) > 0)
      { printf ("Can't rename %s as %s\n", outfilename, newoutfilename);
        exit (EXIT_FAILURE);
      }
      printf ("\\documentclass detected; now writing to %s.", newoutfilename);
      printf ("\n\n");

    }
    fputs (*ln, outfile);
    *ln = *ln + strlen(*ln);
  }

  else if ( prefix ("\\end", *ln)
        && !prefix ("\\end{", *ln) )
  {
    fprintf (outfile, "\\end\n");
    exit(0);
  }


  else  /* everything else */
  { 
    fputs (*ln, outfile);
    *ln = *ln + strlen(*ln);
  }
}

PRIVATE
void process_line ()
{
  char  *ln; 
  ln = &line[0];
  while ( *ln != '\0') 
  {
    while (*ln == ' ') { ln++; fputc (' ', outfile); }
    if (*ln == '%') {
      fprintf (outfile, "%s", ln);
      return;
    }
    process_command (&ln);
  }     
}

PRIVATE
void process_score ()
/* process .tex file */
{
  int c; int i;
  lineno = 0;
  for (i=1; i < MAX_STAFFS; i++) 
  {
    staffs[i] = 1; 
    active[i] = true;
    bar_rest[i] = false;
  }
  spacing = MAX_SPACING;
  restbars = 0;

  c = getc (infile);
  while ( c != EOF )
  {
    ungetc (c, infile);
    fgets(line, LINE_LEN, infile); 
    lineno++;
    process_line ();
    c = getc (infile);
  }     /* c == EOF  */
}     

int main (int argc, char *argv[])
{
  int c;
  char today[12];
  time_t mytime; 
# define NOPTS 5
  struct option longopts[NOPTS] =
  {  { "help", 0, NULL, 'h'},
     { "version", 0, NULL, 'v'},
     { "dotted", 0, NULL, 'd'},
     { "log", 0, NULL, 'l'},
     { NULL, 0, NULL, 0}
  };
  
  time (&mytime);
  strftime (today, 11, "%Y-%m-%d", localtime (&mytime) );
  printf ("This is autosp, version %s.\n", version);
  printf ("Copyright (C) 2014-17  R. D. Tennent\n" );
  printf ("School of Computing, Queen's University, rdt@cs.queensu.ca\n" );
  printf ("License GNU GPL version 2 or later <http://gnu.org/licences/gpl.html>.\n" );
  printf ("There is NO WARRANTY, to the extent permitted by law.\n\n" );

  c = getopt_long (argc, argv, "hvdl", longopts, NULL);
  while (c != -1)
    {
      switch (c)
        {
        case 'h':
          usage ();
          printf ("Please report bugs to rdt@cs.queensu.ca.\n" );
          exit (0);
        case 'v':
          exit (0);
        case 'd':
          dottedbeamnotes = true;
          break;
        case 'l':
          debug = true;
          break;
        case '?':
          exit (EXIT_FAILURE);
        default:
          printf ("Function getopt returned character code 0%o.\n",
                  (unsigned int) c);
          exit (EXIT_FAILURE);
        }
      c = getopt_long (argc, argv, "hvd", longopts, NULL);
    }

  infilename[0] = '\0';
  infilename_n = infilename;
  if (optind < argc)
  { append (infilename, &infilename_n, argv[optind], sizeof (infilename));
    if (!suffix (".aspc", infilename)) 
      append (infilename, &infilename_n, ".aspc", sizeof (infilename));
  }
  else 
  {  usage();
     exit (EXIT_FAILURE);
  }
  infile = fopen (infilename, "r");
  if (infile == NULL )
  { printf ("Can't open %s\n", infilename);
    exit (EXIT_FAILURE);
  }

  printf ("Reading from %s.", infilename);

  optind++;
  outfilename[0] = '\0';
  outfilename_n = outfilename;
  if (optind < argc)  /* user-provided outfilename */
  {
    append (outfilename, &outfilename_n, argv[optind], sizeof (outfilename));
    if (!suffix (".tex", outfilename))
      append (outfilename, &outfilename_n, ".tex", sizeof (outfilename));
  }
  else if (suffix (".aspc", infilename)  )
  { /* output to infile.tex  */
    append (outfilename, &outfilename_n, infilename, sizeof (outfilename));
    outfilename_n -= 4;
    *outfilename_n = '\0';
    append (outfilename, &outfilename_n, "tex", sizeof (outfilename));
  }

  if (*outfilename == '\0')
  { 
    printf ("\nPlease provide a filename for the output.\n\n");
    exit (EXIT_FAILURE);
  }
  else
  {
    outfile = fopen (outfilename, "w");
    if (outfile == NULL)
    { printf ("Can't open %s\n", outfilename);
      exit (EXIT_FAILURE);
    }
    printf (" Writing to %s.", outfilename);
  }

  logfilename[0] = '\0';
  logfilename_n = logfilename;
  append (logfilename, &logfilename_n, infilename, sizeof (logfilename));
  if (suffix (".aspc", infilename) )
    logfilename_n -= 4;
  else
    logfilename_n -= 3;
  *logfilename_n = '\0';
  append (logfilename, &logfilename_n, "alog", sizeof (logfilename));
  if (debug)  
  { logfile = fopen (logfilename, "w");
    if (logfile == NULL)
    { printf ("Can't open %s\n", logfilename);
      exit (EXIT_FAILURE);
    }
    printf (" Log file %s.", logfilename);
  }
  printf ("\n\n");

  fprintf (outfile, "%%  Generated by autosp (%s).\n", version);
  process_score ();
  
  return 0;
}

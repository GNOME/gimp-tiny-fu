/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <gtk/gtk.h>
#include <libgimp/gimp.h>

#include "tinyscheme/scheme.h"

#include "ts-wrapper.h"
#include "tiny-fu-console.h"
#include "tiny-fu-scripts.h"
#include "tiny-fu-server.h"
#include "tiny-fu-text-console.h"

#include "tiny-fu-intl.h"


/* Declare local functions. */

static void      tiny_fu_quit           (void);
static void      tiny_fu_query          (void);
static void      tiny_fu_run            (const gchar      *name,
                                         gint              nparams,
                                         const GimpParam  *params,
                                         gint             *nreturn_vals,
                                         GimpParam       **return_vals);
static void      tiny_fu_auxillary_init (void);
static void      tiny_fu_refresh_proc   (const gchar      *name,
                                         gint              nparams,
                                         const GimpParam  *params,
                                         gint             *nreturn_vals,
                                         GimpParam       **return_vals);


GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,                /* init_proc  */
  tiny_fu_quit,        /* quit_proc  */
  tiny_fu_query,       /* query_proc */
  tiny_fu_run          /* run_proc   */
};



MAIN ()

static void
tiny_fu_quit (void)
{
}

static void
tiny_fu_query (void)
{
  static GimpParamDef console_args[] =
  {
    { GIMP_PDB_INT32,  "run_mode", "Interactive, [non-interactive]" }
  };

  static GimpParamDef textconsole_args[] =
  {
    { GIMP_PDB_INT32,  "run_mode", "Interactive, [non-interactive]" }
  };

  static GimpParamDef eval_args[] =
  {
    { GIMP_PDB_INT32,  "run_mode", "[Interactive], non-interactive" },
    { GIMP_PDB_STRING, "code",     "The code to evaluate" }
  };

  static GimpParamDef server_args[] =
  {
    { GIMP_PDB_INT32,  "run_mode", "[Interactive], non-interactive" },
    { GIMP_PDB_INT32,  "port",     "The port on which to listen for requests" },
    { GIMP_PDB_STRING, "logfile",  "The file to log server activity to" }
  };

  gimp_plugin_domain_register (GETTEXT_PACKAGE "-tiny-fu", NULL);

  gimp_install_procedure ("extension_tiny_fu",
                          "A scheme interpreter for scripting GIMP operations",
                          "More help here later",
                          "Spencer Kimball & Peter Mattis",
                          "Spencer Kimball & Peter Mattis",
                          "1997",
                          NULL,
                          NULL,
                          GIMP_EXTENSION,
                          0, 0, NULL, NULL);

  gimp_install_procedure ("plug_in_tiny_fu_console",
                          "Provides a console mode for tiny-fu development",
                          "Provides an interface which allows interactive "
                                      "scheme development.",
                          "Spencer Kimball & Peter Mattis",
                          "Spencer Kimball & Peter Mattis",
                          "1997",
                          N_("<Toolbox>/Xtns/Tiny-Fu/Tiny-Fu _Console"),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (console_args), 0,
                          console_args, NULL);

  gimp_install_procedure ("plug_in_tiny_fu_text_console",
                          "Provides a text console mode for tiny-fu "
                                      "development",
                          "Provides an interface which allows interactive "
                                      "scheme development.",
                          "Spencer Kimball & Peter Mattis",
                          "Spencer Kimball & Peter Mattis",
                          "1997",
                          NULL,
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (textconsole_args), 0,
                          textconsole_args, NULL);

  gimp_install_procedure ("plug_in_tiny_fu_server",
                          "Provides a server for remote tiny-fu operation",
                          "Provides a server for remote tiny-fu operation",
                          "Spencer Kimball & Peter Mattis",
                          "Spencer Kimball & Peter Mattis",
                          "1997",
                          N_("<Toolbox>/Xtns/Tiny-Fu/_Start Server..."),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (server_args), 0,
                          server_args, NULL);

  gimp_install_procedure ("plug_in_tiny_fu_eval",
                          "Evaluate scheme code",
                          "Evaluate the code under the scheme interpreter "
                                      "(primarily for batch mode)",
                          "Manish Singh",
                          "Manish Singh",
                          "1998",
                          NULL,
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (eval_args), 0,
                          eval_args, NULL);
}

static void
tiny_fu_run (const gchar *name,
        gint              nparams,
        const GimpParam  *param,
        gint             *nreturn_vals,
        GimpParam       **return_vals)
{
  INIT_I18N();

  ts_set_console_mode (0);

  /*  Determine before we allow scripts to register themselves
   *   whether this is the base, automatically installed tiny-fu extension
   */
  if (strcmp (name, "extension_tiny_fu") == 0)
    {
      /*  Setup auxillary temporary procedures for the base extension  */
      tiny_fu_auxillary_init ();

      /*  Init the interpreter and register scripts  */
      tinyscheme_init (TRUE);
    }
  else
    {
      /*  Init the interpreter and do not register scripts  */
      tinyscheme_init (FALSE);
    }

  /*  Load all of the available scripts  */
  tiny_fu_load_all_scripts ();

  if (strcmp (name, "extension_tiny_fu") == 0)
    {
      /*
       *  The main, automatically installed tiny fu extension.  For
       *  things like logos and effects that are runnable from GIMP
       *  menus.
       */

      static GimpParam  values[1];
      GimpPDBStatusType status = GIMP_PDB_SUCCESS;

      /*  Acknowledge that the extension is properly initialized  */
      gimp_extension_ack ();

      while (TRUE)
        gimp_extension_process (0);

      *nreturn_vals = 1;
      *return_vals  = values;

      values[0].type          = GIMP_PDB_STATUS;
      values[0].data.d_status = status;
    }
  else if (strcmp (name, "plug_in_tiny_fu_text_console") == 0)
    {
      /*
       *  The tiny-fu text console for interactive Scheme development
       */

      tiny_fu_text_console_run (name, nparams, param,
                                nreturn_vals, return_vals);
    }
  else if (strcmp (name, "plug_in_tiny_fu_console") == 0)
    {
      /*
       *  The tiny-fu console for interactive Scheme development
       */

      tiny_fu_console_run (name, nparams, param,
                           nreturn_vals, return_vals);
    }
  else if (strcmp (name, "plug_in_tiny_fu_server") == 0)
    {
      /*
       *  The tiny-fu server for remote operation
       */

      tiny_fu_server_run (name, nparams, param,
                          nreturn_vals, return_vals);
    }
  else if (strcmp (name, "plug_in_tiny_fu_eval") == 0)
    {
      /*
       *  A non-interactive "console" (for batch mode)
       */

      tiny_fu_eval_run (name, nparams, param,
                        nreturn_vals, return_vals);
    }
}


static void
tiny_fu_auxillary_init (void)
{
  static GimpParamDef args[] =
  {
    { GIMP_PDB_INT32, "run_mode", "[Interactive], non-interactive" }
  };

  gimp_install_temp_proc ("tiny_fu_refresh",
                          "Re-read all available scripts",
                          "Re-read all available scripts",
                          "Spencer Kimball & Peter Mattis",
                          "Spencer Kimball & Peter Mattis",
                          "1997",
                          N_("<Toolbox>/Xtns/Tiny-Fu/_Refresh Scripts"),
                          NULL,
                          GIMP_TEMPORARY,
                          G_N_ELEMENTS (args), 0,
                          args, NULL,
                          tiny_fu_refresh_proc);
}

static void
tiny_fu_refresh_proc (const gchar      *name,
                      gint              nparams,
                      const GimpParam  *params,
                      gint             *nreturn_vals,
                      GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;

  /*  Reload all of the available scripts  */
  tiny_fu_load_all_scripts ();

  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = status;
}

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

#include <glib.h>       /* Include early for obscure Win32 build reasons */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <gtk/gtk.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "tinyscheme/scheme-private.h"

#include "tiny-fu-types.h"

#include "tiny-fu-interface.h"
#include "tiny-fu-scripts.h"
#include "ts-wrapper.h"

#include "tiny-fu-intl.h"


#define RESPONSE_RESET         1
#define RESPONSE_ABOUT         2

#define TEXT_WIDTH           100
#define COLOR_SAMPLE_WIDTH   100
#define COLOR_SAMPLE_HEIGHT   15
#define SLIDER_WIDTH          80


/*
 *  Local Functions
 */

static void       tiny_fu_load_script   (const GimpDatafileData *file_data,
                                               gpointer          user_data);
static gboolean   tiny_fu_install_script      (gpointer          foo,
                                               SFScript         *script,
                                               gpointer          bar);
static gboolean   tiny_fu_remove_script       (gpointer          foo,
                                               SFScript         *script,
                                               gpointer          bar);
static void       tiny_fu_script_proc         (const gchar      *name,
                                               gint              nparams,
                                               const GimpParam  *params,
                                               gint             *nreturn_vals,
                                               GimpParam       **return_vals);

static SFScript * tiny_fu_find_script         (const gchar      *script_name);
static void       tiny_fu_free_script         (SFScript         *script);


/*
 *  Local variables
 */

static GTree *script_list  = NULL;


/*
 *  Function definitions
 */

void
tiny_fu_find_scripts (void)
{
  gchar *path_str;

  /*  Make sure to clear any existing scripts  */
  if (script_list != NULL)
    {
      g_tree_foreach (script_list,
                      (GTraverseFunc) tiny_fu_remove_script,
                      NULL);
      g_tree_destroy (script_list);
    }

  script_list = g_tree_new ((GCompareFunc) strcoll);

  path_str = gimp_gimprc_query ("script-fu-path");

  if (path_str == NULL)
    return;

  gimp_datafiles_read_directories (path_str, G_FILE_TEST_IS_REGULAR,
                                   tiny_fu_load_script,
                                   NULL);

  g_free (path_str);

  /*  now that all scripts are read in and sorted, tell gimp about them  */
  g_tree_foreach (script_list,
                  (GTraverseFunc) tiny_fu_install_script,
                  NULL);
}

#if 1        /* ~~~~~ */
pointer
my_err(scheme *sc, char *msg)
{
    fprintf(stderr, msg);
    return sc->F;
}
#endif

pointer
tiny_fu_add_script (scheme *sc, pointer a)
{
  GimpParamDef *args;
  SFScript     *script;
  gchar        *val;
  gint          i;
  guchar        r, g, b;
  pointer       color_list;
  pointer       adj_list;
  pointer       brush_list;
  pointer       option_list;
  gchar        *s;

  /*  Check the length of a  */
  if (sc->vptr->list_length (sc, a) < 7)
  {
    g_message ("Too few arguments to tiny-fu-register");
    return sc->NIL;
  }

  /*  Create a new script  */
  script = g_new0 (SFScript, 1);

  /*  Find the script name  */
  val = sc->vptr->string_value (sc->vptr->pair_car (a));
  script->script_name = g_strdup (val);
  a = sc->vptr->pair_cdr (a);

  /* transform the function name into a name containing "_" for each "-".
   * this does not hurt anybody, yet improves the life of many... ;)
   */
  script->pdb_name = g_strdup (val);

  for (s = script->pdb_name; *s; s++)
    if (*s == '-')
      *s = '_';

  /*  Find the script menu_path  */
  val = sc->vptr->string_value (sc->vptr->pair_car (a));
  script->menu_path = g_strdup (val);
  a = sc->vptr->pair_cdr (a);

  /*  Find the script help  */
  val = sc->vptr->string_value (sc->vptr->pair_car (a));
  script->help = g_strdup (val);
  a = sc->vptr->pair_cdr (a);

  /*  Find the script author  */
  val = sc->vptr->string_value (sc->vptr->pair_car (a));
  script->author = g_strdup (val);
  a = sc->vptr->pair_cdr (a);

  /*  Find the script copyright  */
  val = sc->vptr->string_value (sc->vptr->pair_car (a));
  script->copyright = g_strdup (val);
  a = sc->vptr->pair_cdr (a);

  /*  Find the script date  */
  val = sc->vptr->string_value (sc->vptr->pair_car (a));
  script->date = g_strdup (val);
  a = sc->vptr->pair_cdr (a);

  /*  Find the script image types  */
  if (sc->vptr->is_pair (a))
    {
      val = sc->vptr->string_value (sc->vptr->pair_car (a));
      a = sc->vptr->pair_cdr (a);
    }
  else
    {
      val = sc->vptr->string_value (a);
      a = sc->NIL;
    }
  script->img_types = g_strdup (val);

  /*  Check the supplied number of arguments  */
  script->num_args = sc->vptr->list_length (sc, a) / 3;

  args = g_new0 (GimpParamDef, script->num_args + 1);
  args[0].type        = GIMP_PDB_INT32;
  args[0].name        = "run_mode";
  args[0].description = "Interactive, non-interactive";

  script->arg_types    = g_new0 (SFArgType, script->num_args);
  script->arg_labels   = g_new0 (gchar *, script->num_args);
  script->arg_defaults = g_new0 (SFArgValue, script->num_args);
  script->arg_values   = g_new0 (SFArgValue, script->num_args);

  if (script->num_args > 0)
    {
      for (i = 0; i < script->num_args; i++)
        {
          if (a != sc->NIL)
            {
              if (!sc->vptr->is_integer (sc->vptr->pair_car (a)))
                return my_err (sc, "tiny-fu-register: argument types must be integer values");
              script->arg_types[i] = sc->vptr->ivalue (sc->vptr->pair_car (a));
              a = sc->vptr->pair_cdr (a);
            }
          else
            return my_err (sc, "tiny-fu-register: missing type specifier");

          if (a != sc->NIL)
            {
              if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                return my_err (sc, "tiny-fu-register: argument labels must be strings");
              script->arg_labels[i] = g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));
              a = sc->vptr->pair_cdr (a);
            }
          else
            return my_err (sc, "tiny-fu-register: missing arguments label");

          if (a != sc->NIL)
            {
              switch (script->arg_types[i])
                {
                case SF_IMAGE:
                case SF_DRAWABLE:
                case SF_LAYER:
                case SF_CHANNEL:
                  if (!sc->vptr->is_integer (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: drawable defaults must be integer values");
                  script->arg_defaults[i].sfa_image =
                      sc->vptr->ivalue (sc->vptr->pair_car (a));
                  script->arg_values[i].sfa_image =
                      script->arg_defaults[i].sfa_image;

                  switch (script->arg_types[i])
                    {
                    case SF_IMAGE:
                      args[i + 1].type = GIMP_PDB_IMAGE;
                      args[i + 1].name = "image";
                      break;

                    case SF_DRAWABLE:
                      args[i + 1].type = GIMP_PDB_DRAWABLE;
                      args[i + 1].name = "drawable";
                      break;

                    case SF_LAYER:
                      args[i + 1].type = GIMP_PDB_LAYER;
                      args[i + 1].name = "layer";
                      break;

                    case SF_CHANNEL:
                      args[i + 1].type = GIMP_PDB_CHANNEL;
                      args[i + 1].name = "channel";
                      break;

                    default:
                      break;
                    }

                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_COLOR:
                  if (!(sc->vptr->is_list (sc, sc->vptr->pair_car (a)) &&
                                sc->vptr->list_length(sc, sc->vptr->pair_car (a)) == 3))
                     return my_err (sc, "tiny-fu-register: color defaults must be a list of 3 integers");

                  color_list = sc->vptr->pair_car (a);
                  r = CLAMP (sc->vptr->ivalue (sc->vptr->pair_car (color_list)), 0, 255);
                  color_list = sc->vptr->pair_cdr (color_list);
                  g = CLAMP (sc->vptr->ivalue (sc->vptr->pair_car (color_list)), 0, 255);
                  color_list = sc->vptr->pair_cdr (color_list);
                  b = CLAMP (sc->vptr->ivalue (sc->vptr->pair_car (color_list)), 0, 255);

                  gimp_rgb_set_uchar (&script->arg_defaults[i].sfa_color, r, g, b);

                  script->arg_values[i].sfa_color = script->arg_defaults[i].sfa_color;

                  args[i + 1].type        = GIMP_PDB_COLOR;
                  args[i + 1].name        = "color";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_TOGGLE:
                  if (!sc->vptr->is_integer (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: toggle default must be an integer value");

                  script->arg_defaults[i].sfa_toggle =
                    (sc->vptr->ivalue (sc->vptr->pair_car (a))) ? TRUE : FALSE;
                  script->arg_values[i].sfa_toggle =
                    script->arg_defaults[i].sfa_toggle;

                  args[i + 1].type        = GIMP_PDB_INT32;
                  args[i + 1].name        = "toggle";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_VALUE:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: value defaults must be string values");

                  script->arg_defaults[i].sfa_value =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));
                  script->arg_values[i].sfa_value =
                    g_strdup (script->arg_defaults[i].sfa_value);

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = "value";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_STRING:
                case SF_TEXT:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: string defaults must be string values");

                  script->arg_defaults[i].sfa_value =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));
                  script->arg_values[i].sfa_value =
                    g_strdup (script->arg_defaults[i].sfa_value);

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = "string";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_ADJUSTMENT:
                  if (!sc->vptr->is_list (sc, a))
                    return my_err (sc, "tiny-fu-register: adjustment defaults must be a list");

                  adj_list = sc->vptr->pair_car (a);
                  script->arg_defaults[i].sfa_adjustment.value =
                    sc->vptr->rvalue (sc->vptr->pair_car (adj_list));
                  adj_list = sc->vptr->pair_cdr (adj_list);
                  script->arg_defaults[i].sfa_adjustment.lower =
                    sc->vptr->rvalue (sc->vptr->pair_car (adj_list));
                  adj_list = sc->vptr->pair_cdr (adj_list);
                  script->arg_defaults[i].sfa_adjustment.upper =
                    sc->vptr->rvalue (sc->vptr->pair_car (adj_list));
                  adj_list = sc->vptr->pair_cdr (adj_list);
                  script->arg_defaults[i].sfa_adjustment.step =
                    sc->vptr->rvalue (sc->vptr->pair_car (adj_list));
                  adj_list = sc->vptr->pair_cdr (adj_list);
                  script->arg_defaults[i].sfa_adjustment.page =
                    sc->vptr->rvalue (sc->vptr->pair_car (adj_list));
                  adj_list = sc->vptr->pair_cdr (adj_list);
                  script->arg_defaults[i].sfa_adjustment.digits =
                    sc->vptr->ivalue (sc->vptr->pair_car (adj_list));
                  adj_list = sc->vptr->pair_cdr (adj_list);
                  script->arg_defaults[i].sfa_adjustment.type =
                    sc->vptr->ivalue (sc->vptr->pair_car (adj_list));

                  script->arg_values[i].sfa_adjustment.adj = NULL;
                  script->arg_values[i].sfa_adjustment.value =
                    script->arg_defaults[i].sfa_adjustment.value;

                  args[i + 1].type        = GIMP_PDB_FLOAT;
                  args[i + 1].name        = "value";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_FILENAME:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: filename defaults must be string values");
                  /* fallthrough */

                case SF_DIRNAME:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: dirname defaults must be string values");

                  script->arg_defaults[i].sfa_file.filename =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));

#ifdef G_OS_WIN32
                  /* Replace POSIX slashes with Win32 backslashes. This
                   * is just so tiny-fus can be written with only
                   * POSIX directory separators.
                   */
                  val = script->arg_defaults[i].sfa_file.filename;
                  while (*val)
                    {
                      if (*val == '/')
                        *val = '\\';
                      val++;
                    }
#endif
                  script->arg_values[i].sfa_file.filename =
                    g_strdup (script->arg_defaults[i].sfa_file.filename);
                  script->arg_values[i].sfa_file.file_entry = NULL;

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = (script->arg_types[i] == SF_FILENAME ?
                                               "filename" : "dirname");
                  args[i + 1].description = script->arg_labels[i];
                 break;

                case SF_FONT:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: font defaults must be string values");

                  script->arg_defaults[i].sfa_font =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));
                  script->arg_values[i].sfa_font =
                    g_strdup (script->arg_defaults[i].sfa_font);

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = "font";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_PALETTE:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: palette defaults must be string values");

                  script->arg_defaults[i].sfa_palette =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));
                  script->arg_values[i].sfa_palette =
                    g_strdup (script->arg_defaults[i].sfa_pattern);

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = "palette";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_PATTERN:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: pattern defaults must be string values");

                  script->arg_defaults[i].sfa_pattern =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));
                  script->arg_values[i].sfa_pattern =
                    g_strdup (script->arg_defaults[i].sfa_pattern);

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = "pattern";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_BRUSH:
                  if (!sc->vptr->is_list (sc, a))
                    return my_err (sc, "tiny-fu-register: brush defaults must be a list");

                  brush_list = sc->vptr->pair_car (a);
                  script->arg_defaults[i].sfa_brush.name =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (brush_list)));
                  script->arg_values[i].sfa_brush.name =
                    g_strdup (script->arg_defaults[i].sfa_brush.name);

                  brush_list = sc->vptr->pair_cdr (brush_list);
                  script->arg_defaults[i].sfa_brush.opacity =
                    sc->vptr->rvalue (sc->vptr->pair_car (brush_list));
                  script->arg_values[i].sfa_brush.opacity =
                    script->arg_defaults[i].sfa_brush.opacity;

                  brush_list = sc->vptr->pair_cdr (brush_list);
                  script->arg_defaults[i].sfa_brush.spacing =
                    sc->vptr->ivalue (sc->vptr->pair_car (brush_list));
                  script->arg_values[i].sfa_brush.spacing =
                    script->arg_defaults[i].sfa_brush.spacing;

                  brush_list = sc->vptr->pair_cdr (brush_list);
                  script->arg_defaults[i].sfa_brush.paint_mode =
                    sc->vptr->ivalue (sc->vptr->pair_car (brush_list));
                  script->arg_values[i].sfa_brush.paint_mode =
                    script->arg_defaults[i].sfa_brush.paint_mode;

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = "brush";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_GRADIENT:
                  if (!sc->vptr->is_string (sc->vptr->pair_car (a)))
                    return my_err (sc, "tiny-fu-register: gradient defaults must be string values");

                  script->arg_defaults[i].sfa_gradient =
                    g_strdup (sc->vptr->string_value (sc->vptr->pair_car (a)));
                  script->arg_values[i].sfa_gradient =
                    g_strdup (script->arg_defaults[i].sfa_pattern);

                  args[i + 1].type        = GIMP_PDB_STRING;
                  args[i + 1].name        = "gradient";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                case SF_OPTION:
                  if (!sc->vptr->is_list (sc, a))
                    return my_err (sc, "tiny-fu-register: option defaults must be a list");

                  for (option_list = sc->vptr->pair_car (a);
                       option_list != sc->NIL;
                       option_list = sc->vptr->pair_cdr (option_list))
                    {
                      script->arg_defaults[i].sfa_option.list =
                        g_slist_append (script->arg_defaults[i].sfa_option.list,
                                        g_strdup (sc->vptr->string_value (sc->vptr->pair_car (option_list))));
                    }
                  script->arg_defaults[i].sfa_option.history = 0;
                  script->arg_values[i].sfa_option.history = 0;

                  args[i + 1].type        = GIMP_PDB_INT32;
                  args[i + 1].name        = "option";
                  args[i + 1].description = script->arg_labels[i];
                  break;

                default:
                  break;
                }

              a = sc->vptr->pair_cdr (a);
            }
          else
            return my_err (sc, "tiny-fu-register: missing default argument");
        }
    }

  script->args = args;
  g_tree_insert (script_list, gettext (script->menu_path), script);

  return sc->NIL;
}

void
tiny_fu_error_msg (const gchar *command)
{
  g_message (_("Error while executing\n%s\n\n%s"),
             command, ts_get_error_msg ());
}


/*  private functions  */

static void
tiny_fu_load_script (const GimpDatafileData *file_data,
                       gpointer                user_data)
{
  if (gimp_datafiles_check_extension (file_data->filename, ".sct"))
    {
      gchar *command;
      gchar *escaped = g_strescape (file_data->filename, NULL);

      command = g_strdup_printf ("(load \"%s\")", escaped);
      g_free (escaped);

      if (ts_interpret_string(command))
             tiny_fu_error_msg (command);

#ifdef G_OS_WIN32
      /* No, I don't know why, but this is
       * necessary on NT 4.0.
       */
      Sleep(0);
#endif

      g_free (command);
    }
}

/*
 *  The following function is a GTraverseFunction.  Please
 *  note that it frees the script->args structure.  --Sven
 */
static gboolean
tiny_fu_install_script (gpointer  foo,
                          SFScript *script,
                          gpointer  bar)
{
  gchar *menu_path = NULL;

  /* Allow scripts with no menus */
  if (strncmp (script->menu_path, "<None>", 6) != 0)
    menu_path = script->menu_path;

  gimp_install_temp_proc (script->pdb_name,
                          script->help,
                          "",
                          script->author,
                          script->copyright,
                          script->date,
                          menu_path,
                          script->img_types,
                          GIMP_TEMPORARY,
                          script->num_args + 1, 0,
                          script->args, NULL,
                          tiny_fu_script_proc);

  g_free (script->args);
  script->args = NULL;

  return FALSE;
}

/*
 *  The following function is a GTraverseFunction.
 */
static gboolean
tiny_fu_remove_script (gpointer  foo,
                         SFScript *script,
                         gpointer  bar)
{
  tiny_fu_free_script (script);

  return FALSE;
}

static void
tiny_fu_script_proc (const gchar      *name,
                       gint              nparams,
                       const GimpParam  *params,
                       gint             *nreturn_vals,
                       GimpParam       **return_vals)
{
  static GimpParam   values[1];
  GimpPDBStatusType  status = GIMP_PDB_SUCCESS;
  GimpRunMode        run_mode;
  SFScript          *script;
  gint               min_args;
  gchar             *escaped;

  run_mode = params[0].data.d_int32;

  if (! (script = tiny_fu_find_script (name)))
    {
      status = GIMP_PDB_CALLING_ERROR;
    }
  else
    {
      if (script->num_args == 0)
        run_mode = GIMP_RUN_NONINTERACTIVE;

      switch (run_mode)
        {
        case GIMP_RUN_INTERACTIVE:
        case GIMP_RUN_WITH_LAST_VALS:
          /*  Determine whether the script is image based (runs on an image) */
          if (strncmp (script->menu_path, "<Image>", 7) == 0)
            {
              script->arg_values[0].sfa_image    = params[1].data.d_image;
              script->arg_values[1].sfa_drawable = params[2].data.d_drawable;
              script->image_based = TRUE;
            }
          else
            script->image_based = FALSE;

          /*  First acquire information with a dialog  */
          /*  Skip this part if the script takes no parameters */
          min_args = (script->image_based) ? 2 : 0;
          if (script->num_args > min_args)
            {
              tiny_fu_interface (script);
              break;
            }
          /*  else fallthrough  */

        case GIMP_RUN_NONINTERACTIVE:
          /*  Make sure all the arguments are there!  */
          if (nparams != (script->num_args + 1))
              status = GIMP_PDB_CALLING_ERROR;

          if (status == GIMP_PDB_SUCCESS)
            {
              GString *s;
              gchar   *command;
              gchar    buffer[G_ASCII_DTOSTR_BUF_SIZE];
              gint     i;

              s = g_string_new ("(");
              g_string_append (s, script->script_name);

              if (script->num_args)
                {
                  for (i = 0; i < script->num_args; i++)
                    {
                      const GimpParam *param = &params[i + 1];

                      g_string_append_c (s, ' ');

                      switch (script->arg_types[i])
                        {
                        case SF_IMAGE:
                        case SF_DRAWABLE:
                        case SF_LAYER:
                        case SF_CHANNEL:
                          g_string_append_printf (s, "%d",
                                                  params->data.d_image);
                          break;

                        case SF_COLOR:
                          {
                            guchar r, g, b;

                            gimp_rgb_get_uchar (&param->data.d_color,
                                                &r, &g, &b);
                            g_string_append_printf (s, "'(%d %d %d)",
                                                    (gint) r, (gint) g,
                                                    (gint) b);
                          }
                          break;

                        case SF_TOGGLE:
                          g_string_append (s, (param->data.d_int32 ?
                                               "TRUE" : "FALSE"));
                          break;

                        case SF_VALUE:
                          g_string_append (s, param->data.d_string);
                          break;

                        case SF_STRING:
                        case SF_TEXT:
                        case SF_FILENAME:
                        case SF_DIRNAME:
                          escaped = g_strescape (params->data.d_string, NULL);
                          g_string_append_printf (s, "\"%s\"", escaped);
                          g_free (escaped);
                          break;

                        case SF_ADJUSTMENT:
                          g_ascii_dtostr (buffer, sizeof (buffer),
                                          param->data.d_float);
                          g_string_append (s, buffer);
                          break;

                        case SF_FONT:
                        case SF_PALETTE:
                        case SF_PATTERN:
                        case SF_GRADIENT:
                        case SF_BRUSH:
                          g_string_append_printf (s, "\"%s\"",
                                                  param->data.d_string);
                          break;

                        case SF_OPTION:
                          g_string_append_printf (s, "%d",
                                                  param->data.d_int32);
                          break;

                        default:
                          break;
                        }
                    }
                }

              g_string_append_c (s, ')');

              command = g_string_free (s, FALSE);

              /*  run the command through the interpreter  */
              if (ts_interpret_string(command))
                 tiny_fu_error_msg (command);

              g_free (command);
            }
          break;

        default:
          break;
        }
    }

  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = status;
}

/* this is a GTraverseFunction */
static gboolean
tiny_fu_lookup_script (gpointer      *foo,
                         SFScript      *script,
                         gconstpointer *name)
{
  if (strcmp (script->pdb_name, *name) == 0)
    {
      /* store the script in the name pointer and stop the traversal */
      *name = script;
      return TRUE;
    }

  return FALSE;
}

static SFScript *
tiny_fu_find_script (const gchar *pdb_name)
{
  gconstpointer script = pdb_name;

  g_tree_foreach (script_list,
                  (GTraverseFunc) tiny_fu_lookup_script,
                  &script);

  if (script == pdb_name)
    return NULL;

  return (SFScript *) script;
}

static void
tiny_fu_free_script (SFScript *script)
{
  gint i;

  /*  Uninstall the temporary procedure for this script  */
  gimp_uninstall_temp_proc (script->pdb_name);

  if (script)
    {
      g_free (script->script_name);
      g_free (script->menu_path);
      g_free (script->help);
      g_free (script->author);
      g_free (script->copyright);
      g_free (script->date);
      g_free (script->img_types);

      for (i = 0; i < script->num_args; i++)
        {
          g_free (script->arg_labels[i]);
          switch (script->arg_types[i])
            {
            case SF_IMAGE:
            case SF_DRAWABLE:
            case SF_LAYER:
            case SF_CHANNEL:
            case SF_COLOR:
              break;

            case SF_VALUE:
            case SF_STRING:
            case SF_TEXT:
              g_free (script->arg_defaults[i].sfa_value);
              g_free (script->arg_values[i].sfa_value);
              break;

            case SF_ADJUSTMENT:
              break;

            case SF_FILENAME:
            case SF_DIRNAME:
              g_free (script->arg_defaults[i].sfa_file.filename);
              g_free (script->arg_values[i].sfa_file.filename);
              break;

            case SF_FONT:
              g_free (script->arg_defaults[i].sfa_font);
              g_free (script->arg_values[i].sfa_font);
              break;

            case SF_PALETTE:
              g_free (script->arg_defaults[i].sfa_palette);
              g_free (script->arg_values[i].sfa_palette);
              break;

            case SF_PATTERN:
              g_free (script->arg_defaults[i].sfa_pattern);
              g_free (script->arg_values[i].sfa_pattern);
              break;

            case SF_GRADIENT:
              g_free (script->arg_defaults[i].sfa_gradient);
              g_free (script->arg_values[i].sfa_gradient);
              break;

            case SF_BRUSH:
              g_free (script->arg_defaults[i].sfa_brush.name);
              g_free (script->arg_values[i].sfa_brush.name);
              break;

            case SF_OPTION:
              g_slist_foreach (script->arg_defaults[i].sfa_option.list,
                               (GFunc) g_free, NULL);
              g_slist_free (script->arg_defaults[i].sfa_option.list);
              break;

            default:
              break;
            }
        }

      g_free (script->arg_labels);
      g_free (script->arg_defaults);
      g_free (script->arg_types);
      g_free (script->arg_values);

      g_free (script);
    }
}

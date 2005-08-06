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

#include <errno.h>
#include <gdk/gdkkeysyms.h>

#include "libgimp/gimp.h"
#include "libgimp/gimpui.h"

#include "tiny-fu-intl.h"

#include "tinyscheme/scheme.h"
#include "ts-wrapper.h"
#include "tiny-fu-console.h"

#ifdef GIMP_PROC_BROWSER
#include <plug-ins/dbbrowser/gimpprocbrowser.h>
#endif


#define TEXT_WIDTH  480
#define TEXT_HEIGHT 400

#define BUFSIZE     256


typedef struct
{
  GtkTextBuffer *console;
  GtkWidget     *cc;
  GtkWidget     *text_view;

  gint32         input_id;
} ConsoleInterface;


enum TF_RESPONSES {
  RESPONSE_CLEAR, RESPONSE_SAVE
};

/*
 *  Local Functions
 */
static void       tiny_fu_console_interface  (void);
static void       tiny_fu_response           (GtkWidget    *widget,
                                              gint          response_id,
                                              gpointer      data);
static void       tiny_fu_save_dialog        (GtkWidget    *parent);
static void       tiny_fu_save_output        (GtkWidget    *dialog,
                                              gint        response_id,
                                              gpointer    data);
#ifdef GIMP_PROC_BROWSER
static void       tiny_fu_browse_callback    (GtkWidget    *widget,
                                              gpointer      data);
#endif
static gboolean   tiny_fu_cc_is_empty        (void);
static gboolean   tiny_fu_cc_key_function    (GtkWidget    *widget,
                                              GdkEventKey  *event,
                                              gpointer      data);

static void       tiny_fu_open_ts_console  (void);
static void       tiny_fu_close_ts_console (void);


/*
 *  Local variables
 */
static ConsoleInterface cint =
{
  NULL,  /*  console  */
  NULL,  /*  current command  */
  NULL,  /*  text view  */

  -1     /*  input id  */
};

static GList *history     = NULL;
static gint   history_len = 0;
static gint   history_cur = 0;
static gint   history_max = 50;

/*
 *  Function definitions
 */

void
tiny_fu_console_run (const gchar      *name,
                     gint              nparams,
                     const GimpParam  *params,
                     gint             *nreturn_vals,
                     GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;

  run_mode = params[0].data.d_int32;

  switch (run_mode)
    {
    case GIMP_RUN_INTERACTIVE:
      /*  Enable TinyScheme output  */
      tiny_fu_open_ts_console ();

      /*  Run the interface  */
      tiny_fu_console_interface ();

      /*  Clean up  */
      tiny_fu_close_ts_console ();
      break;

    case GIMP_RUN_WITH_LAST_VALS:
    case GIMP_RUN_NONINTERACTIVE:
      status = GIMP_PDB_CALLING_ERROR;
      g_message (_("Tiny-Fu console mode allows only interactive invocation"));
      break;

    default:
      break;
    }

  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = status;
}

static void
tiny_fu_console_interface (void)
{
  GtkWidget  *dialog;
  GtkWidget  *main_vbox;
  GtkWidget  *vbox;
#ifdef GIMP_PROC_BROWSER
  GtkWidget  *button;
#endif
  GtkWidget  *label;
  GtkWidget  *scrolled_window;
  GtkWidget  *hbox;

  gimp_ui_init ("tiny-fu", FALSE);

  dialog = gimp_dialog_new (_("Tiny-Fu Console"), "tiny-fu-console",
                            NULL, 0,
                            gimp_standard_help_func,
                            "plug-in-tiny-fu-console",

                            _("_Save Output"),  RESPONSE_SAVE,
                            _("Cl_ear Output"), RESPONSE_CLEAR,
                            GTK_STOCK_CLOSE,    GTK_RESPONSE_CLOSE,

                            NULL);

  g_signal_connect (dialog, "response",
                    G_CALLBACK (tiny_fu_response),
                    NULL);
  g_signal_connect (dialog, "destroy",
                    G_CALLBACK (gtk_widget_destroyed),
                    &dialog);

  /*  The main vbox  */
  main_vbox = gtk_vbox_new (FALSE, 12);
  gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 12);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), main_vbox,
                      TRUE, TRUE, 0);
  gtk_widget_show (main_vbox);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (main_vbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);

  label = gtk_label_new (_("TinyScheme Output"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  /*  The output text widget  */
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
  gtk_widget_show (scrolled_window);

  cint.console = gtk_text_buffer_new (NULL);
  cint.text_view = gtk_text_view_new_with_buffer (cint.console);
  g_object_unref (cint.console);

  gtk_text_view_set_editable (GTK_TEXT_VIEW (cint.text_view), FALSE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (cint.text_view), 12);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (cint.text_view), 12);
  gtk_widget_set_size_request (cint.text_view, TEXT_WIDTH, TEXT_HEIGHT);
  gtk_container_add (GTK_CONTAINER (scrolled_window), cint.text_view);
  gtk_widget_show (cint.text_view);

  gtk_text_buffer_create_tag (cint.console, "strong",
                              "weight", PANGO_WEIGHT_BOLD,
                              "size",   12 * PANGO_SCALE,
                              NULL);
  gtk_text_buffer_create_tag (cint.console, "emphasis",
                              "style",  PANGO_STYLE_OBLIQUE,
                              "size",   10 * PANGO_SCALE,
                              NULL);
  gtk_text_buffer_create_tag (cint.console, "weak",
                              "size",   10 * PANGO_SCALE,
                              NULL);

  {
    const gchar *greeting_texts[] =
    {
      "strong",   _("Welcome to TinyScheme\n"),
      "weak",     _("Copyright (c) Dimitrios Souflis\n\n"),
      "strong",   _("Tiny-Fu Console - "),
      "emphasis", _("Interactive Scheme Development"),
      NULL
    };

    GtkTextIter cursor;
    gint        i;

    gtk_text_buffer_get_end_iter (cint.console, &cursor);

    for (i = 0; greeting_texts[i]; i += 2)
      {
        gtk_text_buffer_insert_with_tags_by_name (cint.console, &cursor,
                                                  greeting_texts[i + 1], -1,
                                                  greeting_texts[i],
                                                  NULL);
      }
  }

  /*  The current command  */
  vbox = gtk_vbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (main_vbox), vbox, FALSE, FALSE, 0);
  gtk_widget_show (vbox);

  label = gtk_label_new (_("Current Command"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  cint.cc = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (hbox), cint.cc, TRUE, TRUE, 0);
  gtk_widget_grab_focus (cint.cc);
  gtk_widget_show (cint.cc);

  g_signal_connect (cint.cc, "key_press_event",
                    G_CALLBACK (tiny_fu_cc_key_function),
                    NULL);

#ifdef GIMP_PROC_BROWSER
  button = gtk_button_new_with_mnemonic (_("Browse..."));
  gtk_misc_set_padding (GTK_MISC (GTK_BIN (button)->child), 2, 0);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  gtk_widget_show (button);

  g_signal_connect (button, "clicked",
                    G_CALLBACK (tiny_fu_browse_callback),
                    NULL);
#endif

  /*  Initialize the history  */
  history     = g_list_append (history, NULL);
  history_len = 1;

  gtk_widget_show (dialog);

  gtk_main ();

  g_source_remove (cint.input_id);

  if (dialog)
    gtk_widget_destroy (dialog);
}

static void
tiny_fu_response (GtkWidget *widget,
                  gint       response_id,
                  gpointer   data)
{
  GtkTextIter start, end;

  switch (response_id)
  {
  case RESPONSE_CLEAR:
    gtk_text_buffer_get_start_iter (cint.console, &start);
    gtk_text_buffer_get_end_iter (cint.console, &end);
    gtk_text_buffer_delete (cint.console, &start, &end);
    break;
  case RESPONSE_SAVE:
    tiny_fu_save_dialog(widget);
    break;
  default:
    gtk_main_quit ();
    break;
  }
}


static void
tiny_fu_save_dialog(GtkWidget *parent)
{
  static GtkWidget *dialog = NULL;

  if (! dialog)
    {
       dialog =
         gtk_file_chooser_dialog_new (_("Save TinyScheme Output"),
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_SAVE,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                      NULL);

      g_signal_connect (dialog, "destroy",
                        G_CALLBACK (gtk_widget_destroyed),
                        &dialog);
      g_signal_connect (dialog, "response",
                        G_CALLBACK (tiny_fu_save_output),
                        NULL);
    }

  gtk_window_present (GTK_WINDOW (dialog));
}

static void
tiny_fu_save_output (GtkWidget *dialog,
                     gint       response_id,
                     gpointer   data)
{
  GtkTextIter start, end;

  if (response_id == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;
      gchar *str;
      FILE  *fh;

      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      fh = fopen (filename, "w");
      if (! fh)
        {
          gchar *message =
            g_strdup_printf ("Could not open '%s' for writing: %s",
                             gimp_filename_to_utf8 (filename),
                             g_strerror (errno));

          g_message (message);

          g_free (message);
          g_free (filename);

          return;
        }

      gtk_text_buffer_get_start_iter (cint.console, &start);
      gtk_text_buffer_get_end_iter (cint.console, &end);
      str = gtk_text_buffer_get_text (cint.console, &start, &end, TRUE);
      fputs (str, fh);
      fclose (fh);
    }

  gtk_widget_destroy (dialog);
}

#ifdef GIMP_PROC_BROWSER
static void
apply_callback (const gchar        *proc_name,
                const gchar        *scheme_proc_name,
                const gchar        *proc_blurb,
                const gchar        *proc_help,
                const gchar        *proc_author,
                const gchar        *proc_copyright,
                const gchar        *proc_date,
                GimpPDBProcType     proc_type,
                gint                n_params,
                gint                n_return_vals,
                const GimpParamDef *params,
                const GimpParamDef *return_vals)
{
  gint     i;
  GString *text;

  if (proc_name == NULL)
    return;

  text = g_string_new ("(");
  text = g_string_append (text, scheme_proc_name);
  for (i = 0; i < n_params; i++)
    {
      text = g_string_append_c (text, ' ');
      text = g_string_append (text, params[i].name);
    }

  text = g_string_append_c (text, ')');

  gtk_entry_set_text (GTK_ENTRY (cint.cc), text->str);
  g_string_free (text, TRUE);
}

static void
tiny_fu_browse_callback (GtkWidget *widget,
                           gpointer   data)
{
  gtk_quit_add_destroy (1, (GtkObject *)
                        gimp_proc_browser_dialog_new (apply_callback));
}
#endif

static gboolean
tiny_fu_console_idle_scroll_end (gpointer data)
{
  GtkAdjustment *adj = GTK_ADJUSTMENT (data);

  gtk_adjustment_set_value (adj, adj->upper - adj->page_size);

  return FALSE;
}

static void
tiny_fu_console_scroll_end (void)
{
  /*  the text view idle updates so we need to idle scroll too
   */
  g_idle_add (tiny_fu_console_idle_scroll_end,
              GTK_TEXT_VIEW (cint.text_view)->vadjustment);
}

void
tiny_fu_output_to_console (gchar *text)
{
  GtkTextIter cursor;
  
  gtk_text_buffer_get_end_iter (cint.console, &cursor);
  gtk_text_buffer_insert_with_tags_by_name (cint.console, &cursor,
                                            text, -1,
                                            "weak",
                                            NULL);
  
  tiny_fu_console_scroll_end ();
}
  
static gboolean
tiny_fu_cc_is_empty (void)
{
  const gchar *str;

  if ((str = gtk_entry_get_text (GTK_ENTRY (cint.cc))) == NULL)
    return TRUE;

  while (*str)
    {
      if (*str != ' ' && *str != '\t' && *str != '\n')
        return FALSE;

      str ++;
    }

  return TRUE;
}

static gboolean
tiny_fu_cc_key_function (GtkWidget   *widget,
                           GdkEventKey *event,
                           gpointer     data)
{
  GList       *list;
  gint         direction = 0;
  GtkTextIter  cursor;

  switch (event->keyval)
    {
    case GDK_Return:
      if (tiny_fu_cc_is_empty ())
        return TRUE;

      list = g_list_nth (history, (g_list_length (history) - 1));
      if (list->data)
        g_free (list->data);
      list->data = g_strdup (gtk_entry_get_text (GTK_ENTRY (cint.cc)));

      gtk_text_buffer_get_end_iter (cint.console, &cursor);

      gtk_text_buffer_insert_with_tags_by_name (cint.console, &cursor,
                                                "\n=> ", -1,
                                                "strong",
                                                NULL);

      gtk_text_buffer_insert_with_tags_by_name (cint.console, &cursor,
                                                gtk_entry_get_text (GTK_ENTRY (cint.cc)), -1,
                                                "weak",
                                                NULL);

      gtk_text_buffer_insert_with_tags_by_name (cint.console, &cursor,
                                                "\n", -1,
                                                "weak",
                                                NULL);

      tiny_fu_console_scroll_end ();

      gtk_entry_set_text (GTK_ENTRY (cint.cc), "");

      ts_interpret_string ((char *) list->data);

      gimp_displays_flush ();

      history = g_list_append (history, NULL);
      if (history_len == history_max)
        {
          history = g_list_remove (history, history->data);
          if (history->data)
            g_free (history->data);
        }
      else
        history_len++;
      history_cur = g_list_length (history) - 1;

      return TRUE;
      break;

    case GDK_KP_Up:
    case GDK_Up:
      direction = -1;
      break;

    case GDK_KP_Down:
    case GDK_Down:
      direction = 1;
      break;

    case GDK_P:
    case GDK_p:
      if (event->state & GDK_CONTROL_MASK)
        direction = -1;
      break;

    case GDK_N:
    case GDK_n:
      if (event->state & GDK_CONTROL_MASK)
        direction = 1;
      break;

    default:
      break;
    }

  if (direction)
    {
      /*  Make sure we keep track of the current one  */
      if (history_cur == g_list_length (history) - 1)
        {
          list = g_list_nth (history, history_cur);
          if (list->data)
            g_free (list->data);
          list->data = g_strdup (gtk_entry_get_text (GTK_ENTRY (cint.cc)));
        }

      history_cur += direction;
      if (history_cur < 0)
        history_cur = 0;
      if (history_cur >= history_len)
        history_cur = history_len - 1;

      gtk_entry_set_text (GTK_ENTRY (cint.cc),
                          (gchar *) (g_list_nth (history, history_cur))->data);

      gtk_editable_set_position (GTK_EDITABLE (cint.cc), -1);

      return TRUE;
    }

  return FALSE;
}

static void
tiny_fu_open_ts_console (void)
{
  ts_set_print_flag (1);
  ts_set_console_mode (1);
  ts_set_verbose_level (2);
  ts_print_welcome ();
}

static void
tiny_fu_close_ts_console (void)
{
  FILE *output_file;

  ts_set_print_flag (0);
  output_file = ts_get_output_file ();

  if (output_file != stdout)
    fclose (output_file);

  ts_set_console_mode (0);
}

void
tiny_fu_eval_run (const gchar      *name,
                  gint              nparams,
                  const GimpParam  *params,
                  gint             *nreturn_vals,
                  GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode   run_mode;

  run_mode = params[0].data.d_int32;

  switch (run_mode)
    {
    case GIMP_RUN_NONINTERACTIVE:
      /*  Disable Tiny-Fu output  */
      ts_set_output_file (stdout);
      ts_set_verbose_level (0);
      if (ts_interpret_string (params[1].data.d_string) != 0)
        status = GIMP_PDB_EXECUTION_ERROR;
      break;

    case GIMP_RUN_INTERACTIVE:
    case GIMP_RUN_WITH_LAST_VALS:
      status = GIMP_PDB_CALLING_ERROR;
      g_message (_("Tiny-Fu evaluate mode allows only noninteractive invocation"));
      break;

    default:
      break;
    }

  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = status;
}

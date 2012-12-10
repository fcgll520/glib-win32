/*
 * Copyright (C) 2009 - 2011 Vivien Malerba <malerba@gnome-db.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __GDA_THREAD_WRAPPER_H__
#define __GDA_THREAD_WRAPPER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GDA_TYPE_THREAD_WRAPPER            (gda_thread_wrapper_get_type())
#define GDA_THREAD_WRAPPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST (obj, GDA_TYPE_THREAD_WRAPPER, GdaThreadWrapper))
#define GDA_THREAD_WRAPPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST (klass, GDA_TYPE_THREAD_WRAPPER, GdaThreadWrapperClass))
#define GDA_IS_THREAD_WRAPPER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE(obj, GDA_TYPE_THREAD_WRAPPER))
#define GDA_IS_THREAD_WRAPPER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDA_TYPE_THREAD_WRAPPER))
#define GDA_THREAD_WRAPPER_GET_CLASS(o)    (G_TYPE_INSTANCE_GET_CLASS ((o), GDA_TYPE_THREAD_WRAPPER, GdaThreadWrapperClass))

typedef struct _GdaThreadWrapper GdaThreadWrapper;
typedef struct _GdaThreadWrapperClass GdaThreadWrapperClass;
typedef struct _GdaThreadWrapperPrivate GdaThreadWrapperPrivate;

/* error reporting */
extern GQuark gda_thread_wrapper_error_quark (void);
#define GDA_THREAD_WRAPPER_ERROR gda_thread_wrapper_error_quark ()

typedef enum {
	GDA_THREAD_WRAPPER_UNKNOWN_ERROR
} GdaThreadWrapperError;

struct _GdaThreadWrapper {
	GObject            object;
	GdaThreadWrapperPrivate *priv;
};

struct _GdaThreadWrapperClass {
	GObjectClass       object_class;

	/*< private >*/
	/* Padding for future expansion */
	void (*_gda_reserved1) (void);
	void (*_gda_reserved2) (void);
	void (*_gda_reserved3) (void);
	void (*_gda_reserved4) (void);
};

/**
 * GdaThreadNotificationType:
 * @GDA_THREAD_NOTIFICATION_SIGNAL: the notification regards a signal
 * @GDA_THREAD_NOTIFICATION_JOB: the notification regards a job finished
 *
 * Defines the kind of notification which can be obtained when reading from te #GIOChannel
 * returned by gda_thread_wrapper_get_io_channel().
 */
typedef enum {
	GDA_THREAD_NOTIFICATION_JOB    = 0x01,
	GDA_THREAD_NOTIFICATION_SIGNAL = 0x02
} GdaThreadNotificationType;

/**
 * GdaThreadNotification:
 * @type: the notification type
 * @job_id: the job ID, if @type is a #GDA_THREAD_NOTIFICATION_JOB
 *
 * A notification to be read through the #GIOChannel which is returned by gda_thread_wrapper_get_io_channel(),
 * for example:
 * <programlisting><![CDATA[
 * gboolean
 * wrapper_ioc_cb (GIOChannel *source, GIOCondition condition, gpointer data)
 * {
 *     GIOStatus status;
 *     gsize nread;
 *     GdaThreadNotification notif;
 *     if (condition & G_IO_IN) {
 *	   status = g_io_channel_read_chars (source, (gchar*) &notif, sizeof (notif), &nread, NULL);
 *         if ((status != G_IO_STATUS_NORMAL) || (nread != sizeof (notif)))
 *             goto onerror;
 *	   switch (notif.type) {
 *	   case GDA_THREAD_NOTIFICATION_JOB:
 *             check_for_wrapper_result (bcnc);
 *             break;
 *         case GDA_THREAD_NOTIFICATION_SIGNAL:
 *             gda_thread_wrapper_iterate (bcnc->priv->wrapper, FALSE);
 *             break;
 *         default:
 *             goto onerror;
 *             break;
 *	   }
 *   }
 *   if (condition & (G_IO_ERR | G_IO_HUP | G_IO_NVAL))
 *             goto onerror;
 *   return TRUE; // keep callback
 *
 * onerror:
 *   g_io_channel_shutdown (bcnc->priv->ioc, FALSE, NULL);
 *   return FALSE; // removed callback
 * }
 *
 * {
 * [...]
 *     GIOChannel *ioc;
 *     ioc = gda_thread_wrapper_get_io_channel (wrapper);
 *     if (!ioc)
 *         [handle error]
 *     else {
 *         guint watch_id;
 *         watch_id = g_io_add_watch (ioc, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
 *                                    (GIOFunc) wrapper_ioc_cb, NULL);
 *     }
 * }
 * ]]></programlisting>
 */
typedef struct {
	GdaThreadNotificationType type;
	guint                     job_id;
} GdaThreadNotification;

/**
 * GdaThreadWrapperFunc:
 * @arg: pointer to the data (which is the @arg argument passed to gda_thread_wrapper_execute_void())
 * @error: a place to store errors
 * @Returns: a pointer to some data which will be returned by gda_thread_wrapper_fetch_result()
 *
 * Specifies the type of function to be passed to gda_thread_wrapper_execute().
 */
typedef gpointer (*GdaThreadWrapperFunc) (gpointer arg, GError **error);

/**
 * GdaThreadWrapperVoidFunc:
 * @arg: a pointer to the data (which is the @arg argument passed to gda_thread_wrapper_execute_void())
 * @error: a place to store errors
 *
 * Specifies the type of function to be passed to gda_thread_wrapper_execute_void().
 */
typedef void (*GdaThreadWrapperVoidFunc) (gpointer arg, GError **error);

/**
 * GdaThreadWrapperCallback:
 * @wrapper: the #GdaThreadWrapper
 * @instance: a pointer to the instance which emitted the signal
 * @signame: the name of the signal being emitted
 * @n_param_values: number of GValue in @param_values
 * @param_values: array of @n_param_values GValue
 * @gda_reserved: reserved
 * @data: a pointer to the data (which is the @data argument passed to gda_thread_wrapper_connect_raw())
 *
 * Specifies the type of function to be passed to gda_thread_wrapper_connect_raw()
 */
typedef void (*GdaThreadWrapperCallback) (GdaThreadWrapper *wrapper, gpointer instance, const gchar *signame,
					  gint n_param_values, const GValue *param_values, gpointer gda_reserved,
					  gpointer data);

/**
 * SECTION:gda-thread-wrapper
 * @short_description: Execute functions in a sub thread
 * @title: GdaThreadWrapper
 * @stability: Stable
 * @see_also:
 *
 * The purpose of the #GdaThreadWrapper object is to execute functions in an isolated sub thread. As the
 * #GdaThreadWrapper is thread safe, one is able to isolate some code's execution is a <emphasis>private</emphasis>
 * <emphasis>worker</emphasis> thread, and make a non thread safe code thread safe.
 *
 * The downside of this is that the actual execution of the code will be slower as it requires
 * threads to be synchronized.
 *
 * The #GdaThreadWrapper implements its own locking mechanism and can safely be used from multiple
 * threads at once without needing further locking.
 *
 * Each thread using a #GdaThreadWrapper object can use it as if it was the only user: the #GdaThreadWrapper will
 * simply dispatch all the execution requests to its private <emphasis>worker</emphasis> thread and report the
 * execution's status only to the thread which made the request.
 *
 * The user can also specify a callback function to be called when an object exmits a signal while being
 * used by the worker thread, see the gda_thread_wrapper_connect_raw() method.
 *
 * The following diagram illustrates the conceptual working of the #GdaThreadWrapper object: here two user threads
 * are represented (assigned a red and green colors), both using a single #GdaThreadWrapper, so in this diagram, 3 threads
 * are present. The communication between the threads are handled by some #GAsyncQueue objects (in a transparent way for
 * the user, presented here only for illustration purposes). The queue represented in yellow is where jobs are
 * pushed by each user thread (step 1), and popped by the worker thread (step 2). Once the worker thread has finished
 * with a job, it stores the result along with the job and pushes it to the queue dedicated to the user thread
 * (step 3) in this example the red queue (because the job was issued from the thread represented in red). The last
 * step is when the user fetches the result (in its user thread), step 4.
 *
 * If, when the worker thread is busy with a job, a signal is emitted, and if the user has set up a signal handler
 * using gda_thread_wrapper_connect_raw(),
 * then a "job as signal" is created by the worker thread and pushed to the user thread as illustrated
 * at the bottom of the diagram.
 * <mediaobject>
 *   <imageobject role="html">
 *     <imagedata fileref="thread-wrapper.png" format="PNG" contentwidth="170mm"/>
 *   </imageobject>
 *   <textobject>
 *     <phrase>GdaThreadWrapper's conceptual working</phrase>
 *   </textobject>
 * </mediaobject>
 *
 * It's the user's responsability to regularly check if a submitted job has completed
 * (using gda_thread_wrapper_fetch_result()) or if a signal
 * has been emitted by an object while in the worker thread and needs to be handled by the #GdaThreadWrapper
 * to call the associated callback (using gda_thread_wrapper_iterate() or automatically done when
 * calling gda_thread_wrapper_fetch_result()).
 *
 * However, when a main loop is available, the #GdaThreadWrapper can emit notifications through a
 * #GIOChannel which can be intregated in the main loop. The #GIOChannel can be created
 * using gda_thread_wrapper_get_io_channel().
 */

GType                  gda_thread_wrapper_get_type          (void) G_GNUC_CONST;
GdaThreadWrapper      *gda_thread_wrapper_new               (void);
GIOChannel            *gda_thread_wrapper_get_io_channel    (GdaThreadWrapper *wrapper);
void                   gda_thread_wrapper_unset_io_channel  (GdaThreadWrapper *wrapper);

guint                  gda_thread_wrapper_execute           (GdaThreadWrapper *wrapper, GdaThreadWrapperFunc func,
							     gpointer arg, GDestroyNotify arg_destroy_func, GError **error);
guint                  gda_thread_wrapper_execute_void      (GdaThreadWrapper *wrapper, GdaThreadWrapperVoidFunc func,
							     gpointer arg, GDestroyNotify arg_destroy_func, GError **error);
gboolean               gda_thread_wrapper_cancel            (GdaThreadWrapper *wrapper, guint id);
void                   gda_thread_wrapper_iterate           (GdaThreadWrapper *wrapper, gboolean may_block);
gpointer               gda_thread_wrapper_fetch_result      (GdaThreadWrapper *wrapper, gboolean may_lock, 
							     guint exp_id, GError **error);

gint                   gda_thread_wrapper_get_waiting_size (GdaThreadWrapper *wrapper);

gulong                 gda_thread_wrapper_connect_raw       (GdaThreadWrapper *wrapper,
							     gpointer instance,
							     const gchar *sig_name,
							     gboolean private_thread, gboolean private_job,
							     GdaThreadWrapperCallback callback,
							     gpointer data);
void                   gda_thread_wrapper_disconnect       (GdaThreadWrapper *wrapper, gulong id);
void                   gda_thread_wrapper_steal_signal     (GdaThreadWrapper *wrapper, gulong id);
G_END_DECLS

#endif

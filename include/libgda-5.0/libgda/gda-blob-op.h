/*
 * Copyright (C) 2007 Armin Burgmeier <armin@openismus.com>
 * Copyright (C) 2007 - 2011 Vivien Malerba <malerba@gnome-db.org>
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

#ifndef __GDA_BLOB_OP_H__
#define __GDA_BLOB_OP_H__

#include <glib.h>
#include <glib-object.h>
#include <libgda/gda-decl.h>
#include <libgda/gda-value.h>

G_BEGIN_DECLS

#define GDA_TYPE_BLOB_OP            (gda_blob_op_get_type())
#define GDA_BLOB_OP(obj)            (G_TYPE_CHECK_INSTANCE_CAST (obj, GDA_TYPE_BLOB_OP, GdaBlobOp))
#define GDA_BLOB_OP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST (klass, GDA_TYPE_BLOB_OP, GdaBlobOpClass))
#define GDA_IS_BLOB_OP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE(obj, GDA_TYPE_BLOB_OP))
#define GDA_IS_BLOB_OP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GDA_TYPE_BLOB_OP))

struct _GdaBlobOp {
	GObject object;

	/* Padding for future expansion */
	gpointer _gda_reserved1;
};

struct _GdaBlobOpClass {
	GObjectClass parent_class;

	/* Virtual methods */
	glong    (* get_length) (GdaBlobOp *op);
	glong    (* read)       (GdaBlobOp *op, GdaBlob *blob, glong offset, glong size);
	glong    (* write)      (GdaBlobOp *op, GdaBlob *blob, glong offset);
	gboolean (* write_all)  (GdaBlobOp *op, GdaBlob *blob);

	/*< private >*/
	/* Padding for future expansion */
	void (*_gda_reserved1) (void);
	void (*_gda_reserved2) (void);
	void (*_gda_reserved3) (void);
	void (*_gda_reserved4) (void);
};

/**
 * SECTION:gda-blob-op
 * @short_description: Binary data and BLOBs handling
 * @title: Blobs
 * @stability: Stable
 * @see_also:
 *
 * This object is a base class for individual database providers which support BLOB types. It supports
 * operations to read and write data in a BLOB value (of type GDA_BLOB_TYPE).
 *
 * Libgda offers two methods to manipulate binary values as two containers: <link linkend="GdaBinary">GdaBinary</link>
 * and <link linkend="GdaBlob">GdaBlob</link>:
 * <itemizedlist>
 *  <listitem><para>When reading from a data model returned by &LIBGDA; binary data will often be in a GdaBlob
 *   object, and the associated <link linkend="GdaBlobOp">GdaBlobOp</link> object can be used to manipulate the
 *   binary object (in a database for example)</para></listitem>
 *  <listitem><para>When the binary value is created by the user, then there is no point in using a GdaBlob as
 *   there can not be any <link linkend="GdaBlobOp">GdaBlobOp</link> object, so the GdaBinary container is
 *   enough.</para></listitem>
 * </itemizedlist>
 *
 * Note that a <link linkend="GdaBlob">GdaBlob</link> value (the "data" attribute) will often not contain any data
 * (or only some part of the actual BLOB)
 * and that it's up to the user to use the associated <link linkend="GdaBlobOp">GdaBlobOp</link> object to
 * "load" the data into the container (into the actual process heap).
 *
 * For example to load the 1st 40 bytes of a blob:
 *  <programlisting>
 *GValue *blob_value = ...
 *GdaBlob *blob;
 *
 *blob = (GdaBlob*) gda_value_get_blob (blob_value);
 *gda_blob_op_read (blob->op, blob, 0, 40);
 *  </programlisting>
 *
 * Another example is to write the contents of a blob to a file on disk, using a special
 *  <link linkend="GdaBlobOp">GdaBlobOp</link> object (internal to &LIBGDA; which interfaces
 *  with a file in a filesystem):
 *  <programlisting>
 *GValue *blob_value; // value to copy from
 *GValue *tmp_value;
 *GdaBlob *file_blob;
 *
 *GValue *blob_value = ...
 *tmp_value = gda_value_new_blob_from_file ("MyFile.bin");
 *file_blob = (GdaBlob*) gda_value_get_blob (tmp_value);
 *
 *if (! gda_blob_op_write_all (file_blob->op, gda_value_get_blob (blob_value))) {
 *       // error
 *}
 *else {
 *       gsize size;
 *       size = gda_blob_op_get_length (file_blob->op);
 *       g_print ("Wrote %s, size = %d\n", filename, size);
 *}
 *gda_value_free (tmp_value);
 *  </programlisting>
 *
 * For further information, see:
 *  <itemizedlist>
 *    <listitem><para>the section about <link linkend="gen_blobs">Binary large objects (BLOBs)</link>'s
 *        abstraction</para></listitem>
 *    <listitem><para><link linkend="libgda-provider-blobop">Virtual methods for Blob operations</link>
 *        section for more information
 *        about how to implement the virtual methods when creating a database provider</para></listitem>
 *  </itemizedlist>
 */

GType    gda_blob_op_get_type  (void) G_GNUC_CONST;

glong    gda_blob_op_get_length (GdaBlobOp *op);
glong    gda_blob_op_read       (GdaBlobOp *op, GdaBlob *blob, glong offset, glong size);
gboolean gda_blob_op_read_all   (GdaBlobOp *op, GdaBlob *blob);
glong    gda_blob_op_write      (GdaBlobOp *op, GdaBlob *blob, glong offset);
gboolean gda_blob_op_write_all  (GdaBlobOp *op, GdaBlob *blob);

G_END_DECLS

#endif


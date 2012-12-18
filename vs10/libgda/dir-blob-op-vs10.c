/*
 * Copyright (C) 2007 - 2011 Vivien Malerba <malerba@gnome-db.org>
 * Copyright (C) 2010 David King <davidk@openismus.com>
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

#include <string.h>
#define __GDA_INTERNAL__
#include "dir-blob-op.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib/gstdio.h>

struct _GdaDirBlobOpPrivate {
	gchar *complete_filename;
};

static void gda_dir_blob_op_class_init (GdaDirBlobOpClass *klass);
static void gda_dir_blob_op_init       (GdaDirBlobOp *blob,
					  GdaDirBlobOpClass *klass);
static void gda_dir_blob_op_finalize   (GObject *object);

static glong gda_dir_blob_op_get_length (GdaBlobOp *op);
static glong gda_dir_blob_op_read       (GdaBlobOp *op, GdaBlob *blob, glong offset, glong size);
static glong gda_dir_blob_op_write      (GdaBlobOp *op, GdaBlob *blob, glong offset);

static GObjectClass *parent_class = NULL;

/*
 * Object init and finalize
 */
GType
_gda_dir_blob_op_get_type (void)
{
	static GType type = 0;

	if (G_UNLIKELY (type == 0)) {
		static GStaticMutex registering = G_STATIC_MUTEX_INIT;
		static const GTypeInfo info = {
			sizeof (GdaDirBlobOpClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) gda_dir_blob_op_class_init,
			NULL,
			NULL,
			sizeof (GdaDirBlobOp),
			0,
			(GInstanceInitFunc) gda_dir_blob_op_init,
			0
		};
		g_static_mutex_lock (&registering);
		if (type == 0)
			type = g_type_register_static (GDA_TYPE_BLOB_OP, "GdaDirBlobOp", &info, 0);
		g_static_mutex_unlock (&registering);
	}
	return type;
}

static void
gda_dir_blob_op_init (GdaDirBlobOp *op,
			   G_GNUC_UNUSED GdaDirBlobOpClass *klass)
{
	g_return_if_fail (GDA_IS_DIR_BLOB_OP (op));

	op->priv = g_new0 (GdaDirBlobOpPrivate, 1);
	op->priv->complete_filename = NULL;
}

static void
gda_dir_blob_op_class_init (GdaDirBlobOpClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GdaBlobOpClass *blob_class = GDA_BLOB_OP_CLASS (klass);

	parent_class = g_type_class_peek_parent (klass);

	object_class->finalize = gda_dir_blob_op_finalize;
	blob_class->get_length = gda_dir_blob_op_get_length;
	blob_class->read = gda_dir_blob_op_read;
	blob_class->write = gda_dir_blob_op_write;
}


static void
gda_dir_blob_op_finalize (GObject * object)
{
	GdaDirBlobOp *pgop = (GdaDirBlobOp *) object;

	g_return_if_fail (GDA_IS_DIR_BLOB_OP (pgop));

	g_free (pgop->priv->complete_filename);
	g_free (pgop->priv);
	pgop->priv = NULL;

	parent_class->finalize (object);
}

/**
 * _gda_dir_blob_op_new
 */
GdaBlobOp *
_gda_dir_blob_op_new (const gchar *complete_filename)
{
	GdaDirBlobOp *pgop;

	g_return_val_if_fail (complete_filename, NULL);

	pgop = g_object_new (GDA_TYPE_DIR_BLOB_OP, NULL);
	pgop->priv->complete_filename = g_strdup (complete_filename);
	
	return GDA_BLOB_OP (pgop);
}

/**
 * _gda_dir_blob_set_filename
 */
void
_gda_dir_blob_set_filename (GdaDirBlobOp *blob, const gchar *complete_filename)
{
	g_return_if_fail (GDA_IS_DIR_BLOB_OP (blob));
	g_return_if_fail (blob->priv);
	g_return_if_fail (complete_filename);

	g_free (blob->priv->complete_filename);
	blob->priv->complete_filename = g_strdup (complete_filename);
}

/**
 * _gda_dir_blob_get_filename
 */
const gchar *
_gda_dir_blob_get_filename (GdaDirBlobOp *blob)
{
	g_return_val_if_fail (GDA_IS_DIR_BLOB_OP (blob), NULL);
	g_return_val_if_fail (blob->priv, NULL);

	return blob->priv->complete_filename;
}

/*
 * Virtual functions
 */
static glong
gda_dir_blob_op_get_length (GdaBlobOp *op)
{
	GdaDirBlobOp *dirop;
	struct stat filestat;

	g_return_val_if_fail (GDA_IS_DIR_BLOB_OP (op), -1);
	dirop = GDA_DIR_BLOB_OP (op);
	g_return_val_if_fail (dirop->priv, -1);

	if (! g_stat (dirop->priv->complete_filename, &filestat)) 
		return filestat.st_size;
	else
		return -1;
}

static glong
gda_dir_blob_op_read (GdaBlobOp *op, GdaBlob *blob, glong offset, glong size)
{
	GdaDirBlobOp *dirop;
	GdaBinary *bin;
	FILE *file;
	size_t nread;

	g_return_val_if_fail (GDA_IS_DIR_BLOB_OP (op), -1);
	dirop = GDA_DIR_BLOB_OP (op);
	g_return_val_if_fail (dirop->priv, -1);
	if (offset >= G_MAXINT)
		return -1;
	g_return_val_if_fail (blob, -1);

	/* open file */
	file = fopen (dirop->priv->complete_filename, "rb"); /* Flawfinder: ignore */
	if (!file)
		return -1;
	
	/* go to offset */
	if (fseek (file, offset, SEEK_SET) != 0) {
		fclose (file);
		return -1;
	}
	
	bin = (GdaBinary *) blob;
	if (bin->data) {
		g_free (bin->data);
		bin->data = NULL;
	}
	bin->data = g_new0 (guchar, size);
	nread = fread ((char *) (bin->data), 1, size, file);
	bin->binary_length = nread;
	fclose (file);

	return nread;
}

static glong
gda_dir_blob_op_write (GdaBlobOp *op, GdaBlob *blob, glong offset)
{
	GdaDirBlobOp *dirop;
	GdaBinary *bin;
	FILE *file;
	glong nbwritten;

	g_return_val_if_fail (GDA_IS_DIR_BLOB_OP (op), -1);
	dirop = GDA_DIR_BLOB_OP (op);
	g_return_val_if_fail (dirop->priv, -1);
	if (offset >= G_MAXINT)
		return -1;
	g_return_val_if_fail (blob, -1);

	/* open file */
	file = fopen (dirop->priv->complete_filename, "w+"); /* Flawfinder: ignore */
	if (!file)
		return -1;
	
	/* go to offset */
	if (offset > 0) {
		if (fseek (file, offset, SEEK_SET) != 0) {
			fclose (file);
			return -1;
		}
	}

	if (blob->op && (blob->op != op)) {
		/* use data through blob->op */
                #define buf_size 16384
		gint nread = 0;
		GdaBlob *tmpblob = g_new0 (GdaBlob, 1);
		gda_blob_set_op (tmpblob, blob->op);

		nbwritten = 0;

		for (nread = gda_blob_op_read (tmpblob->op, tmpblob, 0, buf_size);
		     nread > 0;
		     nread = gda_blob_op_read (tmpblob->op, tmpblob, nbwritten, buf_size)) {
			GdaBinary *bin = (GdaBinary *) tmpblob;
			glong tmp_written;
			tmp_written = fwrite ((char *) (bin->data), sizeof (guchar), bin->binary_length, file);
			if (tmp_written < bin->binary_length) {
				/* error writing stream */
				gda_blob_free ((gpointer) tmpblob);
				return -1;
			}
			nbwritten += tmp_written;
			if (nread < buf_size)
				/* nothing more to read */
				break;
		}
		fclose (file);
		gda_blob_free ((gpointer) tmpblob);
	}
	else {
		bin = (GdaBinary *) blob;
		nbwritten = fwrite ((char *) (bin->data), 1, bin->binary_length, file);
		fclose (file);
	}

	return (nbwritten >= 0) ? nbwritten : -1;
}

/*
 * Copyright (C) 2005 Dan Winship <danw@src.gnome.org>
 * Copyright (C) 2005 - 2011 Vivien Malerba <malerba@gnome-db.org>
 * Copyright (C) 2005 �lvaro Pe�a <alvaropg@telefonica.net>
 * Copyright (C) 2007 Murray Cumming <murrayc@murrayc.com>
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

#ifndef __GDA_SERVER_PROVIDER_PRIVATE__
#define __GDA_SERVER_PROVIDER_PRIVATE__

#include <libgda/gda-meta-store.h>

struct _GdaServerProviderPrivate {
	GHashTable    *data_handlers; /* key = a GdaServerProviderHandlerInfo pointer, value = a GdaDataHandler */
	GdaSqlParser  *parser;
};

G_END_DECLS

#endif




/* Oren - a streaming media software development kit.
 *
 * Copyright © 2012 TinySoft, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __OREN_SIGNALBUS_H__
#define __OREN_SIGNALBUS_H__

#include "oren-dctypes.h"
#include <gimo.h>

G_BEGIN_DECLS

GType oren_dclogin_bus_get_type (void) G_GNUC_CONST;
GimoSignalBus* oren_dclogin_get_bus (OrenDCLogin *self, GimoContext *context);

G_END_DECLS

#endif /* __OREN_SIGNALBUS_H__ */

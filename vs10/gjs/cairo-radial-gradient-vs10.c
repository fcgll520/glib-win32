/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/* Copyright 2010 litl, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <config.h>

#include <gjs/gjs-module.h>
#include <gjs/compat.h>
#include <cairo.h>
#include "cairo-private.h"

GJS_DEFINE_PROTO("CairoRadialGradient", cairo_radial_gradient, 1)

GJS_NATIVE_CONSTRUCTOR_DECLARE(cairo_radial_gradient)
{
    GJS_NATIVE_CONSTRUCTOR_VARIABLES(cairo_radial_gradient)
    double cx0, cy0, radius0, cx1, cy1, radius1;
    cairo_pattern_t *pattern;

    GJS_NATIVE_CONSTRUCTOR_PRELUDE(cairo_radial_gradient);

    if (!gjs_parse_args(context, "RadialGradient", "ffffff", argc, argv,
                        "cx0", &cx0,
                        "cy0", &cy0,
                        "radius0", &radius0,
                        "cx1", &cx1,
                        "cy1", &cy1,
                        "radius1", &radius1))
        return JS_FALSE;

    pattern = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);

    if (!gjs_cairo_check_status(context, cairo_pattern_status(pattern), "pattern"))
        return JS_FALSE;

    gjs_cairo_pattern_construct(context, object, pattern);
    cairo_pattern_destroy(pattern);

    GJS_NATIVE_CONSTRUCTOR_FINISH(cairo_radial_gradient);

    return JS_TRUE;
}

static void
gjs_cairo_radial_gradient_finalize(JSContext *context,
                                   JSObject  *obj)
{
    gjs_cairo_pattern_finalize_pattern(context, obj);
}

static JSPropertySpec gjs_cairo_radial_gradient_proto_props[] = {
    { NULL }
};

static JSFunctionSpec gjs_cairo_radial_gradient_proto_funcs[] = {
    // getRadialCircles
    { NULL }
};

JSObject *
gjs_cairo_radial_gradient_from_pattern(JSContext       *context,
                                       cairo_pattern_t *pattern)
{
    JSObject *object;

    g_return_val_if_fail(context != NULL, NULL);
    g_return_val_if_fail(pattern != NULL, NULL);
    g_return_val_if_fail(cairo_pattern_get_type(pattern) == CAIRO_PATTERN_TYPE_RADIAL, NULL);

    object = JS_NewObject(context, &gjs_cairo_radial_gradient_class, NULL, NULL);
    if (!object) {
        gjs_throw(context, "failed to create radial gradient pattern");
        return NULL;
    }

    gjs_cairo_pattern_construct(context, object, pattern);

    return object;
}


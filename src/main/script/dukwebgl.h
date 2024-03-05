/**
 * Copyright (c) 2018 Mika Rautio
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *  
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 */

#if !defined(DUK_EXTERNAL_DECL) || !defined(DUK_LOCAL)
#error "Duktape constants not found. Duktape header must be included before dukwebgl!"
#endif

#if !defined(DUK_VERSION) || (DUK_VERSION < 20000)
#error "Duktape 2.0.0 is required in minimum"
#endif

#if !defined(DUKWEBGL_H_INCLUDED)
#define DUKWEBGL_H_INCLUDED

/*
 *  Avoid C++ name mangling
 */

#if defined(__cplusplus)
extern "C" {
#endif

DUK_EXTERNAL_DECL void dukwebgl_bind(duk_context *ctx);

/*
 *  C++ name mangling
 */

#if defined(__cplusplus)
/* end 'extern "C"' wrapper */
}
#endif

#endif /* DUKWEBGL_H_INCLUDED */

#if defined(DUKWEBGL_IMPLEMENTATION)

#include <stdlib.h>

#if !defined(GL_NONE)
#error "OpenGL constants not found. OpenGL header must be included before dukwebgl!"
#endif

/* GLEW does not define GL_VERSION_1_0 but glcorearb.h does */
#if defined(GLEW_VERSION_1_1) && !defined(GL_VERSION_1_0)
#define GL_VERSION_1_0
#endif

#define dukwebgl_bind_function(ctx, c_function_name, js_function_name, argument_count)     duk_push_c_function((ctx), dukwebgl_##c_function_name, (argument_count));     duk_put_prop_string((ctx), -2, #js_function_name)

#define dukwebgl_push_constant_property(ctx, webgl_constant)     duk_push_uint((ctx), (GL_##webgl_constant));     duk_put_prop_string((ctx), -2, #webgl_constant)

DUK_LOCAL void dukwebgl_bind_constants(duk_context *ctx) {
    
#ifdef GL_VERSION_0_0
#endif /* GL_VERSION_0_0 */ 

#ifdef GL_VERSION_1_0
#ifdef GL_READ_BUFFER
    dukwebgl_push_constant_property(ctx, READ_BUFFER);
#endif
#ifdef GL_UNPACK_ROW_LENGTH
    dukwebgl_push_constant_property(ctx, UNPACK_ROW_LENGTH);
#endif
#ifdef GL_UNPACK_SKIP_ROWS
    dukwebgl_push_constant_property(ctx, UNPACK_SKIP_ROWS);
#endif
#ifdef GL_UNPACK_SKIP_PIXELS
    dukwebgl_push_constant_property(ctx, UNPACK_SKIP_PIXELS);
#endif
#ifdef GL_PACK_ROW_LENGTH
    dukwebgl_push_constant_property(ctx, PACK_ROW_LENGTH);
#endif
#ifdef GL_PACK_SKIP_ROWS
    dukwebgl_push_constant_property(ctx, PACK_SKIP_ROWS);
#endif
#ifdef GL_PACK_SKIP_PIXELS
    dukwebgl_push_constant_property(ctx, PACK_SKIP_PIXELS);
#endif
#ifdef GL_COLOR
    dukwebgl_push_constant_property(ctx, COLOR);
#endif
#ifdef GL_DEPTH
    dukwebgl_push_constant_property(ctx, DEPTH);
#endif
#ifdef GL_STENCIL
    dukwebgl_push_constant_property(ctx, STENCIL);
#endif
#ifdef GL_RED
    dukwebgl_push_constant_property(ctx, RED);
#endif
#ifdef GL_DEPTH_BUFFER_BIT
    dukwebgl_push_constant_property(ctx, DEPTH_BUFFER_BIT);
#endif
#ifdef GL_STENCIL_BUFFER_BIT
    dukwebgl_push_constant_property(ctx, STENCIL_BUFFER_BIT);
#endif
#ifdef GL_COLOR_BUFFER_BIT
    dukwebgl_push_constant_property(ctx, COLOR_BUFFER_BIT);
#endif
#ifdef GL_POINTS
    dukwebgl_push_constant_property(ctx, POINTS);
#endif
#ifdef GL_LINES
    dukwebgl_push_constant_property(ctx, LINES);
#endif
#ifdef GL_LINE_LOOP
    dukwebgl_push_constant_property(ctx, LINE_LOOP);
#endif
#ifdef GL_LINE_STRIP
    dukwebgl_push_constant_property(ctx, LINE_STRIP);
#endif
#ifdef GL_TRIANGLES
    dukwebgl_push_constant_property(ctx, TRIANGLES);
#endif
#ifdef GL_TRIANGLE_STRIP
    dukwebgl_push_constant_property(ctx, TRIANGLE_STRIP);
#endif
#ifdef GL_TRIANGLE_FAN
    dukwebgl_push_constant_property(ctx, TRIANGLE_FAN);
#endif
#ifdef GL_ZERO
    dukwebgl_push_constant_property(ctx, ZERO);
#endif
#ifdef GL_ONE
    dukwebgl_push_constant_property(ctx, ONE);
#endif
#ifdef GL_SRC_COLOR
    dukwebgl_push_constant_property(ctx, SRC_COLOR);
#endif
#ifdef GL_ONE_MINUS_SRC_COLOR
    dukwebgl_push_constant_property(ctx, ONE_MINUS_SRC_COLOR);
#endif
#ifdef GL_SRC_ALPHA
    dukwebgl_push_constant_property(ctx, SRC_ALPHA);
#endif
#ifdef GL_ONE_MINUS_SRC_ALPHA
    dukwebgl_push_constant_property(ctx, ONE_MINUS_SRC_ALPHA);
#endif
#ifdef GL_DST_ALPHA
    dukwebgl_push_constant_property(ctx, DST_ALPHA);
#endif
#ifdef GL_ONE_MINUS_DST_ALPHA
    dukwebgl_push_constant_property(ctx, ONE_MINUS_DST_ALPHA);
#endif
#ifdef GL_DST_COLOR
    dukwebgl_push_constant_property(ctx, DST_COLOR);
#endif
#ifdef GL_ONE_MINUS_DST_COLOR
    dukwebgl_push_constant_property(ctx, ONE_MINUS_DST_COLOR);
#endif
#ifdef GL_SRC_ALPHA_SATURATE
    dukwebgl_push_constant_property(ctx, SRC_ALPHA_SATURATE);
#endif
#ifdef GL_FRONT
    dukwebgl_push_constant_property(ctx, FRONT);
#endif
#ifdef GL_BACK
    dukwebgl_push_constant_property(ctx, BACK);
#endif
#ifdef GL_FRONT_AND_BACK
    dukwebgl_push_constant_property(ctx, FRONT_AND_BACK);
#endif
#ifdef GL_CULL_FACE
    dukwebgl_push_constant_property(ctx, CULL_FACE);
#endif
#ifdef GL_BLEND
    dukwebgl_push_constant_property(ctx, BLEND);
#endif
#ifdef GL_DITHER
    dukwebgl_push_constant_property(ctx, DITHER);
#endif
#ifdef GL_STENCIL_TEST
    dukwebgl_push_constant_property(ctx, STENCIL_TEST);
#endif
#ifdef GL_DEPTH_TEST
    dukwebgl_push_constant_property(ctx, DEPTH_TEST);
#endif
#ifdef GL_SCISSOR_TEST
    dukwebgl_push_constant_property(ctx, SCISSOR_TEST);
#endif
#ifdef GL_NO_ERROR
    dukwebgl_push_constant_property(ctx, NO_ERROR);
#endif
#ifdef GL_INVALID_ENUM
    dukwebgl_push_constant_property(ctx, INVALID_ENUM);
#endif
#ifdef GL_INVALID_VALUE
    dukwebgl_push_constant_property(ctx, INVALID_VALUE);
#endif
#ifdef GL_INVALID_OPERATION
    dukwebgl_push_constant_property(ctx, INVALID_OPERATION);
#endif
#ifdef GL_OUT_OF_MEMORY
    dukwebgl_push_constant_property(ctx, OUT_OF_MEMORY);
#endif
#ifdef GL_CW
    dukwebgl_push_constant_property(ctx, CW);
#endif
#ifdef GL_CCW
    dukwebgl_push_constant_property(ctx, CCW);
#endif
#ifdef GL_LINE_WIDTH
    dukwebgl_push_constant_property(ctx, LINE_WIDTH);
#endif
#ifdef GL_CULL_FACE_MODE
    dukwebgl_push_constant_property(ctx, CULL_FACE_MODE);
#endif
#ifdef GL_FRONT_FACE
    dukwebgl_push_constant_property(ctx, FRONT_FACE);
#endif
#ifdef GL_DEPTH_RANGE
    dukwebgl_push_constant_property(ctx, DEPTH_RANGE);
#endif
#ifdef GL_DEPTH_WRITEMASK
    dukwebgl_push_constant_property(ctx, DEPTH_WRITEMASK);
#endif
#ifdef GL_DEPTH_CLEAR_VALUE
    dukwebgl_push_constant_property(ctx, DEPTH_CLEAR_VALUE);
#endif
#ifdef GL_DEPTH_FUNC
    dukwebgl_push_constant_property(ctx, DEPTH_FUNC);
#endif
#ifdef GL_STENCIL_CLEAR_VALUE
    dukwebgl_push_constant_property(ctx, STENCIL_CLEAR_VALUE);
#endif
#ifdef GL_STENCIL_FUNC
    dukwebgl_push_constant_property(ctx, STENCIL_FUNC);
#endif
#ifdef GL_STENCIL_FAIL
    dukwebgl_push_constant_property(ctx, STENCIL_FAIL);
#endif
#ifdef GL_STENCIL_PASS_DEPTH_FAIL
    dukwebgl_push_constant_property(ctx, STENCIL_PASS_DEPTH_FAIL);
#endif
#ifdef GL_STENCIL_PASS_DEPTH_PASS
    dukwebgl_push_constant_property(ctx, STENCIL_PASS_DEPTH_PASS);
#endif
#ifdef GL_STENCIL_REF
    dukwebgl_push_constant_property(ctx, STENCIL_REF);
#endif
#ifdef GL_STENCIL_VALUE_MASK
    dukwebgl_push_constant_property(ctx, STENCIL_VALUE_MASK);
#endif
#ifdef GL_STENCIL_WRITEMASK
    dukwebgl_push_constant_property(ctx, STENCIL_WRITEMASK);
#endif
#ifdef GL_VIEWPORT
    dukwebgl_push_constant_property(ctx, VIEWPORT);
#endif
#ifdef GL_SCISSOR_BOX
    dukwebgl_push_constant_property(ctx, SCISSOR_BOX);
#endif
#ifdef GL_COLOR_CLEAR_VALUE
    dukwebgl_push_constant_property(ctx, COLOR_CLEAR_VALUE);
#endif
#ifdef GL_COLOR_WRITEMASK
    dukwebgl_push_constant_property(ctx, COLOR_WRITEMASK);
#endif
#ifdef GL_UNPACK_ALIGNMENT
    dukwebgl_push_constant_property(ctx, UNPACK_ALIGNMENT);
#endif
#ifdef GL_PACK_ALIGNMENT
    dukwebgl_push_constant_property(ctx, PACK_ALIGNMENT);
#endif
#ifdef GL_MAX_TEXTURE_SIZE
    dukwebgl_push_constant_property(ctx, MAX_TEXTURE_SIZE);
#endif
#ifdef GL_MAX_VIEWPORT_DIMS
    dukwebgl_push_constant_property(ctx, MAX_VIEWPORT_DIMS);
#endif
#ifdef GL_SUBPIXEL_BITS
    dukwebgl_push_constant_property(ctx, SUBPIXEL_BITS);
#endif
#ifdef GL_DONT_CARE
    dukwebgl_push_constant_property(ctx, DONT_CARE);
#endif
#ifdef GL_FASTEST
    dukwebgl_push_constant_property(ctx, FASTEST);
#endif
#ifdef GL_NICEST
    dukwebgl_push_constant_property(ctx, NICEST);
#endif
#ifdef GL_BYTE
    dukwebgl_push_constant_property(ctx, BYTE);
#endif
#ifdef GL_UNSIGNED_BYTE
    dukwebgl_push_constant_property(ctx, UNSIGNED_BYTE);
#endif
#ifdef GL_SHORT
    dukwebgl_push_constant_property(ctx, SHORT);
#endif
#ifdef GL_UNSIGNED_SHORT
    dukwebgl_push_constant_property(ctx, UNSIGNED_SHORT);
#endif
#ifdef GL_INT
    dukwebgl_push_constant_property(ctx, INT);
#endif
#ifdef GL_UNSIGNED_INT
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT);
#endif
#ifdef GL_FLOAT
    dukwebgl_push_constant_property(ctx, FLOAT);
#endif
#ifdef GL_DEPTH_COMPONENT
    dukwebgl_push_constant_property(ctx, DEPTH_COMPONENT);
#endif
#ifdef GL_ALPHA
    dukwebgl_push_constant_property(ctx, ALPHA);
#endif
#ifdef GL_RGB
    dukwebgl_push_constant_property(ctx, RGB);
#endif
#ifdef GL_RGBA
    dukwebgl_push_constant_property(ctx, RGBA);
#endif
#ifdef GL_NEVER
    dukwebgl_push_constant_property(ctx, NEVER);
#endif
#ifdef GL_LESS
    dukwebgl_push_constant_property(ctx, LESS);
#endif
#ifdef GL_EQUAL
    dukwebgl_push_constant_property(ctx, EQUAL);
#endif
#ifdef GL_LEQUAL
    dukwebgl_push_constant_property(ctx, LEQUAL);
#endif
#ifdef GL_GREATER
    dukwebgl_push_constant_property(ctx, GREATER);
#endif
#ifdef GL_NOTEQUAL
    dukwebgl_push_constant_property(ctx, NOTEQUAL);
#endif
#ifdef GL_GEQUAL
    dukwebgl_push_constant_property(ctx, GEQUAL);
#endif
#ifdef GL_ALWAYS
    dukwebgl_push_constant_property(ctx, ALWAYS);
#endif
#ifdef GL_KEEP
    dukwebgl_push_constant_property(ctx, KEEP);
#endif
#ifdef GL_REPLACE
    dukwebgl_push_constant_property(ctx, REPLACE);
#endif
#ifdef GL_INCR
    dukwebgl_push_constant_property(ctx, INCR);
#endif
#ifdef GL_DECR
    dukwebgl_push_constant_property(ctx, DECR);
#endif
#ifdef GL_INVERT
    dukwebgl_push_constant_property(ctx, INVERT);
#endif
#ifdef GL_VENDOR
    dukwebgl_push_constant_property(ctx, VENDOR);
#endif
#ifdef GL_RENDERER
    dukwebgl_push_constant_property(ctx, RENDERER);
#endif
#ifdef GL_VERSION
    dukwebgl_push_constant_property(ctx, VERSION);
#endif
#ifdef GL_NEAREST
    dukwebgl_push_constant_property(ctx, NEAREST);
#endif
#ifdef GL_LINEAR
    dukwebgl_push_constant_property(ctx, LINEAR);
#endif
#ifdef GL_NEAREST_MIPMAP_NEAREST
    dukwebgl_push_constant_property(ctx, NEAREST_MIPMAP_NEAREST);
#endif
#ifdef GL_LINEAR_MIPMAP_NEAREST
    dukwebgl_push_constant_property(ctx, LINEAR_MIPMAP_NEAREST);
#endif
#ifdef GL_NEAREST_MIPMAP_LINEAR
    dukwebgl_push_constant_property(ctx, NEAREST_MIPMAP_LINEAR);
#endif
#ifdef GL_LINEAR_MIPMAP_LINEAR
    dukwebgl_push_constant_property(ctx, LINEAR_MIPMAP_LINEAR);
#endif
#ifdef GL_TEXTURE_MAG_FILTER
    dukwebgl_push_constant_property(ctx, TEXTURE_MAG_FILTER);
#endif
#ifdef GL_TEXTURE_MIN_FILTER
    dukwebgl_push_constant_property(ctx, TEXTURE_MIN_FILTER);
#endif
#ifdef GL_TEXTURE_WRAP_S
    dukwebgl_push_constant_property(ctx, TEXTURE_WRAP_S);
#endif
#ifdef GL_TEXTURE_WRAP_T
    dukwebgl_push_constant_property(ctx, TEXTURE_WRAP_T);
#endif
#ifdef GL_TEXTURE_2D
    dukwebgl_push_constant_property(ctx, TEXTURE_2D);
#endif
#ifdef GL_TEXTURE
    dukwebgl_push_constant_property(ctx, TEXTURE);
#endif
#ifdef GL_REPEAT
    dukwebgl_push_constant_property(ctx, REPEAT);
#endif
#ifdef GL_STENCIL_INDEX
    dukwebgl_push_constant_property(ctx, STENCIL_INDEX);
#endif
#ifdef GL_NONE
    dukwebgl_push_constant_property(ctx, NONE);
#endif
#endif /* GL_VERSION_1_0 */ 

#ifdef GL_VERSION_1_1
#ifdef GL_RGB8
    dukwebgl_push_constant_property(ctx, RGB8);
#endif
#ifdef GL_RGBA8
    dukwebgl_push_constant_property(ctx, RGBA8);
#endif
#ifdef GL_RGB10_A2
    dukwebgl_push_constant_property(ctx, RGB10_A2);
#endif
#ifdef GL_POLYGON_OFFSET_FILL
    dukwebgl_push_constant_property(ctx, POLYGON_OFFSET_FILL);
#endif
#ifdef GL_POLYGON_OFFSET_UNITS
    dukwebgl_push_constant_property(ctx, POLYGON_OFFSET_UNITS);
#endif
#ifdef GL_POLYGON_OFFSET_FACTOR
    dukwebgl_push_constant_property(ctx, POLYGON_OFFSET_FACTOR);
#endif
#ifdef GL_TEXTURE_BINDING_2D
    dukwebgl_push_constant_property(ctx, TEXTURE_BINDING_2D);
#endif
#ifdef GL_RGBA4
    dukwebgl_push_constant_property(ctx, RGBA4);
#endif
#ifdef GL_RGB5_A1
    dukwebgl_push_constant_property(ctx, RGB5_A1);
#endif
#endif /* GL_VERSION_1_1 */ 

#ifdef GL_VERSION_1_2
#ifdef GL_TEXTURE_BINDING_3D
    dukwebgl_push_constant_property(ctx, TEXTURE_BINDING_3D);
#endif
#ifdef GL_UNPACK_SKIP_IMAGES
    dukwebgl_push_constant_property(ctx, UNPACK_SKIP_IMAGES);
#endif
#ifdef GL_UNPACK_IMAGE_HEIGHT
    dukwebgl_push_constant_property(ctx, UNPACK_IMAGE_HEIGHT);
#endif
#ifdef GL_TEXTURE_3D
    dukwebgl_push_constant_property(ctx, TEXTURE_3D);
#endif
#ifdef GL_TEXTURE_WRAP_R
    dukwebgl_push_constant_property(ctx, TEXTURE_WRAP_R);
#endif
#ifdef GL_MAX_3D_TEXTURE_SIZE
    dukwebgl_push_constant_property(ctx, MAX_3D_TEXTURE_SIZE);
#endif
#ifdef GL_UNSIGNED_INT_2_10_10_10_REV
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_2_10_10_10_REV);
#endif
#ifdef GL_MAX_ELEMENTS_VERTICES
    dukwebgl_push_constant_property(ctx, MAX_ELEMENTS_VERTICES);
#endif
#ifdef GL_MAX_ELEMENTS_INDICES
    dukwebgl_push_constant_property(ctx, MAX_ELEMENTS_INDICES);
#endif
#ifdef GL_TEXTURE_MIN_LOD
    dukwebgl_push_constant_property(ctx, TEXTURE_MIN_LOD);
#endif
#ifdef GL_TEXTURE_MAX_LOD
    dukwebgl_push_constant_property(ctx, TEXTURE_MAX_LOD);
#endif
#ifdef GL_TEXTURE_BASE_LEVEL
    dukwebgl_push_constant_property(ctx, TEXTURE_BASE_LEVEL);
#endif
#ifdef GL_TEXTURE_MAX_LEVEL
    dukwebgl_push_constant_property(ctx, TEXTURE_MAX_LEVEL);
#endif
#ifdef GL_ALIASED_LINE_WIDTH_RANGE
    dukwebgl_push_constant_property(ctx, ALIASED_LINE_WIDTH_RANGE);
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4
    dukwebgl_push_constant_property(ctx, UNSIGNED_SHORT_4_4_4_4);
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1
    dukwebgl_push_constant_property(ctx, UNSIGNED_SHORT_5_5_5_1);
#endif
#ifdef GL_UNSIGNED_SHORT_5_6_5
    dukwebgl_push_constant_property(ctx, UNSIGNED_SHORT_5_6_5);
#endif
#ifdef GL_CLAMP_TO_EDGE
    dukwebgl_push_constant_property(ctx, CLAMP_TO_EDGE);
#endif
#endif /* GL_VERSION_1_2 */ 

#ifdef GL_VERSION_1_3
#ifdef GL_SAMPLE_ALPHA_TO_COVERAGE
    dukwebgl_push_constant_property(ctx, SAMPLE_ALPHA_TO_COVERAGE);
#endif
#ifdef GL_SAMPLE_COVERAGE
    dukwebgl_push_constant_property(ctx, SAMPLE_COVERAGE);
#endif
#ifdef GL_SAMPLE_BUFFERS
    dukwebgl_push_constant_property(ctx, SAMPLE_BUFFERS);
#endif
#ifdef GL_SAMPLES
    dukwebgl_push_constant_property(ctx, SAMPLES);
#endif
#ifdef GL_SAMPLE_COVERAGE_VALUE
    dukwebgl_push_constant_property(ctx, SAMPLE_COVERAGE_VALUE);
#endif
#ifdef GL_SAMPLE_COVERAGE_INVERT
    dukwebgl_push_constant_property(ctx, SAMPLE_COVERAGE_INVERT);
#endif
#ifdef GL_COMPRESSED_TEXTURE_FORMATS
    dukwebgl_push_constant_property(ctx, COMPRESSED_TEXTURE_FORMATS);
#endif
#ifdef GL_TEXTURE_CUBE_MAP
    dukwebgl_push_constant_property(ctx, TEXTURE_CUBE_MAP);
#endif
#ifdef GL_TEXTURE_BINDING_CUBE_MAP
    dukwebgl_push_constant_property(ctx, TEXTURE_BINDING_CUBE_MAP);
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_X
    dukwebgl_push_constant_property(ctx, TEXTURE_CUBE_MAP_POSITIVE_X);
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    dukwebgl_push_constant_property(ctx, TEXTURE_CUBE_MAP_NEGATIVE_X);
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    dukwebgl_push_constant_property(ctx, TEXTURE_CUBE_MAP_POSITIVE_Y);
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    dukwebgl_push_constant_property(ctx, TEXTURE_CUBE_MAP_NEGATIVE_Y);
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    dukwebgl_push_constant_property(ctx, TEXTURE_CUBE_MAP_POSITIVE_Z);
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    dukwebgl_push_constant_property(ctx, TEXTURE_CUBE_MAP_NEGATIVE_Z);
#endif
#ifdef GL_MAX_CUBE_MAP_TEXTURE_SIZE
    dukwebgl_push_constant_property(ctx, MAX_CUBE_MAP_TEXTURE_SIZE);
#endif
#ifdef GL_TEXTURE0
    dukwebgl_push_constant_property(ctx, TEXTURE0);
#endif
#ifdef GL_TEXTURE1
    dukwebgl_push_constant_property(ctx, TEXTURE1);
#endif
#ifdef GL_TEXTURE2
    dukwebgl_push_constant_property(ctx, TEXTURE2);
#endif
#ifdef GL_TEXTURE3
    dukwebgl_push_constant_property(ctx, TEXTURE3);
#endif
#ifdef GL_TEXTURE4
    dukwebgl_push_constant_property(ctx, TEXTURE4);
#endif
#ifdef GL_TEXTURE5
    dukwebgl_push_constant_property(ctx, TEXTURE5);
#endif
#ifdef GL_TEXTURE6
    dukwebgl_push_constant_property(ctx, TEXTURE6);
#endif
#ifdef GL_TEXTURE7
    dukwebgl_push_constant_property(ctx, TEXTURE7);
#endif
#ifdef GL_TEXTURE8
    dukwebgl_push_constant_property(ctx, TEXTURE8);
#endif
#ifdef GL_TEXTURE9
    dukwebgl_push_constant_property(ctx, TEXTURE9);
#endif
#ifdef GL_TEXTURE10
    dukwebgl_push_constant_property(ctx, TEXTURE10);
#endif
#ifdef GL_TEXTURE11
    dukwebgl_push_constant_property(ctx, TEXTURE11);
#endif
#ifdef GL_TEXTURE12
    dukwebgl_push_constant_property(ctx, TEXTURE12);
#endif
#ifdef GL_TEXTURE13
    dukwebgl_push_constant_property(ctx, TEXTURE13);
#endif
#ifdef GL_TEXTURE14
    dukwebgl_push_constant_property(ctx, TEXTURE14);
#endif
#ifdef GL_TEXTURE15
    dukwebgl_push_constant_property(ctx, TEXTURE15);
#endif
#ifdef GL_TEXTURE16
    dukwebgl_push_constant_property(ctx, TEXTURE16);
#endif
#ifdef GL_TEXTURE17
    dukwebgl_push_constant_property(ctx, TEXTURE17);
#endif
#ifdef GL_TEXTURE18
    dukwebgl_push_constant_property(ctx, TEXTURE18);
#endif
#ifdef GL_TEXTURE19
    dukwebgl_push_constant_property(ctx, TEXTURE19);
#endif
#ifdef GL_TEXTURE20
    dukwebgl_push_constant_property(ctx, TEXTURE20);
#endif
#ifdef GL_TEXTURE21
    dukwebgl_push_constant_property(ctx, TEXTURE21);
#endif
#ifdef GL_TEXTURE22
    dukwebgl_push_constant_property(ctx, TEXTURE22);
#endif
#ifdef GL_TEXTURE23
    dukwebgl_push_constant_property(ctx, TEXTURE23);
#endif
#ifdef GL_TEXTURE24
    dukwebgl_push_constant_property(ctx, TEXTURE24);
#endif
#ifdef GL_TEXTURE25
    dukwebgl_push_constant_property(ctx, TEXTURE25);
#endif
#ifdef GL_TEXTURE26
    dukwebgl_push_constant_property(ctx, TEXTURE26);
#endif
#ifdef GL_TEXTURE27
    dukwebgl_push_constant_property(ctx, TEXTURE27);
#endif
#ifdef GL_TEXTURE28
    dukwebgl_push_constant_property(ctx, TEXTURE28);
#endif
#ifdef GL_TEXTURE29
    dukwebgl_push_constant_property(ctx, TEXTURE29);
#endif
#ifdef GL_TEXTURE30
    dukwebgl_push_constant_property(ctx, TEXTURE30);
#endif
#ifdef GL_TEXTURE31
    dukwebgl_push_constant_property(ctx, TEXTURE31);
#endif
#ifdef GL_ACTIVE_TEXTURE
    dukwebgl_push_constant_property(ctx, ACTIVE_TEXTURE);
#endif
#endif /* GL_VERSION_1_3 */ 

#ifdef GL_VERSION_1_4
#ifdef GL_MIN
    dukwebgl_push_constant_property(ctx, MIN);
#endif
#ifdef GL_MAX
    dukwebgl_push_constant_property(ctx, MAX);
#endif
#ifdef GL_DEPTH_COMPONENT24
    dukwebgl_push_constant_property(ctx, DEPTH_COMPONENT24);
#endif
#ifdef GL_MAX_TEXTURE_LOD_BIAS
    dukwebgl_push_constant_property(ctx, MAX_TEXTURE_LOD_BIAS);
#endif
#ifdef GL_TEXTURE_COMPARE_MODE
    dukwebgl_push_constant_property(ctx, TEXTURE_COMPARE_MODE);
#endif
#ifdef GL_TEXTURE_COMPARE_FUNC
    dukwebgl_push_constant_property(ctx, TEXTURE_COMPARE_FUNC);
#endif
#ifdef GL_FUNC_ADD
    dukwebgl_push_constant_property(ctx, FUNC_ADD);
#endif
#ifdef GL_BLEND_EQUATION
    dukwebgl_push_constant_property(ctx, BLEND_EQUATION);
#endif
#ifdef GL_FUNC_SUBTRACT
    dukwebgl_push_constant_property(ctx, FUNC_SUBTRACT);
#endif
#ifdef GL_FUNC_REVERSE_SUBTRACT
    dukwebgl_push_constant_property(ctx, FUNC_REVERSE_SUBTRACT);
#endif
#ifdef GL_BLEND_DST_RGB
    dukwebgl_push_constant_property(ctx, BLEND_DST_RGB);
#endif
#ifdef GL_BLEND_SRC_RGB
    dukwebgl_push_constant_property(ctx, BLEND_SRC_RGB);
#endif
#ifdef GL_BLEND_DST_ALPHA
    dukwebgl_push_constant_property(ctx, BLEND_DST_ALPHA);
#endif
#ifdef GL_BLEND_SRC_ALPHA
    dukwebgl_push_constant_property(ctx, BLEND_SRC_ALPHA);
#endif
#ifdef GL_CONSTANT_COLOR
    dukwebgl_push_constant_property(ctx, CONSTANT_COLOR);
#endif
#ifdef GL_ONE_MINUS_CONSTANT_COLOR
    dukwebgl_push_constant_property(ctx, ONE_MINUS_CONSTANT_COLOR);
#endif
#ifdef GL_CONSTANT_ALPHA
    dukwebgl_push_constant_property(ctx, CONSTANT_ALPHA);
#endif
#ifdef GL_ONE_MINUS_CONSTANT_ALPHA
    dukwebgl_push_constant_property(ctx, ONE_MINUS_CONSTANT_ALPHA);
#endif
#ifdef GL_BLEND_COLOR
    dukwebgl_push_constant_property(ctx, BLEND_COLOR);
#endif
#ifdef GL_INCR_WRAP
    dukwebgl_push_constant_property(ctx, INCR_WRAP);
#endif
#ifdef GL_DECR_WRAP
    dukwebgl_push_constant_property(ctx, DECR_WRAP);
#endif
#ifdef GL_MIRRORED_REPEAT
    dukwebgl_push_constant_property(ctx, MIRRORED_REPEAT);
#endif
#ifdef GL_DEPTH_COMPONENT16
    dukwebgl_push_constant_property(ctx, DEPTH_COMPONENT16);
#endif
#endif /* GL_VERSION_1_4 */ 

#ifdef GL_VERSION_1_5
#ifdef GL_CURRENT_QUERY
    dukwebgl_push_constant_property(ctx, CURRENT_QUERY);
#endif
#ifdef GL_QUERY_RESULT
    dukwebgl_push_constant_property(ctx, QUERY_RESULT);
#endif
#ifdef GL_QUERY_RESULT_AVAILABLE
    dukwebgl_push_constant_property(ctx, QUERY_RESULT_AVAILABLE);
#endif
#ifdef GL_STREAM_READ
    dukwebgl_push_constant_property(ctx, STREAM_READ);
#endif
#ifdef GL_STREAM_COPY
    dukwebgl_push_constant_property(ctx, STREAM_COPY);
#endif
#ifdef GL_STATIC_READ
    dukwebgl_push_constant_property(ctx, STATIC_READ);
#endif
#ifdef GL_STATIC_COPY
    dukwebgl_push_constant_property(ctx, STATIC_COPY);
#endif
#ifdef GL_DYNAMIC_READ
    dukwebgl_push_constant_property(ctx, DYNAMIC_READ);
#endif
#ifdef GL_DYNAMIC_COPY
    dukwebgl_push_constant_property(ctx, DYNAMIC_COPY);
#endif
#ifdef GL_ARRAY_BUFFER
    dukwebgl_push_constant_property(ctx, ARRAY_BUFFER);
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER
    dukwebgl_push_constant_property(ctx, ELEMENT_ARRAY_BUFFER);
#endif
#ifdef GL_ARRAY_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, ARRAY_BUFFER_BINDING);
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, ELEMENT_ARRAY_BUFFER_BINDING);
#endif
#ifdef GL_STREAM_DRAW
    dukwebgl_push_constant_property(ctx, STREAM_DRAW);
#endif
#ifdef GL_STATIC_DRAW
    dukwebgl_push_constant_property(ctx, STATIC_DRAW);
#endif
#ifdef GL_DYNAMIC_DRAW
    dukwebgl_push_constant_property(ctx, DYNAMIC_DRAW);
#endif
#ifdef GL_BUFFER_SIZE
    dukwebgl_push_constant_property(ctx, BUFFER_SIZE);
#endif
#ifdef GL_BUFFER_USAGE
    dukwebgl_push_constant_property(ctx, BUFFER_USAGE);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
#endif
#endif /* GL_VERSION_1_5 */ 

#ifdef GL_VERSION_2_0
#ifdef GL_MAX_DRAW_BUFFERS
    dukwebgl_push_constant_property(ctx, MAX_DRAW_BUFFERS);
#endif
#ifdef GL_DRAW_BUFFER0
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER0);
#endif
#ifdef GL_DRAW_BUFFER1
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER1);
#endif
#ifdef GL_DRAW_BUFFER2
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER2);
#endif
#ifdef GL_DRAW_BUFFER3
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER3);
#endif
#ifdef GL_DRAW_BUFFER4
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER4);
#endif
#ifdef GL_DRAW_BUFFER5
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER5);
#endif
#ifdef GL_DRAW_BUFFER6
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER6);
#endif
#ifdef GL_DRAW_BUFFER7
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER7);
#endif
#ifdef GL_DRAW_BUFFER8
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER8);
#endif
#ifdef GL_DRAW_BUFFER9
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER9);
#endif
#ifdef GL_DRAW_BUFFER10
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER10);
#endif
#ifdef GL_DRAW_BUFFER11
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER11);
#endif
#ifdef GL_DRAW_BUFFER12
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER12);
#endif
#ifdef GL_DRAW_BUFFER13
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER13);
#endif
#ifdef GL_DRAW_BUFFER14
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER14);
#endif
#ifdef GL_DRAW_BUFFER15
    dukwebgl_push_constant_property(ctx, DRAW_BUFFER15);
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_FRAGMENT_UNIFORM_COMPONENTS);
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_VERTEX_UNIFORM_COMPONENTS);
#endif
#ifdef GL_SAMPLER_3D
    dukwebgl_push_constant_property(ctx, SAMPLER_3D);
#endif
#ifdef GL_SAMPLER_2D_SHADOW
    dukwebgl_push_constant_property(ctx, SAMPLER_2D_SHADOW);
#endif
#ifdef GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    dukwebgl_push_constant_property(ctx, FRAGMENT_SHADER_DERIVATIVE_HINT);
#endif
#ifdef GL_BLEND_EQUATION_RGB
    dukwebgl_push_constant_property(ctx, BLEND_EQUATION_RGB);
#endif
#ifdef GL_BLEND_EQUATION_ALPHA
    dukwebgl_push_constant_property(ctx, BLEND_EQUATION_ALPHA);
#endif
#ifdef GL_CURRENT_VERTEX_ATTRIB
    dukwebgl_push_constant_property(ctx, CURRENT_VERTEX_ATTRIB);
#endif
#ifdef GL_STENCIL_BACK_FUNC
    dukwebgl_push_constant_property(ctx, STENCIL_BACK_FUNC);
#endif
#ifdef GL_STENCIL_BACK_FAIL
    dukwebgl_push_constant_property(ctx, STENCIL_BACK_FAIL);
#endif
#ifdef GL_STENCIL_BACK_PASS_DEPTH_FAIL
    dukwebgl_push_constant_property(ctx, STENCIL_BACK_PASS_DEPTH_FAIL);
#endif
#ifdef GL_STENCIL_BACK_PASS_DEPTH_PASS
    dukwebgl_push_constant_property(ctx, STENCIL_BACK_PASS_DEPTH_PASS);
#endif
#ifdef GL_STENCIL_BACK_REF
    dukwebgl_push_constant_property(ctx, STENCIL_BACK_REF);
#endif
#ifdef GL_STENCIL_BACK_VALUE_MASK
    dukwebgl_push_constant_property(ctx, STENCIL_BACK_VALUE_MASK);
#endif
#ifdef GL_STENCIL_BACK_WRITEMASK
    dukwebgl_push_constant_property(ctx, STENCIL_BACK_WRITEMASK);
#endif
#ifdef GL_FRAGMENT_SHADER
    dukwebgl_push_constant_property(ctx, FRAGMENT_SHADER);
#endif
#ifdef GL_VERTEX_SHADER
    dukwebgl_push_constant_property(ctx, VERTEX_SHADER);
#endif
#ifdef GL_MAX_VERTEX_ATTRIBS
    dukwebgl_push_constant_property(ctx, MAX_VERTEX_ATTRIBS);
#endif
#ifdef GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    dukwebgl_push_constant_property(ctx, MAX_COMBINED_TEXTURE_IMAGE_UNITS);
#endif
#ifdef GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
    dukwebgl_push_constant_property(ctx, MAX_VERTEX_TEXTURE_IMAGE_UNITS);
#endif
#ifdef GL_MAX_TEXTURE_IMAGE_UNITS
    dukwebgl_push_constant_property(ctx, MAX_TEXTURE_IMAGE_UNITS);
#endif
#ifdef GL_SHADER_TYPE
    dukwebgl_push_constant_property(ctx, SHADER_TYPE);
#endif
#ifdef GL_DELETE_STATUS
    dukwebgl_push_constant_property(ctx, DELETE_STATUS);
#endif
#ifdef GL_LINK_STATUS
    dukwebgl_push_constant_property(ctx, LINK_STATUS);
#endif
#ifdef GL_VALIDATE_STATUS
    dukwebgl_push_constant_property(ctx, VALIDATE_STATUS);
#endif
#ifdef GL_ATTACHED_SHADERS
    dukwebgl_push_constant_property(ctx, ATTACHED_SHADERS);
#endif
#ifdef GL_ACTIVE_UNIFORMS
    dukwebgl_push_constant_property(ctx, ACTIVE_UNIFORMS);
#endif
#ifdef GL_ACTIVE_ATTRIBUTES
    dukwebgl_push_constant_property(ctx, ACTIVE_ATTRIBUTES);
#endif
#ifdef GL_SHADING_LANGUAGE_VERSION
    dukwebgl_push_constant_property(ctx, SHADING_LANGUAGE_VERSION);
#endif
#ifdef GL_CURRENT_PROGRAM
    dukwebgl_push_constant_property(ctx, CURRENT_PROGRAM);
#endif
#ifdef GL_FLOAT_VEC2
    dukwebgl_push_constant_property(ctx, FLOAT_VEC2);
#endif
#ifdef GL_FLOAT_VEC3
    dukwebgl_push_constant_property(ctx, FLOAT_VEC3);
#endif
#ifdef GL_FLOAT_VEC4
    dukwebgl_push_constant_property(ctx, FLOAT_VEC4);
#endif
#ifdef GL_INT_VEC2
    dukwebgl_push_constant_property(ctx, INT_VEC2);
#endif
#ifdef GL_INT_VEC3
    dukwebgl_push_constant_property(ctx, INT_VEC3);
#endif
#ifdef GL_INT_VEC4
    dukwebgl_push_constant_property(ctx, INT_VEC4);
#endif
#ifdef GL_BOOL
    dukwebgl_push_constant_property(ctx, BOOL);
#endif
#ifdef GL_BOOL_VEC2
    dukwebgl_push_constant_property(ctx, BOOL_VEC2);
#endif
#ifdef GL_BOOL_VEC3
    dukwebgl_push_constant_property(ctx, BOOL_VEC3);
#endif
#ifdef GL_BOOL_VEC4
    dukwebgl_push_constant_property(ctx, BOOL_VEC4);
#endif
#ifdef GL_FLOAT_MAT2
    dukwebgl_push_constant_property(ctx, FLOAT_MAT2);
#endif
#ifdef GL_FLOAT_MAT3
    dukwebgl_push_constant_property(ctx, FLOAT_MAT3);
#endif
#ifdef GL_FLOAT_MAT4
    dukwebgl_push_constant_property(ctx, FLOAT_MAT4);
#endif
#ifdef GL_SAMPLER_2D
    dukwebgl_push_constant_property(ctx, SAMPLER_2D);
#endif
#ifdef GL_SAMPLER_CUBE
    dukwebgl_push_constant_property(ctx, SAMPLER_CUBE);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_ENABLED
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_ENABLED);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_SIZE
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_SIZE);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_STRIDE
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_STRIDE);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_TYPE
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_TYPE);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_NORMALIZED);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_POINTER
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_POINTER);
#endif
#ifdef GL_COMPILE_STATUS
    dukwebgl_push_constant_property(ctx, COMPILE_STATUS);
#endif
#endif /* GL_VERSION_2_0 */ 

#ifdef GL_VERSION_2_1
#ifdef GL_PIXEL_PACK_BUFFER
    dukwebgl_push_constant_property(ctx, PIXEL_PACK_BUFFER);
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER
    dukwebgl_push_constant_property(ctx, PIXEL_UNPACK_BUFFER);
#endif
#ifdef GL_PIXEL_PACK_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, PIXEL_PACK_BUFFER_BINDING);
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, PIXEL_UNPACK_BUFFER_BINDING);
#endif
#ifdef GL_FLOAT_MAT2x3
    dukwebgl_push_constant_property(ctx, FLOAT_MAT2x3);
#endif
#ifdef GL_FLOAT_MAT2x4
    dukwebgl_push_constant_property(ctx, FLOAT_MAT2x4);
#endif
#ifdef GL_FLOAT_MAT3x2
    dukwebgl_push_constant_property(ctx, FLOAT_MAT3x2);
#endif
#ifdef GL_FLOAT_MAT3x4
    dukwebgl_push_constant_property(ctx, FLOAT_MAT3x4);
#endif
#ifdef GL_FLOAT_MAT4x2
    dukwebgl_push_constant_property(ctx, FLOAT_MAT4x2);
#endif
#ifdef GL_FLOAT_MAT4x3
    dukwebgl_push_constant_property(ctx, FLOAT_MAT4x3);
#endif
#ifdef GL_SRGB
    dukwebgl_push_constant_property(ctx, SRGB);
#endif
#ifdef GL_SRGB8
    dukwebgl_push_constant_property(ctx, SRGB8);
#endif
#ifdef GL_SRGB8_ALPHA8
    dukwebgl_push_constant_property(ctx, SRGB8_ALPHA8);
#endif
#endif /* GL_VERSION_2_1 */ 

#ifdef GL_VERSION_3_0
#ifdef GL_COMPARE_REF_TO_TEXTURE
    dukwebgl_push_constant_property(ctx, COMPARE_REF_TO_TEXTURE);
#endif
#ifdef GL_RGBA32F
    dukwebgl_push_constant_property(ctx, RGBA32F);
#endif
#ifdef GL_RGB32F
    dukwebgl_push_constant_property(ctx, RGB32F);
#endif
#ifdef GL_RGBA16F
    dukwebgl_push_constant_property(ctx, RGBA16F);
#endif
#ifdef GL_RGB16F
    dukwebgl_push_constant_property(ctx, RGB16F);
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_INTEGER
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_INTEGER);
#endif
#ifdef GL_MAX_ARRAY_TEXTURE_LAYERS
    dukwebgl_push_constant_property(ctx, MAX_ARRAY_TEXTURE_LAYERS);
#endif
#ifdef GL_MIN_PROGRAM_TEXEL_OFFSET
    dukwebgl_push_constant_property(ctx, MIN_PROGRAM_TEXEL_OFFSET);
#endif
#ifdef GL_MAX_PROGRAM_TEXEL_OFFSET
    dukwebgl_push_constant_property(ctx, MAX_PROGRAM_TEXEL_OFFSET);
#endif
#ifdef GL_MAX_VARYING_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_VARYING_COMPONENTS);
#endif
#ifdef GL_TEXTURE_2D_ARRAY
    dukwebgl_push_constant_property(ctx, TEXTURE_2D_ARRAY);
#endif
#ifdef GL_TEXTURE_BINDING_2D_ARRAY
    dukwebgl_push_constant_property(ctx, TEXTURE_BINDING_2D_ARRAY);
#endif
#ifdef GL_R11F_G11F_B10F
    dukwebgl_push_constant_property(ctx, R11F_G11F_B10F);
#endif
#ifdef GL_UNSIGNED_INT_10F_11F_11F_REV
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_10F_11F_11F_REV);
#endif
#ifdef GL_RGB9_E5
    dukwebgl_push_constant_property(ctx, RGB9_E5);
#endif
#ifdef GL_UNSIGNED_INT_5_9_9_9_REV
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_5_9_9_9_REV);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_MODE
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_BUFFER_MODE);
#endif
#ifdef GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYINGS
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_VARYINGS);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_START
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_BUFFER_START);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_BUFFER_SIZE);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
#endif
#ifdef GL_RASTERIZER_DISCARD
    dukwebgl_push_constant_property(ctx, RASTERIZER_DISCARD);
#endif
#ifdef GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS);
#endif
#ifdef GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS
    dukwebgl_push_constant_property(ctx, MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS);
#endif
#ifdef GL_INTERLEAVED_ATTRIBS
    dukwebgl_push_constant_property(ctx, INTERLEAVED_ATTRIBS);
#endif
#ifdef GL_SEPARATE_ATTRIBS
    dukwebgl_push_constant_property(ctx, SEPARATE_ATTRIBS);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_BUFFER);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_BUFFER_BINDING);
#endif
#ifdef GL_RGBA32UI
    dukwebgl_push_constant_property(ctx, RGBA32UI);
#endif
#ifdef GL_RGB32UI
    dukwebgl_push_constant_property(ctx, RGB32UI);
#endif
#ifdef GL_RGBA16UI
    dukwebgl_push_constant_property(ctx, RGBA16UI);
#endif
#ifdef GL_RGB16UI
    dukwebgl_push_constant_property(ctx, RGB16UI);
#endif
#ifdef GL_RGBA8UI
    dukwebgl_push_constant_property(ctx, RGBA8UI);
#endif
#ifdef GL_RGB8UI
    dukwebgl_push_constant_property(ctx, RGB8UI);
#endif
#ifdef GL_RGBA32I
    dukwebgl_push_constant_property(ctx, RGBA32I);
#endif
#ifdef GL_RGB32I
    dukwebgl_push_constant_property(ctx, RGB32I);
#endif
#ifdef GL_RGBA16I
    dukwebgl_push_constant_property(ctx, RGBA16I);
#endif
#ifdef GL_RGB16I
    dukwebgl_push_constant_property(ctx, RGB16I);
#endif
#ifdef GL_RGBA8I
    dukwebgl_push_constant_property(ctx, RGBA8I);
#endif
#ifdef GL_RGB8I
    dukwebgl_push_constant_property(ctx, RGB8I);
#endif
#ifdef GL_RED_INTEGER
    dukwebgl_push_constant_property(ctx, RED_INTEGER);
#endif
#ifdef GL_RGB_INTEGER
    dukwebgl_push_constant_property(ctx, RGB_INTEGER);
#endif
#ifdef GL_RGBA_INTEGER
    dukwebgl_push_constant_property(ctx, RGBA_INTEGER);
#endif
#ifdef GL_SAMPLER_2D_ARRAY
    dukwebgl_push_constant_property(ctx, SAMPLER_2D_ARRAY);
#endif
#ifdef GL_SAMPLER_2D_ARRAY_SHADOW
    dukwebgl_push_constant_property(ctx, SAMPLER_2D_ARRAY_SHADOW);
#endif
#ifdef GL_SAMPLER_CUBE_SHADOW
    dukwebgl_push_constant_property(ctx, SAMPLER_CUBE_SHADOW);
#endif
#ifdef GL_UNSIGNED_INT_VEC2
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_VEC2);
#endif
#ifdef GL_UNSIGNED_INT_VEC3
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_VEC3);
#endif
#ifdef GL_UNSIGNED_INT_VEC4
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_VEC4);
#endif
#ifdef GL_INT_SAMPLER_2D
    dukwebgl_push_constant_property(ctx, INT_SAMPLER_2D);
#endif
#ifdef GL_INT_SAMPLER_3D
    dukwebgl_push_constant_property(ctx, INT_SAMPLER_3D);
#endif
#ifdef GL_INT_SAMPLER_CUBE
    dukwebgl_push_constant_property(ctx, INT_SAMPLER_CUBE);
#endif
#ifdef GL_INT_SAMPLER_2D_ARRAY
    dukwebgl_push_constant_property(ctx, INT_SAMPLER_2D_ARRAY);
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_2D
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_SAMPLER_2D);
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_3D
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_SAMPLER_3D);
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_CUBE
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_SAMPLER_CUBE);
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_SAMPLER_2D_ARRAY);
#endif
#ifdef GL_DEPTH_COMPONENT32F
    dukwebgl_push_constant_property(ctx, DEPTH_COMPONENT32F);
#endif
#ifdef GL_DEPTH32F_STENCIL8
    dukwebgl_push_constant_property(ctx, DEPTH32F_STENCIL8);
#endif
#ifdef GL_FLOAT_32_UNSIGNED_INT_24_8_REV
    dukwebgl_push_constant_property(ctx, FLOAT_32_UNSIGNED_INT_24_8_REV);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_RED_SIZE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_GREEN_SIZE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_BLUE_SIZE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE);
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_DEFAULT);
#endif
#ifdef GL_UNSIGNED_INT_24_8
    dukwebgl_push_constant_property(ctx, UNSIGNED_INT_24_8);
#endif
#ifdef GL_DEPTH24_STENCIL8
    dukwebgl_push_constant_property(ctx, DEPTH24_STENCIL8);
#endif
#ifdef GL_UNSIGNED_NORMALIZED
    dukwebgl_push_constant_property(ctx, UNSIGNED_NORMALIZED);
#endif
#ifdef GL_DRAW_FRAMEBUFFER_BINDING
    dukwebgl_push_constant_property(ctx, DRAW_FRAMEBUFFER_BINDING);
#endif
#ifdef GL_READ_FRAMEBUFFER
    dukwebgl_push_constant_property(ctx, READ_FRAMEBUFFER);
#endif
#ifdef GL_DRAW_FRAMEBUFFER
    dukwebgl_push_constant_property(ctx, DRAW_FRAMEBUFFER);
#endif
#ifdef GL_READ_FRAMEBUFFER_BINDING
    dukwebgl_push_constant_property(ctx, READ_FRAMEBUFFER_BINDING);
#endif
#ifdef GL_RENDERBUFFER_SAMPLES
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_SAMPLES);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER);
#endif
#ifdef GL_MAX_COLOR_ATTACHMENTS
    dukwebgl_push_constant_property(ctx, MAX_COLOR_ATTACHMENTS);
#endif
#ifdef GL_COLOR_ATTACHMENT1
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT1);
#endif
#ifdef GL_COLOR_ATTACHMENT2
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT2);
#endif
#ifdef GL_COLOR_ATTACHMENT3
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT3);
#endif
#ifdef GL_COLOR_ATTACHMENT4
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT4);
#endif
#ifdef GL_COLOR_ATTACHMENT5
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT5);
#endif
#ifdef GL_COLOR_ATTACHMENT6
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT6);
#endif
#ifdef GL_COLOR_ATTACHMENT7
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT7);
#endif
#ifdef GL_COLOR_ATTACHMENT8
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT8);
#endif
#ifdef GL_COLOR_ATTACHMENT9
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT9);
#endif
#ifdef GL_COLOR_ATTACHMENT10
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT10);
#endif
#ifdef GL_COLOR_ATTACHMENT11
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT11);
#endif
#ifdef GL_COLOR_ATTACHMENT12
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT12);
#endif
#ifdef GL_COLOR_ATTACHMENT13
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT13);
#endif
#ifdef GL_COLOR_ATTACHMENT14
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT14);
#endif
#ifdef GL_COLOR_ATTACHMENT15
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT15);
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
#endif
#ifdef GL_MAX_SAMPLES
    dukwebgl_push_constant_property(ctx, MAX_SAMPLES);
#endif
#ifdef GL_HALF_FLOAT
    dukwebgl_push_constant_property(ctx, HALF_FLOAT);
#endif
#ifdef GL_RG
    dukwebgl_push_constant_property(ctx, RG);
#endif
#ifdef GL_RG_INTEGER
    dukwebgl_push_constant_property(ctx, RG_INTEGER);
#endif
#ifdef GL_R8
    dukwebgl_push_constant_property(ctx, R8);
#endif
#ifdef GL_RG8
    dukwebgl_push_constant_property(ctx, RG8);
#endif
#ifdef GL_R16F
    dukwebgl_push_constant_property(ctx, R16F);
#endif
#ifdef GL_R32F
    dukwebgl_push_constant_property(ctx, R32F);
#endif
#ifdef GL_RG16F
    dukwebgl_push_constant_property(ctx, RG16F);
#endif
#ifdef GL_RG32F
    dukwebgl_push_constant_property(ctx, RG32F);
#endif
#ifdef GL_R8I
    dukwebgl_push_constant_property(ctx, R8I);
#endif
#ifdef GL_R8UI
    dukwebgl_push_constant_property(ctx, R8UI);
#endif
#ifdef GL_R16I
    dukwebgl_push_constant_property(ctx, R16I);
#endif
#ifdef GL_R16UI
    dukwebgl_push_constant_property(ctx, R16UI);
#endif
#ifdef GL_R32I
    dukwebgl_push_constant_property(ctx, R32I);
#endif
#ifdef GL_R32UI
    dukwebgl_push_constant_property(ctx, R32UI);
#endif
#ifdef GL_RG8I
    dukwebgl_push_constant_property(ctx, RG8I);
#endif
#ifdef GL_RG8UI
    dukwebgl_push_constant_property(ctx, RG8UI);
#endif
#ifdef GL_RG16I
    dukwebgl_push_constant_property(ctx, RG16I);
#endif
#ifdef GL_RG16UI
    dukwebgl_push_constant_property(ctx, RG16UI);
#endif
#ifdef GL_RG32I
    dukwebgl_push_constant_property(ctx, RG32I);
#endif
#ifdef GL_RG32UI
    dukwebgl_push_constant_property(ctx, RG32UI);
#endif
#ifdef GL_VERTEX_ARRAY_BINDING
    dukwebgl_push_constant_property(ctx, VERTEX_ARRAY_BINDING);
#endif
#ifdef GL_FRAMEBUFFER
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER);
#endif
#ifdef GL_RENDERBUFFER
    dukwebgl_push_constant_property(ctx, RENDERBUFFER);
#endif
#ifdef GL_STENCIL_INDEX8
    dukwebgl_push_constant_property(ctx, STENCIL_INDEX8);
#endif
#ifdef GL_DEPTH_STENCIL
    dukwebgl_push_constant_property(ctx, DEPTH_STENCIL);
#endif
#ifdef GL_RENDERBUFFER_WIDTH
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_WIDTH);
#endif
#ifdef GL_RENDERBUFFER_HEIGHT
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_HEIGHT);
#endif
#ifdef GL_RENDERBUFFER_INTERNAL_FORMAT
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_INTERNAL_FORMAT);
#endif
#ifdef GL_RENDERBUFFER_RED_SIZE
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_RED_SIZE);
#endif
#ifdef GL_RENDERBUFFER_GREEN_SIZE
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_GREEN_SIZE);
#endif
#ifdef GL_RENDERBUFFER_BLUE_SIZE
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_BLUE_SIZE);
#endif
#ifdef GL_RENDERBUFFER_ALPHA_SIZE
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_ALPHA_SIZE);
#endif
#ifdef GL_RENDERBUFFER_DEPTH_SIZE
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_DEPTH_SIZE);
#endif
#ifdef GL_RENDERBUFFER_STENCIL_SIZE
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_STENCIL_SIZE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
#endif
#ifdef GL_COLOR_ATTACHMENT0
    dukwebgl_push_constant_property(ctx, COLOR_ATTACHMENT0);
#endif
#ifdef GL_DEPTH_ATTACHMENT
    dukwebgl_push_constant_property(ctx, DEPTH_ATTACHMENT);
#endif
#ifdef GL_STENCIL_ATTACHMENT
    dukwebgl_push_constant_property(ctx, STENCIL_ATTACHMENT);
#endif
#ifdef GL_DEPTH_STENCIL_ATTACHMENT
    dukwebgl_push_constant_property(ctx, DEPTH_STENCIL_ATTACHMENT);
#endif
#ifdef GL_FRAMEBUFFER_COMPLETE
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_COMPLETE);
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
#endif
#ifdef GL_FRAMEBUFFER_UNSUPPORTED
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_UNSUPPORTED);
#endif
#ifdef GL_FRAMEBUFFER_BINDING
    dukwebgl_push_constant_property(ctx, FRAMEBUFFER_BINDING);
#endif
#ifdef GL_RENDERBUFFER_BINDING
    dukwebgl_push_constant_property(ctx, RENDERBUFFER_BINDING);
#endif
#ifdef GL_MAX_RENDERBUFFER_SIZE
    dukwebgl_push_constant_property(ctx, MAX_RENDERBUFFER_SIZE);
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
    dukwebgl_push_constant_property(ctx, INVALID_FRAMEBUFFER_OPERATION);
#endif
#endif /* GL_VERSION_3_0 */ 

#ifdef GL_VERSION_3_1
#ifdef GL_R8_SNORM
    dukwebgl_push_constant_property(ctx, R8_SNORM);
#endif
#ifdef GL_RG8_SNORM
    dukwebgl_push_constant_property(ctx, RG8_SNORM);
#endif
#ifdef GL_RGB8_SNORM
    dukwebgl_push_constant_property(ctx, RGB8_SNORM);
#endif
#ifdef GL_RGBA8_SNORM
    dukwebgl_push_constant_property(ctx, RGBA8_SNORM);
#endif
#ifdef GL_SIGNED_NORMALIZED
    dukwebgl_push_constant_property(ctx, SIGNED_NORMALIZED);
#endif
#ifdef GL_COPY_READ_BUFFER
    dukwebgl_push_constant_property(ctx, COPY_READ_BUFFER);
#endif
#ifdef GL_COPY_WRITE_BUFFER
    dukwebgl_push_constant_property(ctx, COPY_WRITE_BUFFER);
#endif
#ifdef GL_UNIFORM_BUFFER
    dukwebgl_push_constant_property(ctx, UNIFORM_BUFFER);
#endif
#ifdef GL_UNIFORM_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, UNIFORM_BUFFER_BINDING);
#endif
#ifdef GL_UNIFORM_BUFFER_START
    dukwebgl_push_constant_property(ctx, UNIFORM_BUFFER_START);
#endif
#ifdef GL_UNIFORM_BUFFER_SIZE
    dukwebgl_push_constant_property(ctx, UNIFORM_BUFFER_SIZE);
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_BLOCKS
    dukwebgl_push_constant_property(ctx, MAX_VERTEX_UNIFORM_BLOCKS);
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_BLOCKS
    dukwebgl_push_constant_property(ctx, MAX_FRAGMENT_UNIFORM_BLOCKS);
#endif
#ifdef GL_MAX_COMBINED_UNIFORM_BLOCKS
    dukwebgl_push_constant_property(ctx, MAX_COMBINED_UNIFORM_BLOCKS);
#endif
#ifdef GL_MAX_UNIFORM_BUFFER_BINDINGS
    dukwebgl_push_constant_property(ctx, MAX_UNIFORM_BUFFER_BINDINGS);
#endif
#ifdef GL_MAX_UNIFORM_BLOCK_SIZE
    dukwebgl_push_constant_property(ctx, MAX_UNIFORM_BLOCK_SIZE);
#endif
#ifdef GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
#endif
#ifdef GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
#endif
#ifdef GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
    dukwebgl_push_constant_property(ctx, UNIFORM_BUFFER_OFFSET_ALIGNMENT);
#endif
#ifdef GL_ACTIVE_UNIFORM_BLOCKS
    dukwebgl_push_constant_property(ctx, ACTIVE_UNIFORM_BLOCKS);
#endif
#ifdef GL_UNIFORM_TYPE
    dukwebgl_push_constant_property(ctx, UNIFORM_TYPE);
#endif
#ifdef GL_UNIFORM_SIZE
    dukwebgl_push_constant_property(ctx, UNIFORM_SIZE);
#endif
#ifdef GL_UNIFORM_BLOCK_INDEX
    dukwebgl_push_constant_property(ctx, UNIFORM_BLOCK_INDEX);
#endif
#ifdef GL_UNIFORM_OFFSET
    dukwebgl_push_constant_property(ctx, UNIFORM_OFFSET);
#endif
#ifdef GL_UNIFORM_ARRAY_STRIDE
    dukwebgl_push_constant_property(ctx, UNIFORM_ARRAY_STRIDE);
#endif
#ifdef GL_UNIFORM_MATRIX_STRIDE
    dukwebgl_push_constant_property(ctx, UNIFORM_MATRIX_STRIDE);
#endif
#ifdef GL_UNIFORM_IS_ROW_MAJOR
    dukwebgl_push_constant_property(ctx, UNIFORM_IS_ROW_MAJOR);
#endif
#ifdef GL_UNIFORM_BLOCK_BINDING
    dukwebgl_push_constant_property(ctx, UNIFORM_BLOCK_BINDING);
#endif
#ifdef GL_UNIFORM_BLOCK_DATA_SIZE
    dukwebgl_push_constant_property(ctx, UNIFORM_BLOCK_DATA_SIZE);
#endif
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS
    dukwebgl_push_constant_property(ctx, UNIFORM_BLOCK_ACTIVE_UNIFORMS);
#endif
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES
    dukwebgl_push_constant_property(ctx, UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES);
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER
    dukwebgl_push_constant_property(ctx, UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER);
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER
    dukwebgl_push_constant_property(ctx, UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER);
#endif
#ifdef GL_INVALID_INDEX
    dukwebgl_push_constant_property(ctx, INVALID_INDEX);
#endif
#endif /* GL_VERSION_3_1 */ 

#ifdef GL_VERSION_3_2
#ifdef GL_MAX_VERTEX_OUTPUT_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_VERTEX_OUTPUT_COMPONENTS);
#endif
#ifdef GL_MAX_FRAGMENT_INPUT_COMPONENTS
    dukwebgl_push_constant_property(ctx, MAX_FRAGMENT_INPUT_COMPONENTS);
#endif
#ifdef GL_MAX_SERVER_WAIT_TIMEOUT
    dukwebgl_push_constant_property(ctx, MAX_SERVER_WAIT_TIMEOUT);
#endif
#ifdef GL_OBJECT_TYPE
    dukwebgl_push_constant_property(ctx, OBJECT_TYPE);
#endif
#ifdef GL_SYNC_CONDITION
    dukwebgl_push_constant_property(ctx, SYNC_CONDITION);
#endif
#ifdef GL_SYNC_STATUS
    dukwebgl_push_constant_property(ctx, SYNC_STATUS);
#endif
#ifdef GL_SYNC_FLAGS
    dukwebgl_push_constant_property(ctx, SYNC_FLAGS);
#endif
#ifdef GL_SYNC_FENCE
    dukwebgl_push_constant_property(ctx, SYNC_FENCE);
#endif
#ifdef GL_SYNC_GPU_COMMANDS_COMPLETE
    dukwebgl_push_constant_property(ctx, SYNC_GPU_COMMANDS_COMPLETE);
#endif
#ifdef GL_UNSIGNALED
    dukwebgl_push_constant_property(ctx, UNSIGNALED);
#endif
#ifdef GL_SIGNALED
    dukwebgl_push_constant_property(ctx, SIGNALED);
#endif
#ifdef GL_ALREADY_SIGNALED
    dukwebgl_push_constant_property(ctx, ALREADY_SIGNALED);
#endif
#ifdef GL_TIMEOUT_EXPIRED
    dukwebgl_push_constant_property(ctx, TIMEOUT_EXPIRED);
#endif
#ifdef GL_CONDITION_SATISFIED
    dukwebgl_push_constant_property(ctx, CONDITION_SATISFIED);
#endif
#ifdef GL_WAIT_FAILED
    dukwebgl_push_constant_property(ctx, WAIT_FAILED);
#endif
#ifdef GL_SYNC_FLUSH_COMMANDS_BIT
    dukwebgl_push_constant_property(ctx, SYNC_FLUSH_COMMANDS_BIT);
#endif
/* NOT IMPLEMENTED: TIMEOUT_IGNORED */
#endif /* GL_VERSION_3_2 */ 

#ifdef GL_VERSION_3_3
#ifdef GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    dukwebgl_push_constant_property(ctx, VERTEX_ATTRIB_ARRAY_DIVISOR);
#endif
#ifdef GL_ANY_SAMPLES_PASSED
    dukwebgl_push_constant_property(ctx, ANY_SAMPLES_PASSED);
#endif
#ifdef GL_SAMPLER_BINDING
    dukwebgl_push_constant_property(ctx, SAMPLER_BINDING);
#endif
#ifdef GL_RGB10_A2UI
    dukwebgl_push_constant_property(ctx, RGB10_A2UI);
#endif
#ifdef GL_INT_2_10_10_10_REV
    dukwebgl_push_constant_property(ctx, INT_2_10_10_10_REV);
#endif
#endif /* GL_VERSION_3_3 */ 

#ifdef GL_VERSION_4_0
#ifdef GL_TRANSFORM_FEEDBACK
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BINDING
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_BINDING);
#endif
#endif /* GL_VERSION_4_0 */ 

#ifdef GL_VERSION_4_1
#ifdef GL_MAX_VERTEX_UNIFORM_VECTORS
    dukwebgl_push_constant_property(ctx, MAX_VERTEX_UNIFORM_VECTORS);
#endif
#ifdef GL_MAX_VARYING_VECTORS
    dukwebgl_push_constant_property(ctx, MAX_VARYING_VECTORS);
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_VECTORS
    dukwebgl_push_constant_property(ctx, MAX_FRAGMENT_UNIFORM_VECTORS);
#endif
#ifdef GL_LOW_FLOAT
    dukwebgl_push_constant_property(ctx, LOW_FLOAT);
#endif
#ifdef GL_MEDIUM_FLOAT
    dukwebgl_push_constant_property(ctx, MEDIUM_FLOAT);
#endif
#ifdef GL_HIGH_FLOAT
    dukwebgl_push_constant_property(ctx, HIGH_FLOAT);
#endif
#ifdef GL_LOW_INT
    dukwebgl_push_constant_property(ctx, LOW_INT);
#endif
#ifdef GL_MEDIUM_INT
    dukwebgl_push_constant_property(ctx, MEDIUM_INT);
#endif
#ifdef GL_HIGH_INT
    dukwebgl_push_constant_property(ctx, HIGH_INT);
#endif
#ifdef GL_RGB565
    dukwebgl_push_constant_property(ctx, RGB565);
#endif
#endif /* GL_VERSION_4_1 */ 

#ifdef GL_VERSION_4_2
#ifdef GL_COPY_READ_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, COPY_READ_BUFFER_BINDING);
#endif
#ifdef GL_COPY_WRITE_BUFFER_BINDING
    dukwebgl_push_constant_property(ctx, COPY_WRITE_BUFFER_BINDING);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_PAUSED
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_PAUSED);
#endif
#ifdef GL_TRANSFORM_FEEDBACK_ACTIVE
    dukwebgl_push_constant_property(ctx, TRANSFORM_FEEDBACK_ACTIVE);
#endif
#ifdef GL_TEXTURE_IMMUTABLE_FORMAT
    dukwebgl_push_constant_property(ctx, TEXTURE_IMMUTABLE_FORMAT);
#endif
#endif /* GL_VERSION_4_2 */ 

#ifdef GL_VERSION_4_3
#ifdef GL_ANY_SAMPLES_PASSED_CONSERVATIVE
    dukwebgl_push_constant_property(ctx, ANY_SAMPLES_PASSED_CONSERVATIVE);
#endif
#ifdef GL_MAX_ELEMENT_INDEX
    dukwebgl_push_constant_property(ctx, MAX_ELEMENT_INDEX);
#endif
#ifdef GL_TEXTURE_IMMUTABLE_LEVELS
    dukwebgl_push_constant_property(ctx, TEXTURE_IMMUTABLE_LEVELS);
#endif
#endif /* GL_VERSION_4_3 */ 

#ifdef GL_VERSION_4_4
#endif /* GL_VERSION_4_4 */ 

#ifdef GL_VERSION_4_5
#endif /* GL_VERSION_4_5 */ 

#ifdef GL_VERSION_4_6
#endif /* GL_VERSION_4_6 */ 

} /* dukwebgl_bind_constants */

DUK_LOCAL void dukwebgl_create_object_uint(duk_context *ctx, GLuint id) {
    if (id == 0) {
        duk_push_null(ctx);
        return;
    }

    duk_idx_t obj = duk_push_object(ctx);
    duk_push_uint(ctx, id);
    duk_put_prop_string(ctx, obj, "_id");
}

DUK_LOCAL GLuint dukwebgl_get_object_id_uint(duk_context *ctx, duk_idx_t obj_idx) {
    GLuint ret = 0;

    /* everything else than object assumed null */
    if (duk_is_object(ctx, obj_idx)) {
        duk_get_prop_string(ctx, obj_idx, "_id");
        ret = (GLuint)duk_to_uint(ctx, -1);
        duk_pop(ctx);
    }

    return ret;
}

DUK_LOCAL void dukwebgl_create_object_int(duk_context *ctx, GLint id) {
    if (id < 0) {
        duk_push_null(ctx);
        return;
    }

    duk_idx_t obj = duk_push_object(ctx);
    duk_push_int(ctx, id);
    duk_put_prop_string(ctx, obj, "_id");
}

DUK_LOCAL GLint dukwebgl_get_object_id_int(duk_context *ctx, duk_idx_t obj_idx) {
    GLint ret = 0;

    /* everything else than object assumed null */
    if (duk_is_object(ctx, obj_idx)) {
        duk_get_prop_string(ctx, obj_idx, "_id");
        ret = (GLint)duk_to_int(ctx, -1);
        duk_pop(ctx);
    }

    return ret;
}

#ifdef GL_VERSION_1_0

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getContextAttributes(duk_context *ctx) {
    /*
     * ref. https://www.khronos.org/registry/webgl/specs/latest/1.0/#WEBGLCONTEXTATTRIBUTES
     * Note: Sensible fixed defaults here
     */
    duk_idx_t obj = duk_push_object(ctx);
    duk_push_boolean(ctx, 1);
    duk_put_prop_string(ctx, obj, "alpha");
    duk_push_boolean(ctx, 1);
    duk_put_prop_string(ctx, obj, "depth");
    duk_push_boolean(ctx, 1);
    duk_put_prop_string(ctx, obj, "stencil");
    duk_push_boolean(ctx, 1);
    duk_put_prop_string(ctx, obj, "antialias");
    duk_push_boolean(ctx, 1);
    duk_put_prop_string(ctx, obj, "premultipliedAlpha");
    duk_push_boolean(ctx, 0);
    duk_put_prop_string(ctx, obj, "preserveDrawingBuffer");
    duk_push_string(ctx, "default");
    duk_put_prop_string(ctx, obj, "powerPreference");
    duk_push_boolean(ctx, 0);
    duk_put_prop_string(ctx, obj, "failIfMajorPerformanceCaveat");

    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_isContextLost(duk_context *ctx) {
    /*
     * ref. https://www.khronos.org/registry/webgl/specs/latest/1.0/#5.14.13
     * Note: context is assumably never lost in Duktape implementation.
     * This function is not available in normal OpenGL.
     */
    duk_push_false(ctx);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getSupportedExtensions(duk_context *ctx) {
    /*
     * ref. https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getSupportedExtensions
     * Note: No extensions supported at the moment
     * Will return an empty array.
     */
    duk_push_array(ctx);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getExtension(duk_context *ctx) {
    /*
     * ref. https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getExtension
     * Note: No extensions supported at the moment
     * Will return null.
     */
    /*const GLchar *extension =*/ (const GLchar *)duk_get_string(ctx, 0);
    duk_push_null(ctx);
    return 1;
}

#endif /* GL_VERSION_1_0 */

#define DEFINE_CREATE_OBJECT(jsFunctionName, cFunctionName) \
    DUK_LOCAL duk_ret_t dukwebgl_custom_impl_##jsFunctionName (duk_context *ctx) { \
        GLuint ids[1]; \
        cFunctionName (1, ids); \
        dukwebgl_create_object_uint(ctx, ids[0]); \
        return 1; \
    }

#define DEFINE_DELETE_OBJECT(jsFunctionName, cFunctionName) \
    DUK_LOCAL duk_ret_t dukwebgl_custom_impl_##jsFunctionName (duk_context *ctx) { \
        GLuint id = dukwebgl_get_object_id_uint(ctx, 0); \
        GLuint ids[1] = { id }; \
        cFunctionName (1, ids); \
        return 0; \
    }

#ifdef GL_VERSION_2_0

DEFINE_CREATE_OBJECT(createBuffer, glGenBuffers)
DEFINE_DELETE_OBJECT(deleteBuffer, glDeleteBuffers)

DEFINE_CREATE_OBJECT(createTexture, glGenTextures)
DEFINE_DELETE_OBJECT(deleteTexture, glDeleteTextures)

DEFINE_CREATE_OBJECT(createQuery, glGenQueries)
DEFINE_DELETE_OBJECT(deleteQuery, glDeleteQueries)

/* FIXME: srcOffset / srcLength support : https://developer.mozilla.org/en-US/docs/Web/API/WebGL2RenderingContext/uniformMatrix */
#define DEFINE_UNIFORM_MATRIX(jsFunctionName, cFunctionName) \
    DUK_LOCAL duk_ret_t dukwebgl_custom_impl_##jsFunctionName (duk_context *ctx) { \
        GLint location = dukwebgl_get_object_id_int(ctx, 0); \
        GLboolean transpose = (GLboolean)(duk_get_boolean(ctx, 1) == 1 ? GL_TRUE : GL_FALSE); \
        duk_size_t count = 0; \
        const GLfloat *value = (const GLfloat *)duk_get_buffer_data(ctx, 2, &count); \
        cFunctionName (location, 1, transpose, value); \
        return 0; \
    }

DEFINE_UNIFORM_MATRIX(uniformMatrix2fv, glUniformMatrix2fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix3fv, glUniformMatrix3fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix4fv, glUniformMatrix4fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix2x3fv, glUniformMatrix2x3fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix2x4fv, glUniformMatrix2x4fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix3x2fv, glUniformMatrix3x2fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix3x4fv, glUniformMatrix3x4fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix4x2fv, glUniformMatrix4x2fv)
DEFINE_UNIFORM_MATRIX(uniformMatrix4x3fv, glUniformMatrix4x3fv)

#define DEFINE_UNIFORM_FV(jsFunctionName, cType, cFunctionName) \
    DUK_LOCAL duk_ret_t dukwebgl_custom_impl_##jsFunctionName (duk_context *ctx) { \
        GLint location = dukwebgl_get_object_id_int(ctx, 0); \
        duk_size_t count = 0; \
        const cType *value = (const cType *)duk_get_buffer_data(ctx, 2, &count); \
        cFunctionName (location, (GLsizei)count, value); \
        return 0; \
    }

DEFINE_UNIFORM_FV(uniform1fv, GLfloat, glUniform1fv)
DEFINE_UNIFORM_FV(uniform2fv, GLfloat, glUniform2fv)
DEFINE_UNIFORM_FV(uniform3fv, GLfloat, glUniform3fv)
DEFINE_UNIFORM_FV(uniform4fv, GLfloat, glUniform4fv)

DEFINE_UNIFORM_FV(uniform1iv, GLint, glUniform1iv)
DEFINE_UNIFORM_FV(uniform2iv, GLint, glUniform2iv)
DEFINE_UNIFORM_FV(uniform3iv, GLint, glUniform3iv)
DEFINE_UNIFORM_FV(uniform4iv, GLint, glUniform4iv)

DEFINE_UNIFORM_FV(uniform1uiv, GLuint, glUniform1uiv)
DEFINE_UNIFORM_FV(uniform2uiv, GLuint, glUniform2uiv)
DEFINE_UNIFORM_FV(uniform3uiv, GLuint, glUniform3uiv)
DEFINE_UNIFORM_FV(uniform4uiv, GLuint, glUniform4uiv)

/* ref. https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getProgramParameter */
DUK_LOCAL void dukwebgl_push_boolean_program_parameter(duk_context *ctx, GLuint program, GLenum pname) {
    GLint value = 0; 
    glGetProgramiv(program, pname, &value);
    duk_push_boolean(ctx, value == GL_TRUE ? 1 : 0);
}
DUK_LOCAL void dukwebgl_push_int_program_parameter(duk_context *ctx, GLuint program, GLenum pname) {
    GLint value = 0; 
    glGetProgramiv(program, pname, &value);
    duk_push_int(ctx, value);
}
DUK_LOCAL void dukwebgl_push_uint_program_parameter(duk_context *ctx, GLuint program, GLenum pname) {
    GLint value = 0; 
    glGetProgramiv(program, pname, &value);
    duk_push_uint(ctx, (unsigned int)value);
}
DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getProgramParameter(duk_context *ctx) {
    GLuint program = dukwebgl_get_object_id_uint(ctx, 0);
    GLenum pname = (GLenum)duk_get_uint(ctx, 1);

    switch(pname) {
#ifdef GL_DELETE_STATUS
        case GL_DELETE_STATUS:
            dukwebgl_push_boolean_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_LINK_STATUS
        case GL_LINK_STATUS:
            dukwebgl_push_boolean_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_VALIDATE_STATUS
        case GL_VALIDATE_STATUS:
            dukwebgl_push_boolean_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_ATTACHED_SHADERS
        case GL_ATTACHED_SHADERS:
            dukwebgl_push_int_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_ACTIVE_ATTRIBUTES
        case GL_ACTIVE_ATTRIBUTES:
            dukwebgl_push_int_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_ACTIVE_UNIFORMS
        case GL_ACTIVE_UNIFORMS:
            dukwebgl_push_int_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_MODE
        case GL_TRANSFORM_FEEDBACK_BUFFER_MODE:
            dukwebgl_push_uint_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYINGS
        case GL_TRANSFORM_FEEDBACK_VARYINGS:
            dukwebgl_push_int_program_parameter(ctx, program, pname);
            break;
#endif
#ifdef GL_ACTIVE_UNIFORM_BLOCKS
        case GL_ACTIVE_UNIFORM_BLOCKS:
            dukwebgl_push_int_program_parameter(ctx, program, pname);
            break;
#endif
        default:
            /* Unknown parameter case not defined by the MDN specs */
            duk_push_undefined(ctx);
            break;
    }

    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getProgramInfoLog(duk_context *ctx) {
    GLuint program = dukwebgl_get_object_id_uint(ctx, 0);

    const GLsizei maxLength = 4096;
    GLchar infoLog[maxLength];
    GLsizei length = 0;

    glGetProgramInfoLog(program, maxLength, &length, infoLog);

    duk_push_string(ctx, (const char*)infoLog);

    return 1;
}

/* ref. https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getShaderParameter */
DUK_LOCAL void dukwebgl_push_boolean_shader_parameter(duk_context *ctx, GLuint program, GLenum pname) {
    GLint value = 0; 
    glGetShaderiv(program, pname, &value);
    duk_push_boolean(ctx, value == GL_TRUE ? 1 : 0);
}
DUK_LOCAL void dukwebgl_push_uint_shader_parameter(duk_context *ctx, GLuint program, GLenum pname) {
    GLint value = 0; 
    glGetShaderiv(program, pname, &value);
    duk_push_uint(ctx, (unsigned int)value);
}
DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getShaderParameter(duk_context *ctx) {
    GLuint shader = dukwebgl_get_object_id_uint(ctx, 0);
    GLenum pname = (GLenum)duk_get_uint(ctx, 1);

    switch(pname) {
#ifdef GL_DELETE_STATUS
        case GL_DELETE_STATUS:
            dukwebgl_push_boolean_shader_parameter(ctx, shader, pname);
            break;
#endif
#ifdef GL_COMPILE_STATUS
        case GL_COMPILE_STATUS:
            dukwebgl_push_boolean_shader_parameter(ctx, shader, pname);
            break;
#endif
#ifdef GL_SHADER_TYPE
        case GL_SHADER_TYPE:
            dukwebgl_push_uint_shader_parameter(ctx, shader, pname);
            break;
#endif
        default:
            /* Unknown parameter case not defined by the MDN specs */
            duk_push_undefined(ctx);
            break;
    }

    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getShaderInfoLog(duk_context *ctx) {
    GLuint shader = dukwebgl_get_object_id_uint(ctx, 0);

    const GLsizei maxLength = 4096;
    GLchar infoLog[maxLength];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, maxLength, &length, infoLog);

    duk_push_string(ctx, (const char*)infoLog);

    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_shaderSource(duk_context *ctx) {
    GLuint shader = dukwebgl_get_object_id_uint(ctx, 0);
    const GLchar *string = (const GLchar *)duk_get_string(ctx, 1);

    glShaderSource(shader, 1, &string, NULL);

    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getShaderSource(duk_context *ctx) {
    GLuint shader = dukwebgl_get_object_id_uint(ctx, 0);

    const GLsizei maxLength = 65536;
    GLchar source[maxLength];
    GLsizei length = 0;

    glGetShaderSource(shader, maxLength, &length, source);

    duk_push_string(ctx, (const char*)source);

    return 1;
}

/* ref. https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bufferData */
DUK_LOCAL duk_ret_t dukwebgl_custom_impl_bufferData(duk_context *ctx) {
    int argc = duk_get_top(ctx);

    GLenum target = (GLenum)duk_get_uint(ctx, 0);

    duk_size_t data_size = 0;
    GLvoid *data = NULL;
    if (duk_is_buffer_data(ctx, 1)) {
        data = duk_get_buffer_data(ctx, 1, &data_size);
    } else {
        /* WebGL 1 alternative */
        data_size = (duk_size_t)duk_get_uint(ctx, 1);
    }

    GLenum usage = (GLenum)duk_get_uint(ctx, 2);

    if (argc > 3) {
        /* WebGL 2 mandatory */
        GLuint src_offset = (GLuint)duk_get_uint(ctx, 3);
        data = (GLvoid*)((char*)data + src_offset);
        data_size = data_size - src_offset;

        if (argc > 4) {
            /* WebGL 2 optional */
            GLuint length = (GLuint)duk_get_uint(ctx, 4);

            if (length > 0 && (GLsizeiptr)length <= data_size) {
                data_size = (GLsizeiptr)length;
            }

        }
    }

    glBufferData(target, (GLsizeiptr)((char*)NULL + data_size), (const GLvoid *)data, usage);
    /* GL 4.5: glNamedBufferData(target, (GLsizeiptr)(NULL + data_size), (const GLvoid *)data, usage); */

    return 0;
}

/* ref. https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/bufferSubData */
DUK_LOCAL duk_ret_t dukwebgl_custom_impl_bufferSubData(duk_context *ctx) {
    int argc = duk_get_top(ctx);

    GLenum target = (GLenum)duk_get_uint(ctx, 0);

    GLintptr offset = (GLintptr)((char*)NULL + duk_get_uint(ctx, 1));

    duk_size_t data_size = 0;
    GLvoid * data = NULL;
    if (duk_is_buffer_data(ctx, 2)) {
        data = duk_get_buffer_data(ctx, 2, &data_size);
    }

    if (argc > 3) {
        GLuint src_offset = (GLuint)duk_get_uint(ctx, 3);
        data = (GLvoid*)((char*)data + src_offset);
        data_size = data_size - src_offset;

        if (argc > 4) {
            GLuint length = (GLuint)duk_get_uint(ctx, 4);

            if (data_size > 0 && (GLsizeiptr)length <= data_size) {
                data_size = (GLsizeiptr)length;
            }
        }

    }

    glBufferSubData(target, offset, (GLsizeiptr)((char*)NULL + data_size), (const GLvoid *)data);

    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_drawBuffers(duk_context *ctx) {
    duk_idx_t idx = 0;
    if (duk_is_array(ctx, idx)) {
        duk_get_prop_string(ctx, idx, "length");
        unsigned int length = duk_to_uint(ctx, -1);
        duk_pop(ctx);

        GLenum *bufs = (GLenum*)malloc(sizeof(GLenum) * length);
        if (bufs == NULL) {
            /* TODO: maybe some better way needed in out-of-memory cases */
            return DUK_ERR_ERROR;
        }

        for(unsigned int i = 0; i < length; i++) {
            duk_get_prop_index(ctx, idx, 0);
            GLenum buf = (GLenum)duk_to_uint(ctx, -1);
            duk_pop(ctx);

            bufs[i] = buf;
        }

        glDrawBuffers((GLsizei)length, (const GLenum *)bufs);

        free(bufs);

        return 0;
    }

    return DUK_ERR_TYPE_ERROR;
}

/* ref. https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/texImage2D */

DUK_LOCAL void * dukwebgl_get_pixels(duk_context *ctx, duk_idx_t idx) {
    void * pixels = NULL;
    if (duk_is_buffer_data(ctx, idx)) {
        /* ArrayBufferView / BufferSource */
        pixels = duk_get_buffer_data(ctx, idx, NULL);
    } else if (duk_is_object(ctx, idx)) {
        /* ref. https://developer.mozilla.org/en-US/docs/Web/API/ImageData */
        if (duk_has_prop_string(ctx, idx, "data")) {
            duk_get_prop_string(ctx, idx, "data");
            if (duk_is_buffer_data(ctx, -1)) {
                pixels = duk_get_buffer_data(ctx, -1, NULL);
                duk_pop(ctx);
            } else {
                /* unrecognized object.data variable type */
        return NULL;
            }
        } else {
            /* unrecognized object type */
        return NULL;
        }
    } else {
        /* FIXME: TBD GLintptr offset */
        /* unrecognized argument type */
    return NULL;
    }

    return pixels;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_texImage2D(duk_context *ctx) {
    int argc = duk_get_top(ctx);

    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint internalformat = (GLint)duk_get_int(ctx, 2);
    GLsizei width = 0;
    GLsizei height = 0;
    GLint border = 0;
    GLenum format = 0;
    GLenum type = 0;
    void *pixels = NULL;

    /* FIXME: partial implementation. figure out HTMLImageElement, HTMLCanvasElement, HTMLVideoElement, ImageBitmap */
    if (argc == 6) {
        format = (GLenum)duk_get_uint(ctx, 3);
        type = (GLenum)duk_get_uint(ctx, 4);

        if (duk_is_object(ctx, 6)) {
            /* ref. https://developer.mozilla.org/en-US/docs/Web/API/ImageData */
            if (duk_has_prop_string(ctx, 6, "width")) {
                duk_get_prop_string(ctx, 6, "width");
                width = (GLsizei)duk_get_int(ctx, -1);
                duk_pop(ctx);
            }
            if (duk_has_prop_string(ctx, 6, "height")) {
                duk_get_prop_string(ctx, 6, "height");
                height = (GLsizei)duk_get_int(ctx, -1);
                duk_pop(ctx);
            }
            if (duk_has_prop_string(ctx, 6, "data")) {
                duk_get_prop_string(ctx, 6, "data");
                if (duk_is_buffer_data(ctx, -1)) {
                    pixels = duk_get_buffer_data(ctx, -1, NULL);
                    duk_pop(ctx);
                }
            }
        }

        pixels = dukwebgl_get_pixels(ctx, 8);
    } else {
        width = (GLsizei)duk_get_int(ctx, 3);
        height = (GLsizei)duk_get_int(ctx, 4);
        border = (GLint)duk_get_int(ctx, 5);
        format = (GLenum)duk_get_uint(ctx, 6);
        type = (GLenum)duk_get_uint(ctx, 7);

        pixels = dukwebgl_get_pixels(ctx, 8);

        if (argc > 8) {
            GLuint offset = (GLuint)duk_get_uint(ctx, 9);
            pixels = (char*)pixels + offset;
        }
    }

    glTexImage2D(target,level,internalformat,width,height,border,format,type,pixels);

    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_readPixels(duk_context *ctx) {
    int argc = duk_get_top(ctx);

    GLint x = (GLint)duk_get_int(ctx, 0);
    GLint y = (GLint)duk_get_int(ctx, 1);
    GLsizei width = (GLsizei)duk_get_int(ctx, 2);
    GLsizei height = (GLsizei)duk_get_int(ctx, 3);
    GLenum format = (GLenum)duk_get_uint(ctx, 4);
    GLenum type = (GLenum)duk_get_uint(ctx, 5);
    void * pixels = (void *)duk_get_buffer_data(ctx, 6, NULL);

    GLuint dstoffset = 0;
    if (argc > 7) {
        dstoffset = (GLuint)duk_get_uint(ctx, 8);
    pixels = (char*)pixels + dstoffset;
    }

    glReadPixels(x,y,width,height,format,type,pixels);

    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_texSubImage2D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint xoffset = (GLint)duk_get_int(ctx, 2);
    GLint yoffset = (GLint)duk_get_int(ctx, 3);
    GLsizei width = (GLsizei)duk_get_int(ctx, 4);
    GLsizei height = (GLsizei)duk_get_int(ctx, 5);
    GLenum format = (GLenum)duk_get_uint(ctx, 6);
    GLenum type = (GLenum)duk_get_uint(ctx, 7);

    const void * pixels = dukwebgl_get_pixels(ctx, 8);

    glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);

    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_texImage3D(duk_context *ctx) {
    int argc = duk_get_top(ctx);

    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint internalformat = (GLint)duk_get_int(ctx, 2);
    GLsizei width = (GLsizei)duk_get_int(ctx, 3);
    GLsizei height = (GLsizei)duk_get_int(ctx, 4);
    GLsizei depth = (GLsizei)duk_get_int(ctx, 5);
    GLint border = (GLint)duk_get_int(ctx, 6);
    GLenum format = (GLenum)duk_get_uint(ctx, 7);
    GLenum type = (GLenum)duk_get_uint(ctx, 8);
    const void * pixels = dukwebgl_get_pixels(ctx, 9);

    GLuint offset = 0;
    if (argc > 9) {
        offset = (GLuint)duk_get_uint(ctx, 10);
        pixels = (char*)pixels + offset;
    }

    glTexImage3D(target,level,internalformat,width,height,depth,border,format,type,pixels);
    return 0;
}

DUK_LOCAL void dukwebgl_create_WebGLActiveInfo(duk_context *ctx, GLchar *name, GLsizei length, GLenum type, GLint size) {
    if (length <= 0) {
	/* return value not defined in invalid case for WebGL
	   OpenGL errors will happen anyway */
        duk_push_undefined(ctx);
        return;
    }

    duk_idx_t obj = duk_push_object(ctx);
    duk_push_string(ctx, (const char*)name);
    duk_put_prop_string(ctx, obj, "name");
    duk_push_uint(ctx, type);
    duk_put_prop_string(ctx, obj, "type");
    duk_push_int(ctx, size);
    duk_put_prop_string(ctx, obj, "size");
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getActiveAttrib(duk_context *ctx) {
    GLuint program = dukwebgl_get_object_id_uint(ctx, 0);
    GLuint index = (GLuint)duk_get_uint(ctx, 1);

    const GLsizei bufSize = 1024;
    GLchar name[bufSize];
    GLsizei length = 0;
    GLenum type;
    GLint size;

    glGetActiveAttrib(program, index, bufSize, &length, &size, &type, name);

    dukwebgl_create_WebGLActiveInfo(ctx, name, length, type, size);

    return 1;
}

#define DEFINE_GET_PARAMETER_GLENUM { \
    GLint value = 0; \
    glGetIntegerv(pname, &value); \
    duk_push_uint(ctx, (GLenum)value); \
    break; }
#define DEFINE_GET_PARAMETER_GLINT { \
    GLint value = 0; \
    glGetIntegerv(pname, &value); \
    duk_push_int(ctx, value); \
    break; }
#define DEFINE_GET_PARAMETER_GLUINT { \
    GLint value = 0; \
    glGetIntegerv(pname, &value); \
    duk_push_uint(ctx, (GLuint)value); \
    break; }
#define DEFINE_GET_PARAMETER_GLFLOAT { \
    GLfloat value = 0; \
    glGetFloatv(pname, &value); \
    duk_push_number(ctx, value); \
    break; }
#define DEFINE_GET_PARAMETER_GLBOOLEAN { \
    GLint value = 0; \
    glGetIntegerv(pname, &value); \
    duk_push_boolean(ctx, value == GL_TRUE ? 1 : 0); \
    break; }
#define DEFINE_GET_PARAMETER_WEBGLBUFFER { \
    GLint value = 0; \
    glGetIntegerv(pname, &value); \
    dukwebgl_create_object_uint(ctx, (GLuint)value); \
    break; }
#define DEFINE_GET_PARAMETER_WEBGLPROGRAM { \
    GLint value = 0; \
    glGetIntegerv(pname, &value); \
    dukwebgl_create_object_uint(ctx, (GLuint)value); \
    break; }
#define DEFINE_GET_PARAMETER_UNDEFINED { \
    duk_push_undefined(ctx); \
    break; }


DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getParameter(duk_context *ctx) {
    GLenum pname = (GLenum)duk_get_uint(ctx, 0);

    switch(pname) {
// WebGL 1

#ifdef GL_ACTIVE_TEXTURE
        case GL_ACTIVE_TEXTURE: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_ALIASED_LINE_WIDTH_RANGE
        case GL_ALIASED_LINE_WIDTH_RANGE: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_ALIASED_POINT_SIZE_RANGE
        case GL_ALIASED_POINT_SIZE_RANGE: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_ALPHA_BITS
        case GL_ALPHA_BITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_ARRAY_BUFFER_BINDING
        case GL_ARRAY_BUFFER_BINDING: DEFINE_GET_PARAMETER_WEBGLBUFFER
#endif
#ifdef GL_BLEND
        case GL_BLEND: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_BLEND_COLOR
        case GL_BLEND_COLOR: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_BLEND_DST_ALPHA
        case GL_BLEND_DST_ALPHA: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_BLEND_DST_RGB
        case GL_BLEND_DST_RGB: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_BLEND_EQUATION
        case GL_BLEND_EQUATION: DEFINE_GET_PARAMETER_GLENUM
#elif GL_BLEND_EQUATION_RGB
        case GL_BLEND_EQUATION_RGB: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_BLEND_EQUATION_ALPHA
        case GL_BLEND_EQUATION_ALPHA: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_BLEND_SRC_ALPHA
        case GL_BLEND_SRC_ALPHA: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_BLEND_SRC_RGB
        case GL_BLEND_SRC_RGB: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_BLUE_BITS
        case GL_BLUE_BITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_COLOR_CLEAR_VALUE
        case GL_COLOR_CLEAR_VALUE: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_COLOR_WRITEMASK
        case GL_COLOR_WRITEMASK: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_COMPRESSED_TEXTURE_FORMATS
        case GL_COMPRESSED_TEXTURE_FORMATS: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_CULL_FACE
        case GL_CULL_FACE: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_CULL_FACE_MODE
        case GL_CULL_FACE_MODE: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_CURRENT_PROGRAM
        case GL_CURRENT_PROGRAM: DEFINE_GET_PARAMETER_WEBGLPROGRAM
#endif
#ifdef GL_DEPTH_BITS
        case GL_DEPTH_BITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_DEPTH_CLEAR_VALUE
        case GL_DEPTH_CLEAR_VALUE: DEFINE_GET_PARAMETER_GLFLOAT
#endif
#ifdef GL_DEPTH_FUNC
        case GL_DEPTH_FUNC: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_DEPTH_RANGE
        case GL_DEPTH_RANGE: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_DEPTH_TEST
        case GL_DEPTH_TEST: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_DEPTH_WRITEMASK
        case GL_DEPTH_WRITEMASK: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_DITHER
        case GL_DITHER: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER_BINDING
        case GL_ELEMENT_ARRAY_BUFFER_BINDING: DEFINE_GET_PARAMETER_WEBGLBUFFER
#endif
#ifdef GL_FRAMEBUFFER_BINDING
        case GL_FRAMEBUFFER_BINDING: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_FRONT_FACE
        case GL_FRONT_FACE: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_GENERATE_MIPMAP_HINT
        case GL_GENERATE_MIPMAP_HINT: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_GREEN_BITS
        case GL_GREEN_BITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_FORMAT
        case GL_IMPLEMENTATION_COLOR_READ_FORMAT: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_TYPE
        case GL_IMPLEMENTATION_COLOR_READ_TYPE: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_LINE_WIDTH
        case GL_LINE_WIDTH: DEFINE_GET_PARAMETER_GLFLOAT
#endif
#ifdef GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
        case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_CUBE_MAP_TEXTURE_SIZE
        case GL_MAX_CUBE_MAP_TEXTURE_SIZE: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_VECTORS
        case GL_MAX_FRAGMENT_UNIFORM_VECTORS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_RENDERBUFFER_SIZE
        case GL_MAX_RENDERBUFFER_SIZE: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_TEXTURE_IMAGE_UNITS
        case GL_MAX_TEXTURE_IMAGE_UNITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_TEXTURE_SIZE
        case GL_MAX_TEXTURE_SIZE: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_VARYING_VECTORS
        case GL_MAX_VARYING_VECTORS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_VERTEX_ATTRIBS
        case GL_MAX_VERTEX_ATTRIBS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
        case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_VECTORS
        case GL_MAX_VERTEX_UNIFORM_VECTORS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_MAX_VIEWPORT_DIMS
        case GL_MAX_VIEWPORT_DIMS: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_PACK_ALIGNMENT
        case GL_PACK_ALIGNMENT: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_POLYGON_OFFSET_FACTOR
        case GL_POLYGON_OFFSET_FACTOR: DEFINE_GET_PARAMETER_GLFLOAT
#endif
#ifdef GL_POLYGON_OFFSET_FILL
        case GL_POLYGON_OFFSET_FILL: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_POLYGON_OFFSET_UNITS
        case GL_POLYGON_OFFSET_UNITS: DEFINE_GET_PARAMETER_GLFLOAT
#endif
#ifdef GL_RED_BITS
        case GL_RED_BITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_RENDERBUFFER_BINDING
        case GL_RENDERBUFFER_BINDING: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_RENDERER
        case GL_RENDERER: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_SAMPLE_BUFFERS
        case GL_SAMPLE_BUFFERS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_SAMPLE_COVERAGE_INVERT
        case GL_SAMPLE_COVERAGE_INVERT: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_SAMPLE_COVERAGE_VALUE
        case GL_SAMPLE_COVERAGE_VALUE: DEFINE_GET_PARAMETER_GLFLOAT
#endif
#ifdef GL_SAMPLES
        case GL_SAMPLES: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_SCISSOR_BOX
        case GL_SCISSOR_BOX: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_SCISSOR_TEST
        case GL_SCISSOR_TEST: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_SHADING_LANGUAGE_VERSION
        case GL_SHADING_LANGUAGE_VERSION: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_STENCIL_BACK_FAIL
        case GL_STENCIL_BACK_FAIL: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_BACK_FUNC
        case GL_STENCIL_BACK_FUNC: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_BACK_PASS_DEPTH_FAIL
        case GL_STENCIL_BACK_PASS_DEPTH_FAIL: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_BACK_PASS_DEPTH_PASS
        case GL_STENCIL_BACK_PASS_DEPTH_PASS: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_BACK_REF
        case GL_STENCIL_BACK_REF: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_STENCIL_BACK_VALUE_MASK
        case GL_STENCIL_BACK_VALUE_MASK: DEFINE_GET_PARAMETER_GLUINT
#endif
#ifdef GL_STENCIL_BACK_WRITEMASK
        case GL_STENCIL_BACK_WRITEMASK: DEFINE_GET_PARAMETER_GLUINT
#endif
#ifdef GL_STENCIL_BITS
        case GL_STENCIL_BITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_STENCIL_CLEAR_VALUE
        case GL_STENCIL_CLEAR_VALUE: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_STENCIL_FAIL
        case GL_STENCIL_FAIL: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_FUNC
        case GL_STENCIL_FUNC: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_PASS_DEPTH_FAIL
        case GL_STENCIL_PASS_DEPTH_FAIL: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_PASS_DEPTH_PASS
        case GL_STENCIL_PASS_DEPTH_PASS: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_STENCIL_REF
        case GL_STENCIL_REF: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_STENCIL_TEST
        case GL_STENCIL_TEST: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_STENCIL_VALUE_MASK
        case GL_STENCIL_VALUE_MASK: DEFINE_GET_PARAMETER_GLUINT
#endif
#ifdef GL_STENCIL_WRITEMASK
        case GL_STENCIL_WRITEMASK: DEFINE_GET_PARAMETER_GLUINT
#endif
#ifdef GL_SUBPIXEL_BITS
        case GL_SUBPIXEL_BITS: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_TEXTURE_BINDING_2D
        case GL_TEXTURE_BINDING_2D: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_TEXTURE_BINDING_CUBE_MAP
        case GL_TEXTURE_BINDING_CUBE_MAP: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_UNPACK_ALIGNMENT
        case GL_UNPACK_ALIGNMENT: DEFINE_GET_PARAMETER_GLINT
#endif
#ifdef GL_UNPACK_COLORSPACE_CONVERSION_WEBGL
        case GL_UNPACK_COLORSPACE_CONVERSION_WEBGL: DEFINE_GET_PARAMETER_GLENUM
#endif
#ifdef GL_UNPACK_FLIP_Y_WEBGL
        case GL_UNPACK_FLIP_Y_WEBGL: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL
        case GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL: DEFINE_GET_PARAMETER_GLBOOLEAN
#endif
#ifdef GL_VENDOR
        case GL_VENDOR: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_VERSION
        case GL_VERSION: DEFINE_GET_PARAMETER_UNDEFINED
#endif
#ifdef GL_VIEWPORT
        case GL_VIEWPORT: DEFINE_GET_PARAMETER_UNDEFINED
#endif

// WebGL 2

        default: DEFINE_GET_PARAMETER_UNDEFINED
    }

    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getActiveUniform(duk_context *ctx) {
    GLuint program = dukwebgl_get_object_id_uint(ctx, 0);
    GLuint index = (GLuint)duk_get_uint(ctx, 1);

    const GLsizei bufSize = 1024;
    GLchar name[bufSize];
    GLsizei length = 0;
    GLenum type;
    GLint size;

    glGetActiveUniform(program, index, bufSize, &length, &size, &type, name);

    dukwebgl_create_WebGLActiveInfo(ctx, name, length, type, size);

    return 1;
}

#endif /* GL_VERSION_2_0 */

#ifdef GL_VERSION_3_0

#define DEFINE_CLEAR_BUFFER_V(jsFunctionName, cType, cFunctionName) \
    DUK_LOCAL duk_ret_t dukwebgl_custom_impl_##jsFunctionName (duk_context *ctx) { \
        int argc = duk_get_top(ctx); \
        GLenum buffer = (GLenum)duk_get_uint(ctx, 0); \
        GLint drawbuffer = (GLint)duk_get_int(ctx, 1); \
        cType *value = (cType *)duk_get_buffer_data(ctx, 2, NULL); \
        if (argc > 3) { \
            GLuint srcOffset = (GLuint)duk_get_uint(ctx, 3); \
            value = value + srcOffset; \
        } \
        cFunctionName (buffer, drawbuffer, (const cType *)value); \
        return 0; \
    }

DEFINE_CLEAR_BUFFER_V(clearBufferfv, GLfloat, glClearBufferfv)
DEFINE_CLEAR_BUFFER_V(clearBufferiv, GLint, glClearBufferiv)
DEFINE_CLEAR_BUFFER_V(clearBufferuiv, GLuint, glClearBufferuiv)

DEFINE_CREATE_OBJECT(createVertexArray, glGenVertexArrays)
DEFINE_DELETE_OBJECT(deleteVertexArray, glDeleteVertexArrays)

DEFINE_CREATE_OBJECT(createFramebuffer, glGenFramebuffers)
DEFINE_DELETE_OBJECT(deleteFramebuffer, glDeleteFramebuffers)

DEFINE_CREATE_OBJECT(createRenderbuffer, glGenRenderbuffers)
DEFINE_DELETE_OBJECT(deleteRenderbuffer, glDeleteRenderbuffers)

#endif /* GL_VERSION_3_0 */

#ifdef GL_VERSION_3_1

DUK_LOCAL duk_ret_t dukwebgl_custom_impl_getActiveUniformBlockName(duk_context *ctx) {
    GLuint program = dukwebgl_get_object_id_uint(ctx, 0);
    GLuint uniformBlockIndex = duk_get_uint(ctx, 1);

    const GLsizei maxLength = 1024;
    GLchar uniformBlockName[maxLength];
    GLsizei length = 0;

    glGetActiveUniformBlockName(program, uniformBlockIndex, maxLength, &length, uniformBlockName);

    duk_push_string(ctx, (const char*)uniformBlockName);

    return 1;
}

#endif /* GL_VERSION_3_1 */

#ifdef GL_VERSION_3_2

/* only utilized with WebGLSync / GLsync */
DUK_LOCAL void dukwebgl_create_object_ptr(duk_context *ctx, void *ptr) {
    if (ptr == NULL) {
        duk_push_null(ctx);
        return;
    }

    duk_idx_t obj = duk_push_object(ctx);
    duk_push_pointer(ctx, ptr);
    duk_put_prop_string(ctx, obj, "_ptr");
}

DUK_LOCAL void* dukwebgl_get_object_ptr(duk_context *ctx, duk_idx_t obj_idx) {
    void* ret = NULL;

    /* everything else than object assumed null */
    if (duk_is_object(ctx, obj_idx)) {
        duk_get_prop_string(ctx, obj_idx, "_ptr");
        ret = duk_to_pointer(ctx, -1);
        duk_pop(ctx);
    }

    return ret;
}

DEFINE_CREATE_OBJECT(createSampler, glGenSamplers)
DEFINE_DELETE_OBJECT(deleteSampler, glDeleteSamplers)

#endif /* GL_VERSION_3_2 */

#ifdef GL_VERSION_4_0

DEFINE_CREATE_OBJECT(createTransformFeedback, glGenTransformFeedbacks)
DEFINE_DELETE_OBJECT(deleteTransformFeedback, glDeleteTransformFeedbacks)

#endif /* GL_VERSION_4_0 */

#ifdef GL_VERSION_0_0
    /* NOT IMPLEMENTED: void getBufferSubData ([{"original":"GLenum","type":"GLenum"},{"original":"GLintptr","type":"GLintptr"},{"original":"ArrayBufferView","type":"ArrayBufferView"},{"original":"GLuint","type":"GLuint"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: void invalidateFramebuffer ([{"original":"GLenum","type":"GLenum"},{"original":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"GLenum","baseName":"GLenum","escapedBaseName":"GLenum","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}],"type":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"GLenum","baseName":"GLenum","escapedBaseName":"GLenum","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}]}]) */
    /* NOT IMPLEMENTED: void invalidateSubFramebuffer ([{"original":"GLenum","type":"GLenum"},{"original":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"GLenum","baseName":"GLenum","escapedBaseName":"GLenum","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}],"type":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"GLenum","baseName":"GLenum","escapedBaseName":"GLenum","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}]},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"}]) */
    /* NOT IMPLEMENTED: any getInternalformatParameter ([{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: void texSubImage3D ([{"original":"GLenum","type":"GLenum"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"},{"original":"ArrayBufferView","type":"ArrayBufferView"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: void compressedTexImage3D ([{"original":"GLenum","type":"GLenum"},{"original":"GLint","type":"GLint"},{"original":"GLenum","type":"GLenum"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLint","type":"GLint"},{"original":"ArrayBufferView","type":"ArrayBufferView"},{"original":"GLuint","type":"GLuint"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: void compressedTexSubImage3D ([{"original":"GLenum","type":"GLenum"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLenum","type":"GLenum"},{"original":"ArrayBufferView","type":"ArrayBufferView"},{"original":"GLuint","type":"GLuint"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: WebGLQuery getQuery ([{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getQueryParameter ([{"original":"WebGLQuery","type":"WebGLQuery"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getSamplerParameter ([{"original":"WebGLSampler","type":"WebGLSampler"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getSyncParameter ([{"original":"WebGLSync","type":"WebGLSync"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: void transformFeedbackVaryings ([{"original":"WebGLProgram","type":"WebGLProgram"},{"original":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"DOMString","baseName":"DOMString","escapedBaseName":"DOMString","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}],"type":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"DOMString","baseName":"DOMString","escapedBaseName":"DOMString","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}]},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: WebGLActiveInfo getTransformFeedbackVarying ([{"original":"WebGLProgram","type":"WebGLProgram"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: any getIndexedParameter ([{"original":"GLenum","type":"GLenum"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: [object Object] getUniformIndices ([{"original":"WebGLProgram","type":"WebGLProgram"},{"original":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"DOMString","baseName":"DOMString","escapedBaseName":"DOMString","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}],"type":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"DOMString","baseName":"DOMString","escapedBaseName":"DOMString","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}]}]) */
    /* NOT IMPLEMENTED: any getActiveUniforms ([{"original":"WebGLProgram","type":"WebGLProgram"},{"original":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"GLuint","baseName":"GLuint","escapedBaseName":"GLuint","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}],"type":[{"type":"argument-type","generic":null,"nullable":null,"union":false,"idlType":"GLuint","baseName":"GLuint","escapedBaseName":"GLuint","prefix":null,"postfix":null,"separator":null,"extAttrs":null,"trivia":{"base":""}}]},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getActiveUniformBlockParameter ([{"original":"WebGLProgram","type":"WebGLProgram"},{"original":"GLuint","type":"GLuint"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: void compressedTexImage2D ([{"original":"GLenum","type":"GLenum"},{"original":"GLint","type":"GLint"},{"original":"GLenum","type":"GLenum"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLint","type":"GLint"},{"original":"ArrayBufferView","type":"ArrayBufferView"},{"original":"GLuint","type":"GLuint"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: void compressedTexSubImage2D ([{"original":"GLenum","type":"GLenum"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLenum","type":"GLenum"},{"original":"ArrayBufferView","type":"ArrayBufferView"},{"original":"GLuint","type":"GLuint"},{"original":"GLuint","type":"GLuint"}]) */
    /* NOT IMPLEMENTED: void compressedTexImage2D ([{"original":"GLenum","type":"GLenum"},{"original":"GLint","type":"GLint"},{"original":"GLenum","type":"GLenum"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLint","type":"GLint"},{"original":"ArrayBufferView","type":"ArrayBufferView"}]) */
    /* NOT IMPLEMENTED: void compressedTexSubImage2D ([{"original":"GLenum","type":"GLenum"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLint","type":"GLint"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLsizei","type":"GLsizei"},{"original":"GLenum","type":"GLenum"},{"original":"ArrayBufferView","type":"ArrayBufferView"}]) */
    /* NOT IMPLEMENTED: [object Object] getAttachedShaders ([{"original":"WebGLProgram","type":"WebGLProgram"}]) */
    /* NOT IMPLEMENTED: any getBufferParameter ([{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getParameter ([{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getFramebufferAttachmentParameter ([{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getRenderbufferParameter ([{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: WebGLShaderPrecisionFormat getShaderPrecisionFormat ([{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getTexParameter ([{"original":"GLenum","type":"GLenum"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: any getUniform ([{"original":"WebGLProgram","type":"WebGLProgram"},{"original":"WebGLUniformLocation","type":"WebGLUniformLocation"}]) */
    /* NOT IMPLEMENTED: any getVertexAttrib ([{"original":"GLuint","type":"GLuint"},{"original":"GLenum","type":"GLenum"}]) */
    /* NOT IMPLEMENTED: GLsizeiptr getVertexAttribOffset ([{"original":"GLuint","type":"GLuint"},{"original":"GLenum","type":"GLenum"}]) */
#endif /* GL_VERSION_0_0 */

#ifdef GL_VERSION_1_0

DUK_LOCAL duk_ret_t dukwebgl_glReadBuffer(duk_context *ctx) {
    GLenum src = (GLenum)duk_get_uint(ctx, 0);
    glReadBuffer(src);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBlendFunc(duk_context *ctx) {
    GLenum sfactor = (GLenum)duk_get_uint(ctx, 0);
    GLenum dfactor = (GLenum)duk_get_uint(ctx, 1);
    glBlendFunc(sfactor,dfactor);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glClear(duk_context *ctx) {
    GLbitfield mask = (GLbitfield)duk_get_uint(ctx, 0);
    glClear(mask);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glClearColor(duk_context *ctx) {
    GLfloat red = (GLfloat)duk_get_number(ctx, 0);
    GLfloat green = (GLfloat)duk_get_number(ctx, 1);
    GLfloat blue = (GLfloat)duk_get_number(ctx, 2);
    GLfloat alpha = (GLfloat)duk_get_number(ctx, 3);
    glClearColor(red,green,blue,alpha);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glClearDepth(duk_context *ctx) {
    GLdouble depth = (GLdouble)duk_get_number(ctx, 0);
    glClearDepth(depth);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glClearStencil(duk_context *ctx) {
    GLint s = (GLint)duk_get_int(ctx, 0);
    glClearStencil(s);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glColorMask(duk_context *ctx) {
    GLboolean red = (GLboolean)(duk_get_boolean(ctx, 0) == 1 ? GL_TRUE : GL_FALSE);
    GLboolean green = (GLboolean)(duk_get_boolean(ctx, 1) == 1 ? GL_TRUE : GL_FALSE);
    GLboolean blue = (GLboolean)(duk_get_boolean(ctx, 2) == 1 ? GL_TRUE : GL_FALSE);
    GLboolean alpha = (GLboolean)(duk_get_boolean(ctx, 3) == 1 ? GL_TRUE : GL_FALSE);
    glColorMask(red,green,blue,alpha);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCullFace(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    glCullFace(mode);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDepthFunc(duk_context *ctx) {
    GLenum func = (GLenum)duk_get_uint(ctx, 0);
    glDepthFunc(func);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDepthMask(duk_context *ctx) {
    GLboolean flag = (GLboolean)(duk_get_boolean(ctx, 0) == 1 ? GL_TRUE : GL_FALSE);
    glDepthMask(flag);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDepthRange(duk_context *ctx) {
    GLdouble n = (GLdouble)duk_get_number(ctx, 0);
    GLdouble f = (GLdouble)duk_get_number(ctx, 1);
    glDepthRange(n,f);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDisable(duk_context *ctx) {
    GLenum cap = (GLenum)duk_get_uint(ctx, 0);
    glDisable(cap);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glEnable(duk_context *ctx) {
    GLenum cap = (GLenum)duk_get_uint(ctx, 0);
    glEnable(cap);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glFinish(duk_context *ctx) {
    glFinish();
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glFlush(duk_context *ctx) {
    glFlush();
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glFrontFace(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    glFrontFace(mode);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glGetError(duk_context *ctx) {
    GLenum ret = glGetError();
    duk_push_uint(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glHint(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum mode = (GLenum)duk_get_uint(ctx, 1);
    glHint(target,mode);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsEnabled(duk_context *ctx) {
    GLenum cap = (GLenum)duk_get_uint(ctx, 0);
    GLboolean ret = glIsEnabled(cap);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glLineWidth(duk_context *ctx) {
    GLfloat width = (GLfloat)duk_get_number(ctx, 0);
    glLineWidth(width);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glPixelStorei(duk_context *ctx) {
    GLenum pname = (GLenum)duk_get_uint(ctx, 0);
    GLint param = (GLint)duk_get_int(ctx, 1);
    glPixelStorei(pname,param);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glScissor(duk_context *ctx) {
    GLint x = (GLint)duk_get_int(ctx, 0);
    GLint y = (GLint)duk_get_int(ctx, 1);
    GLsizei width = (GLsizei)duk_get_int(ctx, 2);
    GLsizei height = (GLsizei)duk_get_int(ctx, 3);
    glScissor(x,y,width,height);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glStencilFunc(duk_context *ctx) {
    GLenum func = (GLenum)duk_get_uint(ctx, 0);
    GLint ref = (GLint)duk_get_int(ctx, 1);
    GLuint mask = (GLuint)duk_get_uint(ctx, 2);
    glStencilFunc(func,ref,mask);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glStencilMask(duk_context *ctx) {
    GLuint mask = (GLuint)duk_get_uint(ctx, 0);
    glStencilMask(mask);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glStencilOp(duk_context *ctx) {
    GLenum fail = (GLenum)duk_get_uint(ctx, 0);
    GLenum zfail = (GLenum)duk_get_uint(ctx, 1);
    GLenum zpass = (GLenum)duk_get_uint(ctx, 2);
    glStencilOp(fail,zfail,zpass);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glTexParameterf(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum pname = (GLenum)duk_get_uint(ctx, 1);
    GLfloat param = (GLfloat)duk_get_number(ctx, 2);
    glTexParameterf(target,pname,param);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glTexParameteri(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum pname = (GLenum)duk_get_uint(ctx, 1);
    GLint param = (GLint)duk_get_int(ctx, 2);
    glTexParameteri(target,pname,param);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glViewport(duk_context *ctx) {
    GLint x = (GLint)duk_get_int(ctx, 0);
    GLint y = (GLint)duk_get_int(ctx, 1);
    GLsizei width = (GLsizei)duk_get_int(ctx, 2);
    GLsizei height = (GLsizei)duk_get_int(ctx, 3);
    glViewport(x,y,width,height);
    return 0;
}
#endif /* GL_VERSION_1_0 */

#ifdef GL_VERSION_1_1

DUK_LOCAL duk_ret_t dukwebgl_glBindTexture(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint texture = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glBindTexture(target,texture);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCopyTexImage2D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLenum internalformat = (GLenum)duk_get_uint(ctx, 2);
    GLint x = (GLint)duk_get_int(ctx, 3);
    GLint y = (GLint)duk_get_int(ctx, 4);
    GLsizei width = (GLsizei)duk_get_int(ctx, 5);
    GLsizei height = (GLsizei)duk_get_int(ctx, 6);
    GLint border = (GLint)duk_get_int(ctx, 7);
    glCopyTexImage2D(target,level,internalformat,x,y,width,height,border);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCopyTexSubImage2D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint xoffset = (GLint)duk_get_int(ctx, 2);
    GLint yoffset = (GLint)duk_get_int(ctx, 3);
    GLint x = (GLint)duk_get_int(ctx, 4);
    GLint y = (GLint)duk_get_int(ctx, 5);
    GLsizei width = (GLsizei)duk_get_int(ctx, 6);
    GLsizei height = (GLsizei)duk_get_int(ctx, 7);
    glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDrawArrays(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    GLint first = (GLint)duk_get_int(ctx, 1);
    GLsizei count = (GLsizei)duk_get_int(ctx, 2);
    glDrawArrays(mode,first,count);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDrawElements(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    GLsizei count = (GLsizei)duk_get_int(ctx, 1);
    GLenum type = (GLenum)duk_get_uint(ctx, 2);
    const void * indices = (const void *)((char*)NULL + duk_get_int(ctx, 3));
    glDrawElements(mode,count,type,indices);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsTexture(duk_context *ctx) {
    GLuint texture = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsTexture(texture);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glPolygonOffset(duk_context *ctx) {
    GLfloat factor = (GLfloat)duk_get_number(ctx, 0);
    GLfloat units = (GLfloat)duk_get_number(ctx, 1);
    glPolygonOffset(factor,units);
    return 0;
}
#endif /* GL_VERSION_1_1 */

#ifdef GL_VERSION_1_2

DUK_LOCAL duk_ret_t dukwebgl_glTexSubImage3D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint xoffset = (GLint)duk_get_int(ctx, 2);
    GLint yoffset = (GLint)duk_get_int(ctx, 3);
    GLint zoffset = (GLint)duk_get_int(ctx, 4);
    GLsizei width = (GLsizei)duk_get_int(ctx, 5);
    GLsizei height = (GLsizei)duk_get_int(ctx, 6);
    GLsizei depth = (GLsizei)duk_get_int(ctx, 7);
    GLenum format = (GLenum)duk_get_uint(ctx, 8);
    GLenum type = (GLenum)duk_get_uint(ctx, 9);
    const void * pixels = (const void *)((char*)NULL + duk_get_int(ctx, 10));
    glTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
    return 0;
}
    /* Cannot process method: void texSubImage3D => glTexSubImage3D. argument: TexImageSource vs. const void * */
DUK_LOCAL duk_ret_t dukwebgl_glCopyTexSubImage3D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint xoffset = (GLint)duk_get_int(ctx, 2);
    GLint yoffset = (GLint)duk_get_int(ctx, 3);
    GLint zoffset = (GLint)duk_get_int(ctx, 4);
    GLint x = (GLint)duk_get_int(ctx, 5);
    GLint y = (GLint)duk_get_int(ctx, 6);
    GLsizei width = (GLsizei)duk_get_int(ctx, 7);
    GLsizei height = (GLsizei)duk_get_int(ctx, 8);
    glCopyTexSubImage3D(target,level,xoffset,yoffset,zoffset,x,y,width,height);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDrawRangeElements(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    GLuint start = (GLuint)duk_get_uint(ctx, 1);
    GLuint end = (GLuint)duk_get_uint(ctx, 2);
    GLsizei count = (GLsizei)duk_get_int(ctx, 3);
    GLenum type = (GLenum)duk_get_uint(ctx, 4);
    const void * indices = (const void *)((char*)NULL + duk_get_int(ctx, 5));
    glDrawRangeElements(mode,start,end,count,type,indices);
    return 0;
}
#endif /* GL_VERSION_1_2 */

#ifdef GL_VERSION_1_3

DUK_LOCAL duk_ret_t dukwebgl_glCompressedTexImage3D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLenum internalformat = (GLenum)duk_get_uint(ctx, 2);
    GLsizei width = (GLsizei)duk_get_int(ctx, 3);
    GLsizei height = (GLsizei)duk_get_int(ctx, 4);
    GLsizei depth = (GLsizei)duk_get_int(ctx, 5);
    GLint border = (GLint)duk_get_int(ctx, 6);
    GLsizei imageSize = (GLsizei)duk_get_int(ctx, 7);
    const void * data = (const void *)((char*)NULL + duk_get_int(ctx, 8));
    glCompressedTexImage3D(target,level,internalformat,width,height,depth,border,imageSize,data);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCompressedTexSubImage3D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint xoffset = (GLint)duk_get_int(ctx, 2);
    GLint yoffset = (GLint)duk_get_int(ctx, 3);
    GLint zoffset = (GLint)duk_get_int(ctx, 4);
    GLsizei width = (GLsizei)duk_get_int(ctx, 5);
    GLsizei height = (GLsizei)duk_get_int(ctx, 6);
    GLsizei depth = (GLsizei)duk_get_int(ctx, 7);
    GLenum format = (GLenum)duk_get_uint(ctx, 8);
    GLsizei imageSize = (GLsizei)duk_get_int(ctx, 9);
    const void * data = (const void *)((char*)NULL + duk_get_int(ctx, 10));
    glCompressedTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,height,depth,format,imageSize,data);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCompressedTexImage2D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLenum internalformat = (GLenum)duk_get_uint(ctx, 2);
    GLsizei width = (GLsizei)duk_get_int(ctx, 3);
    GLsizei height = (GLsizei)duk_get_int(ctx, 4);
    GLint border = (GLint)duk_get_int(ctx, 5);
    GLsizei imageSize = (GLsizei)duk_get_int(ctx, 6);
    const void * data = (const void *)((char*)NULL + duk_get_int(ctx, 7));
    glCompressedTexImage2D(target,level,internalformat,width,height,border,imageSize,data);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCompressedTexSubImage2D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLint level = (GLint)duk_get_int(ctx, 1);
    GLint xoffset = (GLint)duk_get_int(ctx, 2);
    GLint yoffset = (GLint)duk_get_int(ctx, 3);
    GLsizei width = (GLsizei)duk_get_int(ctx, 4);
    GLsizei height = (GLsizei)duk_get_int(ctx, 5);
    GLenum format = (GLenum)duk_get_uint(ctx, 6);
    GLsizei imageSize = (GLsizei)duk_get_int(ctx, 7);
    const void * data = (const void *)((char*)NULL + duk_get_int(ctx, 8));
    glCompressedTexSubImage2D(target,level,xoffset,yoffset,width,height,format,imageSize,data);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glActiveTexture(duk_context *ctx) {
    GLenum texture = (GLenum)duk_get_uint(ctx, 0);
    glActiveTexture(texture);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glSampleCoverage(duk_context *ctx) {
    GLfloat value = (GLfloat)duk_get_number(ctx, 0);
    GLboolean invert = (GLboolean)(duk_get_boolean(ctx, 1) == 1 ? GL_TRUE : GL_FALSE);
    glSampleCoverage(value,invert);
    return 0;
}
#endif /* GL_VERSION_1_3 */

#ifdef GL_VERSION_1_4

DUK_LOCAL duk_ret_t dukwebgl_glBlendColor(duk_context *ctx) {
    GLfloat red = (GLfloat)duk_get_number(ctx, 0);
    GLfloat green = (GLfloat)duk_get_number(ctx, 1);
    GLfloat blue = (GLfloat)duk_get_number(ctx, 2);
    GLfloat alpha = (GLfloat)duk_get_number(ctx, 3);
    glBlendColor(red,green,blue,alpha);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBlendEquation(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    glBlendEquation(mode);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBlendFuncSeparate(duk_context *ctx) {
    GLenum sfactorRGB = (GLenum)duk_get_uint(ctx, 0);
    GLenum dfactorRGB = (GLenum)duk_get_uint(ctx, 1);
    GLenum sfactorAlpha = (GLenum)duk_get_uint(ctx, 2);
    GLenum dfactorAlpha = (GLenum)duk_get_uint(ctx, 3);
    glBlendFuncSeparate(sfactorRGB,dfactorRGB,sfactorAlpha,dfactorAlpha);
    return 0;
}
#endif /* GL_VERSION_1_4 */

#ifdef GL_VERSION_1_5

DUK_LOCAL duk_ret_t dukwebgl_glIsQuery(duk_context *ctx) {
    GLuint id = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsQuery(id);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glBeginQuery(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint id = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glBeginQuery(target,id);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glEndQuery(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    glEndQuery(target);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindBuffer(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint buffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glBindBuffer(target,buffer);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsBuffer(duk_context *ctx) {
    GLuint buffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsBuffer(buffer);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}
#endif /* GL_VERSION_1_5 */

#ifdef GL_VERSION_2_0

DUK_LOCAL duk_ret_t dukwebgl_glAttachShader(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLuint shader = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glAttachShader(program,shader);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindAttribLocation(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLuint index = (GLuint)duk_get_uint(ctx, 1);
    const GLchar * name = (const GLchar *)duk_get_string(ctx, 2);
    glBindAttribLocation(program,index,name);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBlendEquationSeparate(duk_context *ctx) {
    GLenum modeRGB = (GLenum)duk_get_uint(ctx, 0);
    GLenum modeAlpha = (GLenum)duk_get_uint(ctx, 1);
    glBlendEquationSeparate(modeRGB,modeAlpha);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCompileShader(duk_context *ctx) {
    GLuint shader = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    glCompileShader(shader);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCreateProgram(duk_context *ctx) {
    GLuint ret = glCreateProgram();
    dukwebgl_create_object_uint(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glCreateShader(duk_context *ctx) {
    GLenum type = (GLenum)duk_get_uint(ctx, 0);
    GLuint ret = glCreateShader(type);
    dukwebgl_create_object_uint(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glDeleteProgram(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    glDeleteProgram(program);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDeleteShader(duk_context *ctx) {
    GLuint shader = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    glDeleteShader(shader);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDetachShader(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLuint shader = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glDetachShader(program,shader);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDisableVertexAttribArray(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    glDisableVertexAttribArray(index);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glEnableVertexAttribArray(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    glEnableVertexAttribArray(index);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glGetAttribLocation(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    const GLchar * name = (const GLchar *)duk_get_string(ctx, 1);
    GLint ret = glGetAttribLocation(program,name);
    duk_push_int(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glGetUniformLocation(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    const GLchar * name = (const GLchar *)duk_get_string(ctx, 1);
    GLint ret = glGetUniformLocation(program,name);
    dukwebgl_create_object_int(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsProgram(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsProgram(program);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsShader(duk_context *ctx) {
    GLuint shader = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsShader(shader);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glLinkProgram(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    glLinkProgram(program);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glStencilFuncSeparate(duk_context *ctx) {
    GLenum face = (GLenum)duk_get_uint(ctx, 0);
    GLenum func = (GLenum)duk_get_uint(ctx, 1);
    GLint ref = (GLint)duk_get_int(ctx, 2);
    GLuint mask = (GLuint)duk_get_uint(ctx, 3);
    glStencilFuncSeparate(face,func,ref,mask);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glStencilMaskSeparate(duk_context *ctx) {
    GLenum face = (GLenum)duk_get_uint(ctx, 0);
    GLuint mask = (GLuint)duk_get_uint(ctx, 1);
    glStencilMaskSeparate(face,mask);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glStencilOpSeparate(duk_context *ctx) {
    GLenum face = (GLenum)duk_get_uint(ctx, 0);
    GLenum sfail = (GLenum)duk_get_uint(ctx, 1);
    GLenum dpfail = (GLenum)duk_get_uint(ctx, 2);
    GLenum dppass = (GLenum)duk_get_uint(ctx, 3);
    glStencilOpSeparate(face,sfail,dpfail,dppass);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform1f(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLfloat v0 = (GLfloat)duk_get_number(ctx, 1);
    glUniform1f(location,v0);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform1i(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLint v0 = (GLint)duk_get_int(ctx, 1);
    glUniform1i(location,v0);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform2f(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLfloat v0 = (GLfloat)duk_get_number(ctx, 1);
    GLfloat v1 = (GLfloat)duk_get_number(ctx, 2);
    glUniform2f(location,v0,v1);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform2i(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLint v0 = (GLint)duk_get_int(ctx, 1);
    GLint v1 = (GLint)duk_get_int(ctx, 2);
    glUniform2i(location,v0,v1);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform3f(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLfloat v0 = (GLfloat)duk_get_number(ctx, 1);
    GLfloat v1 = (GLfloat)duk_get_number(ctx, 2);
    GLfloat v2 = (GLfloat)duk_get_number(ctx, 3);
    glUniform3f(location,v0,v1,v2);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform3i(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLint v0 = (GLint)duk_get_int(ctx, 1);
    GLint v1 = (GLint)duk_get_int(ctx, 2);
    GLint v2 = (GLint)duk_get_int(ctx, 3);
    glUniform3i(location,v0,v1,v2);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform4f(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLfloat v0 = (GLfloat)duk_get_number(ctx, 1);
    GLfloat v1 = (GLfloat)duk_get_number(ctx, 2);
    GLfloat v2 = (GLfloat)duk_get_number(ctx, 3);
    GLfloat v3 = (GLfloat)duk_get_number(ctx, 4);
    glUniform4f(location,v0,v1,v2,v3);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform4i(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLint v0 = (GLint)duk_get_int(ctx, 1);
    GLint v1 = (GLint)duk_get_int(ctx, 2);
    GLint v2 = (GLint)duk_get_int(ctx, 3);
    GLint v3 = (GLint)duk_get_int(ctx, 4);
    glUniform4i(location,v0,v1,v2,v3);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUseProgram(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    glUseProgram(program);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glValidateProgram(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    glValidateProgram(program);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib1f(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLfloat x = (GLfloat)duk_get_number(ctx, 1);
    glVertexAttrib1f(index,x);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib1fv(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    const GLfloat * v = (const GLfloat *)duk_get_buffer_data(ctx, 1, NULL);
    glVertexAttrib1fv(index,v);
    return 0;
}
    /* Cannot process method: void vertexAttrib1fv => glVertexAttrib1fv. argument: [object Object] vs. const GLfloat * */
DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib2f(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLfloat x = (GLfloat)duk_get_number(ctx, 1);
    GLfloat y = (GLfloat)duk_get_number(ctx, 2);
    glVertexAttrib2f(index,x,y);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib2fv(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    const GLfloat * v = (const GLfloat *)duk_get_buffer_data(ctx, 1, NULL);
    glVertexAttrib2fv(index,v);
    return 0;
}
    /* Cannot process method: void vertexAttrib2fv => glVertexAttrib2fv. argument: [object Object] vs. const GLfloat * */
DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib3f(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLfloat x = (GLfloat)duk_get_number(ctx, 1);
    GLfloat y = (GLfloat)duk_get_number(ctx, 2);
    GLfloat z = (GLfloat)duk_get_number(ctx, 3);
    glVertexAttrib3f(index,x,y,z);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib3fv(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    const GLfloat * v = (const GLfloat *)duk_get_buffer_data(ctx, 1, NULL);
    glVertexAttrib3fv(index,v);
    return 0;
}
    /* Cannot process method: void vertexAttrib3fv => glVertexAttrib3fv. argument: [object Object] vs. const GLfloat * */
DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib4f(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLfloat x = (GLfloat)duk_get_number(ctx, 1);
    GLfloat y = (GLfloat)duk_get_number(ctx, 2);
    GLfloat z = (GLfloat)duk_get_number(ctx, 3);
    GLfloat w = (GLfloat)duk_get_number(ctx, 4);
    glVertexAttrib4f(index,x,y,z,w);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttrib4fv(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    const GLfloat * v = (const GLfloat *)duk_get_buffer_data(ctx, 1, NULL);
    glVertexAttrib4fv(index,v);
    return 0;
}
    /* Cannot process method: void vertexAttrib4fv => glVertexAttrib4fv. argument: [object Object] vs. const GLfloat * */
DUK_LOCAL duk_ret_t dukwebgl_glVertexAttribPointer(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLint size = (GLint)duk_get_int(ctx, 1);
    GLenum type = (GLenum)duk_get_uint(ctx, 2);
    GLboolean normalized = (GLboolean)(duk_get_boolean(ctx, 3) == 1 ? GL_TRUE : GL_FALSE);
    GLsizei stride = (GLsizei)duk_get_int(ctx, 4);
    const void * pointer = (const void *)((char*)NULL + duk_get_int(ctx, 5));
    glVertexAttribPointer(index,size,type,normalized,stride,pointer);
    return 0;
}
#endif /* GL_VERSION_2_0 */

#ifdef GL_VERSION_2_1
#endif /* GL_VERSION_2_1 */

#ifdef GL_VERSION_3_0

DUK_LOCAL duk_ret_t dukwebgl_glBlitFramebuffer(duk_context *ctx) {
    GLint srcX0 = (GLint)duk_get_int(ctx, 0);
    GLint srcY0 = (GLint)duk_get_int(ctx, 1);
    GLint srcX1 = (GLint)duk_get_int(ctx, 2);
    GLint srcY1 = (GLint)duk_get_int(ctx, 3);
    GLint dstX0 = (GLint)duk_get_int(ctx, 4);
    GLint dstY0 = (GLint)duk_get_int(ctx, 5);
    GLint dstX1 = (GLint)duk_get_int(ctx, 6);
    GLint dstY1 = (GLint)duk_get_int(ctx, 7);
    GLbitfield mask = (GLbitfield)duk_get_uint(ctx, 8);
    GLenum filter = (GLenum)duk_get_uint(ctx, 9);
    glBlitFramebuffer(srcX0,srcY0,srcX1,srcY1,dstX0,dstY0,dstX1,dstY1,mask,filter);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glFramebufferTextureLayer(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum attachment = (GLenum)duk_get_uint(ctx, 1);
    GLuint texture = (GLuint)dukwebgl_get_object_id_uint(ctx, 2);
    GLint level = (GLint)duk_get_int(ctx, 3);
    GLint layer = (GLint)duk_get_int(ctx, 4);
    glFramebufferTextureLayer(target,attachment,texture,level,layer);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glRenderbufferStorageMultisample(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLsizei samples = (GLsizei)duk_get_int(ctx, 1);
    GLenum internalformat = (GLenum)duk_get_uint(ctx, 2);
    GLsizei width = (GLsizei)duk_get_int(ctx, 3);
    GLsizei height = (GLsizei)duk_get_int(ctx, 4);
    glRenderbufferStorageMultisample(target,samples,internalformat,width,height);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glGetFragDataLocation(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    const GLchar * name = (const GLchar *)duk_get_string(ctx, 1);
    GLint ret = glGetFragDataLocation(program,name);
    duk_push_int(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform1ui(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLuint v0 = (GLuint)duk_get_uint(ctx, 1);
    glUniform1ui(location,v0);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform2ui(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLuint v0 = (GLuint)duk_get_uint(ctx, 1);
    GLuint v1 = (GLuint)duk_get_uint(ctx, 2);
    glUniform2ui(location,v0,v1);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform3ui(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLuint v0 = (GLuint)duk_get_uint(ctx, 1);
    GLuint v1 = (GLuint)duk_get_uint(ctx, 2);
    GLuint v2 = (GLuint)duk_get_uint(ctx, 3);
    glUniform3ui(location,v0,v1,v2);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniform4ui(duk_context *ctx) {
    GLint location = (GLint)dukwebgl_get_object_id_int(ctx, 0);
    GLuint v0 = (GLuint)duk_get_uint(ctx, 1);
    GLuint v1 = (GLuint)duk_get_uint(ctx, 2);
    GLuint v2 = (GLuint)duk_get_uint(ctx, 3);
    GLuint v3 = (GLuint)duk_get_uint(ctx, 4);
    glUniform4ui(location,v0,v1,v2,v3);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttribI4i(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLint x = (GLint)duk_get_int(ctx, 1);
    GLint y = (GLint)duk_get_int(ctx, 2);
    GLint z = (GLint)duk_get_int(ctx, 3);
    GLint w = (GLint)duk_get_int(ctx, 4);
    glVertexAttribI4i(index,x,y,z,w);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttribI4iv(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    const GLint * v = (const GLint *)duk_get_buffer_data(ctx, 1, NULL);
    glVertexAttribI4iv(index,v);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttribI4ui(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLuint x = (GLuint)duk_get_uint(ctx, 1);
    GLuint y = (GLuint)duk_get_uint(ctx, 2);
    GLuint z = (GLuint)duk_get_uint(ctx, 3);
    GLuint w = (GLuint)duk_get_uint(ctx, 4);
    glVertexAttribI4ui(index,x,y,z,w);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttribI4uiv(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    const GLuint * v = (const GLuint *)duk_get_buffer_data(ctx, 1, NULL);
    glVertexAttribI4uiv(index,v);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttribIPointer(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLint size = (GLint)duk_get_int(ctx, 1);
    GLenum type = (GLenum)duk_get_uint(ctx, 2);
    GLsizei stride = (GLsizei)duk_get_int(ctx, 3);
    const void * pointer = (const void *)((char*)NULL + duk_get_int(ctx, 4));
    glVertexAttribIPointer(index,size,type,stride,pointer);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glClearBufferfi(duk_context *ctx) {
    GLenum buffer = (GLenum)duk_get_uint(ctx, 0);
    GLint drawbuffer = (GLint)duk_get_int(ctx, 1);
    GLfloat depth = (GLfloat)duk_get_number(ctx, 2);
    GLint stencil = (GLint)duk_get_int(ctx, 3);
    glClearBufferfi(buffer,drawbuffer,depth,stencil);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBeginTransformFeedback(duk_context *ctx) {
    GLenum primitiveMode = (GLenum)duk_get_uint(ctx, 0);
    glBeginTransformFeedback(primitiveMode);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glEndTransformFeedback(duk_context *ctx) {
    glEndTransformFeedback();
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindBufferBase(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint index = (GLuint)duk_get_uint(ctx, 1);
    GLuint buffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 2);
    glBindBufferBase(target,index,buffer);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindBufferRange(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint index = (GLuint)duk_get_uint(ctx, 1);
    GLuint buffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 2);
    GLintptr offset = (GLintptr)((char*)NULL + duk_get_int(ctx, 3));
    GLsizeiptr size = (GLsizeiptr)duk_get_int(ctx, 4);
    glBindBufferRange(target,index,buffer,offset,size);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsVertexArray(duk_context *ctx) {
    GLuint array = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsVertexArray(array);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindVertexArray(duk_context *ctx) {
    GLuint array = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    glBindVertexArray(array);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindFramebuffer(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint framebuffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glBindFramebuffer(target,framebuffer);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindRenderbuffer(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint renderbuffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glBindRenderbuffer(target,renderbuffer);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glCheckFramebufferStatus(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum ret = glCheckFramebufferStatus(target);
    duk_push_uint(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glFramebufferRenderbuffer(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum attachment = (GLenum)duk_get_uint(ctx, 1);
    GLenum renderbuffertarget = (GLenum)duk_get_uint(ctx, 2);
    GLuint renderbuffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 3);
    glFramebufferRenderbuffer(target,attachment,renderbuffertarget,renderbuffer);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glFramebufferTexture2D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum attachment = (GLenum)duk_get_uint(ctx, 1);
    GLenum textarget = (GLenum)duk_get_uint(ctx, 2);
    GLuint texture = (GLuint)dukwebgl_get_object_id_uint(ctx, 3);
    GLint level = (GLint)duk_get_int(ctx, 4);
    glFramebufferTexture2D(target,attachment,textarget,texture,level);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glGenerateMipmap(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    glGenerateMipmap(target);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsFramebuffer(duk_context *ctx) {
    GLuint framebuffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsFramebuffer(framebuffer);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsRenderbuffer(duk_context *ctx) {
    GLuint renderbuffer = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsRenderbuffer(renderbuffer);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glRenderbufferStorage(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLenum internalformat = (GLenum)duk_get_uint(ctx, 1);
    GLsizei width = (GLsizei)duk_get_int(ctx, 2);
    GLsizei height = (GLsizei)duk_get_int(ctx, 3);
    glRenderbufferStorage(target,internalformat,width,height);
    return 0;
}
#endif /* GL_VERSION_3_0 */

#ifdef GL_VERSION_3_1

DUK_LOCAL duk_ret_t dukwebgl_glCopyBufferSubData(duk_context *ctx) {
    GLenum readTarget = (GLenum)duk_get_uint(ctx, 0);
    GLenum writeTarget = (GLenum)duk_get_uint(ctx, 1);
    GLintptr readOffset = (GLintptr)((char*)NULL + duk_get_int(ctx, 2));
    GLintptr writeOffset = (GLintptr)((char*)NULL + duk_get_int(ctx, 3));
    GLsizeiptr size = (GLsizeiptr)duk_get_int(ctx, 4);
    glCopyBufferSubData(readTarget,writeTarget,readOffset,writeOffset,size);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDrawArraysInstanced(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    GLint first = (GLint)duk_get_int(ctx, 1);
    GLsizei count = (GLsizei)duk_get_int(ctx, 2);
    GLsizei instancecount = (GLsizei)duk_get_int(ctx, 3);
    glDrawArraysInstanced(mode,first,count,instancecount);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glDrawElementsInstanced(duk_context *ctx) {
    GLenum mode = (GLenum)duk_get_uint(ctx, 0);
    GLsizei count = (GLsizei)duk_get_int(ctx, 1);
    GLenum type = (GLenum)duk_get_uint(ctx, 2);
    const void * indices = (const void *)((char*)NULL + duk_get_int(ctx, 3));
    GLsizei instancecount = (GLsizei)duk_get_int(ctx, 4);
    glDrawElementsInstanced(mode,count,type,indices,instancecount);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glGetUniformBlockIndex(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    const GLchar * uniformBlockName = (const GLchar *)duk_get_string(ctx, 1);
    GLuint ret = glGetUniformBlockIndex(program,uniformBlockName);
    duk_push_uint(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glUniformBlockBinding(duk_context *ctx) {
    GLuint program = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLuint uniformBlockIndex = (GLuint)duk_get_uint(ctx, 1);
    GLuint uniformBlockBinding = (GLuint)duk_get_uint(ctx, 2);
    glUniformBlockBinding(program,uniformBlockIndex,uniformBlockBinding);
    return 0;
}
#endif /* GL_VERSION_3_1 */

#ifdef GL_VERSION_3_2

DUK_LOCAL duk_ret_t dukwebgl_glFenceSync(duk_context *ctx) {
    GLenum condition = (GLenum)duk_get_uint(ctx, 0);
    GLbitfield flags = (GLbitfield)duk_get_uint(ctx, 1);
    GLsync ret = glFenceSync(condition,flags);
    dukwebgl_create_object_ptr(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsSync(duk_context *ctx) {
    GLsync sync = (GLsync)dukwebgl_get_object_ptr(ctx, 0);
    GLboolean ret = glIsSync(sync);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glDeleteSync(duk_context *ctx) {
    GLsync sync = (GLsync)dukwebgl_get_object_ptr(ctx, 0);
    glDeleteSync(sync);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glClientWaitSync(duk_context *ctx) {
    GLsync sync = (GLsync)dukwebgl_get_object_ptr(ctx, 0);
    GLbitfield flags = (GLbitfield)duk_get_uint(ctx, 1);
    GLuint64 timeout = (GLuint64)duk_get_uint(ctx, 2);
    GLenum ret = glClientWaitSync(sync,flags,timeout);
    duk_push_uint(ctx, ret);
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glWaitSync(duk_context *ctx) {
    GLsync sync = (GLsync)dukwebgl_get_object_ptr(ctx, 0);
    GLbitfield flags = (GLbitfield)duk_get_uint(ctx, 1);
    GLuint64 timeout = (GLuint64)duk_get_int(ctx, 2);
    glWaitSync(sync,flags,timeout);
    return 0;
}
#endif /* GL_VERSION_3_2 */

#ifdef GL_VERSION_3_3

DUK_LOCAL duk_ret_t dukwebgl_glVertexAttribDivisor(duk_context *ctx) {
    GLuint index = (GLuint)duk_get_uint(ctx, 0);
    GLuint divisor = (GLuint)duk_get_uint(ctx, 1);
    glVertexAttribDivisor(index,divisor);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glIsSampler(duk_context *ctx) {
    GLuint sampler = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsSampler(sampler);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindSampler(duk_context *ctx) {
    GLuint unit = (GLuint)duk_get_uint(ctx, 0);
    GLuint sampler = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glBindSampler(unit,sampler);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glSamplerParameteri(duk_context *ctx) {
    GLuint sampler = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLenum pname = (GLenum)duk_get_uint(ctx, 1);
    GLint param = (GLint)duk_get_int(ctx, 2);
    glSamplerParameteri(sampler,pname,param);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glSamplerParameterf(duk_context *ctx) {
    GLuint sampler = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLenum pname = (GLenum)duk_get_uint(ctx, 1);
    GLfloat param = (GLfloat)duk_get_number(ctx, 2);
    glSamplerParameterf(sampler,pname,param);
    return 0;
}
#endif /* GL_VERSION_3_3 */

#ifdef GL_VERSION_4_0

DUK_LOCAL duk_ret_t dukwebgl_glIsTransformFeedback(duk_context *ctx) {
    GLuint id = (GLuint)dukwebgl_get_object_id_uint(ctx, 0);
    GLboolean ret = glIsTransformFeedback(id);
    duk_push_boolean(ctx, (ret == GL_TRUE ? 1 : 0));
    return 1;
}

DUK_LOCAL duk_ret_t dukwebgl_glBindTransformFeedback(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLuint id = (GLuint)dukwebgl_get_object_id_uint(ctx, 1);
    glBindTransformFeedback(target,id);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glPauseTransformFeedback(duk_context *ctx) {
    glPauseTransformFeedback();
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glResumeTransformFeedback(duk_context *ctx) {
    glResumeTransformFeedback();
    return 0;
}
#endif /* GL_VERSION_4_0 */

#ifdef GL_VERSION_4_1
#endif /* GL_VERSION_4_1 */

#ifdef GL_VERSION_4_2

DUK_LOCAL duk_ret_t dukwebgl_glTexStorage2D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLsizei levels = (GLsizei)duk_get_int(ctx, 1);
    GLenum internalformat = (GLenum)duk_get_uint(ctx, 2);
    GLsizei width = (GLsizei)duk_get_int(ctx, 3);
    GLsizei height = (GLsizei)duk_get_int(ctx, 4);
    glTexStorage2D(target,levels,internalformat,width,height);
    return 0;
}

DUK_LOCAL duk_ret_t dukwebgl_glTexStorage3D(duk_context *ctx) {
    GLenum target = (GLenum)duk_get_uint(ctx, 0);
    GLsizei levels = (GLsizei)duk_get_int(ctx, 1);
    GLenum internalformat = (GLenum)duk_get_uint(ctx, 2);
    GLsizei width = (GLsizei)duk_get_int(ctx, 3);
    GLsizei height = (GLsizei)duk_get_int(ctx, 4);
    GLsizei depth = (GLsizei)duk_get_int(ctx, 5);
    glTexStorage3D(target,levels,internalformat,width,height,depth);
    return 0;
}
#endif /* GL_VERSION_4_2 */

#ifdef GL_VERSION_4_3
#endif /* GL_VERSION_4_3 */

#ifdef GL_VERSION_4_4
#endif /* GL_VERSION_4_4 */

#ifdef GL_VERSION_4_5
#endif /* GL_VERSION_4_5 */

#ifdef GL_VERSION_4_6
#endif /* GL_VERSION_4_6 */

DUK_LOCAL duk_ret_t dukwebgl_WebGL2RenderingContext(duk_context *ctx) {
    duk_push_object(ctx);

    if (!duk_is_constructor_call(ctx)) {
        return DUK_RET_TYPE_ERROR;
    }

    return 0;
}

void dukwebgl_bind(duk_context *ctx) {
    duk_push_c_function(ctx, dukwebgl_WebGL2RenderingContext, 0);
    duk_push_object(ctx);
    dukwebgl_bind_constants(ctx);

#ifdef GL_VERSION_0_0
#endif /* GL_VERSION_0_0 */

#ifdef GL_VERSION_1_0
        dukwebgl_bind_function(ctx, custom_impl_getContextAttributes, getContextAttributes, 0);
        dukwebgl_bind_function(ctx, custom_impl_isContextLost, isContextLost, 0);
        dukwebgl_bind_function(ctx, custom_impl_getSupportedExtensions, getSupportedExtensions, 0);
        dukwebgl_bind_function(ctx, custom_impl_getExtension, getExtension, 1);
        dukwebgl_bind_function(ctx, glReadBuffer, readBuffer, 1);
        dukwebgl_bind_function(ctx, glBlendFunc, blendFunc, 2);
        dukwebgl_bind_function(ctx, glClear, clear, 1);
        dukwebgl_bind_function(ctx, glClearColor, clearColor, 4);
        dukwebgl_bind_function(ctx, glClearDepth, clearDepth, 1);
        dukwebgl_bind_function(ctx, glClearStencil, clearStencil, 1);
        dukwebgl_bind_function(ctx, glColorMask, colorMask, 4);
        dukwebgl_bind_function(ctx, glCullFace, cullFace, 1);
        dukwebgl_bind_function(ctx, glDepthFunc, depthFunc, 1);
        dukwebgl_bind_function(ctx, glDepthMask, depthMask, 1);
        dukwebgl_bind_function(ctx, glDepthRange, depthRange, 2);
        dukwebgl_bind_function(ctx, glDisable, disable, 1);
        dukwebgl_bind_function(ctx, glEnable, enable, 1);
        dukwebgl_bind_function(ctx, glFinish, finish, 0);
        dukwebgl_bind_function(ctx, glFlush, flush, 0);
        dukwebgl_bind_function(ctx, glFrontFace, frontFace, 1);
        dukwebgl_bind_function(ctx, glGetError, getError, 0);
        dukwebgl_bind_function(ctx, glHint, hint, 2);
        dukwebgl_bind_function(ctx, glIsEnabled, isEnabled, 1);
        dukwebgl_bind_function(ctx, glLineWidth, lineWidth, 1);
        dukwebgl_bind_function(ctx, glPixelStorei, pixelStorei, 2);
        dukwebgl_bind_function(ctx, glScissor, scissor, 4);
        dukwebgl_bind_function(ctx, glStencilFunc, stencilFunc, 3);
        dukwebgl_bind_function(ctx, glStencilMask, stencilMask, 1);
        dukwebgl_bind_function(ctx, glStencilOp, stencilOp, 3);
        dukwebgl_bind_function(ctx, glTexParameterf, texParameterf, 3);
        dukwebgl_bind_function(ctx, glTexParameteri, texParameteri, 3);
        dukwebgl_bind_function(ctx, glViewport, viewport, 4);
#endif /* GL_VERSION_1_0 */

#ifdef GL_VERSION_1_1
        dukwebgl_bind_function(ctx, glBindTexture, bindTexture, 2);
        dukwebgl_bind_function(ctx, glCopyTexImage2D, copyTexImage2D, 8);
        dukwebgl_bind_function(ctx, glCopyTexSubImage2D, copyTexSubImage2D, 8);
        dukwebgl_bind_function(ctx, glDrawArrays, drawArrays, 3);
        dukwebgl_bind_function(ctx, glDrawElements, drawElements, 4);
        dukwebgl_bind_function(ctx, glIsTexture, isTexture, 1);
        dukwebgl_bind_function(ctx, glPolygonOffset, polygonOffset, 2);
#endif /* GL_VERSION_1_1 */

#ifdef GL_VERSION_1_2
        dukwebgl_bind_function(ctx, glTexSubImage3D, texSubImage3D, 11);
        dukwebgl_bind_function(ctx, glCopyTexSubImage3D, copyTexSubImage3D, 9);
        dukwebgl_bind_function(ctx, glDrawRangeElements, drawRangeElements, 6);
#endif /* GL_VERSION_1_2 */

#ifdef GL_VERSION_1_3
        dukwebgl_bind_function(ctx, glCompressedTexImage3D, compressedTexImage3D, 9);
        dukwebgl_bind_function(ctx, glCompressedTexSubImage3D, compressedTexSubImage3D, 11);
        dukwebgl_bind_function(ctx, glCompressedTexImage2D, compressedTexImage2D, 8);
        dukwebgl_bind_function(ctx, glCompressedTexSubImage2D, compressedTexSubImage2D, 9);
        dukwebgl_bind_function(ctx, glActiveTexture, activeTexture, 1);
        dukwebgl_bind_function(ctx, glSampleCoverage, sampleCoverage, 2);
#endif /* GL_VERSION_1_3 */

#ifdef GL_VERSION_1_4
        dukwebgl_bind_function(ctx, glBlendColor, blendColor, 4);
        dukwebgl_bind_function(ctx, glBlendEquation, blendEquation, 1);
        dukwebgl_bind_function(ctx, glBlendFuncSeparate, blendFuncSeparate, 4);
#endif /* GL_VERSION_1_4 */

#ifdef GL_VERSION_1_5
        dukwebgl_bind_function(ctx, glIsQuery, isQuery, 1);
        dukwebgl_bind_function(ctx, glBeginQuery, beginQuery, 2);
        dukwebgl_bind_function(ctx, glEndQuery, endQuery, 1);
        dukwebgl_bind_function(ctx, glBindBuffer, bindBuffer, 2);
        dukwebgl_bind_function(ctx, glIsBuffer, isBuffer, 1);
#endif /* GL_VERSION_1_5 */

#ifdef GL_VERSION_2_0
        dukwebgl_bind_function(ctx, custom_impl_getProgramParameter, getProgramParameter, 2);
        dukwebgl_bind_function(ctx, custom_impl_getProgramInfoLog, getProgramInfoLog, 1);
        dukwebgl_bind_function(ctx, custom_impl_shaderSource, shaderSource, 2);
        dukwebgl_bind_function(ctx, custom_impl_getShaderSource, getShaderSource, 1);
        dukwebgl_bind_function(ctx, custom_impl_getShaderParameter, getShaderParameter, 2);
        dukwebgl_bind_function(ctx, custom_impl_getShaderInfoLog, getShaderInfoLog, 1);
        dukwebgl_bind_function(ctx, custom_impl_createBuffer, createBuffer, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteBuffer, deleteBuffer, 1);
        dukwebgl_bind_function(ctx, custom_impl_createTexture, createTexture, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteTexture, deleteTexture, 1);
        dukwebgl_bind_function(ctx, custom_impl_createQuery, createQuery, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteQuery, deleteQuery, 1);
        dukwebgl_bind_function(ctx, custom_impl_bufferData, bufferData, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_bufferSubData, bufferSubData, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_drawBuffers, drawBuffers, 1);
        dukwebgl_bind_function(ctx, custom_impl_texImage2D, texImage2D, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_readPixels, readPixels, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_texSubImage2D, texSubImage2D, 9);
        dukwebgl_bind_function(ctx, custom_impl_texImage3D, texImage3D, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix2fv, uniformMatrix2fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix3fv, uniformMatrix3fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix4fv, uniformMatrix4fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix2x3fv, uniformMatrix2x3fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix2x4fv, uniformMatrix2x4fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix3x2fv, uniformMatrix3x2fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix3x4fv, uniformMatrix3x4fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix4x2fv, uniformMatrix4x2fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniformMatrix4x3fv, uniformMatrix4x3fv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_uniform1fv, uniform1fv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform2fv, uniform2fv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform3fv, uniform3fv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform4fv, uniform4fv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform1iv, uniform1iv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform2iv, uniform2iv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform3iv, uniform3iv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform4iv, uniform4iv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform1uiv, uniform1uiv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform2uiv, uniform2uiv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform3uiv, uniform3uiv, 2);
        dukwebgl_bind_function(ctx, custom_impl_uniform4uiv, uniform4uiv, 2);
        dukwebgl_bind_function(ctx, custom_impl_getActiveUniform, getActiveUniform, 2);
        dukwebgl_bind_function(ctx, custom_impl_getActiveAttrib, getActiveAttrib, 2);
        dukwebgl_bind_function(ctx, custom_impl_getParameter, getParameter, 1);
        dukwebgl_bind_function(ctx, glAttachShader, attachShader, 2);
        dukwebgl_bind_function(ctx, glBindAttribLocation, bindAttribLocation, 3);
        dukwebgl_bind_function(ctx, glBlendEquationSeparate, blendEquationSeparate, 2);
        dukwebgl_bind_function(ctx, glCompileShader, compileShader, 1);
        dukwebgl_bind_function(ctx, glCreateProgram, createProgram, 0);
        dukwebgl_bind_function(ctx, glCreateShader, createShader, 1);
        dukwebgl_bind_function(ctx, glDeleteProgram, deleteProgram, 1);
        dukwebgl_bind_function(ctx, glDeleteShader, deleteShader, 1);
        dukwebgl_bind_function(ctx, glDetachShader, detachShader, 2);
        dukwebgl_bind_function(ctx, glDisableVertexAttribArray, disableVertexAttribArray, 1);
        dukwebgl_bind_function(ctx, glEnableVertexAttribArray, enableVertexAttribArray, 1);
        dukwebgl_bind_function(ctx, glGetAttribLocation, getAttribLocation, 2);
        dukwebgl_bind_function(ctx, glGetUniformLocation, getUniformLocation, 2);
        dukwebgl_bind_function(ctx, glIsProgram, isProgram, 1);
        dukwebgl_bind_function(ctx, glIsShader, isShader, 1);
        dukwebgl_bind_function(ctx, glLinkProgram, linkProgram, 1);
        dukwebgl_bind_function(ctx, glStencilFuncSeparate, stencilFuncSeparate, 4);
        dukwebgl_bind_function(ctx, glStencilMaskSeparate, stencilMaskSeparate, 2);
        dukwebgl_bind_function(ctx, glStencilOpSeparate, stencilOpSeparate, 4);
        dukwebgl_bind_function(ctx, glUniform1f, uniform1f, 2);
        dukwebgl_bind_function(ctx, glUniform1i, uniform1i, 2);
        dukwebgl_bind_function(ctx, glUniform2f, uniform2f, 3);
        dukwebgl_bind_function(ctx, glUniform2i, uniform2i, 3);
        dukwebgl_bind_function(ctx, glUniform3f, uniform3f, 4);
        dukwebgl_bind_function(ctx, glUniform3i, uniform3i, 4);
        dukwebgl_bind_function(ctx, glUniform4f, uniform4f, 5);
        dukwebgl_bind_function(ctx, glUniform4i, uniform4i, 5);
        dukwebgl_bind_function(ctx, glUseProgram, useProgram, 1);
        dukwebgl_bind_function(ctx, glValidateProgram, validateProgram, 1);
        dukwebgl_bind_function(ctx, glVertexAttrib1f, vertexAttrib1f, 2);
        dukwebgl_bind_function(ctx, glVertexAttrib1fv, vertexAttrib1fv, 2);
        dukwebgl_bind_function(ctx, glVertexAttrib2f, vertexAttrib2f, 3);
        dukwebgl_bind_function(ctx, glVertexAttrib2fv, vertexAttrib2fv, 2);
        dukwebgl_bind_function(ctx, glVertexAttrib3f, vertexAttrib3f, 4);
        dukwebgl_bind_function(ctx, glVertexAttrib3fv, vertexAttrib3fv, 2);
        dukwebgl_bind_function(ctx, glVertexAttrib4f, vertexAttrib4f, 5);
        dukwebgl_bind_function(ctx, glVertexAttrib4fv, vertexAttrib4fv, 2);
        dukwebgl_bind_function(ctx, glVertexAttribPointer, vertexAttribPointer, 6);
#endif /* GL_VERSION_2_0 */

#ifdef GL_VERSION_2_1
#endif /* GL_VERSION_2_1 */

#ifdef GL_VERSION_3_0
        dukwebgl_bind_function(ctx, custom_impl_createVertexArray, createVertexArray, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteVertexArray, deleteVertexArray, 1);
        dukwebgl_bind_function(ctx, custom_impl_createFramebuffer, createFramebuffer, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteFramebuffer, deleteFramebuffer, 1);
        dukwebgl_bind_function(ctx, custom_impl_createRenderbuffer, createRenderbuffer, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteRenderbuffer, deleteRenderbuffer, 1);
        dukwebgl_bind_function(ctx, custom_impl_clearBufferfv, clearBufferfv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_clearBufferiv, clearBufferiv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, custom_impl_clearBufferuiv, clearBufferuiv, DUK_VARARGS);
        dukwebgl_bind_function(ctx, glBlitFramebuffer, blitFramebuffer, 10);
        dukwebgl_bind_function(ctx, glFramebufferTextureLayer, framebufferTextureLayer, 5);
        dukwebgl_bind_function(ctx, glRenderbufferStorageMultisample, renderbufferStorageMultisample, 5);
        dukwebgl_bind_function(ctx, glGetFragDataLocation, getFragDataLocation, 2);
        dukwebgl_bind_function(ctx, glUniform1ui, uniform1ui, 2);
        dukwebgl_bind_function(ctx, glUniform2ui, uniform2ui, 3);
        dukwebgl_bind_function(ctx, glUniform3ui, uniform3ui, 4);
        dukwebgl_bind_function(ctx, glUniform4ui, uniform4ui, 5);
        dukwebgl_bind_function(ctx, glVertexAttribI4i, vertexAttribI4i, 5);
        dukwebgl_bind_function(ctx, glVertexAttribI4iv, vertexAttribI4iv, 2);
        dukwebgl_bind_function(ctx, glVertexAttribI4ui, vertexAttribI4ui, 5);
        dukwebgl_bind_function(ctx, glVertexAttribI4uiv, vertexAttribI4uiv, 2);
        dukwebgl_bind_function(ctx, glVertexAttribIPointer, vertexAttribIPointer, 5);
        dukwebgl_bind_function(ctx, glClearBufferfi, clearBufferfi, 4);
        dukwebgl_bind_function(ctx, glBeginTransformFeedback, beginTransformFeedback, 1);
        dukwebgl_bind_function(ctx, glEndTransformFeedback, endTransformFeedback, 0);
        dukwebgl_bind_function(ctx, glBindBufferBase, bindBufferBase, 3);
        dukwebgl_bind_function(ctx, glBindBufferRange, bindBufferRange, 5);
        dukwebgl_bind_function(ctx, glIsVertexArray, isVertexArray, 1);
        dukwebgl_bind_function(ctx, glBindVertexArray, bindVertexArray, 1);
        dukwebgl_bind_function(ctx, glBindFramebuffer, bindFramebuffer, 2);
        dukwebgl_bind_function(ctx, glBindRenderbuffer, bindRenderbuffer, 2);
        dukwebgl_bind_function(ctx, glCheckFramebufferStatus, checkFramebufferStatus, 1);
        dukwebgl_bind_function(ctx, glFramebufferRenderbuffer, framebufferRenderbuffer, 4);
        dukwebgl_bind_function(ctx, glFramebufferTexture2D, framebufferTexture2D, 5);
        dukwebgl_bind_function(ctx, glGenerateMipmap, generateMipmap, 1);
        dukwebgl_bind_function(ctx, glIsFramebuffer, isFramebuffer, 1);
        dukwebgl_bind_function(ctx, glIsRenderbuffer, isRenderbuffer, 1);
        dukwebgl_bind_function(ctx, glRenderbufferStorage, renderbufferStorage, 4);
#endif /* GL_VERSION_3_0 */

#ifdef GL_VERSION_3_1
        dukwebgl_bind_function(ctx, custom_impl_getActiveUniformBlockName, getActiveUniformBlockName, 2);
        dukwebgl_bind_function(ctx, glCopyBufferSubData, copyBufferSubData, 5);
        dukwebgl_bind_function(ctx, glDrawArraysInstanced, drawArraysInstanced, 4);
        dukwebgl_bind_function(ctx, glDrawElementsInstanced, drawElementsInstanced, 5);
        dukwebgl_bind_function(ctx, glGetUniformBlockIndex, getUniformBlockIndex, 2);
        dukwebgl_bind_function(ctx, glUniformBlockBinding, uniformBlockBinding, 3);
#endif /* GL_VERSION_3_1 */

#ifdef GL_VERSION_3_2
        dukwebgl_bind_function(ctx, custom_impl_createSampler, createSampler, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteSampler, deleteSampler, 1);
        dukwebgl_bind_function(ctx, glFenceSync, fenceSync, 2);
        dukwebgl_bind_function(ctx, glIsSync, isSync, 1);
        dukwebgl_bind_function(ctx, glDeleteSync, deleteSync, 1);
        dukwebgl_bind_function(ctx, glClientWaitSync, clientWaitSync, 3);
        dukwebgl_bind_function(ctx, glWaitSync, waitSync, 3);
#endif /* GL_VERSION_3_2 */

#ifdef GL_VERSION_3_3
        dukwebgl_bind_function(ctx, glVertexAttribDivisor, vertexAttribDivisor, 2);
        dukwebgl_bind_function(ctx, glIsSampler, isSampler, 1);
        dukwebgl_bind_function(ctx, glBindSampler, bindSampler, 2);
        dukwebgl_bind_function(ctx, glSamplerParameteri, samplerParameteri, 3);
        dukwebgl_bind_function(ctx, glSamplerParameterf, samplerParameterf, 3);
#endif /* GL_VERSION_3_3 */

#ifdef GL_VERSION_4_0
        dukwebgl_bind_function(ctx, custom_impl_createTransformFeedback, createTransformFeedback, 0);
        dukwebgl_bind_function(ctx, custom_impl_deleteTransformFeedback, deleteTransformFeedback, 1);
        dukwebgl_bind_function(ctx, glIsTransformFeedback, isTransformFeedback, 1);
        dukwebgl_bind_function(ctx, glBindTransformFeedback, bindTransformFeedback, 2);
        dukwebgl_bind_function(ctx, glPauseTransformFeedback, pauseTransformFeedback, 0);
        dukwebgl_bind_function(ctx, glResumeTransformFeedback, resumeTransformFeedback, 0);
#endif /* GL_VERSION_4_0 */

#ifdef GL_VERSION_4_1
#endif /* GL_VERSION_4_1 */

#ifdef GL_VERSION_4_2
        dukwebgl_bind_function(ctx, glTexStorage2D, texStorage2D, 5);
        dukwebgl_bind_function(ctx, glTexStorage3D, texStorage3D, 6);
#endif /* GL_VERSION_4_2 */

#ifdef GL_VERSION_4_3
#endif /* GL_VERSION_4_3 */

#ifdef GL_VERSION_4_4
#endif /* GL_VERSION_4_4 */

#ifdef GL_VERSION_4_5
#endif /* GL_VERSION_4_5 */

#ifdef GL_VERSION_4_6
#endif /* GL_VERSION_4_6 */

    /* Function binding coverage = 140/176 = 79.54545454545455 % */

    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, "WebGL2RenderingContext");
} /* dukwebgl_bind */

#endif /* DUKWEBGL_IMPLEMENTATION */


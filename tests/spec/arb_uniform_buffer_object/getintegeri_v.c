/*
 * Copyright © 2012 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/** @file getintegeri_v.c
 *
 * From the GL_ARB_uniform_buffer_object spec:
 *
 *     "To query the starting offset or size of the range of each
 *      buffer object binding used for uniform buffers, call
 *      GetIntegeri_v with <param> set to UNIFORM_BUFFER_START or
 *      UNIFORM_BUFFER_SIZE respectively. <index> must be in the range
 *      zero to the value of MAX_UNIFORM_BUFFER_BINDINGS - 1. If the
 *      parameter (starting offset or size) was not specified when the
 *      buffer object was bound, zero is returned. If no buffer object
 *      is bound to <index>, -1 is returned."
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(
    10 /*window_width*/,
    10 /*window_height*/,
    GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA)

static bool pass = true;

static void
test_index(int line, GLenum e, int index, int expected)
{
	GLint val;

	glGetIntegeri_v(e, index, &val);
	if (val != expected) {
		printf("%s:%d: %s[%d] was %d, expected %d\n",
		       __FILE__, line, piglit_get_gl_enum_name(e), index,
		       val, expected);
		pass = false;
	}
}

void
test_range(int line, int index, int bo, int offset, int size)
{
	test_index(line, GL_UNIFORM_BUFFER_BINDING, index, bo);
	test_index(line, GL_UNIFORM_BUFFER_START, index, offset);
	test_index(line, GL_UNIFORM_BUFFER_SIZE, index, size);
}

void
piglit_init(int argc, char **argv)
{
	GLuint bo[2];
	int size = 1024;
	GLint max_bindings;
	GLint junk;

	piglit_require_extension("GL_ARB_uniform_buffer_object");

	test_range(__LINE__, 0, 0, -1, -1);

	glGenBuffers(2, bo);
	glBindBuffer(GL_UNIFORM_BUFFER, bo[0]);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_READ);
	glBindBuffer(GL_UNIFORM_BUFFER, bo[1]);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_READ);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, bo[0], 0, 1);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, bo[1], 2, 3);
	test_range(__LINE__, 0, bo[0], 0, 1);
	test_range(__LINE__, 1, bo[1], 2, 3);

	/* There's a bit of a contradiction in the spec.  On the one
	 * hand, "BindBufferBase is equivalent to calling
	 * BindBufferRange with offset zero and size equal to the size
	 * of buffer", but on the other hand, " If the parameter
	 * (starting offset or size) was not specified when the buffer
	 * object was bound, zero is returned".  This is clarified by
	 * the GL 4.2 specification, which says that "BindBufferBase
	 * binds the entire buffer, even when the size of the buffer
	 * is changed after the binding is established.", so the zero
	 * return for the size makes sense since it's effectively
	 * computed at render time.
	 */
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, bo[1]);
	test_range(__LINE__, 1, bo[1], 0, 0);

	/* Is binding a BO of 0 valid?  It's not clear to me from the
	 * spec ("The error INVALID_OPERATION is generated by
	 * BindBufferRange and BindBufferBase if <buffer> is not the
	 * name of a valid buffer object.", and glIsBuffer returns
	 * false for 0), but it seems obviously parallel to the rest
	 * of the GL API, including glBindBuffer(), to allow it
	 */
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
	test_range(__LINE__, 0, 0, -1, -1);

	/* Test the error condition. */
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_bindings);
	glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, max_bindings, &junk);
	if (!piglit_check_gl_error(GL_INVALID_VALUE))
		pass = false;

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

enum piglit_result piglit_display(void)
{
	/* UNREACHED */
	return PIGLIT_FAIL;
}


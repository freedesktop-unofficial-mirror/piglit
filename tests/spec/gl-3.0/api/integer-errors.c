/* Copyright 2012 VMware, Inc.
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
 * \file integer-errors.c
 * Do error checking for functions taking signed/unsigned integer
 * (non-normalized) formats, such as glTex[Sub]Image, glDrawPixels and
 * glReadPixels.
 */


#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_MAIN(100 /*window_width*/,
                    100 /*window_height*/,
                    GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE);

static bool
test_api_errors(void)
{
	/* clear any prev errors */
	while (glGetError())
		;

	/* use a new tex obj */
	glBindTexture(GL_TEXTURE_2D, 42);

	/* Check that glDrawPixels of integer data is illegal */
	{
		static const GLfloat pixel[4] = {0, 0, 0, 0};

		glDrawPixels(1, 1, GL_RGBA_INTEGER_EXT, GL_INT, pixel);
		if (!piglit_check_gl_error(GL_INVALID_OPERATION))
			return false;
	}

	/* Check glTexImage for invalid internalFormat/format/type combos */
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI_EXT, 1, 1, 0,
			     GL_RGBA_INTEGER, GL_FLOAT, NULL);
		if (!piglit_check_gl_error(GL_INVALID_ENUM))
			return false;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0,
			     GL_RGBA_INTEGER, GL_SHORT, NULL);
		if (!piglit_check_gl_error(GL_INVALID_OPERATION))
			return false;
	}

	/* Check glTexSubImage for invalid format/type combination */
	{
		/* make valid texture image here */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI_EXT, 8, 8, 0,
			     GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
		if (!piglit_check_gl_error(GL_NO_ERROR))
			return false;

		glTexSubImage2D(GL_TEXTURE_2D, 0,
				0, 0, 4, 4,
				GL_RGBA_INTEGER, GL_FLOAT, NULL);
		if (!piglit_check_gl_error(GL_INVALID_ENUM))
			return false;
	}

	/* Check for GL_INVALID_OPERATION when trying to copy framebuffer pixels
	 * to an integer texture when the framebuffer is not an integer format.
	 */
	{
		/* make valid texture image here */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI_EXT, 4, 4, 0,
			     GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, NULL);
		if (!piglit_check_gl_error(GL_NO_ERROR))
			return false;

		glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
				    0, 0, 0, 0, 4, 4);
		if (!piglit_check_gl_error(GL_INVALID_OPERATION))
			return false;
	}

	/* Is GL_INVALID_ENUM generated by glReadPixels? */
	{
		GLfloat buf[64];
		glReadPixels(0, 0, 4, 4, GL_RGBA_INTEGER, GL_FLOAT, buf);
		if (!piglit_check_gl_error(GL_INVALID_ENUM))
			return false;
	}

	/* Is GL_INVALID_OPERATION generated by glReadPixels? */
	{
		GLuint buf[64];
		glReadPixels(0, 0, 4, 4, GL_RGBA_INTEGER, GL_UNSIGNED_INT, buf);
		if (!piglit_check_gl_error(GL_INVALID_OPERATION))
			return false;
	}

	return true;
}


/**
 * Test specific combinations of (internalFormat, format, type) with
 * glTexImage to see that they're accepted (no GL errors).
 */
static bool
test_teximage_format_combos(void)
{
	/* These format combinations should all work */
	struct {
		GLenum intFormat, srcFormat, srcType;
	} formats[] = {
		{ GL_RGBA8UI_EXT, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE },
		{ GL_RGBA8UI_EXT, GL_RGBA_INTEGER, GL_SHORT },
		{ GL_RGBA8UI_EXT, GL_RGBA_INTEGER, GL_UNSIGNED_INT_8_8_8_8 },
		{ GL_RGBA8UI_EXT, GL_BGRA_INTEGER, GL_UNSIGNED_INT_8_8_8_8 },
		{ GL_LUMINANCE8I_EXT, GL_RGBA_INTEGER, GL_UNSIGNED_INT_8_8_8_8 },
		{ GL_RGB16I_EXT, GL_RGB_INTEGER, GL_UNSIGNED_SHORT_5_6_5 },
		{ GL_RGB32I_EXT, GL_RGB_INTEGER, GL_UNSIGNED_SHORT_5_6_5 }
	};
	int i;
	GLenum err;
	bool pass = GL_TRUE;

	while (glGetError() != GL_NO_ERROR)
		;

	for (i = 0; i < ARRAY_SIZE(formats); i++) {
		glTexImage2D(GL_TEXTURE_2D, 0, formats[i].intFormat,
			     16, 16, 0,
			     formats[i].srcFormat, formats[i].srcType, NULL);
		err = glGetError();
		if (err != GL_NO_ERROR) {
			fprintf(stderr,
				"integer-errors failure: glTexImage2D(0x%x, 0x%x, 0x%x)"
				" generated error 0x%x (case %d)\n",
				formats[i].intFormat,
				formats[i].srcFormat,
				formats[i].srcType, err, i);
			pass = false;
		}
	}

	return pass;
}



enum piglit_result
piglit_display(void)
{
	return PIGLIT_PASS;
}


void
piglit_init(int argc, char **argv)
{
	bool pass;

	piglit_require_gl_version(30);

	pass = test_api_errors();

	pass = pass && test_teximage_format_combos();

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

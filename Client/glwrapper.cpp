/* glwrapper.c                            -*- mode: C; tab-width: 4 -*-
 *
 * GLTrace OpenGL debugger/trace utlility
 * Version:  2.3
 * Copyright (C) 1999-2002
 *		Phil Frisbie, Jr. (phil@hawksoft.com)
 *
 * With original function counting code contributed by
 *		Keith Harrison (keithh@netcomuk.co.uk)
 * GLX wrapper code contributed by
 *		Bert Freudenberg (bert@freudenbergs.de)
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "gltrace.h"
#include "input.h"
#include <iostream>

using namespace std;

extern unsigned int nCallCount[];
extern GLboolean gltraceInit();
extern int GetEXTProcIndex(LPCSTR name);
extern int fontBitmapString(char *str);
extern GLboolean bFSPenable;        /* enable the FSP counter */
extern GLboolean bLogEnable;        /* enable normal logging */
extern GLboolean bDisableExt;       /* disable extensions */
extern GLfloat *fpsColor;           /* color of the FPS counter */

GLboolean bOptionKeydown = GL_FALSE;		/* is the key down for logging optional functions */
GLboolean bOneframeKeydown = GL_FALSE;		/* is the key down for one-shot (frame) logging of functions */
GLboolean bOnOffKeydown = GL_TRUE;		    /* is the key down for logging optional functions */
GLboolean bLogCalls;		                /* TRUE if any logging is being done */

#define PACKET_LENGTH 1400

extern GLboolean stream = GL_FALSE;
extern NLbyte		*netBuffer;
extern NLbyte		*bigBuffer;
extern int			*readyToSend;
extern int			count;
extern int			packetCount;
int texCount = 0;

extern glv GLV;

#define PRINT_COMMA print(",");\

#define DISPLAYLOCAL
#define LOGTOFILE
//#define SENDTEX

#ifdef LOGTOFILE

#define START(X)	\
    if(bCountCalls == GL_TRUE) nCallCount[FP_##X]++;\
	if(1)\
	{\
		if(1)\
		{\
			if (strcmp(GLN[FP_##X], "wglSwapBuffers") == 0) {\
				AddIntToStream(354);\
				print("---swap sent---\n");\
				SendStream();\
			} else if(show_status[FP_##X] == SUPPRESS) {\
				print("%s(",GLN[FP_##X]);\
				AddIntToStream(FP_##X);\

#define END	\
				print(");\n");\
			} if(count > PACKET_LENGTH - 110) SendStream();\
		}\
	}\

#else

#define START(X) \
	{\
		{\
			if (strcmp(GLN[FP_##X], "wglSwapBuffers") == 0) {\
				AddIntToStream(354);\
				SendStream();\
			} else if(show_status[FP_##X] == SUPPRESS) {\
				AddIntToStream(FP_##X);\

#define END	\
			} if(count > PACKET_LENGTH - 100) SendStream();\
		}\
	}\

#endif

#define RESULT \

#define ENDRESULT \
			} if(count > PACKET_LENGTH - 110) SendStream();\
		}\
	}\

static void drawMessage(int x, int y, char *string, ...)
{
	char	    buffer[256];
	va_list	    args;
    GLfloat     color[4], position[4];
    GLboolean   texturing;

	va_start(args, string);
	vsprintf(buffer, string, args);
	va_end(args);

    /* save OpenGL state */
    GLV.glPushAttrib(GL_ALL_ATTRIB_BITS);
    GLV.glGetFloatv(GL_CURRENT_COLOR, color);
    GLV.glGetBooleanv(GL_TEXTURE_2D, &texturing);
    GLV.glGetFloatv(GL_CURRENT_RASTER_POSITION, position);

    /* set up the state we need */
	GLV.glDisable(GL_TEXTURE_2D);
	GLV.glColor3fv(fpsColor);
    GLV.glRasterPos2i(x, y);

	fontBitmapString(buffer);

    /* restore OpenGL state */
    GLV.glPopAttrib();
    GLV.glColor4fv(color);
    if(texturing == GL_TRUE)
    {
        GLV.glEnable(GL_TEXTURE_2D);
    }
    GLV.glRasterPos4fv(position);
}

static void renderFPS(void)
{
    static int      frames = 0;
    static time_t   last;
    static GLboolean first = GL_TRUE;
    static float    fps = 0.0f;

    frames++;
    if(first == GL_TRUE)
    {
        time(&last);
        first = GL_FALSE;
    }
    else
    {
        time_t  now;
        int     diff;

        time(&now);
        /* update no more than once per second */
        diff = (int)(now - last);
        if(diff > 0)
        {
            fps = (float)frames / diff;
            frames = 0;
            last = now;
        }
    }
#ifdef _WIN32
    if(getKeyState(GLT_FPS) == GL_TRUE)
#else
	/* XXX: we do not have keyboard support yet - always draw */
#endif
    {
        drawMessage(5, 40, "FPS: %4.2f", fps);
    }
}

static void updateKeys(void)
{
    if(bOneframeKeydown == GL_TRUE)
    {
        print("\n******************************\nEnd one frame logging\n******************************\n\n");
    }
    updateKeyState();
	bOptionKeydown = getKeyState(GLT_OPTION);
    bOneframeKeydown = getKeyState(GLT_ONEFRAME);
    bOnOffKeydown = (GLboolean)(getKeyState(GLT_ON_OFF) && bLogEnable);
    if(bOptionKeydown == GL_TRUE || bOneframeKeydown == GL_TRUE || bOnOffKeydown == GL_TRUE)
    {
        bLogCalls = GL_TRUE;
    }
    if(bOneframeKeydown == GL_TRUE)
    {
        print("\n\n******************************\nBegin one frame logging\n******************************\n");
    }
}

#include "ext.h"

//
// OpenGL function handlers
//

void GLAPIENTRY glAccum (GLenum op, GLfloat value)
{
	START(glAccum);
	print_value(_GLenum, &op);
	PRINT_COMMA
	print_value(_GLfloat, &value);
	END;
#ifdef DISPLAYLOCAL
	GLV.glAccum (op,value);
#endif
}

void GLAPIENTRY glAlphaFunc (GLenum func, GLclampf ref)
{
	START(glAlphaFunc);
	print_value(_GLenum, &func);
	PRINT_COMMA
	print_value(_GLclampf, &ref);
	END;
#ifdef DISPLAYLOCAL
	GLV.glAlphaFunc (func, ref);
#endif
}

GLboolean GLAPIENTRY glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences)
{
	GLboolean   result;
    int         i;

	result = GLV.glAreTexturesResident(n,textures,residences);
    START(glAreTexturesResident);
	print_value(_GLsizei, &n);
	PRINT_COMMA
    print("[");
    for(i=0;i<n;i++)
    {
        if(i != 0) PRINT_COMMA
        print("%d is %s", textures[i], (residences[i] == GL_TRUE) ? "TRUE" : "FALSE");
    }
    print("]");
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

void GLAPIENTRY glArrayElement (GLint i)
{
	START(glArrayElement);
	print_value(_GLint, &i);
	END;
#ifdef DISPLAYLOCAL
	GLV.glArrayElement (i);
#endif
}

void GLAPIENTRY glBegin (GLenum mode)
{
	START(glBegin);
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glBegin (mode);
#endif
}

void GLAPIENTRY glBindTexture (GLenum target, GLuint texture)
{
	START(glBindTexture);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLuint, &texture);
	END;
#ifdef DISPLAYLOCAL
	GLV.glBindTexture (target, texture);
#endif
}

void GLAPIENTRY glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
	//int i;
	//if(PACKET_LENGTH < count + width * height) SendStream();
	START(glBitmap);
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	PRINT_COMMA
	print_value(_GLfloat, &xorig);
	PRINT_COMMA
	print_value(_GLfloat, &yorig);
	PRINT_COMMA
	print_value(_GLfloat, &xmove);
	PRINT_COMMA
	print_value(_GLfloat, &ymove);
	PRINT_COMMA
	//print_value(HEX, &bitmap);
	//StreamBytes(bitmap, width*height);
	//for(i = 0; i < width * height; i++) {
	//	AddByteToStream((((NLbyte*)bitmap)[i]));
	//}
	END;
#ifdef DISPLAYLOCAL
	GLV.glBitmap (width, height, xorig, yorig, xmove, ymove, bitmap);
#endif
}

void GLAPIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
{
	useZeroOne = GL_TRUE;
	START(glBlendFunc);
	print_value(_GLenum, &sfactor);
	PRINT_COMMA
	print_value(_GLenum, &dfactor);
	END;
#ifdef DISPLAYLOCAL
	GLV.glBlendFunc (sfactor, dfactor);
#endif
	useZeroOne = GL_FALSE;
}

void GLAPIENTRY glCallList (GLuint list)
{
	START(glCallList);
	print_value(_GLuint, &list);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCallList (list);
#endif
}

void GLAPIENTRY glCallLists (GLsizei n, GLenum type, const GLvoid *lists)
{
	START(glCallLists);
	print_value(_GLsizei, &n);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLvoid, &lists);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCallLists (n, type, lists);
#endif
}

void GLAPIENTRY glClear (GLbitfield mask)
{
	START(glClear);
	print_value(_GLbitfield, &mask);
	END;
#ifdef DISPLAYLOCAL
	GLV.glClear (mask);
#endif
}

void GLAPIENTRY glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	START(glClearAccum);
	print_value(_GLfloat, &red);
	PRINT_COMMA
	print_value(_GLfloat, &green);
	PRINT_COMMA
	print_value(_GLfloat, &blue);
	PRINT_COMMA
	print_value(_GLfloat, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glClearAccum (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	START(glClearColor);
	print_value(_GLclampf, &red);
	PRINT_COMMA
	print_value(_GLclampf, &green);
	PRINT_COMMA
	print_value(_GLclampf, &blue);
	PRINT_COMMA
	print_value(_GLclampf, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glClearColor (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glClearDepth (GLclampd depth)
{
	START(glClearDepth);
	print_value(_GLclampd, &depth);
	END;
#ifdef DISPLAYLOCAL
	GLV.glClearDepth (depth);
#endif
}

void GLAPIENTRY glClearIndex (GLfloat c)
{
	START(glClearIndex);
	print_value(_GLfloat, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glClearIndex (c);
#endif
}

void GLAPIENTRY glClearStencil (GLint s)
{
	START(glClearStencil);
	print_value(_GLint, &s);
	END;
#ifdef DISPLAYLOCAL
	GLV.glClearStencil (s);
#endif
}

void GLAPIENTRY glClipPlane (GLenum plane, const GLdouble *equation)
{
	START(glClipPlane);
	print_value(_GLenum, &plane);
	PRINT_COMMA
	print_value(_GLdouble4, &equation);
	END;
#ifdef DISPLAYLOCAL
	GLV.glClipPlane (plane, equation);
#endif
}

void GLAPIENTRY glColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
	START(glColor3b);
	print_value(_GLbyte, &red);
	PRINT_COMMA
	print_value(_GLbyte, &green);
	PRINT_COMMA
	print_value(_GLbyte, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3b (red, green, blue);
#endif
}

void GLAPIENTRY glColor3bv (const GLbyte *v)
{
	START(glColor3bv);
	print_value(_GLbyte3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3bv (v);
#endif
}

void GLAPIENTRY glColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
	START(glColor3d);
	print_value(_GLdouble, &red);
	PRINT_COMMA
	print_value(_GLdouble, &green);
	PRINT_COMMA
	print_value(_GLdouble, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3d (red, green, blue);
#endif
}

void GLAPIENTRY glColor3dv (const GLdouble *v)
{
	START(glColor3dv);
	print_value(_GLdouble3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3dv (v);
#endif
}

void GLAPIENTRY glColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
	START(glColor3f);
	print_value(_GLfloat, &red);
	PRINT_COMMA
	print_value(_GLfloat, &green);
	PRINT_COMMA
	print_value(_GLfloat, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3f (red, green, blue);
#endif
}

void GLAPIENTRY glColor3fv (const GLfloat *v)
{
	START(glColor3fv);
	print_value(_GLfloat3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3fv (v);
#endif
}

void GLAPIENTRY glColor3i (GLint red, GLint green, GLint blue)
{
	START(glColor3i);
	print_value(_GLint, &red);
	PRINT_COMMA
	print_value(_GLint, &green);
	PRINT_COMMA
	print_value(_GLint, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3i (red, green, blue);
#endif
}

void GLAPIENTRY glColor3iv (const GLint *v)
{
	START(glColor3iv);
	print_value(_GLint3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3iv (v);
#endif
}

void GLAPIENTRY glColor3s (GLshort red, GLshort green, GLshort blue)
{
	START(glColor3s);
	print_value(_GLshort, &red);
	PRINT_COMMA
	print_value(_GLshort, &green);
	PRINT_COMMA
	print_value(_GLshort, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3s (red, green, blue);
#endif
}

void GLAPIENTRY glColor3sv (const GLshort *v)
{
	START(glColor3sv);
	print_value(_GLshort3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3sv (v);
#endif
}

void GLAPIENTRY glColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
	START(glColor3ub);
	print_value(_GLubyte, &red);
	PRINT_COMMA
	print_value(_GLubyte, &green);
	PRINT_COMMA
	print_value(_GLubyte, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3ub (red, green, blue);
#endif
}

void GLAPIENTRY glColor3ubv (const GLubyte *v)
{
	START(glColor3ubv);
	print_value(_GLubyte3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3ubv (v);
#endif
}

void GLAPIENTRY glColor3ui (GLuint red, GLuint green, GLuint blue)
{
	START(glColor3ui);
	print_value(_GLuint, &red);
	PRINT_COMMA
	print_value(_GLuint, &green);
	PRINT_COMMA
	print_value(_GLuint, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3ui (red, green, blue);
#endif
}

void GLAPIENTRY glColor3uiv (const GLuint *v)
{
	START(glColor3uiv);
	print_value(_GLuint3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3uiv (v);
#endif
}

void GLAPIENTRY glColor3us (GLushort red, GLushort green, GLushort blue)
{
	START(glColor3us);
	print_value(_GLushort, &red);
	PRINT_COMMA
	print_value(_GLushort, &green);
	PRINT_COMMA
	print_value(_GLushort, &blue);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3us (red, green, blue);
#endif
}

void GLAPIENTRY glColor3usv (const GLushort *v)
{
	START(glColor3usv);
	print_value(_GLushort3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor3usv (v);
#endif
}

void GLAPIENTRY glColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
	START(glColor4b);
	print_value(_GLbyte, &red);
	PRINT_COMMA
	print_value(_GLbyte, &green);
	PRINT_COMMA
	print_value(_GLbyte, &blue);
	PRINT_COMMA
	print_value(_GLbyte, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4b (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4bv (const GLbyte *v)
{
	START(glColor4bv);
	print_value(_GLbyte4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4bv (v);
#endif
}

void GLAPIENTRY glColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
	START(glColor4d);
	print_value(_GLdouble, &red);
	PRINT_COMMA
	print_value(_GLdouble, &green);
	PRINT_COMMA
	print_value(_GLdouble, &blue);
	PRINT_COMMA
	print_value(_GLdouble, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4d (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4dv (const GLdouble *v)
{
	START(glColor4dv);
	print_value(_GLdouble4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4dv (v);
#endif
}

void GLAPIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	START(glColor4f);
	print_value(_GLfloat, &red);
	PRINT_COMMA
	print_value(_GLfloat, &green);
	PRINT_COMMA
	print_value(_GLfloat, &blue);
	PRINT_COMMA
	print_value(_GLfloat, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4f (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4fv (const GLfloat *v)
{
	START(glColor4fv);
	print_value(_GLfloat4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4fv (v);
#endif
}

void GLAPIENTRY glColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{
	START(glColor4i);
	print_value(_GLint, &red);
	PRINT_COMMA
	print_value(_GLint, &green);
	PRINT_COMMA
	print_value(_GLint, &blue);
	PRINT_COMMA
	print_value(_GLint, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4i (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4iv (const GLint *v)
{                  
	START(glColor4iv);
	print_value(_GLint4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4iv (v);
#endif
}

void GLAPIENTRY glColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
	START(glColor4s);
	print_value(_GLshort, &red);
	PRINT_COMMA
	print_value(_GLshort, &green);
	PRINT_COMMA
	print_value(_GLshort, &blue);
	PRINT_COMMA
	print_value(_GLshort, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4s (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4sv (const GLshort *v)
{
	START(glColor4sv);
	print_value(_GLshort4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4sv (v);
#endif
}

void GLAPIENTRY glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	START(glColor4ub);
	print_value(_GLubyte, &red);
	PRINT_COMMA
	print_value(_GLubyte, &green);
	PRINT_COMMA
	print_value(_GLubyte, &blue);
	PRINT_COMMA
	print_value(_GLubyte, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4ub (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4ubv (const GLubyte *v)
{
	START(glColor4ubv);
	print_value(_GLubyte4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4ubv (v);
#endif
}

void GLAPIENTRY glColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
	START(glColor4ui);
	print_value(_GLuint, &red);
	PRINT_COMMA
	print_value(_GLuint, &green);
	PRINT_COMMA
	print_value(_GLuint, &blue);
	PRINT_COMMA
	print_value(_GLuint, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4ui (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4uiv (const GLuint *v)
{
	START(glColor4uiv);
	print_value(_GLuint4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4uiv (v);
#endif
}

void GLAPIENTRY glColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
	START(glColor4us);
	print_value(_GLushort, &red);
	PRINT_COMMA
	print_value(_GLushort, &green);
	PRINT_COMMA
	print_value(_GLushort, &blue);
	PRINT_COMMA
	print_value(_GLushort, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4us (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColor4usv (const GLushort *v)
{
	START(glColor4usv);
	print_value(_GLshort4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColor4usv (v);
#endif
}

void GLAPIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	START(glColorMask);
	print_value(_GLboolean, &red);
	PRINT_COMMA
	print_value(_GLboolean, &green);
	PRINT_COMMA
	print_value(_GLboolean, &blue);
	PRINT_COMMA
	print_value(_GLboolean, &alpha);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColorMask (red, green, blue, alpha);
#endif
}

void GLAPIENTRY glColorMaterial (GLenum face, GLenum mode)
{
	START(glColorMaterial);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColorMaterial (face, mode);
#endif
}

void GLAPIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	START(glColorPointer);
	print_value(_GLint, &size);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLsizei, &stride);
	PRINT_COMMA
	print_value(_GLvoid, &pointer);
	END;
#ifdef DISPLAYLOCAL
	GLV.glColorPointer (size, type, stride, pointer);
#endif
}

void GLAPIENTRY glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
	START(glCopyPixels);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	PRINT_COMMA
	print_value(_GLenum, &type);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCopyPixels (x, y, width, height, type);
#endif
}

void GLAPIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
{
	START(glCopyTexImage1D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLenum, &internalFormat);
	PRINT_COMMA
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLint, &border);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCopyTexImage1D (target, level, internalFormat, x, y, width, border);
#endif
}

void GLAPIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	START(glCopyTexImage2D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLenum, &internalFormat);
	PRINT_COMMA
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	PRINT_COMMA
	print_value(_GLint, &border);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCopyTexImage2D (target, level, internalFormat, x, y, width, height, border);
#endif
}

void GLAPIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
	START(glCopyTexSubImage1D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLint, &xoffset);
	PRINT_COMMA
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCopyTexSubImage1D (target, level, xoffset, x, y, width);
#endif
}

void GLAPIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	START(glCopyTexSubImage2D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLint, &xoffset);
	PRINT_COMMA
	print_value(_GLint, &yoffset);
	PRINT_COMMA
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCopyTexSubImage2D (target, level, xoffset, yoffset, x, y, width, height);
#endif
}

void GLAPIENTRY glCullFace (GLenum mode)
{
	START(glCullFace);
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glCullFace (mode);
#endif
}

void GLAPIENTRY glDeleteLists (GLuint list, GLsizei range)
{
	START(glDeleteLists);
	print_value(_GLuint, &list);
	PRINT_COMMA
	print_value(_GLsizei, &range);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDeleteLists (list, range);
#endif
}

void GLAPIENTRY glDeleteTextures (GLsizei n, const GLuint *textures)
{
	START(glDeleteTextures);
	print_value(_GLsizei, &n);
	PRINT_COMMA
	print_value(_GLvoid, &textures);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDeleteTextures (n, textures);
#endif
}

void GLAPIENTRY glDepthFunc (GLenum func)
{
	START(glDepthFunc);
	print_value(_GLenum, &func);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDepthFunc (func);
#endif
}

void GLAPIENTRY glDepthMask (GLboolean flag)
{
	START(glDepthMask);
	print_value(_GLboolean, &flag);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDepthMask (flag);
#endif
}

void GLAPIENTRY glDepthRange (GLclampd zNear, GLclampd zFar)
{
	START(glDepthRange);
	print_value(_GLclampd, &zNear);
	PRINT_COMMA
	print_value(_GLclampd, &zFar);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDepthRange (zNear, zFar);
#endif
}

void GLAPIENTRY glDisable (GLenum cap)
{
	START(glDisable);
	print_value(_GLenum, &cap);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDisable (cap);
#endif
}

void GLAPIENTRY glDisableClientState (GLenum array)
{
	START(glDisableClientState);
	print_value(_GLenum, &array);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDisableClientState (array);
#endif
}

void GLAPIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
	START(glDrawArrays);
	print_value(_GLenum, &mode);
	PRINT_COMMA
	print_value(_GLint, &first);
	PRINT_COMMA
	print_value(_GLsizei, &count);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDrawArrays (mode, first, count);
#endif
}

void GLAPIENTRY glDrawBuffer (GLenum mode)
{
	START(glDrawBuffer);
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDrawBuffer (mode);
#endif
}

void GLAPIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	START(glDrawElements);
	print_value(_GLenum, &mode);
	PRINT_COMMA
	print_value(_GLsizei, &count);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	//print_value(_GLvoid, &indices);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDrawElements (mode, count, type, indices);
#endif
}

void GLAPIENTRY glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	START(glDrawPixels);
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	PRINT_COMMA
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLvoid, &pixels);
	END;
#ifdef DISPLAYLOCAL
	GLV.glDrawPixels (width, height, format, type, pixels);
#endif
}

void GLAPIENTRY glEdgeFlag (GLboolean flag)
{
	START(glEdgeFlag);
	print_value(_GLboolean, &flag);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEdgeFlag (flag);
#endif
}

void GLAPIENTRY glEdgeFlagPointer (GLsizei stride, const GLvoid *pointer)
{
	START(glEdgeFlagPointer);
	print_value(_GLsizei, &stride);
	PRINT_COMMA
	print_value(_GLvoid, &pointer);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEdgeFlagPointer (stride, pointer);
#endif
}

void GLAPIENTRY glEdgeFlagv (const GLboolean *flag)
{
	START(glEdgeFlagv);
	print_value(_GLboolean, &flag);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEdgeFlagv (flag);
#endif
}

void GLAPIENTRY glEnable (GLenum cap)
{
	START(glEnable);
	print_value(_GLenum, &cap);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEnable (cap);
#endif
}

void GLAPIENTRY glEnableClientState (GLenum array)
{
	START(glEnableClientState);
	print_value(_GLenum, &array);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEnableClientState (array);
#endif
}

void GLAPIENTRY glEnd ()
{
	START(glEnd);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEnd();;
#endif
}

void GLAPIENTRY glEndList ()
{
	START(glEndList);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEndList();;
#endif
}

void GLAPIENTRY glEvalCoord1d (GLdouble u)
{
	START(glEvalCoord1d);
	print_value(_GLdouble, &u);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord1d (u);
#endif
}

void GLAPIENTRY glEvalCoord1dv (const GLdouble *u)
{
	START(glEvalCoord1dv);
	print_value(_GLdouble1, &u);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord1dv (u);
#endif
}

void GLAPIENTRY glEvalCoord1f (GLfloat u)
{
	START(glEvalCoord1f);
	print_value(_GLfloat, &u);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord1f (u);
#endif
}

void GLAPIENTRY glEvalCoord1fv (const GLfloat *u)
{
	START(glEvalCoord1fv);
	print_value(_GLfloat1, &u);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord1fv (u);
#endif
}

void GLAPIENTRY glEvalCoord2d (GLdouble u, GLdouble v)
{
	START(glEvalCoord2d);
	print_value(_GLdouble, &u);
	PRINT_COMMA
	print_value(_GLdouble, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord2d (u,v);
#endif
}

void GLAPIENTRY glEvalCoord2dv (const GLdouble *u)
{
	START(glEvalCoord2dv);
	print_value(_GLdouble2, &u);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord2dv (u);
#endif
}

void GLAPIENTRY glEvalCoord2f (GLfloat u, GLfloat v)
{
	START(glEvalCoord2f);
	print_value(_GLfloat, &u);
	PRINT_COMMA
	print_value(_GLfloat, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord2f (u,v);
#endif
}

void GLAPIENTRY glEvalCoord2fv (const GLfloat *u)
{
	START(glEvalCoord2fv);
	print_value(_GLfloat2, &u);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalCoord2fv (u);
#endif
}

void GLAPIENTRY glEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{
	START(glEvalMesh1);
	print_value(_GLenum, &mode);
	PRINT_COMMA
	print_value(_GLint, &i1);
	PRINT_COMMA
	print_value(_GLint, &i2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalMesh1 (mode, i1, i2);
#endif
}

void GLAPIENTRY glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
	START(glEvalMesh2);
	print_value(_GLenum, &mode);
	PRINT_COMMA
	print_value(_GLint, &i1);
	PRINT_COMMA
	print_value(_GLint, &i2);
	PRINT_COMMA
	print_value(_GLint, &j1);
	PRINT_COMMA
	print_value(_GLint, &j2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalMesh2 (mode, i1, i2, j1, j2);
#endif
}

void GLAPIENTRY glEvalPoint1 (GLint i)
{
	START(glEvalPoint1);
	print_value(_GLint, &i);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalPoint1 (i);
#endif
}

void GLAPIENTRY glEvalPoint2 (GLint i, GLint j)
{
	START(glEvalPoint2);
	print_value(_GLint, &i);
	PRINT_COMMA
	print_value(_GLint, &j);
	END;
#ifdef DISPLAYLOCAL
	GLV.glEvalPoint2 (i, j);
#endif
}

void GLAPIENTRY glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer)
{
	START(glFeedbackBuffer);
	print_value(_GLsizei, &size);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLvoid, &buffer);
	END;
#ifdef DISPLAYLOCAL
	GLV.glFeedbackBuffer (size, type, buffer);
#endif
}

void GLAPIENTRY glFinish ()
{
    if(bFSPenable == GL_TRUE)
    {
        renderFPS();
    }
	START(glFinish);
	END;
#ifdef DISPLAYLOCAL
	GLV.glFinish();
    updateKeys();
#endif
}

void GLAPIENTRY glFlush ()
{
    if(bFSPenable == GL_TRUE)
    {
        renderFPS();
    }
	START(glFlush);
	END;
#ifdef DISPLAYLOCAL
	GLV.glFlush();
    updateKeys();
#endif
}

void GLAPIENTRY glFogf (GLenum pname, GLfloat param)
{
	START(glFogf);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glFogf (pname, param);
#endif
}

void GLAPIENTRY glFogfv (GLenum pname, const GLfloat *params)
{
	START(glFogfv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if(pname == GL_FOG_COLOR)
	{
		print_value(_GLfloat4, &params);
	}
	else
	{
		print_value(_GLfloat1, &params);
	}
	END;
#ifdef DISPLAYLOCAL
	GLV.glFogfv (pname, params);
#endif
}

void GLAPIENTRY glFogi (GLenum pname, GLint param)
{
	START(glFogi);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLint, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glFogi (pname, param);
#endif
}

void GLAPIENTRY glFogiv (GLenum pname, const GLint *params)
{
	START(glFogiv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if(pname == GL_FOG_COLOR)
	{
		print_value(_GLint4, &params);
	}
	else
	{
		print_value(_GLint1, &params);
	}
	END;
#ifdef DISPLAYLOCAL
	GLV.glFogiv (pname, params);
#endif
}

void GLAPIENTRY glFrontFace (GLenum mode)
{
	START(glFrontFace);
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glFrontFace (mode);
#endif
}

void GLAPIENTRY glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	START(glFrustum);
	print_value(_GLdouble, &left);
	PRINT_COMMA
	print_value(_GLdouble, &right);
	PRINT_COMMA
	print_value(_GLdouble, &bottom);
	PRINT_COMMA
	print_value(_GLdouble, &top);
	PRINT_COMMA
	print_value(_GLdouble, &zNear);
	PRINT_COMMA
	print_value(_GLdouble, &zFar);
	END;
#ifdef DISPLAYLOCAL
	GLV.glFrustum (left, right, bottom, top, zNear, zFar);
#endif
}

GLuint GLAPIENTRY glGenLists (GLsizei range)
{
	GLuint result;

    result = GLV.glGenLists(range);
	START(glGenLists);
	print_value(_GLsizei, &range);
	RESULT;
	print_value(_GLuint, &result);
	ENDRESULT;
	return result;
}

void GLAPIENTRY glGenTextures (GLsizei n, GLuint *textures)
{
	START(glGenTextures);
	print_value(_GLsizei, &n);
	PRINT_COMMA
	//print_value(_GLvoid, &textures);
	END;
#ifdef DISPLAYLOCAL
	GLV.glGenTextures (n, textures);
#endif
}

void GLAPIENTRY glGetBooleanv (GLenum pname, GLboolean *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetBooleanv (pname, params);
	START(glGetBooleanv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if(pname == GL_COLOR_WRITEMASK)
	{
		print_value(_GLboolean4, &params);
	}
	else
	{
		print_value(_GLboolean1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetClipPlane (GLenum plane, GLdouble *equation)
{
#ifdef DISPLAYLOCAL
	GLV.glGetClipPlane (plane, equation);
	START(glGetClipPlane);
	print_value(_GLenum, &plane);
	PRINT_COMMA
	print_value(_GLdouble4, &equation);
	END;
#endif
}

void GLAPIENTRY glGetDoublev (GLenum pname, GLdouble *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetDoublev (pname, params);
	START(glGetDoublev);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if((pname == GL_MODELVIEW_MATRIX)||(pname == GL_PROJECTION_MATRIX)||
		(pname == GL_TEXTURE_MATRIX))
	{
		print_value(_GLdouble16, &params);
	}
	else if((pname == GL_ACCUM_CLEAR_VALUE)||(pname == GL_COLOR_CLEAR_VALUE)||
		(pname == GL_CURRENT_COLOR)||(pname == GL_CURRENT_RASTER_COLOR)||
		(pname == GL_CURRENT_RASTER_POSITION)||(pname == GL_CURRENT_RASTER_TEXTURE_COORDS)||
		(pname == GL_CURRENT_TEXTURE_COORDS)||(pname == GL_FOG_COLOR)||
		(pname == GL_LIGHT_MODEL_AMBIENT)||(pname == GL_MAP2_GRID_DOMAIN)||
		(pname == GL_SCISSOR_BOX)||(pname == GL_VIEWPORT))
	{
		print_value(_GLdouble4, &params);
	}
	else if((pname == GL_DEPTH_RANGE)||(pname == GL_LINE_WIDTH_RANGE)||
		(pname == GL_MAP1_GRID_DOMAIN)||(pname == GL_MAP2_GRID_SEGMENTS)||
		(pname == GL_MAX_VIEWPORT_DIMS)||(pname == GL_POINT_SIZE_RANGE)||
		(pname == GL_POLYGON_MODE))
	{
		print_value(_GLdouble2, &params);
	}
	else
	{
		print_value(_GLdouble1, &params);
	}
	END;
#endif
}

GLenum GLAPIENTRY glGetError ()
{
	GLenum result;

    result = GLV.glGetError();
	START(glGetError);
    RESULT;
	print_value(_GLenum, &result);
    ENDRESULT;
	return result;
}

void GLAPIENTRY glGetFloatv (GLenum pname, GLfloat *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetFloatv (pname, params);
	START(glGetFloatv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if((pname == GL_MODELVIEW_MATRIX)||(pname == GL_PROJECTION_MATRIX)||
		(pname == GL_TEXTURE_MATRIX))
	{
		print_value(_GLfloat16, &params);
	}
	else if((pname == GL_ACCUM_CLEAR_VALUE)||(pname == GL_COLOR_CLEAR_VALUE)||
		(pname == GL_CURRENT_COLOR)||(pname == GL_CURRENT_RASTER_COLOR)||
		(pname == GL_CURRENT_RASTER_POSITION)||(pname == GL_CURRENT_RASTER_TEXTURE_COORDS)||
		(pname == GL_CURRENT_TEXTURE_COORDS)||(pname == GL_FOG_COLOR)||
		(pname == GL_LIGHT_MODEL_AMBIENT)||(pname == GL_MAP2_GRID_DOMAIN)||
		(pname == GL_SCISSOR_BOX)||(pname == GL_VIEWPORT))
	{
		print_value(_GLfloat4, &params);
	}
	else if((pname == GL_DEPTH_RANGE)||(pname == GL_LINE_WIDTH_RANGE)||
		(pname == GL_MAP1_GRID_DOMAIN)||(pname == GL_MAP2_GRID_SEGMENTS)||
		(pname == GL_MAX_VIEWPORT_DIMS)||(pname == GL_POINT_SIZE_RANGE)||
		(pname == GL_POLYGON_MODE))
	{
		print_value(_GLfloat2, &params);
	}
	else
	{
		print_value(_GLfloat1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetIntegerv (GLenum pname, GLint *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetIntegerv (pname, params);
	START(glGetIntegerv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if((pname == GL_ACCUM_CLEAR_VALUE)||(pname == GL_COLOR_CLEAR_VALUE)||
		(pname == GL_CURRENT_COLOR)||(pname == GL_CURRENT_RASTER_COLOR)||
		(pname == GL_CURRENT_RASTER_POSITION)||(pname == GL_CURRENT_RASTER_TEXTURE_COORDS)||
		(pname == GL_CURRENT_TEXTURE_COORDS)||(pname == GL_FOG_COLOR)||
		(pname == GL_LIGHT_MODEL_AMBIENT)||(pname == GL_MAP2_GRID_DOMAIN)||
		(pname == GL_SCISSOR_BOX)||(pname == GL_VIEWPORT))
	{
		print_value(_GLint4, &params);
	}
	else if((pname == GL_DEPTH_RANGE)||(pname == GL_LINE_WIDTH_RANGE)||
		(pname == GL_MAP1_GRID_DOMAIN)||(pname == GL_MAP2_GRID_SEGMENTS)||
		(pname == GL_MAX_VIEWPORT_DIMS)||(pname == GL_POINT_SIZE_RANGE)||
		(pname == GL_POLYGON_MODE))
	{
		print_value(_GLint2, &params);
	}
	else
	{
		print_value(_GLint1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetLightfv (light, pname, params);
	START(glGetLightfv);
	print_value(_GLenum, &light);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if((pname == GL_AMBIENT)||(pname == GL_DIFFUSE)||
		(pname == GL_SPECULAR)||(pname == GL_POSITION))
	{
		print_value(_GLfloat4, &params);
	}
	else if(pname == GL_SPOT_DIRECTION)
	{
		print_value(_GLfloat3, &params);
	}
	else
	{
		print_value(_GLfloat1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetLightiv (GLenum light, GLenum pname, GLint *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetLightiv (light, pname, params);
	START(glGetLightiv);
	print_value(_GLenum, &light);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if((pname == GL_AMBIENT)||(pname == GL_DIFFUSE)||
		(pname == GL_SPECULAR)||(pname == GL_POSITION))
	{
		print_value(_GLint4, &params);
	}
	else if(pname == GL_SPOT_DIRECTION)
	{
		print_value(_GLint3, &params);
	}
	else
	{
		print_value(_GLint1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetMapdv (GLenum target, GLenum query, GLdouble *v)
{
#ifdef DISPLAYLOCAL
	GLV.glGetMapdv (target, query, v);
	START(glGetMapdv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &query);
	PRINT_COMMA
    switch (query) {

    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
        print_value(_GLdouble1, &v);
        break;

    case GL_MAP1_TEXTURE_COORD_2:
        print_value(_GLdouble2, &v);
        break;

    case GL_MAP1_VERTEX_3:
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
        print_value(_GLdouble3, &v);
        break;

    case GL_MAP1_VERTEX_4:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
        print_value(_GLdouble4, &v);
        break;
    }
	END;
#endif
}

void GLAPIENTRY glGetMapfv (GLenum target, GLenum query, GLfloat *v)
{
#ifdef DISPLAYLOCAL
	GLV.glGetMapfv (target, query, v);
	START(glGetMapfv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &query);
	PRINT_COMMA
    switch (query) {

    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
        print_value(_GLfloat1, &v);
        break;

    case GL_MAP1_TEXTURE_COORD_2:
    case GL_MAP2_TEXTURE_COORD_2:
        print_value(_GLfloat2, &v);
        break;

    case GL_MAP1_VERTEX_3:
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
    case GL_MAP2_VERTEX_3:
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
        print_value(_GLfloat3, &v);
        break;

    case GL_MAP1_VERTEX_4:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
    case GL_MAP2_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
        print_value(_GLfloat4, &v);
        break;
    }
	END;
#endif
}

void GLAPIENTRY glGetMapiv (GLenum target, GLenum query, GLint *v)
{
#ifdef DISPLAYLOCAL
	GLV.glGetMapiv (target, query, v);
	START(glGetMapiv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &query);
	PRINT_COMMA
    switch (query) {

    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
        print_value(_GLint1, &v);
        break;

    case GL_MAP1_TEXTURE_COORD_2:
    case GL_MAP2_TEXTURE_COORD_2:
        print_value(_GLint2, &v);
        break;

    case GL_MAP1_VERTEX_3:
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
    case GL_MAP2_VERTEX_3:
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
        print_value(_GLint3, &v);
        break;

    case GL_MAP1_VERTEX_4:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
    case GL_MAP2_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
        print_value(_GLint4, &v);
        break;
    }
	END;
#endif
}

void GLAPIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetMaterialfv (face, pname, params);
	START(glGetMaterialfv);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	if(pname == GL_SHININESS)
	{
		print_value(_GLfloat1, &params);
	}
	else if(pname == GL_COLOR_INDEXES)
	{
		print_value(_GLfloat3, &params);
	}
	else
	{
		print_value(_GLfloat4, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetMaterialiv (GLenum face, GLenum pname, GLint *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetMaterialiv (face, pname, params);
	START(glGetMaterialiv);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	if(pname == GL_SHININESS)
	{
		print_value(_GLint1, &params);
	}
	else if(pname == GL_COLOR_INDEXES)
	{
		print_value(_GLint3, &params);
	}
	else
	{
		print_value(_GLint4, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetPixelMapfv (GLenum map, GLfloat *values)
{
#ifdef DISPLAYLOCAL
	GLV.glGetPixelMapfv (map, values);
	START(glGetPixelMapfv);
	print_value(_GLenum, &map);
	PRINT_COMMA
	print_value(_GLvoid, &values);
	END;
#endif
}

void GLAPIENTRY glGetPixelMapuiv (GLenum map, GLuint *values)
{
#ifdef DISPLAYLOCAL
	GLV.glGetPixelMapuiv (map, values);
	START(glGetPixelMapuiv);
	print_value(_GLenum, &map);
	PRINT_COMMA
	print_value(_GLvoid, &values);
	END;
#endif
}

void GLAPIENTRY glGetPixelMapusv (GLenum map, GLushort *values)
{
#ifdef DISPLAYLOCAL
	GLV.glGetPixelMapusv (map, values);
	START(glGetPixelMapusv);
	print_value(_GLenum, &map);
	PRINT_COMMA
	print_value(_GLvoid, &values);
	END;
#endif
}

void GLAPIENTRY glGetPointerv (GLenum pname, GLvoid* *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetPointerv (pname, params);
	START(glGetPointerv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLvoid, &params);
	END;
#endif
}

void GLAPIENTRY glGetPolygonStipple (GLubyte *mask)
{
#ifdef DISPLAYLOCAL
	GLV.glGetPolygonStipple (mask);
	START(glGetPolygonStipple);
	print_value(_GLvoid, &mask);
	END;
#endif
}

const GLubyte* GLAPIENTRY glGetString (GLenum name)
{
	const GLubyte* result;

	result = GLV.glGetString(name);
	START(glGetString);
	print_value(_GLenum, &name);
    RESULT;
	print_value(STR, &result);
    ENDRESULT;
	return result;
}

void GLAPIENTRY glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexEnvfv (target, pname, params);
	START(glGetTexEnvfv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &target);
	PRINT_COMMA
	if(pname == GL_TEXTURE_ENV_COLOR)
	{
		print_value(_GLfloat4, &params);
	}
	else
	{
		print_value(_GLfloat1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetTexEnviv (GLenum target, GLenum pname, GLint *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexEnviv (target, pname, params);
	START(glGetTexEnviv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &target);
	PRINT_COMMA
	if(pname == GL_TEXTURE_ENV_COLOR)
	{
		print_value(_GLint4, &params);
	}
	else
	{
		print_value(_GLint1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexGendv (coord, pname, params);
	START(glGetTexGendv);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	if(pname == GL_TEXTURE_GEN_MODE)
	{
		print_value(_GLdouble1, &params);
	}
	else
	{
		print_value(_GLdouble4, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexGenfv (coord, pname, params);
	START(glGetTexGenfv);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	if(pname == GL_TEXTURE_GEN_MODE)
	{
		print_value(_GLfloat1, &params);
	}
	else
	{
		print_value(_GLfloat4, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetTexGeniv (GLenum coord, GLenum pname, GLint *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexGeniv (coord, pname, params);
	START(glGetTexGeniv);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	if(pname == GL_TEXTURE_GEN_MODE)
	{
		print_value(_GLint1, &params);
	}
	else
	{
		print_value(_GLint4, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexImage (target, level, format, type, pixels);
	START(glGetTexImage);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLvoid, &pixels);
	END;
#endif
}

void GLAPIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexLevelParameterfv (target, level, pname, params);
	START(glGetTexLevelParameterfv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat1, &params);
	END;
#endif
}

void GLAPIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexLevelParameteriv (target, level, pname, params);
	START(glGetTexLevelParameteriv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLint1, &params);
	END;
#endif
}

void GLAPIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexParameterfv (target, pname, params);
	START(glGetTexParameterfv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if(pname == GL_TEXTURE_BORDER_COLOR)
	{
		print_value(_GLfloat4, &params);
	}
	else
	{
		print_value(_GLfloat1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
#ifdef DISPLAYLOCAL
	GLV.glGetTexParameteriv (target, pname, params);
	START(glGetTexParameteriv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if(pname == GL_TEXTURE_BORDER_COLOR)
	{
		print_value(_GLint4, &params);
	}
	else
	{
		print_value(_GLint1, &params);
	}
	END;
#endif
}

void GLAPIENTRY glHint (GLenum target, GLenum mode)
{
	START(glHint);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glHint (target, mode);
#endif
}

void GLAPIENTRY glIndexMask (GLuint mask)
{
	START(glIndexMask);
	print_value(_GLuint, &mask);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexMask (mask);
#endif
}

void GLAPIENTRY glIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
	START(glIndexPointer);
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLsizei, &stride);
	PRINT_COMMA
	memcpy((NLubyte *)&netBuffer[count], (NLubyte *)pointer, 24);
	count += 24;
	END;

#ifdef DISPLAYLOCAL
	GLV.glIndexPointer (type, stride, pointer);
#endif

}

void GLAPIENTRY glIndexd (GLdouble c)
{
	START(glIndexd);
	print_value(_GLdouble, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexd (c);
#endif
}

void GLAPIENTRY glIndexdv (const GLdouble *c)
{
	START(glIndexdv);
	print_value(_GLdouble1, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexdv (c);
#endif
}

void GLAPIENTRY glIndexf (GLfloat c)
{
	START(glIndexf);
	print_value(_GLfloat, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexf (c);
#endif
}

void GLAPIENTRY glIndexfv (const GLfloat *c)
{
	START(glIndexfv);
	print_value(_GLfloat1, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexfv (c);
#endif
}

void GLAPIENTRY glIndexi (GLint c)
{
	START(glIndexi);
	print_value(_GLint, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexi (c);
#endif
}

void GLAPIENTRY glIndexiv (const GLint *c)
{
	START(glIndexiv);
	print_value(_GLint1, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexiv (c);
#endif
}

void GLAPIENTRY glIndexs (GLshort c)
{
	START(glIndexs);
	print_value(_GLshort, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexs (c);
#endif
}

void GLAPIENTRY glIndexsv (const GLshort *c)
{
	START(glIndexsv);
	print_value(_GLshort1, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexsv (c);
#endif
}

void GLAPIENTRY glIndexub (GLubyte c)
{
	START(glIndexub);
	print_value(_GLubyte, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexub (c);
#endif
}

void GLAPIENTRY glIndexubv (const GLubyte *c)
{
	START(glIndexubv);
	print_value(_GLubyte1, &c);
	END;
#ifdef DISPLAYLOCAL
	GLV.glIndexubv (c);
#endif
}

void GLAPIENTRY glInitNames ()
{
	START(glInitNames);
	END;
#ifdef DISPLAYLOCAL
	GLV.glInitNames();
#endif
}

void GLAPIENTRY glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer)
{
	START(glInterleavedArrays);
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLsizei, &stride);
	PRINT_COMMA
	print_value(_GLvoid, &pointer);
	END;
#ifdef DISPLAYLOCAL
	GLV.glInterleavedArrays (format, stride, pointer);
#endif
}

GLboolean GLAPIENTRY glIsEnabled (GLenum cap)
{
	GLboolean result;

	result = GLV.glIsEnabled(cap);
	START(glIsEnabled);
	print_value(_GLenum, &cap);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

GLboolean GLAPIENTRY glIsList (GLuint list)
{
	GLboolean result;

	result = GLV.glIsList(list);
	START(glIsEnabled);
	print_value(_GLuint, &list);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

GLboolean GLAPIENTRY glIsTexture (GLuint texture)
{
	GLboolean result;

	result = GLV.glIsTexture(texture);
	START(glIsEnabled);
	print_value(_GLuint, &texture);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

void GLAPIENTRY glLightModelf (GLenum pname, GLfloat param)
{
	START(glLightModelf);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLightModelf (pname, param);
#endif
}

void GLAPIENTRY glLightModelfv (GLenum pname, const GLfloat *params)
{
	START(glLightModelfv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if(pname == GL_LIGHT_MODEL_AMBIENT)
	{
		print_value(_GLfloat4, &params);
	}
	else
	{
		print_value(_GLfloat1, &params);
	}
	END;
#ifdef DISPLAYLOCAL
	GLV.glLightModelfv (pname,params);
#endif
}

void GLAPIENTRY glLightModeli (GLenum pname, GLint param)
{
	START(glLightModeli);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLint, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLightModeli (pname, param);
#endif
}

void GLAPIENTRY glLightModeliv (GLenum pname, const GLint *params)
{
	START(glLightModeliv);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if(pname == GL_LIGHT_MODEL_AMBIENT)
	{
		print_value(_GLint4, &params);
	}
	else
	{
		print_value(_GLint1, &params);
	}
	END;
#ifdef DISPLAYLOCAL
	GLV.glLightModeliv (pname,params);
#endif
}

void GLAPIENTRY glLightf (GLenum light, GLenum pname, GLfloat param)
{
	START(glLightf);
	print_value(_GLenum, &light);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLightf (light, pname, param);
#endif
}

void GLAPIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
	START(glLightf);
	print_value(_GLenum, &light);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if((pname == GL_AMBIENT)||(pname == GL_DIFFUSE)||(pname == GL_SPECULAR)||
		(pname == GL_POSITION))
	{
		print_value(_GLfloat4, &params);
	}
	else if(pname == GL_SPOT_DIRECTION)
	{
		print_value(_GLfloat3, &params);
	}
	else
	{
		print_value(_GLfloat1, &params);
	}
	END;
#ifdef DISPLAYLOCAL
	GLV.glLightfv (light, pname, params);
#endif
}

void GLAPIENTRY glLighti (GLenum light, GLenum pname, GLint param)
{
	START(glLighti);
	print_value(_GLenum, &light);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLint, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLighti (light, pname, param);
#endif
}

void GLAPIENTRY glLightiv (GLenum light, GLenum pname, const GLint *params)
{
	START(glLighti);
	print_value(_GLenum, &light);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	if((pname == GL_AMBIENT)||(pname == GL_DIFFUSE)||(pname == GL_SPECULAR)||
		(pname == GL_POSITION))
	{
		print_value(_GLint4, &params);
	}
	else if(pname == GL_SPOT_DIRECTION)
	{
		print_value(_GLint3, &params);
	}
	else
	{
		print_value(_GLint1, &params);
	}
	END;
#ifdef DISPLAYLOCAL
	GLV.glLightiv (light, pname, params);
#endif
}

void GLAPIENTRY glLineStipple (GLint factor, GLushort pattern)
{
	START(glLineStipple);
	print_value(_GLint, &factor);
	PRINT_COMMA
	print_value(_GLushort, &pattern);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLineStipple (factor, pattern);
#endif
}

void GLAPIENTRY glLineWidth (GLfloat width)
{
	START(glLineWidth);
	print_value(_GLfloat, &width);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLineWidth (width);
#endif
}

void GLAPIENTRY glListBase (GLuint base)
{
	START(glListBase);
	print_value(_GLuint, &base);
	END;
#ifdef DISPLAYLOCAL
	GLV.glListBase (base);
#endif
}

void GLAPIENTRY glLoadIdentity ()
{
	START(glLoadIdentity);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLoadIdentity();
#endif
}

void GLAPIENTRY glLoadMatrixd (const GLdouble *m)
{
	START(glLoadMatrixd);
	print_value(_GLdouble16, &m);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLoadMatrixd(m);
#endif
}

void GLAPIENTRY glLoadMatrixf (const GLfloat *m)
{
	START(glLoadMatrixf);
	print_value(_GLfloat16, &m);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLoadMatrixf (m);
#endif
}

void GLAPIENTRY glLoadName (GLuint name)
{
	START(glLoadName);
	print_value(_GLuint, &name);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLoadName (name);
#endif
}

void GLAPIENTRY glLogicOp (GLenum opcode)
{
	START(glLogicOp);
	print_value(_GLenum, &opcode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glLogicOp (opcode);
#endif
}

void GLAPIENTRY glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
{
	START(glMap1d);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLdouble, &u1);
	PRINT_COMMA
	print_value(_GLdouble, &u2);
	PRINT_COMMA
	print_value(_GLint, &stride);
	PRINT_COMMA
	print_value(_GLint, &order);
	PRINT_COMMA
    switch (target) {

    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
        print_value(_GLdouble1, &points);
        break;

    case GL_MAP1_TEXTURE_COORD_2:
        print_value(_GLdouble2, &points);
        break;

    case GL_MAP1_VERTEX_3:
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
        print_value(_GLdouble3, &points);
        break;

    case GL_MAP1_VERTEX_4:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
        print_value(_GLdouble4, &points);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glMap1d (target, u1, u2, stride, order, points);
#endif
}

void GLAPIENTRY glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
{
	START(glMap1f);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLfloat, &u1);
	PRINT_COMMA
	print_value(_GLfloat, &u2);
	PRINT_COMMA
	print_value(_GLint, &stride);
	PRINT_COMMA
	print_value(_GLint, &order);
	PRINT_COMMA
    switch (target) {

    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
        print_value(_GLfloat1, &points);
        break;

    case GL_MAP1_TEXTURE_COORD_2:
        print_value(_GLfloat2, &points);
        break;

    case GL_MAP1_VERTEX_3:
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
        print_value(_GLfloat3, &points);
        break;

    case GL_MAP1_VERTEX_4:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
        print_value(_GLfloat4, &points);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glMap1f (target, u1, u2, stride, order, points);
#endif
}

void GLAPIENTRY glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
{
	START(glMap2d);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLdouble, &u1);
	PRINT_COMMA
	print_value(_GLdouble, &u2);
	PRINT_COMMA
	print_value(_GLint, &ustride);
	PRINT_COMMA
	print_value(_GLint, &uorder);
	PRINT_COMMA
	print_value(_GLdouble, &v1);
	PRINT_COMMA
	print_value(_GLdouble, &v2);
	PRINT_COMMA
	print_value(_GLint, &vstride);
	PRINT_COMMA
	print_value(_GLint, &vorder);
	PRINT_COMMA
    switch (target) {

    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
        print_value(_GLdouble1, &points);
        break;

    case GL_MAP2_TEXTURE_COORD_2:
        print_value(_GLdouble2, &points);
        break;

    case GL_MAP2_VERTEX_3:
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
        print_value(_GLdouble3, &points);
        break;

    case GL_MAP2_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
        print_value(_GLdouble4, &points);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glMap2d (target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
#endif
}

void GLAPIENTRY glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
{
	START(glMap2f);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLfloat, &u1);
	PRINT_COMMA
	print_value(_GLfloat, &u2);
	PRINT_COMMA
	print_value(_GLint, &ustride);
	PRINT_COMMA
	print_value(_GLint, &uorder);
	PRINT_COMMA
	print_value(_GLfloat, &v1);
	PRINT_COMMA
	print_value(_GLfloat, &v2);
	PRINT_COMMA
	print_value(_GLint, &vstride);
	PRINT_COMMA
	print_value(_GLint, &vorder);
	PRINT_COMMA
    switch (target) {

    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
        print_value(_GLfloat1, &points);
        break;

    case GL_MAP2_TEXTURE_COORD_2:
        print_value(_GLfloat2, &points);
        break;

    case GL_MAP2_VERTEX_3:
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
        print_value(_GLfloat3, &points);
        break;

    case GL_MAP2_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
        print_value(_GLfloat4, &points);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glMap2f (target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
#endif
}

void GLAPIENTRY glMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{
	START(glMapGrid1d);
	print_value(_GLint, &un);
	PRINT_COMMA
	print_value(_GLdouble, &u1);
	PRINT_COMMA
	print_value(_GLdouble, &u2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMapGrid1d (un, u1, u2);
#endif
}

void GLAPIENTRY glMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{
	START(glMapGrid1f);
	print_value(_GLint, &un);
	PRINT_COMMA
	print_value(_GLfloat, &u1);
	PRINT_COMMA
	print_value(_GLfloat, &u2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMapGrid1f (un, u1, u2);
#endif
}

void GLAPIENTRY glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
	START(glMapGrid1d);
	print_value(_GLint, &un);
	PRINT_COMMA
	print_value(_GLdouble, &u1);
	PRINT_COMMA
	print_value(_GLdouble, &u2);
	PRINT_COMMA
	print_value(_GLint, &vn);
	PRINT_COMMA
	print_value(_GLdouble, &v1);
	PRINT_COMMA
	print_value(_GLdouble, &v2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMapGrid2d (un, u1, u2, vn, v1, v2);
#endif
}

void GLAPIENTRY glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
	START(glMapGrid1f);
	print_value(_GLint, &un);
	PRINT_COMMA
	print_value(_GLfloat, &u1);
	PRINT_COMMA
	print_value(_GLfloat, &u2);
	PRINT_COMMA
	print_value(_GLint, &vn);
	PRINT_COMMA
	print_value(_GLfloat, &v1);
	PRINT_COMMA
	print_value(_GLfloat, &v2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMapGrid2f (un, u1, u2, vn, v1, v2);
#endif
}

void GLAPIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
	START(glMaterialf);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMaterialf (face, pname, param);
#endif
}

void GLAPIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
	START(glMaterialfv);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
    case GL_AMBIENT_AND_DIFFUSE:
	    print_value(_GLfloat4, &params);
        break;
    case GL_COLOR_INDEXES:
	    print_value(_GLfloat3, &params);
        break;
    case GL_SHININESS:
    default:
	    print_value(_GLfloat1, &params);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glMaterialfv (face, pname, params);
#endif
}

void GLAPIENTRY glMateriali (GLenum face, GLenum pname, GLint param)
{
	START(glMateriali);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLint, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMateriali (face, pname, param);
#endif
}

void GLAPIENTRY glMaterialiv (GLenum face, GLenum pname, const GLint *params)
{
	START(glMaterialiv);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
    case GL_AMBIENT_AND_DIFFUSE:
	    print_value(_GLint4, &params);
        break;
    case GL_COLOR_INDEXES:
	    print_value(_GLint3, &params);
        break;
    case GL_SHININESS:
    default:
	    print_value(_GLint1, &params);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glMaterialiv (face, pname, params);
#endif
}

void GLAPIENTRY glMatrixMode (GLenum mode)
{
	START(glMatrixMode);
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMatrixMode (mode);
#endif
}

void GLAPIENTRY glMultMatrixd (const GLdouble *m)
{
	START(glMultMatrixd);
	print_value(_GLdouble16, &m);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMultMatrixd (m);
#endif
}

void GLAPIENTRY glMultMatrixf (const GLfloat *m)
{
	START(glMultMatrixf);
	print_value(_GLfloat16, &m);
	END;
#ifdef DISPLAYLOCAL
	GLV.glMultMatrixf (m);
#endif
}

void GLAPIENTRY glNewList (GLuint list, GLenum mode)
{
	START(glNewList);
	print_value(_GLuint, &list);
	PRINT_COMMA
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNewList (list,mode);
#endif
}

void GLAPIENTRY glNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{
	START(glNormal3b);
	print_value(_GLbyte, &nx);
	PRINT_COMMA
	print_value(_GLbyte, &ny);
	PRINT_COMMA
	print_value(_GLbyte, &nz);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3b (nx, ny, nz);
#endif
}

void GLAPIENTRY glNormal3bv (const GLbyte *v)
{
	START(glNormal3bv);
	print_value(_GLbyte3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3bv (v);
#endif
}

void GLAPIENTRY glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{
	START(glNormal3d);
	print_value(_GLdouble, &nx);
	PRINT_COMMA
	print_value(_GLdouble, &ny);
	PRINT_COMMA
	print_value(_GLdouble, &nz);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3d (nx, ny, nz);
#endif
}

void GLAPIENTRY glNormal3dv (const GLdouble *v)
{
	START(glNormal3dv);
	print_value(_GLdouble3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3dv (v);
#endif
}

void GLAPIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
	START(glNormal3f);
	print_value(_GLfloat, &nx);
	PRINT_COMMA
	print_value(_GLfloat, &ny);
	PRINT_COMMA
	print_value(_GLfloat, &nz);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3f (nx, ny, nz);
#endif
}

void GLAPIENTRY glNormal3fv (const GLfloat *v)
{
	START(glNormal3fv);
	print_value(_GLfloat3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3fv (v);
#endif
}

void GLAPIENTRY glNormal3i (GLint nx, GLint ny, GLint nz)
{
	START(glNormal3i);
	print_value(_GLint, &nx);
	PRINT_COMMA
	print_value(_GLint, &ny);
	PRINT_COMMA
	print_value(_GLint, &nz);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3i (nx, ny, nz);
#endif
}

void GLAPIENTRY glNormal3iv (const GLint *v)
{
	START(glNormal3iv);
	print_value(_GLint3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3iv (v);
#endif
}

void GLAPIENTRY glNormal3s (GLshort nx, GLshort ny, GLshort nz)
{
	START(glNormal3s);
	print_value(_GLshort, &nx);
	PRINT_COMMA
	print_value(_GLshort, &ny);
	PRINT_COMMA
	print_value(_GLshort, &nz);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3s (nx, ny, nz);
#endif
}

void GLAPIENTRY glNormal3sv (const GLshort *v)
{
	START(glNormal3sv);
	print_value(_GLshort3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormal3sv (v);
#endif
}

void GLAPIENTRY glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
	writeShort(netBuffer, count, 401);
	writeShort(netBuffer, count, 96);
	writeShort(netBuffer, count, 72);
	writeShort(netBuffer, count, 24);

	START(glNormalPointer);
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLsizei, &stride);
	PRINT_COMMA
	memcpy((NLbyte *)&netBuffer[count], (NLbyte *)pointer, 72);
	count += 72;
	END;
#ifdef DISPLAYLOCAL
	GLV.glNormalPointer (type, stride, pointer);
#endif
}

void GLAPIENTRY glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	START(glOrtho);
	print_value(_GLdouble, &left);
	PRINT_COMMA
	print_value(_GLdouble, &right);
	PRINT_COMMA
	print_value(_GLdouble, &bottom);
	PRINT_COMMA
	print_value(_GLdouble, &top);
	PRINT_COMMA
	print_value(_GLdouble, &zNear);
	PRINT_COMMA
	print_value(_GLdouble, &zFar);
	PRINT_COMMA
	END;
#ifdef DISPLAYLOCAL
	GLV.glOrtho (left, right, bottom, top, zNear, zFar);
#endif
}

void GLAPIENTRY glPassThrough (GLfloat token)
{
	START(glPassThrough);
	print_value(_GLfloat, &token);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPassThrough (token);
#endif
}

void GLAPIENTRY glPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values)
{
    int i;

	START(glPixelMapfv);
	print_value(_GLenum, &map);
	PRINT_COMMA
	print_value(_GLsizei, &mapsize);
	PRINT_COMMA
    print("[");
    for(i=0;i<mapsize;i++)
    {
        if(i != 0) PRINT_COMMA
        print_value(_GLfloat, &values[i]);
    }
    print("]");
	END;
#ifdef DISPLAYLOCAL
    GLV.glPixelMapfv (map, mapsize, values);
#endif
}

void GLAPIENTRY glPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values)
{
    int i;

    START(glPixelMapuiv);
	print_value(_GLenum, &map);
	PRINT_COMMA
	print_value(_GLsizei, &mapsize);
	PRINT_COMMA
    print("[");
    for(i=0;i<mapsize;i++)
    {
        if(i != 0) PRINT_COMMA
        print_value(_GLuint, &values[i]);
    }
    print("]");
	END;
#ifdef DISPLAYLOCAL
	GLV.glPixelMapuiv (map, mapsize, values);
#endif
}

void GLAPIENTRY glPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values)
{
    int i;

	START(glPixelMapusv);
	print_value(_GLenum, &map);
	PRINT_COMMA
	print_value(_GLsizei, &mapsize);
	PRINT_COMMA
    print("[");
    for(i=0;i<mapsize;i++)
    {
        if(i != 0) PRINT_COMMA
        print_value(_GLushort, &values[i]);
    }
    print("]");
	END;
#ifdef DISPLAYLOCAL
	GLV.glPixelMapusv (map, mapsize, values);
#endif
}

void GLAPIENTRY glPixelStoref (GLenum pname, GLfloat param)
{
	START(glPixelStoref);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPixelStoref (pname, param);
#endif
}

void GLAPIENTRY glPixelStorei (GLenum pname, GLint param)
{
	START(glPixelStorei);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLint, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPixelStorei (pname, param);
#endif
}

void GLAPIENTRY glPixelTransferf (GLenum pname, GLfloat param)
{
	START(glPixelTransferf);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPixelTransferf (pname, param);
#endif
}

void GLAPIENTRY glPixelTransferi (GLenum pname, GLint param)
{
	START(glPixelTransferi);
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLint, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPixelTransferi (pname, param);
#endif
}

void GLAPIENTRY glPixelZoom (GLfloat xfactor, GLfloat yfactor)
{
	START(glPixelZoom);
	print_value(_GLfloat, &xfactor);
	PRINT_COMMA
	print_value(_GLfloat, &yfactor);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPixelZoom (xfactor, yfactor);
#endif
}

void GLAPIENTRY glPointSize (GLfloat size)
{
	START(glPointSize);
	print_value(_GLfloat, &size);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPointSize (size);
#endif
}

void GLAPIENTRY glPolygonMode (GLenum face, GLenum mode)
{
	START(glPolygonMode);
	print_value(_GLenum, &face);
	PRINT_COMMA
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPolygonMode (face, mode);
#endif
}

void GLAPIENTRY glPolygonOffset (GLfloat factor, GLfloat units)
{
	START(glPolygonOffset);
	print_value(_GLfloat, &factor);
	PRINT_COMMA
	print_value(_GLfloat, &units);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPolygonOffset (factor, units);
#endif
}

void GLAPIENTRY glPolygonStipple (const GLubyte *mask)
{
	START(glPolygonStipple);
	print_value(_GLvoid, &mask);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPolygonStipple (mask);
#endif
}

void GLAPIENTRY glPopAttrib ()
{
	START(glPopAttrib);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPopAttrib();
#endif
}

void GLAPIENTRY glPopClientAttrib ()
{
	START(glPopClientAttrib);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPopClientAttrib();
#endif
}

void GLAPIENTRY glPopMatrix ()
{
	START(glPopMatrix);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPopMatrix();
#endif
}

void GLAPIENTRY glPopName ()
{
	START(glPopName);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPopName();
#endif
}

void GLAPIENTRY glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities)
{
    int i;

	START(glPrioritizeTextures);
	print_value(_GLsizei, &n);
	PRINT_COMMA
    print("[");
    for(i=0;i<n;i++)
    {
        if(i != 0) PRINT_COMMA
        print("%ud is %f", textures[i], priorities[i]);
    }
    print("]");
	END;
#ifdef DISPLAYLOCAL
	GLV.glPrioritizeTextures (n, textures, priorities);
#endif
}

void GLAPIENTRY glPushAttrib (GLbitfield mask)
{
	START(glPushAttrib);
	print_value(_GLbitfield, &mask);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPushAttrib (mask);
#endif
}

void GLAPIENTRY glPushClientAttrib (GLbitfield mask)
{
	START(glPushClientAttrib);
	print_value(_GLbitfield, &mask);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPushClientAttrib (mask);
#endif
}

void GLAPIENTRY glPushMatrix ()
{
	START(glPushMatrix);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPushMatrix();
#endif
}

void GLAPIENTRY glPushName (GLuint name)
{
	START(glPushName);
	print_value(_GLuint, &name);
	END;
#ifdef DISPLAYLOCAL
	GLV.glPushName (name);
#endif
}

void GLAPIENTRY glRasterPos2d (GLdouble x, GLdouble y)
{
	START(glRasterPos2d);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2d (x, y);
#endif
}

void GLAPIENTRY glRasterPos2dv (const GLdouble *v)
{
	START(glRasterPos2dv);
	print_value(_GLdouble2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2dv (v);
#endif
}

void GLAPIENTRY glRasterPos2f (GLfloat x, GLfloat y)
{
	START(glRasterPos2f);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2f (x, y);
#endif
}

void GLAPIENTRY glRasterPos2fv (const GLfloat *v)
{
	START(glRasterPos2fv);
	print_value(_GLfloat2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2fv (v);
#endif
}

void GLAPIENTRY glRasterPos2i (GLint x, GLint y)
{
	START(glRasterPos2i);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2i (x, y);
#endif
}

void GLAPIENTRY glRasterPos2iv (const GLint *v)
{
	START(glRasterPos2iv);
	print_value(_GLint2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2iv (v);
#endif
}

void GLAPIENTRY glRasterPos2s (GLshort x, GLshort y)
{
	START(glRasterPos2s);
	print_value(_GLshort, &x);
	PRINT_COMMA
	print_value(_GLshort, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2s (x, y);
#endif
}

void GLAPIENTRY glRasterPos2sv (const GLshort *v)
{
	START(glRasterPos2sv);
	print_value(_GLshort2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos2sv (v);
#endif
}

void GLAPIENTRY glRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
	START(glRasterPos3d);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	PRINT_COMMA
	print_value(_GLdouble, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3d (x, y, z);
#endif
}

void GLAPIENTRY glRasterPos3dv (const GLdouble *v)
{
	START(glRasterPos3dv);
	print_value(_GLdouble3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3dv (v);
#endif
}

void GLAPIENTRY glRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
	START(glRasterPos3f);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	PRINT_COMMA
	print_value(_GLfloat, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3f (x, y, z);
#endif
}

void GLAPIENTRY glRasterPos3fv (const GLfloat *v)
{
	START(glRasterPos3fv);
	print_value(_GLfloat3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3fv (v);
#endif
}

void GLAPIENTRY glRasterPos3i (GLint x, GLint y, GLint z)
{
	START(glRasterPos3i);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLint, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3i (x, y, z);
#endif
}

void GLAPIENTRY glRasterPos3iv (const GLint *v)
{
	START(glRasterPos3iv);
	print_value(_GLint3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3iv (v);
#endif
}

void GLAPIENTRY glRasterPos3s (GLshort x, GLshort y, GLshort z)
{
	START(glRasterPos3s);
	print_value(_GLshort, &x);
	PRINT_COMMA
	print_value(_GLshort, &y);
	PRINT_COMMA
	print_value(_GLshort, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3s (x, y, z);
#endif
}

void GLAPIENTRY glRasterPos3sv (const GLshort *v)
{
	START(glRasterPos3sv);
	print_value(_GLshort3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos3sv (v);
#endif
}

void GLAPIENTRY glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	START(glRasterPos4d);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	PRINT_COMMA
	print_value(_GLdouble, &z);
	PRINT_COMMA
	print_value(_GLdouble, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4d (x, y, z, w);
#endif
}

void GLAPIENTRY glRasterPos4dv (const GLdouble *v)
{
	START(glRasterPos4dv);
	print_value(_GLdouble, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4dv (v);
#endif
}

void GLAPIENTRY glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	START(glRasterPos4f);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	PRINT_COMMA
	print_value(_GLfloat, &z);
	PRINT_COMMA
	print_value(_GLfloat, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4f (x, y, z, w);
#endif
}

void GLAPIENTRY glRasterPos4fv (const GLfloat *v)
{
	START(glRasterPos4fv);
	print_value(_GLfloat, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4fv (v);
#endif
}

void GLAPIENTRY glRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
	START(glRasterPos4i);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLint, &z);
	PRINT_COMMA
	print_value(_GLint, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4i (x, y, z, w);
#endif
}

void GLAPIENTRY glRasterPos4iv (const GLint *v)
{
	START(glRasterPos4iv);
	print_value(_GLint, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4iv (v);
#endif
}

void GLAPIENTRY glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
	START(glRasterPos4i);
	print_value(_GLshort, &x);
	PRINT_COMMA
	print_value(_GLshort, &y);
	PRINT_COMMA
	print_value(_GLshort, &z);
	PRINT_COMMA
	print_value(_GLshort, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4s (x, y, z, w);
#endif
}

void GLAPIENTRY glRasterPos4sv (const GLshort *v)
{
	START(glRasterPos4sv);
	print_value(_GLshort, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRasterPos4sv (v);
#endif
}

void GLAPIENTRY glReadBuffer (GLenum mode)
{
	START(glReadBuffer);
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glReadBuffer (mode);
#endif
}

void GLAPIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
#ifdef DISPLAYLOCAL
	GLV.glReadPixels (x, y, width, height, format, type, pixels);
	START(glReadPixels);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	PRINT_COMMA
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
    /*FIXME?*/
	print_value(_GLvoid, &pixels);
	END;
#endif
}

void GLAPIENTRY glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
	START(glRectd);
	print_value(_GLdouble, &x1);
	PRINT_COMMA
	print_value(_GLdouble, &y1);
	PRINT_COMMA
	print_value(_GLdouble, &x2);
	PRINT_COMMA
	print_value(_GLdouble, &y2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRectd (x1, y1, x2, y2);
#endif
}

void GLAPIENTRY glRectdv (const GLdouble *v1, const GLdouble *v2)
{
	START(glRectdv);
	print_value(_GLdouble2, &v1);
	PRINT_COMMA
	print_value(_GLdouble2, &v2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRectdv (v1, v2);
#endif
}

void GLAPIENTRY glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	START(glRectf);
	print_value(_GLfloat, &x1);
	PRINT_COMMA
	print_value(_GLfloat, &y1);
	PRINT_COMMA
	print_value(_GLfloat, &x2);
	PRINT_COMMA
	print_value(_GLfloat, &y2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRectf (x1, y1, x2, y2);
#endif
}

void GLAPIENTRY glRectfv (const GLfloat *v1, const GLfloat *v2)
{
	START(glRectfv);
	print_value(_GLfloat2, &v1);
	PRINT_COMMA
	print_value(_GLfloat2, &v2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRectfv (v1, v2);
#endif
}

void GLAPIENTRY glRecti (GLint x1, GLint y1, GLint x2, GLint y2)
{
	START(glRecti);
	print_value(_GLint, &x1);
	PRINT_COMMA
	print_value(_GLint, &y1);
	PRINT_COMMA
	print_value(_GLint, &x2);
	PRINT_COMMA
	print_value(_GLint, &y2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRecti (x1, y1, x2, y2);
#endif
}

void GLAPIENTRY glRectiv (const GLint *v1, const GLint *v2)
{
	START(glRectiv);
	print_value(_GLint2, &v1);
	PRINT_COMMA
	print_value(_GLint2, &v2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRectiv (v1, v2);
#endif
}

void GLAPIENTRY glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
	START(glRects);
	print_value(_GLshort, &x1);
	PRINT_COMMA
	print_value(_GLshort, &y1);
	PRINT_COMMA
	print_value(_GLshort, &x2);
	PRINT_COMMA
	print_value(_GLshort, &y2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRects (x1, y1, x2, y2);
#endif
}

void GLAPIENTRY glRectsv (const GLshort *v1, const GLshort *v2)
{
	START(glRectsv);
	print_value(_GLshort2, &v1);
	PRINT_COMMA
	print_value(_GLshort2, &v2);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRectsv (v1, v2);
#endif
}

GLint GLAPIENTRY glRenderMode (GLenum mode)
{
	GLint result;

	result = GLV.glRenderMode(mode);
    START(glRenderMode);
	print_value(_GLenum, &mode);
    RESULT;
	print_value(_GLint, &result);
    ENDRESULT;
	return result;
}

void GLAPIENTRY glRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	START(glRotated);
	print_value(_GLdouble, &angle);
	PRINT_COMMA
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	PRINT_COMMA
	print_value(_GLdouble, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRotated (angle, x, y, z);
#endif
}

void GLAPIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	START(glRotatef);
	print_value(_GLfloat, &angle);
	PRINT_COMMA
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	PRINT_COMMA
	print_value(_GLfloat, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glRotatef (angle, x, y, z);
#endif
}

void GLAPIENTRY glScaled (GLdouble x, GLdouble y, GLdouble z)
{
	START(glScaled);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	PRINT_COMMA
	print_value(_GLdouble, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glScaled (x, y, z);
#endif
}

void GLAPIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z)
{
	START(glScalef);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	PRINT_COMMA
	print_value(_GLfloat, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glScalef (x, y, z);
#endif
}

void GLAPIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
	START(glScissor);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	END;
#ifdef DISPLAYLOCAL
	GLV.glScissor (x, y, width, height);
#endif
}

void GLAPIENTRY glSelectBuffer (GLsizei size, GLuint *buffer)
{
	START(glSelectBuffer);
	print_value(_GLsizei, &size);
	PRINT_COMMA
	print_value(_GLvoid, &buffer);
	END;
#ifdef DISPLAYLOCAL
	GLV.glSelectBuffer (size, buffer);
#endif
}

void GLAPIENTRY glShadeModel (GLenum mode)
{
	START(glShadeModel);
	print_value(_GLenum, &mode);
	END;
#ifdef DISPLAYLOCAL
	GLV.glShadeModel (mode);
#endif
}

void GLAPIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask)
{
	START(glStencilFunc);
	print_value(_GLenum, &func);
	PRINT_COMMA
	print_value(_GLint, &ref);
	PRINT_COMMA
	print_value(_GLuint, &mask);
	END;
#ifdef DISPLAYLOCAL
	GLV.glStencilFunc (func, ref, mask);
#endif
}

void GLAPIENTRY glStencilMask (GLuint mask)
{
	START(glStencilMask);
	print_value(_GLuint, &mask);
	END;
#ifdef DISPLAYLOCAL
	GLV.glStencilMask (mask);
#endif
}

void GLAPIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
	useZeroOne = GL_TRUE;
	START(glStencilOp);
	print_value(_GLenum, &fail);
	PRINT_COMMA
	print_value(_GLenum, &zfail);
	PRINT_COMMA
	print_value(_GLenum, &zpass);
	END;
#ifdef DISPLAYLOCAL
	GLV.glStencilOp (fail, zfail, zpass);
	useZeroOne = GL_FALSE;
#endif
}

void GLAPIENTRY glTexCoord1d (GLdouble s)
{
	START(glTexCoord1d);
	print_value(_GLdouble, &s);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1d (s);
#endif
}

void GLAPIENTRY glTexCoord1dv (const GLdouble *v)
{
	START(glTexCoord1dv);
	print_value(_GLdouble1, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1dv (v);
#endif
}

void GLAPIENTRY glTexCoord1f (GLfloat s)
{
	START(glTexCoord1f);
	print_value(_GLfloat, &s);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1f (s);
#endif
}

void GLAPIENTRY glTexCoord1fv (const GLfloat *v)
{
	START(glTexCoord1fv);
	print_value(_GLfloat1, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1fv (v);
#endif
}

void GLAPIENTRY glTexCoord1i (GLint s)
{
	START(glTexCoord1i);
	print_value(_GLint, &s);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1i (s);
#endif
}

void GLAPIENTRY glTexCoord1iv (const GLint *v)
{
	START(glTexCoord1iv);
	print_value(_GLint1, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1iv (v);
#endif
}

void GLAPIENTRY glTexCoord1s (GLshort s)
{
	START(glTexCoord1s);
	print_value(_GLshort, &s);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1s (s);
#endif
}

void GLAPIENTRY glTexCoord1sv (const GLshort *v)
{
	START(glTexCoord1sv);
	print_value(_GLshort1, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord1sv (v);
#endif
}

void GLAPIENTRY glTexCoord2d (GLdouble s, GLdouble t)
{
	START(glTexCoord2d);
	print_value(_GLdouble, &s);
	PRINT_COMMA
	print_value(_GLdouble, &t);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2d (s, t);
#endif
}

void GLAPIENTRY glTexCoord2dv (const GLdouble *v)
{
	START(glTexCoord2dv);
	print_value(_GLdouble2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2dv (v);
#endif
}

void GLAPIENTRY glTexCoord2f (GLfloat s, GLfloat t)
{
	START(glTexCoord2f);
	print_value(_GLfloat, &s);
	PRINT_COMMA
	print_value(_GLfloat, &t);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2f (s, t);
#endif
}

void GLAPIENTRY glTexCoord2fv (const GLfloat *v)
{
	START(glTexCoord2fv);
	print_value(_GLfloat2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2fv (v);
#endif
}

void GLAPIENTRY glTexCoord2i (GLint s, GLint t)
{
	START(glTexCoord2i);
	print_value(_GLint, &s);
	PRINT_COMMA
	print_value(_GLint, &t);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2i (s, t);
#endif
}

void GLAPIENTRY glTexCoord2iv (const GLint *v)
{
	START(glTexCoord2iv);
	print_value(_GLint2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2iv (v);
#endif
}

void GLAPIENTRY glTexCoord2s (GLshort s, GLshort t)
{
	START(glTexCoord2s);
	print_value(_GLshort, &s);
	PRINT_COMMA
	print_value(_GLshort, &t);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2s (s, t);
#endif
}

void GLAPIENTRY glTexCoord2sv (const GLshort *v)
{
	START(glTexCoord2sv);
	print_value(_GLshort2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord2sv (v);
#endif
}

void GLAPIENTRY glTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
	START(glTexCoord3d);
	print_value(_GLdouble, &s);
	PRINT_COMMA
	print_value(_GLdouble, &t);
	PRINT_COMMA
	print_value(_GLdouble, &r);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3d (s, t, r);
#endif
}

void GLAPIENTRY glTexCoord3dv (const GLdouble *v)
{
	START(glTexCoord3dv);
	print_value(_GLdouble3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3dv (v);
#endif
}

void GLAPIENTRY glTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
	START(glTexCoord3f);
	print_value(_GLfloat, &s);
	PRINT_COMMA
	print_value(_GLfloat, &t);
	PRINT_COMMA
	print_value(_GLfloat, &r);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3f (s, t, r);
#endif
}

void GLAPIENTRY glTexCoord3fv (const GLfloat *v)
{
	START(glTexCoord3fv);
	print_value(_GLfloat3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3fv (v);
#endif
}

void GLAPIENTRY glTexCoord3i (GLint s, GLint t, GLint r)
{
	START(glTexCoord3i);
	print_value(_GLint, &s);
	PRINT_COMMA
	print_value(_GLint, &t);
	PRINT_COMMA
	print_value(_GLint, &r);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3i (s, t, r);
#endif
}

void GLAPIENTRY glTexCoord3iv (const GLint *v)
{
	START(glTexCoord3iv);
	print_value(_GLint3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3iv (v);
#endif
}

void GLAPIENTRY glTexCoord3s (GLshort s, GLshort t, GLshort r)
{
	START(glTexCoord3s);
	print_value(_GLshort, &s);
	PRINT_COMMA
	print_value(_GLshort, &t);
	PRINT_COMMA
	print_value(_GLshort, &r);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3s (s, t, r);
#endif
}

void GLAPIENTRY glTexCoord3sv (const GLshort *v)
{
	START(glTexCoord3sv);
	print_value(_GLshort3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord3sv (v);
#endif
}

void GLAPIENTRY glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
	START(glTexCoord4d);
	print_value(_GLdouble, &s);
	PRINT_COMMA
	print_value(_GLdouble, &t);
	PRINT_COMMA
	print_value(_GLdouble, &r);
	PRINT_COMMA
	print_value(_GLdouble, &q);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4d (s, t, r, q);
#endif
}

void GLAPIENTRY glTexCoord4dv (const GLdouble *v)
{
	START(glTexCoord4dv);
	print_value(_GLdouble4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4dv (v);
#endif
}

void GLAPIENTRY glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	START(glTexCoord2f);
	print_value(_GLfloat, &s);
	PRINT_COMMA
	print_value(_GLfloat, &t);
	PRINT_COMMA
	print_value(_GLfloat, &r);
	PRINT_COMMA
	print_value(_GLfloat, &q);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4f (s, t, r, q);
#endif
}

void GLAPIENTRY glTexCoord4fv (const GLfloat *v)
{
	START(glTexCoord4fv);
	print_value(_GLfloat4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4fv (v);
#endif
}

void GLAPIENTRY glTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
	START(glTexCoord4i);
	print_value(_GLint, &s);
	PRINT_COMMA
	print_value(_GLint, &t);
	PRINT_COMMA
	print_value(_GLint, &r);
	PRINT_COMMA
	print_value(_GLint, &q);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4i (s, t, r, q);
#endif
}

void GLAPIENTRY glTexCoord4iv (const GLint *v)
{
	START(glTexCoord4iv);
	print_value(_GLint4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4iv (v);
#endif
}

void GLAPIENTRY glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
	START(glTexCoord4s);
	print_value(_GLshort, &s);
	PRINT_COMMA
	print_value(_GLshort, &t);
	PRINT_COMMA
	print_value(_GLshort, &r);
	PRINT_COMMA
	print_value(_GLshort, &q);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4s (s, t, r, q);
#endif
}

void GLAPIENTRY glTexCoord4sv (const GLshort *v)
{
	START(glTexCoord4sv);
	print_value(_GLshort4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoord4sv (v);
#endif
}

void GLAPIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	START(glTexCoordPointer);
	print_value(_GLint, &size);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLsizei, &stride);
	PRINT_COMMA
	print_value(_GLvoid, &pointer);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexCoordPointer (size, type, stride, pointer);
#endif
}

void GLAPIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    GLenum p = (GLenum)param;

	START(glTexEnvf);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLfloat, &param);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexEnvf (target, pname, param);
#endif
}

void GLAPIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
	START(glTexEnvfv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_TEXTURE_ENV_MODE:
        {
            GLenum p = (GLenum)params[0];

            print_value(_GLenum, &p);
        }
        break;
    case GL_TEXTURE_ENV_COLOR:
        print_value(_GLfloat4, &params);
        break;

    default:
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexEnvfv (target, pname, params);
#endif
}

void GLAPIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param)
{
    GLenum p = (GLenum)param;

	START(glTexEnvi);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLenum, &p);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexEnvi (target, pname, param);
#endif
}

void GLAPIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params)
{
	START(glTexEnviv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_TEXTURE_ENV_MODE:
        {
            GLenum p = (GLenum)params[0];

            print_value(_GLenum, &p);
        }
        break;
    case GL_TEXTURE_ENV_COLOR:
        print_value(_GLint4, &params);
        break;

    default:
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexEnviv (target, pname, params);
#endif
}

void GLAPIENTRY glTexGend (GLenum coord, GLenum pname, GLdouble param)
{
    GLenum p = (GLenum)param;

	START(glTexGend);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLenum, &p);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexGend (coord, pname, param);
#endif
}

void GLAPIENTRY glTexGendv (GLenum coord, GLenum pname, const GLdouble *params)
{
	START(glTexGendv);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {

    case GL_TEXTURE_GEN_MODE:
        {
            GLenum p = (GLenum)params[0];

            print_value(_GLenum, &p);
        }
        break;

    case GL_OBJECT_PLANE:
    case GL_EYE_PLANE:
        print_value(_GLdouble4, &params);
        break;

    default:
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexGendv (coord, pname, params);
#endif
}

void GLAPIENTRY glTexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    GLenum p = (GLenum)param;

	START(glTexGenf);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLenum, &p);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexGenf (coord, pname, param);
#endif
}

void GLAPIENTRY glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params)
{
	START(glTexGenfv);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {

    case GL_TEXTURE_GEN_MODE:
        {
            GLenum p = (GLenum)params[0];

            print_value(_GLenum, &p);
        }
        break;

    case GL_OBJECT_PLANE:
    case GL_EYE_PLANE:
        print_value(_GLfloat4, &params);
        break;

    default:
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexGenfv (coord, pname, params);
#endif
}

void GLAPIENTRY glTexGeni (GLenum coord, GLenum pname, GLint param)
{
    GLenum p = (GLenum)param;

	START(glTexGeni);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
	print_value(_GLenum, &p);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexGeni (coord, pname, param);
#endif
}

void GLAPIENTRY glTexGeniv (GLenum coord, GLenum pname, const GLint *params)
{
	START(glTexGeniv);
	print_value(_GLenum, &coord);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {

    case GL_TEXTURE_GEN_MODE:
        {
            GLenum p = (GLenum)params[0];

            print_value(_GLenum, &p);
        }
        break;

    case GL_OBJECT_PLANE:
    case GL_EYE_PLANE:
        print_value(_GLint4, &params);
        break;

    default:
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexGeniv (coord, pname, params);
#endif
}

void GLAPIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	START(glTexImage1D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLint, &internalformat);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLint, &border);
	PRINT_COMMA
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLvoid, &pixels);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexImage1D (target, level, internalformat, width, border, format, type, pixels);
#endif
}

void GLAPIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	unsigned long i, nBytes, sBytes = 0, tBytes, typeModifier;
	NLbyte *ptr = (NLbyte*)pixels;
	//if(texCount > 1000) return;
	//texCount++;

	//printf("glTexImage2D\n");
	START(glTexImage2D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLint, &internalformat);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	PRINT_COMMA
	print_value(_GLint, &border);
	PRINT_COMMA
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA

	
	//for(i = 0; i < width * height; i++) {
	//	AddByteToStream((((NLbyte*)pixels)[i]));
	//}
	//StreamTexBytes(width * height, pixels);
	
	//textures in distinct packets atm
#ifdef SENDTEX
	SendStream();
	typeModifier = 4;
	switch(type) {
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			typeModifier = 2;
			break;
		case GL_UNSIGNED_INT:
			typeModifier = 4;
	}
	nBytes = width * height * typeModifier;
	//cout << "texture type: " << type << ": " << width << ", " << height << ": " << nBytes << endl;
	while(sBytes < nBytes) {
		tBytes = nBytes - sBytes > PACKET_LENGTH ? PACKET_LENGTH : nBytes - sBytes;
		memcpy((NLubyte *)netBuffer, (NLubyte *)ptr, tBytes);
		
		SendPacket();
		ptr = ptr + tBytes;
		sBytes += tBytes;
		//printf("%i < %i\n",sBytes,nBytes);
	}
	//printf("\n");
	netBuffer[0] = (unsigned char)ID_USER_PACKET_ENUM;

	count = 1;
	AddIntToStream(400);
	AddIntToStream(packetCount);
#endif
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexImage2D (target, level, internalformat, width, height, border, format, type, pixels);
#endif
}

void GLAPIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    GLenum p = (GLenum)param;

	START(glTexParameterf);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_TEXTURE_PRIORITY:
        print_value(_GLfloat, &param);
        break;
    default:
        print_value(_GLenum, &p);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexParameterf (target, pname, param);
#endif
}

void GLAPIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
	START(glTexParameterfv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_TEXTURE_BORDER_COLOR:
        print_value(_GLfloat4, &params);
        break;
    case GL_TEXTURE_PRIORITY:
        print_value(_GLfloat1, &params);
            break;
    default:
        {
            GLenum p = (GLenum)params;

            print_value(_GLenum, &p);
        }
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexParameterfv (target, pname, params);
#endif
}

void GLAPIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
    GLenum p = (GLenum)param;

	START(glTexParameteri);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_TEXTURE_PRIORITY:
        print_value(_GLint, &param);
        break;
    default:
        print_value(_GLenum, &p);
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexParameteri (target, pname, param);
#endif
}

void GLAPIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
	START(glTexParameteriv);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLenum, &pname);
	PRINT_COMMA
    switch (pname) {
    case GL_TEXTURE_BORDER_COLOR:
        print_value(_GLint4, &params);
        break;
    case GL_TEXTURE_PRIORITY:
        print_value(_GLint1, &params);
            break;
    default:
        {
            GLenum p = (GLenum)params;

            print_value(_GLenum, &p);
        }
        break;
    }
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexParameteriv (target, pname, params);
#endif
}

void GLAPIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels)
{
	START(glTexSubImage1D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLint, &xoffset);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLvoid, &pixels);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexSubImage1D (target, level, xoffset, width, format, type, pixels);
#endif
}

void GLAPIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	unsigned long i, nBytes, sBytes = 0, tBytes, typeModifier;
	NLbyte *ptr = (NLbyte*)pixels;
	//int i;
	//if(texCount > 5) return;
	//	texCount++;
	//if( width * height > PACKET_LENGTH) {
	//	print("---TEXTURE TOO LARGE - OMITTING---\n");
	//	return;
	//}
	//if(PACKET_LENGTH < count + width * height) SendStream();
	
	START(glTexSubImage2D);
	print_value(_GLenum, &target);
	PRINT_COMMA
	print_value(_GLint, &level);
	PRINT_COMMA
	print_value(_GLint, &xoffset);
	PRINT_COMMA
	print_value(_GLint, &yoffset);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	PRINT_COMMA
	print_value(_GLenum, &format);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	//for(i = 0; i < width * height; i++) {
	//	AddByteToStream((((NLbyte*)pixels)[i]));
	//}
#ifdef SENDTEX
	SendStream();
	typeModifier = 4;
	switch(type) {
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			typeModifier = 2;
			break;
		case GL_UNSIGNED_INT:
			typeModifier = 4;
	}
	nBytes = width * height * typeModifier;
	//cout << "texture type: " << type << ": " << width << ", " << height << ": " << nBytes << endl;
	while(sBytes < nBytes) {
		tBytes = nBytes - sBytes > PACKET_LENGTH ? PACKET_LENGTH : nBytes - sBytes;
		memcpy((NLubyte *)netBuffer, (NLubyte *)ptr, tBytes);
		
		SendPacket();
		ptr = ptr + tBytes;
		sBytes += tBytes;
		//printf("%i < %i\n",sBytes,nBytes);
	}
	//printf("\n");
	netBuffer[0] = (unsigned char)ID_USER_PACKET_ENUM;

	count = 1;
	AddIntToStream(400);
	AddIntToStream(packetCount);
#endif
	END;
#ifdef DISPLAYLOCAL
	GLV.glTexSubImage2D (target, level, xoffset, yoffset, width, height, format, type, pixels);
#endif
}

void GLAPIENTRY glTranslated (GLdouble x, GLdouble y, GLdouble z)
{
	START(glTranslated);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	PRINT_COMMA
	print_value(_GLdouble, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTranslated (x, y, z);
#endif
}

void GLAPIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
	START(glTranslatef);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	PRINT_COMMA
	print_value(_GLfloat, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glTranslatef (x, y, z);
#endif
}

void GLAPIENTRY glVertex2d (GLdouble x, GLdouble y)
{
	START(glVertex2d);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2d (x, y);
#endif
}

void GLAPIENTRY glVertex2dv (const GLdouble *v)
{
	START(glVertex2dv);
	print_value(_GLdouble2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2dv (v);
#endif
}

void GLAPIENTRY glVertex2f (GLfloat x, GLfloat y)
{
	START(glVertex2f);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2f (x, y);
#endif
}

void GLAPIENTRY glVertex2fv (const GLfloat *v)
{
	START(glVertex2fv);
	print_value(_GLfloat2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2fv (v);
#endif
}

void GLAPIENTRY glVertex2i (GLint x, GLint y)
{
	START(glVertex2i);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2i (x, y);
#endif
}

void GLAPIENTRY glVertex2iv (const GLint *v)
{
	START(glVertex2iv);
	print_value(_GLint2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2iv (v);
#endif
}

void GLAPIENTRY glVertex2s (GLshort x, GLshort y)
{
	START(glVertex2s);
	print_value(_GLshort, &x);
	PRINT_COMMA
	print_value(_GLshort, &y);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2s (x, y);
#endif
}

void GLAPIENTRY glVertex2sv (const GLshort *v)
{
	START(glVertex2sv);
	print_value(_GLshort2, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex2sv (v);
#endif
}

void GLAPIENTRY glVertex3d (GLdouble x, GLdouble y, GLdouble z)
{
	START(glVertex3d);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	PRINT_COMMA
	print_value(_GLdouble, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3d (x, y, z);
#endif
}

void GLAPIENTRY glVertex3dv (const GLdouble *v)
{
	START(glVertex3dv);
	print_value(_GLdouble3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3dv (v);
#endif
}

void GLAPIENTRY glVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
	START(glVertex3f);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	PRINT_COMMA
	print_value(_GLfloat, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3f (x, y, z);
#endif
}

void GLAPIENTRY glVertex3fv (const GLfloat *v)
{
	START(glVertex3fv);
	print_value(_GLfloat3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3fv (v);
#endif
}

void GLAPIENTRY glVertex3i (GLint x, GLint y, GLint z)
{
	START(glVertex3i);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLint, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3i (x, y, z);
#endif
}

void GLAPIENTRY glVertex3iv (const GLint *v)
{
	START(glVertex3iv);
	print_value(_GLint3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3iv (v);
#endif
}

void GLAPIENTRY glVertex3s (GLshort x, GLshort y, GLshort z)
{
	START(glVertex2s);
	print_value(_GLshort, &x);
	PRINT_COMMA
	print_value(_GLshort, &y);
	PRINT_COMMA
	print_value(_GLshort, &z);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3s (x, y, z);
#endif
}

void GLAPIENTRY glVertex3sv (const GLshort *v)
{
	START(glVertex3sv);
	print_value(_GLshort3, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex3sv (v);
#endif
}

void GLAPIENTRY glVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	START(glVertex4d);
	print_value(_GLdouble, &x);
	PRINT_COMMA
	print_value(_GLdouble, &y);
	PRINT_COMMA
	print_value(_GLdouble, &z);
	PRINT_COMMA
	print_value(_GLdouble, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4d (x, y, z, w);
#endif
}

void GLAPIENTRY glVertex4dv (const GLdouble *v)
{
	START(glVertex4dv);
	print_value(_GLdouble4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4dv (v);
#endif
}

void GLAPIENTRY glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	START(glVertex4f);
	print_value(_GLfloat, &x);
	PRINT_COMMA
	print_value(_GLfloat, &y);
	PRINT_COMMA
	print_value(_GLfloat, &z);
	PRINT_COMMA
	print_value(_GLfloat, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4f (x, y, z, w);
#endif
}

void GLAPIENTRY glVertex4fv (const GLfloat *v)
{
	START(glVertex4fv);
	print_value(_GLfloat4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4fv (v);
#endif
}

void GLAPIENTRY glVertex4i (GLint x, GLint y, GLint z, GLint w)
{
	START(glVertex4i);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLint, &z);
	PRINT_COMMA
	print_value(_GLint, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4i (x, y, z, w);
#endif
}

void GLAPIENTRY glVertex4iv (const GLint *v)
{
	START(glVertex4iv);
	print_value(_GLint4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4iv (v);
#endif
}

void GLAPIENTRY glVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
	START(glVertex2s);
	print_value(_GLshort, &x);
	PRINT_COMMA
	print_value(_GLshort, &y);
	PRINT_COMMA
	print_value(_GLshort, &z);
	PRINT_COMMA
	print_value(_GLshort, &w);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4s (x, y, z, w);
#endif
}

void GLAPIENTRY glVertex4sv (const GLshort *v)
{
	START(glVertex4sv);
	print_value(_GLshort4, &v);
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertex4sv (v);
#endif
}

void GLAPIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	START(glVertexPointer);
	print_value(_GLint, &size);
	PRINT_COMMA
	print_value(_GLenum, &type);
	PRINT_COMMA
	print_value(_GLsizei, &stride);
	PRINT_COMMA
	memcpy((NLbyte *)&netBuffer[count], (NLbyte *)pointer, 96);
	count += 96; //(24 * sizeof(GLfloat));
	END;
#ifdef DISPLAYLOCAL
	GLV.glVertexPointer (size, type, stride, pointer);
#endif
}

void GLAPIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	START(glViewport);
	print_value(_GLint, &x);
	PRINT_COMMA
	print_value(_GLint, &y);
	PRINT_COMMA
	print_value(_GLsizei, &width);
	PRINT_COMMA
	print_value(_GLsizei, &height);
	END;
#ifdef DISPLAYLOCAL
	GLV.glViewport (x, y, width, height);
#endif
}

#if defined WIN32 || defined WIN64

/* WGL functions */
int GLAPIENTRY wglChoosePixelFormat (HDC a, CONST PIXELFORMATDESCRIPTOR *b)
{
	int result;

	(void)gltraceInit();
	result = GLV.wglChoosePixelFormat(a,b);
	START(wglChoosePixelFormat);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(PFD, &b);
    RESULT;
	print_value(_GLint, &result);
    ENDRESULT;
    return result;
}

BOOL GLAPIENTRY wglCopyContext (HGLRC a, HGLRC b, UINT c)
{
	BOOL result;

	(void)gltraceInit();
	result = GLV.wglCopyContext(a,b,c);
	START(wglCopyContext);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(HEX, &b);
	PRINT_COMMA
	print_value(_GLuint, &c);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

HGLRC GLAPIENTRY wglCreateContext (HDC a)
{
	HGLRC result;

	(void)gltraceInit();
	result = GLV.wglCreateContext(a);
	START(wglCreateContext);
	print_value(HEX, &a);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

HGLRC GLAPIENTRY wglCreateLayerContext (HDC a, int b)
{
	HGLRC result;

	(void)gltraceInit();
	result = GLV.wglCreateLayerContext(a,b);
	START(wglCreateLayerContext);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLuint, &b);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
	return result;
}

BOOL GLAPIENTRY wglDeleteContext (HGLRC a)
{
	BOOL result;

	(void)gltraceInit();
	result = GLV.wglDeleteContext(a);
	START(wglDeleteContext);
	print_value(HEX, &a);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

BOOL GLAPIENTRY wglDescribeLayerPlane (HDC a, int b, int c, UINT d, LPLAYERPLANEDESCRIPTOR e)
{
	BOOL result;

	(void)gltraceInit();
	result = GLV.wglDescribeLayerPlane(a,b,c,d,e);
	START(wglDescribeLayerPlane);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLint, &c);
	PRINT_COMMA
	print_value(_GLuint, &d);
	PRINT_COMMA
	print_value(LPD, &e);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

int GLAPIENTRY wglDescribePixelFormat (HDC a, int b, UINT c, LPPIXELFORMATDESCRIPTOR d)
{
	int result;
	
	(void)gltraceInit();
	result = GLV.wglDescribePixelFormat(a,b,c,d);
	START(wglDescribePixelFormat);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLuint, &c);
	PRINT_COMMA
	print_value(PFD, &d);
    RESULT;
	print_value(_GLint, &result);
    ENDRESULT;
	return result;
}

HGLRC GLAPIENTRY wglGetCurrentContext (void)
{
	HGLRC result;

	(void)gltraceInit();
	result = GLV.wglGetCurrentContext();
	START(wglGetCurrentContext);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
	return result;
}

HDC GLAPIENTRY wglGetCurrentDC (void)
{
	HDC result;

	(void)gltraceInit();
	result = GLV.wglGetCurrentDC();
	START(wglGetCurrentDC);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
	return result;
}

PROC GLAPIENTRY wglGetDefaultProcAddress (LPCSTR a)
{
	PROC result;

	(void)gltraceInit();
	result = GLV.wglGetDefaultProcAddress(a);
	START(wglGetDefaultProcAddress);
	print_value(STR, &a);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
	return result;
}

int GLAPIENTRY wglGetLayerPaletteEntries (HDC a, int b, int c, int d, COLORREF *e)
{
	int result;

	result = GLV.wglGetLayerPaletteEntries(a,b,c,d,e);
	START(wglGetLayerPaletteEntries);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLint, &c);
	PRINT_COMMA
	print_value(_GLint, &d);
	PRINT_COMMA
	print_value(HEX, &e);
    RESULT;
	print_value(_GLint, &result);
    ENDRESULT;
	return result;
}

int GLAPIENTRY wglGetPixelFormat (HDC a)
{
	int result;

	(void)gltraceInit();
	result = GLV.wglGetPixelFormat(a);
	START(wglGetPixelFormat);
	print_value(HEX, &a);
    RESULT;
	print_value(_GLint, &result);
    ENDRESULT;
	return result;
}

PROC  GLAPIENTRY wglGetProcAddress (LPCSTR a)
{
	int	index;
	PROC result;
	
	(void)gltraceInit();
    if(bDisableExt == GL_TRUE)
    {
	    START(wglGetProcAddress);
	    print_value(STR, &a);
        RESULT;
	    print_value(STR, "disabled");
        ENDRESULT;
        return NULL;
    }
	result = GLV.wglGetProcAddress(a);
	START(wglGetProcAddress);
	print_value(STR, &a);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
	index = GetEXTProcIndex(a);
	if (index >= 0 && result != NULL)
	{
		PROC result2;
		
		((unsigned long *) (&GLV_EXT))[index] = (unsigned long)result;
		result2 = (PROC)(((unsigned long *) (&GLV_EXT_RETURN))[index]);
		if (result2 != NULL)
		{
			result = result2;
			print("%s extension found\n", a);
		}
	}
    else
    {
        print("%s extension NOT found\n", a);
    }
	return result;
}

BOOL  GLAPIENTRY wglMakeCurrent (HDC a, HGLRC b)
{
	BOOL result;

	(void)gltraceInit();
	result = GLV.wglMakeCurrent(a,b);
	START(wglMakeCurrent);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(HEX, &b);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

BOOL  GLAPIENTRY wglRealizeLayerPalette (HDC a, int b, BOOL c)
{
	BOOL result;

	result = GLV.wglRealizeLayerPalette(a,b,c);
	START(wglRealizeLayerPalette);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLboolean, &c);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

int GLAPIENTRY wglSetLayerPaletteEntries (HDC a, int b, int c, int d, CONST COLORREF *e)
{
	int result;

	result = GLV.wglSetLayerPaletteEntries(a,b,c,d,e);
	START(wglSetLayerPaletteEntries);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLint, &c);
	PRINT_COMMA
	print_value(_GLint, &d);
	PRINT_COMMA
	print_value(HEX, &e);
    RESULT;
	print_value(_GLint, &result);
    ENDRESULT;
	return result;
}

BOOL  GLAPIENTRY wglSetPixelFormat (HDC a, int b, CONST PIXELFORMATDESCRIPTOR *c)
{
	BOOL result;
	
	(void)gltraceInit();
	result = GLV.wglSetPixelFormat(a,b,c);
	START(wglSetPixelFormat);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(PFD, &c);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

BOOL  GLAPIENTRY wglShareLists (HGLRC a, HGLRC b)
{
	BOOL result;

	result = GLV.wglShareLists(a,b);
	START(wglShareLists);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(HEX, &b);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

BOOL  GLAPIENTRY wglSwapBuffers (HDC a)
{
	BOOL result;
	
    if(bFSPenable == GL_TRUE)
    {
        renderFPS();
    }
	result = GLV.wglSwapBuffers(a);
	START(wglSwapBuffers);
	//print_value(HEX, &a);
    RESULT;
	//print_value(_GLboolean, &result);
    ENDRESULT;

    updateKeys();
	return result;
}

BOOL  GLAPIENTRY wglSwapLayerBuffers (HDC a, UINT b)
{
	BOOL result;

    /* is this right? */
    if(bFSPenable == GL_TRUE)
    {
        renderFPS();
    }
	result = GLV.wglSwapLayerBuffers(a,b);
	START(wglSwapLayerBuffers);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLuint, &b);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;

    updateKeys();
	return result;
}

BOOL  GLAPIENTRY wglUseFontBitmapsA (HDC a, DWORD b, DWORD c, DWORD d)
{
	BOOL result;

	result = GLV.wglUseFontBitmapsA(a,b,c,d);
	START(wglUseFontBitmapsA);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLint, &c);
	PRINT_COMMA
	print_value(_GLint, &d);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

BOOL  GLAPIENTRY wglUseFontBitmapsW (HDC a, DWORD b, DWORD c, DWORD d)
{
	BOOL result;

	result = GLV.wglUseFontBitmapsW(a,b,c,d);
	START(wglUseFontBitmapsW);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLint, &c);
	PRINT_COMMA
	print_value(_GLint, &d);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

BOOL  GLAPIENTRY wglUseFontOutlinesA (HDC a, DWORD b, DWORD c, DWORD d, FLOAT e, FLOAT f, int g, LPGLYPHMETRICSFLOAT h)
{
	BOOL result;

	result = GLV.wglUseFontOutlinesA(a,b,c,d,e,f,g,h);
	START(wglUseFontOutlinesA);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLint, &c);
	PRINT_COMMA
	print_value(_GLint, &d);
	PRINT_COMMA
	print_value(_GLfloat, &e);
	PRINT_COMMA
	print_value(_GLfloat, &f);
	PRINT_COMMA
	print_value(_GLint, &g);
	PRINT_COMMA
	print_value(HEX, &h);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

BOOL  GLAPIENTRY wglUseFontOutlinesW (HDC a, DWORD b, DWORD c, DWORD d, FLOAT e, FLOAT f, int g, LPGLYPHMETRICSFLOAT h)
{
	BOOL result;

	result = GLV.wglUseFontOutlinesW(a,b,c,d,e,f,g,h);
	START(wglUseFontOutlinesW);
	print_value(HEX, &a);
	PRINT_COMMA
	print_value(_GLint, &b);
	PRINT_COMMA
	print_value(_GLint, &c);
	PRINT_COMMA
	print_value(_GLint, &d);
	PRINT_COMMA
	print_value(_GLfloat, &e);
	PRINT_COMMA
	print_value(_GLfloat, &f);
	PRINT_COMMA
	print_value(_GLint, &g);
	PRINT_COMMA
	print_value(HEX, &h);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
	return result;
}

#else

/* GLX functions */

#include <X11/Xutil.h>

XVisualInfo* glXChooseVisual (Display *dpy, int screen, int *attribList)
{
	XVisualInfo* result;
	int *attrib;

	(void)gltraceInit();
    updateKeys();
	result = GLV.glXChooseVisual(dpy, screen, attribList);
	START(glXChooseVisual);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &screen);
	print(",{");
	for (attrib = attribList; *attrib; attrib++) {
	  print_value(GLX_attrib, attrib);
	  PRINT_COMMA
	  switch (*attrib) {
	  case 1:  case 4: case 5: case 6: break;
	  default: print("%u,", *++attrib);
	  }
	}
	print("None}");
    RESULT;
	print_value(GLX_visual_info, &result);
    ENDRESULT;
    return result;
}

void glXCopyContext (Display *dpy, GLXContext src, GLXContext dst, unsigned long mask)
{
	(void)gltraceInit();
#ifdef DISPLAYLOCAL
	GLV.glXCopyContext(dpy, src, dst, mask);
	START(glXCopyContext);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &src);
	PRINT_COMMA
	print_value(HEX, &dst);
	PRINT_COMMA
	print_value(_GLbitfield, &mask);
    END;
#endif
}

GLXContext glXCreateContext (Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct)
{
	GLXContext result;

	(void)gltraceInit();
    updateKeys();
	result = GLV.glXCreateContext(dpy, vis, shareList, direct);
	START(glXCreateContext);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &vis);
	PRINT_COMMA
	print_value(HEX, &shareList);
	PRINT_COMMA
	print_value(_GLboolean, &direct);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

GLXPixmap glXCreateGLXPixmap (Display *dpy, XVisualInfo *vis, Pixmap pixmap)
{
	GLXPixmap result;

	(void)gltraceInit();
    updateKeys();
	result = GLV.glXCreateGLXPixmap(dpy, vis, pixmap);
	START(glXCreateGLXPixmap);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &vis);
	PRINT_COMMA
	print_value(_GLuint, &pixmap);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

void glXDestroyContext (Display *dpy, GLXContext ctx)
{
	(void)gltraceInit();

	START(glXDestroyContext);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &ctx);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXDestroyContext(dpy, ctx);
#endif
}

void glXDestroyGLXPixmap (Display *dpy, GLXPixmap pix)
{
	(void)gltraceInit();

	START(glXDestroyGLXPixmap);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLuint, &pix);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXDestroyGLXPixmap(dpy, pix);
#endif
}

int glXGetConfig (Display *dpy, XVisualInfo *vis, int attrib, int *value)
{
	int result;

	(void)gltraceInit();
	result = GLV.glXGetConfig(dpy, vis, attrib, value);
	START(glXGetConfig);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &vis);
	PRINT_COMMA
	print_value(GLX_attrib, &attrib);
	PRINT_COMMA
	print_value(_GLint1, &value);
    RESULT;
	print_value(GLX_return, &result);
    ENDRESULT;
    return result;
}

GLXContext glXGetCurrentContext (void)
{
	GLXContext result;

	(void)gltraceInit();
	result = GLV.glXGetCurrentContext();
	START(glXGetCurrentContext);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

GLXDrawable glXGetCurrentDrawable (void)
{
	GLXDrawable result;

	(void)gltraceInit();
	result = GLV.glXGetCurrentDrawable();
	START(glXGetCurrentDrawable);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

Bool glXIsDirect (Display *dpy, GLXContext ctx)
{
	Bool result;

	(void)gltraceInit();
	result = GLV.glXIsDirect(dpy, ctx);
	START(glXIsDirect);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &ctx);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
    return result;
}

Bool glXMakeCurrent (Display *dpy, GLXDrawable drawable, GLXContext ctx)
{
	Bool result;

	(void)gltraceInit();
	result = GLV.glXMakeCurrent(dpy, drawable, ctx);
	START(glXMakeCurrent);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLuint, &drawable);
	PRINT_COMMA
	print_value(HEX, &ctx);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
    return result;
}


Bool glXQueryExtension (Display *dpy, int *errorBase, int *eventBase)
{
	Bool result;

	(void)gltraceInit();
	result = GLV.glXQueryExtension(dpy, errorBase, eventBase);
	START(glXQueryExtension);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint1, &errorBase);
	PRINT_COMMA
	print_value(_GLint1, &eventBase);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
    return result;
}


Bool glXQueryVersion (Display *dpy, int *major, int *minor)
{
	Bool result;

	(void)gltraceInit();
	result = GLV.glXQueryVersion(dpy, major, minor);
	START(glXQueryVersion);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint1, &major);
	PRINT_COMMA
	print_value(_GLint1, &minor);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
    return result;
}


void glXSwapBuffers (Display *dpy, GLXDrawable drawable)
{
	(void)gltraceInit();

    if(bFSPenable == GL_TRUE)
    {
        renderFPS();
    }

	START(glXSwapBuffers);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLuint, &drawable);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXSwapBuffers(dpy, drawable);
    updateKeys();
#endif
}

void glXUseXFont (Font font, int first, int count, int listBase)
{
	(void)gltraceInit();
	START(glXUseXFont);
	print_value(_GLuint, &font);
	PRINT_COMMA
	print_value(_GLint, &first);
	PRINT_COMMA
	print_value(_GLint, &count);
	PRINT_COMMA
	print_value(_GLint, &listBase);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXUseXFont(font, first, count, listBase);
#endif
}

void glXWaitGL (void)
{
	(void)gltraceInit();
	START(glXWaitGL);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXWaitGL();
#endif
}

void glXWaitX (void)
{
	(void)gltraceInit();
	START(glXWaitX);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXWaitX();
#endif
}

const char * glXGetClientString (Display *dpy, int name )
{
	const char *result;

	(void)gltraceInit();
	result = GLV.glXGetClientString(dpy, name);
	START(glXGetClientString);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	switch (name) {
	case 1: print("GLX_VENDOR"); break;
	case 2: print("GLX_VERSION"); break;
	case 3: print("GLX_EXTENSIONS"); break;
	default: print_value(_GLint, &name);
    }
    RESULT;
	print_value(STR, &result);
    ENDRESULT;
    return result;
}

const char * glXQueryServerString (Display *dpy, int screen, int name )
{
	const char *result;

	(void)gltraceInit();
	result = GLV.glXQueryServerString(dpy, screen, name);
	START(glXQueryServerString);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &screen);
	PRINT_COMMA
	switch (name) {
	case 1: print("GLX_VENDOR"); break;
	case 2: print("GLX_VERSION"); break;
	case 3: print("GLX_EXTENSIONS"); break;
	default: print_value(_GLint, &name);
    }
    RESULT;
	print_value(STR, &result);
    ENDRESULT;
    return result;
}

const char * glXQueryExtensionsString (Display *dpy, int screen)
{
	const char *result;

	(void)gltraceInit();
	result = GLV.glXQueryExtensionsString(dpy, screen);
	START(glXQueryExtensionsString);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLuint, &screen);
    RESULT;
	print_value(STR, &result);
    ENDRESULT;
    return result;
}

GLXFBConfig * glXGetFBConfigs (Display *dpy, int screen, int *nelements)
{
	GLXFBConfig *result;

	(void)gltraceInit();
	result = GLV.glXGetFBConfigs(dpy, screen, nelements);
	START(glXGetFBConfigs);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &screen);
	PRINT_COMMA
	print_value(_GLint1, &nelements);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

GLXFBConfig * glXChooseFBConfig (Display *dpy, int screen, const int *attrib_list, int *nelements)
{
	GLXFBConfig *result;

	(void)gltraceInit();
	result = GLV.glXChooseFBConfig(dpy, screen, attrib_list, nelements);
	START(glXChooseFBConfig);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &screen);
	PRINT_COMMA
	print_value(GLX_attrib_value_list, &attrib_list);
	PRINT_COMMA
	print_value(_GLint1, &nelements);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}


int glXGetFBConfigAttrib (Display *dpy, GLXFBConfig config, int attribute, int *value)
{
	int result;

	(void)gltraceInit();
	result = GLV.glXGetFBConfigAttrib(dpy, config, attribute, value);
	START(glXGetFBConfigAttrib);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &config);
	PRINT_COMMA
	print_value(GLX_attrib, &attribute);
	PRINT_COMMA
	print_value(_GLint1, &value);
    RESULT;
	print_value(GLX_return, &result);
    ENDRESULT;
    return result;
}

XVisualInfo * glXGetVisualFromFBConfig (Display *dpy, GLXFBConfig config)
{
	XVisualInfo *result;

	(void)gltraceInit();
	result = GLV.glXGetVisualFromFBConfig(dpy, config);
	START(glXGetVisualFromFBConfig);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &config);
    RESULT;
	print_value(GLX_visual_info, &result);
    ENDRESULT;
    return result;
}

GLXWindow glXCreateWindow (Display *dpy, GLXFBConfig config, Window win, const int *attrib_list)
{
	GLXWindow result;

	(void)gltraceInit();
    updateKeys();
	result = GLV.glXCreateWindow(dpy, config, win, attrib_list);
	START(glXCreateWindow);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &config);
	PRINT_COMMA
	print_value(_GLuint, &win);
	PRINT_COMMA
	print_value(GLX_attrib_value_list, &attrib_list);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

void glXDestroyWindow (Display *dpy, GLXWindow win)
{
	(void)gltraceInit();
	START(glXDestroyWindow);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &win);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXDestroyWindow(dpy, win);
#endif
}

GLXPixmap glXCreatePixmap (Display *dpy, GLXFBConfig config, Pixmap pixmap, const int *attrib_list)
{
	GLXPixmap result;

	(void)gltraceInit();
    updateKeys();
	result = GLV.glXCreatePixmap(dpy, config, pixmap, attrib_list);
	START(glXCreatePixmap);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &config);
	PRINT_COMMA
	print_value(_GLuint, &pixmap);
	PRINT_COMMA
	print_value(GLX_attrib_value_list, &attrib_list);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

void glXDestroyPixmap (Display *dpy, GLXPixmap pixmap)
{
	(void)gltraceInit();
	START(glXDestroyPixmap);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &pixmap);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXDestroyPixmap(dpy, pixmap);
#endif
}

GLXPbuffer glXCreatePbuffer (Display *dpy, GLXFBConfig config, const int *attrib_list)
{
	GLXPbuffer result;

	(void)gltraceInit();
    updateKeys();
	result = GLV.glXCreatePbuffer(dpy, config, attrib_list);
	START(glXCreatePbuffer);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &config);
	PRINT_COMMA
	print_value(GLX_attrib_value_list, &attrib_list);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

void glXDestroyPbuffer (Display *dpy, GLXPbuffer pbuf)
{
	(void)gltraceInit();
	START(glXDestroyPbuffer);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &pbuf);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXDestroyPbuffer(dpy, pbuf);
#endif
}


void glXQueryDrawable (Display *dpy, GLXDrawable draw, int attribute, unsigned int *value)
{
	(void)gltraceInit();
	START(glXQueryDrawable);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &draw);
	PRINT_COMMA
	print_value(_GLint, &attribute);
	PRINT_COMMA
	print_value(HEX, &value);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXQueryDrawable(dpy, draw, attribute, value);
#endif
}

GLXContext glXCreateNewContext (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct)
{
	GLXContext result;

	(void)gltraceInit();
    updateKeys();
	result = GLV.glXCreateNewContext(dpy, config, render_type, share_list, direct);
	START(glXCreateNewContext);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &config);
	PRINT_COMMA
	print_value(_GLint, &render_type);
	PRINT_COMMA
	print_value(HEX, &share_list);
	PRINT_COMMA
	print_value(_GLboolean, &direct);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

Bool glXMakeContextCurrent (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx)
{
	Bool result;

	(void)gltraceInit();
	result = GLV.glXMakeContextCurrent(dpy,  draw, read, ctx);
	START(glXMakeContextCurrent);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLuint, &draw);
	PRINT_COMMA
	print_value(_GLuint, &read);
	PRINT_COMMA
	print_value(HEX, &ctx);
    RESULT;
	print_value(_GLboolean, &result);
    ENDRESULT;
    return result;
}


GLXDrawable glXGetCurrentReadDrawable (void)
{
	GLXDrawable result;

	(void)gltraceInit();
	result = GLV.glXGetCurrentReadDrawable();
	START(glXGetCurrentReadDrawable);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

Display * glXGetCurrentDisplay (void)
{
	Display * result;

	(void)gltraceInit();
	result = GLV.glXGetCurrentDisplay();
	START(glXGetCurrentDisplay);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

int glXQueryContext (Display *dpy, GLXContext ctx, int attribute, int *value)
{
	int result;

	(void)gltraceInit();
	result = GLV.glXQueryContext(dpy, ctx, attribute, value);
	START(glXQueryContext);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &ctx);
	PRINT_COMMA
	print_value(_GLint, &attribute);
	PRINT_COMMA
	print_value(HEX, &value);
    RESULT;
	print_value(GLX_return, &result);
    ENDRESULT;
    return result;
}

void glXSelectEvent (Display *dpy, GLXDrawable draw, unsigned long event_mask)
{
	(void)gltraceInit();
	START(glXSelectEvent);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &draw);
	PRINT_COMMA
	print_value(_GLuint, &event_mask);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXSelectEvent(dpy, draw, event_mask);
#endif
}

void glXGetSelectedEvent (Display *dpy, GLXDrawable draw, unsigned long *event_mask)
{
	(void)gltraceInit();
	START(glXGetSelectedEvent);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &draw);
	PRINT_COMMA
	print_value(HEX, &event_mask);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXGetSelectedEvent(dpy, draw, event_mask);
#endif
}

GLXContextID glXGetContextIDEXT (const GLXContext ctx)
{
	GLXContextID result;

	(void)gltraceInit();
	result = GLV.glXGetContextIDEXT(ctx);
	START(glXGetContextIDEXT);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

GLXDrawable glXGetCurrentDrawableEXT (void)
{
	GLXDrawable result;

	(void)gltraceInit();
	result = GLV.glXGetCurrentDrawableEXT();
	START(glXGetCurrentDrawableEXT);
    RESULT;
	print_value(_GLuint, &result);
    ENDRESULT;
    return result;
}

GLXContext glXImportContextEXT (Display *dpy, GLXContextID contextID)
{
	GLXContext result;

	(void)gltraceInit();
	result = GLV.glXImportContextEXT(dpy, contextID);
	START(glXImportContextEXT);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(_GLint, &contextID);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

void glXFreeContextEXT (Display *dpy, GLXContext ctx)
{
	(void)gltraceInit();
	START(glXFreeContextEXT);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &ctx);
    END;
#ifdef DISPLAYLOCAL
	GLV.glXFreeContextEXT(dpy, ctx);
#endif
}

int glXQueryContextInfoEXT (Display *dpy, GLXContext ctx, int attribute, int *value)
{
	int result;

	(void)gltraceInit();
	result = GLV.glXQueryContextInfoEXT(dpy, ctx, attribute, value);
	START(glXQueryContextInfoEXT);
	print_value(STR, &DisplayString(dpy));
	PRINT_COMMA
	print_value(HEX, &ctx);
	PRINT_COMMA
	print_value(_GLint, &attribute);
	PRINT_COMMA
	print_value(HEX, &value);
    RESULT;
	print_value(GLX_return, &result);
    ENDRESULT;
    return result;
}

void (*glXGetProcAddressARB(const GLubyte *procName))(void)
{
	void (*result)(void);

	(void)gltraceInit();
	result = GLV.glXGetProcAddressARB(procName);
	START(glXGetProcAddressARB);
	print_value(STR, &procName);
    RESULT;
	print_value(HEX, &result);
    ENDRESULT;
    return result;
}

#endif

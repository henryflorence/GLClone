/* log.c                              -*- mode: C; tab-width: 4 -*-
 *
 * GLTrace2 OpenGL debugger/trace utlility
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

#include <sys/types.h>
#include <sys/timeb.h>
#include "gltrace.h"

const char *fmt;

#define LOGTOFILE

/* Misc. globals */
extern void extInitCallCount(void);
extern void extPrintCallCounts(void);

extern struct timeb starttime; /* Time when trace started */

extern FILE *text_file;    /* Output text file (NULL for stdout) */

extern char *GLenum_names[65536]; /* Pointer to text string for each GLenum value */

extern GLboolean useZeroOne;	/* To correct GL_ZERO and GL_ONE overlap with other enumerants */

extern int show_status[];
extern int show_status_EXT[];
NLbyte	*netBuffer;
NLbyte	*bigBuffer;
int		*readyToSend;
int		count;
int		packetCount;
//const char *fmt = "  %-32s %d\n";


/* Keep track of how many times each GL function/extension is called. */
unsigned int nCallCount[FP_LAST];
extern unsigned int nCallCountEXT[];

#ifdef _WIN32

GL_ENUM_PAIR PFD_flags_list[] =
{
	{ "PFD_DOUBLEBUFFER",					PFD_DOUBLEBUFFER},
	{ "PFD_STEREO",							PFD_STEREO},
	{ "PFD_DRAW_TO_WINDOW",					PFD_DRAW_TO_WINDOW},
	{ "PFD_DRAW_TO_BITMAP",					PFD_DRAW_TO_BITMAP},
	{ "PFD_SUPPORT_GDI",					PFD_SUPPORT_GDI},
	{ "PFD_SUPPORT_OPENGL",					PFD_SUPPORT_OPENGL},
	{ "PFD_GENERIC_FORMAT",					PFD_GENERIC_FORMAT},
	{ "PFD_NEED_PALETTE",					PFD_NEED_PALETTE},
	{ "PFD_NEED_SYSTEM_PALETTE",			PFD_NEED_SYSTEM_PALETTE},
	{ "PFD_SWAP_EXCHANGE",					PFD_SWAP_EXCHANGE},
	{ "PFD_SWAP_COPY",						PFD_SWAP_COPY},
	{ "PFD_SWAP_LAYER_BUFFERS",				PFD_SWAP_LAYER_BUFFERS},
	{ "PFD_GENERIC_ACCELERATED",			PFD_GENERIC_ACCELERATED},
	{ "PFD_DEPTH_DONTCARE",					PFD_DEPTH_DONTCARE},
	{ "PFD_DOUBLEBUFFER_DONTCARE",			PFD_DOUBLEBUFFER_DONTCARE},
	{ "PFD_STEREO_DONTCARE",				PFD_STEREO_DONTCARE}
};

GL_ENUM_PAIR LPD_flags_list[] =
{
	{ "LPD_DOUBLEBUFFER",					LPD_DOUBLEBUFFER},
	{ "LPD_STEREO",							LPD_STEREO},
	{ "LPD_SUPPORT_GDI",					LPD_SUPPORT_GDI},
	{ "LPD_SUPPORT_OPENGL",					LPD_SUPPORT_OPENGL},
	{ "LPD_SWAP_EXCHANGE",					LPD_SWAP_EXCHANGE},
	{ "LPD_SWAP_COPY",						LPD_SWAP_COPY},
	{ "LPD_TRANSPARENT",					LPD_TRANSPARENT},
	{ "LPD_SHARE_DEPTH",			        LPD_SHARE_DEPTH},
	{ "LPD_SHARE_STENCIL",				    LPD_SHARE_STENCIL},
	{ "LPD_SHARE_ACCUM",				    LPD_SHARE_ACCUM}
};

#else

const char* GLX_return_codes[] = 
{ "0",
  "GLX_BAD_SCREEN",
  "GLX_BAD_ATTRIBUTE", 
  "GLX_NO_EXTENSION", 
  "GLX_BAD_VISUAL", 
  "GLX_BAD_CONTEXT",
  "GLX_BAD_VALUE",
  "GLX_BAD_ENUM"
};

const char* GLX_visual_classes[] =
{
  "StaticGray",
  "GrayScale",
  "StaticColor",
  "PseudoColor",
  "TrueColor",
  "DirectColor"
};

GL_ENUM_PAIR GLX_attribs[] =
{
	{ "GLX_USE_GL",                    1},
	{ "GLX_BUFFER_SIZE",               2},
	{ "GLX_LEVEL",                     3},
	{ "GLX_RGBA",                      4},
	{ "GLX_DOUBLEBUFFER",              5},
	{ "GLX_STEREO",                    6},
	{ "GLX_AUX_BUFFERS",               7},
	{ "GLX_RED_SIZE",                  8},
	{ "GLX_GREEN_SIZE",                9},
	{ "GLX_BLUE_SIZE",                 10},
	{ "GLX_ALPHA_SIZE",                11},
	{ "GLX_DEPTH_SIZE",                12},
	{ "GLX_STENCIL_SIZE",              13},
	{ "GLX_ACCUM_RED_SIZE",            14},
	{ "GLX_ACCUM_GREEN_SIZE",          15},
	{ "GLX_ACCUM_BLUE_SIZE",           16},
	{ "GLX_ACCUM_ALPHA_SIZE",          17},
	{ "GLX_CONFIG_CAVEAT",             0x20},
	{ "GLX_X_VISUAL_TYPE",             0x22},
	{ "GLX_TRANSPARENT_TYPE",          0x23},
	{ "GLX_TRANSPARENT_INDEX_VALUE",   0x24},
	{ "GLX_TRANSPARENT_RED_VALUE",     0x25},
	{ "GLX_TRANSPARENT_GREEN_VALUE",   0x26},
	{ "GLX_TRANSPARENT_BLUE_VALUE",    0x27},
	{ "GLX_TRANSPARENT_ALPHA_VALUE",   0x28},
	{ "GLX_DRAWABLE_TYPE",             0x8010},
	{ "GLX_RENDER_TYPE",               0x8011},
	{ "GLX_X_RENDERABLE",              0x8012},
	{ "GLX_FBCONFIG_ID",               0x8013},
	{ "GLX_MAX_PBUFFER_WIDTH",         0x8016},
	{ "GLX_MAX_PBUFFER_HEIGHT",        0x8017},
	{ "GLX_MAX_PBUFFER_PIXELS",        0x8018},
	{ "GLX_VISUAL_ID",                 0x800B},
	{ "GLX_PRESERVED_CONTENTS",        0x801B},
	{ "GLX_LARGEST_PBUFFER",           0x801C},
	{ "GLX_PBUFFER_HEIGHT",            0x8040},
	{ "GLX_PBUFFER_WIDTH",             0x8041},

};
#endif

GL_ENUM_PAIR GLbitfield_list[] =
{
	{ "GL_CURRENT_BIT",		               0x00000001},
	{ "GL_POINT_BIT",		               0x00000002},
	{ "GL_LINE_BIT",		               0x00000004},
	{ "GL_POLYGON_BIT",		               0x00000008},
	{ "GL_POLYGON_STIPPLE_BIT",	           0x00000010},
	{ "GL_PIXEL_MODE_BIT",	               0x00000020},
	{ "GL_LIGHTING_BIT",	               0x00000040},
	{ "GL_FOG_BIT",		                   0x00000080},
	{ "GL_DEPTH_BUFFER_BIT",               0x00000100},
	{ "GL_ACCUM_BUFFER_BIT",               0x00000200},
	{ "GL_STENCIL_BUFFER_BIT",             0x00000400},
	{ "GL_VIEWPORT_BIT",                   0x00000800},
	{ "GL_TRANSFORM_BIT",                  0x00001000},
	{ "GL_ENABLE_BIT",                     0x00002000},
	{ "GL_COLOR_BUFFER_BIT",               0x00004000},
	{ "GL_HINT_BIT",                       0x00008000},
	{ "GL_EVAL_BIT",                       0x00010000},
	{ "GL_LIST_BIT",                       0x00020000},
	{ "GL_TEXTURE_BIT",                    0x00040000},
	{ "GL_SCISSOR_BIT",                    0x00080000},
    { "GL_MULTISAMPLE_BIT_ARB",            0x20000000}
/*	{ "GL_ALL_ATTRIB_BITS",                0x000fffff} */
};

#ifdef _WIN32

GL_ENUM_PAIR bufferregionbitfield_list[] =
{
	{ "WGL_FRONT_COLOR_BUFFER_BIT_ARB",    0x00000001},
	{ "WGL_BACK_COLOR_BUFFER_BIT_ARB",	   0x00000002},
	{ "WGL_DEPTH_BUFFER_BIT_ARB",          0x00000004},
	{ "WGL_STENCIL_BUFFER_BIT_ARB",	       0x00000008}
};

#endif

/*
  Send output to file or debug console
*/

void print(const char *fmt, ...)
{
	static char work_string[4096];
	
	if ((fmt == NULL) || (strlen(fmt) > sizeof(work_string)))
	{
		strcpy(work_string, "(String missing or too large)");
	}
	else
	{
		va_list ap;
		
		va_start(ap, fmt);
		
		(void)vsprintf(work_string, fmt, ap);
		
		va_end(ap);
	}
	
	if (text_file == NULL)
	{
		printf(work_string);
	}
	else
	{
		(void)fputs(work_string,text_file);
	}
}

/*
  Print typed value by reference
*/
void StreamTexBytes(int nBytes, const GLvoid *pixels)
{
	SendStream();

}
void print_value(TYPE type, const void *value)
{
	int				i, j;
	unsigned int	*v = (unsigned int *) value;
	signed char		*sc = *(signed char **) value;
	unsigned char	*uc = *(unsigned char **) value;
	short			*ss = *(short **) value;
	unsigned short	*us = *(unsigned short **) value;
	int				*si = *(int **) value;
	unsigned int	*ui = *(unsigned int **) value;
	float			*f = *(float **) value;
	double			*d = *(double **) value;
	
#ifdef _WIN32
	PIXELFORMATDESCRIPTOR   *desc;
    LAYERPLANEDESCRIPTOR    *lpd;
#else
	XVisualInfo             *xvi;
	int                     *attrib, *attrib_list;
#endif

	switch (type)
	{
#ifdef LOGTOFILE
	default:		print("0x%X",*(unsigned *) v); break;
		
	case _GLvoid:
	case HEX:		print("0x%X",*(unsigned *) v); break;
#endif
	case _GLclampd:
	case _GLdouble:	
#ifdef LOGTOFILE
		print("%f",  *(double *) v); 
#endif
		writeDouble(netBuffer, count, *(double *) v);
		break;
	case _GLdouble1:
#ifdef LOGTOFILE
		print(" { %f } ", d[0]); 
#endif
		writeDouble(netBuffer, count, d[0]);
		break;
	case _GLdouble2:
#ifdef LOGTOFILE
		print(" { %f, %f } ", d[0], d[1]); 
#endif
		writeDouble(netBuffer, count, d[0]); 
		writeDouble(netBuffer, count, d[1]); 
		break;
	case _GLdouble3:
#ifdef LOGTOFILE
		print(" { %f, %f, %f } ", d[0], d[1], d[2]);
#endif
		writeDouble(netBuffer, count, d[0]); 
		writeDouble(netBuffer, count, d[1]); 
		writeDouble(netBuffer, count, d[2]); 
		break;
	case _GLdouble4:
#ifdef LOGTOFILE
		print(" { %f, %f, %f, %f } ", d[0], d[1], d[2], d[3]); 
#endif
		writeDouble(netBuffer, count, d[0]);
		writeDouble(netBuffer, count, d[1]);
		writeDouble(netBuffer, count, d[2]);
		writeDouble(netBuffer, count, d[3]);
		break;
	case _GLdouble16:
#ifdef LOGTOFILE
		print("\n");
		print(" { %f, %f, %f, %f } ", d[0], d[4], d[8], d[12]);
		print(" { %f, %f, %f, %f } ", d[1], d[5], d[9], d[13]);
		print(" { %f, %f, %f, %f } ", d[2], d[6], d[10], d[14]);
		print(" { %f, %f, %f, %f } ", d[3], d[7], d[11], d[15]);
#endif
		writeDouble(netBuffer, count, d[0]); writeDouble(netBuffer, count, d[1]); writeDouble(netBuffer, count, d[2]); writeDouble(netBuffer, count, d[3]);
		writeDouble(netBuffer, count, d[4]); writeDouble(netBuffer, count, d[5]); writeDouble(netBuffer, count, d[6]); writeDouble(netBuffer, count, d[7]);
		writeDouble(netBuffer, count, d[8]); writeDouble(netBuffer, count, d[9]); writeDouble(netBuffer, count, d[10]); writeDouble(netBuffer, count, d[11]);
		writeDouble(netBuffer, count, d[12]); writeDouble(netBuffer, count, d[13]); writeDouble(netBuffer, count, d[14]); writeDouble(netBuffer, count, d[15]);

		break;

	case STR:
#ifdef LOGTOFILE
		print("\"%s\"",  *(char **) v);
#endif
		AddToStream( *(char **) v);
		break;
		
	case _GLbyte:
#ifdef LOGTOFILE
		print("%d",  *(signed char *) v);
#endif
		writeByte(netBuffer, count, *(signed char *) v);
		break;
	case _GLbyte1:
#ifdef LOGTOFILE
		print("{ %d } ", sc[0]);
#endif
		writeByte(netBuffer, count, sc[0]);
		break;
	case _GLbyte2:
#ifdef LOGTOFILE
		print("{ %d, %d } ", sc[0], sc[1]);
#endif
		writeByte(netBuffer, count, sc[0]);
		writeByte(netBuffer, count, sc[1]);
		break;
	case _GLbyte3:
#ifdef LOGTOFILE
		print("{ %d, %d, %d } ", sc[0], sc[1], sc[2]);
#endif
		writeByte(netBuffer, count, sc[0]);
		writeByte(netBuffer, count, sc[1]);
		writeByte(netBuffer, count, sc[2]); 
		break;
	case _GLbyte4:
#ifdef LOGTOFILE
		print("{ %d, %d, %d, %d } ", sc[0], sc[1], sc[2], sc[3]);
#endif 
		writeByte(netBuffer, count, sc[0]);
		writeByte(netBuffer, count, sc[1]);
		writeByte(netBuffer, count, sc[2]);
		writeByte(netBuffer, count, sc[3]);
		break;
		
	case _GLubyte:
#ifdef LOGTOFILE
		print("%u",  *(unsigned char *) v);
#endif
		writeByte(netBuffer, count, *(signed char *) v);
		break;
	case _GLubyte1:
#ifdef LOGTOFILE
		print("{ %u } ", uc[0]); 
#endif
		writeByte(netBuffer, count, uc[0]);
		break;
	case _GLubyte2:
#ifdef LOGTOFILE
		print("{ %u, %u } ", uc[0], uc[1]);
#endif
		writeByte(netBuffer, count, uc[0]);
		writeByte(netBuffer, count, uc[1]); 
		break;
	case _GLubyte3:	
#ifdef LOGTOFILE
		print("{ %u, %u, %u } ", uc[0], uc[1], uc[2]);
#endif
		writeByte(netBuffer, count, uc[0]);
		writeByte(netBuffer, count, uc[1]);
		writeByte(netBuffer, count, uc[2]); 
		break;
	case _GLubyte4:
#ifdef LOGTOFILE
		print("{ %u, %u, %u, %u } ", uc[0], uc[1], uc[2], uc[3]); 
#endif
		writeByte(netBuffer, count, uc[0]);
		writeByte(netBuffer, count, uc[1]);
		writeByte(netBuffer, count, uc[2]);
		writeByte(netBuffer, count, uc[3]);
		break;
	case _GLshort:
#ifdef LOGTOFILE
		print("%d",  *(short *) v); 
#endif
		writeShort(netBuffer, count, *(short *) v);
		break;

	case _GLshort1:
#ifdef LOGTOFILE
		print("{ %d } ", ss[0]);
#endif
		writeShort(netBuffer, count, ss[0]);
		break;
	case _GLshort2:
#ifdef LOGTOFILE
		print("{ %d, %d } ", ss[0], ss[1]);
#endif
		writeShort(netBuffer, count, ss[0]);
		writeShort(netBuffer, count, ss[1]);
		break;
	case _GLshort3:
#ifdef LOGTOFILE
		print("{ %d, %d, %d } ", ss[0], ss[1], ss[2]);
#endif
		writeShort(netBuffer, count, ss[0]);
		writeShort(netBuffer, count, ss[1]);
		writeShort(netBuffer, count, ss[2]);
		break;
	case _GLshort4:
#ifdef LOGTOFILE
		print("{ %d, %d, %d, %d } ", ss[0], ss[1], ss[2], ss[3]);
#endif
		writeShort(netBuffer, count, ss[0]);
		writeShort(netBuffer, count, ss[1]);
		writeShort(netBuffer, count, ss[2]);
		writeShort(netBuffer, count, ss[3]);
		break;
		
	case _GLushort:
#ifdef LOGTOFILE
		print("%u",  *(unsigned short *) v);
#endif 
		writeShort(netBuffer, count, *(short *) v);
		break;
	case _GLushort1:
#ifdef LOGTOFILE
		print("{ %u } ", us[0]);
#endif
		writeShort(netBuffer, count, us[0]);
		break;
	case _GLushort2:
#ifdef LOGTOFILE
		print("{ %u, %u } ", us[0], us[1]);
#endif
		writeShort(netBuffer, count, us[0]);
		writeShort(netBuffer, count, us[1]);
		break;
	case _GLushort3:
#ifdef LOGTOFILE
		print("{ %u, %u, %u } ", us[0], us[1], us[2]);
#endif
		writeShort(netBuffer, count, us[0]);
		writeShort(netBuffer, count, us[1]);
		writeShort(netBuffer, count, us[2]);
		break;
	case _GLushort4:
#ifdef LOGTOFILE
		print("{ %u, %u, %u, %u } ", us[0], us[1], us[2], us[3]);
#endif
		writeShort(netBuffer, count, us[0]);
		writeShort(netBuffer, count, us[1]);
		writeShort(netBuffer, count, us[2]);
		writeShort(netBuffer, count, us[3]);
		break;
		
	case _GLsizei:
	case _GLint:
#ifdef LOGTOFILE
			print("%d",  *(int *) v);
#endif
			writeLong(netBuffer, count, *(int *) v);
			break;
	case _GLint1:
#ifdef LOGTOFILE
			print("{ %d } ", si[0]);
#endif
			writeLong(netBuffer, count, si[0]);
			break;
	case _GLint2:
#ifdef LOGTOFILE
			print("{ %d, %d } ", si[0], si[1]);
#endif
			writeLong(netBuffer, count, si[0]);
			writeLong(netBuffer, count, si[1]);
			break;
	case _GLint3:
#ifdef LOGTOFILE
			print("{ %d, %d, %d } ", si[0], si[1], si[2]);
#endif
			writeLong(netBuffer, count, si[0]);
			writeLong(netBuffer, count, si[1]);
			writeLong(netBuffer, count, si[2]);
			break;
	case _GLint4:
#ifdef LOGTOFILE
		print("{ %d, %d, %d, %d } ", si[0], si[1], si[2], si[3]);
#endif
		writeLong(netBuffer, count, si[0]); 
		writeLong(netBuffer, count, si[1]);
		writeLong(netBuffer, count, si[2]);
		writeLong(netBuffer, count, si[3]);
		break;
		
	case _GLuint:
#ifdef LOGTOFILE
		print("%u",  *(unsigned *) v); 
#endif
		writeLong(netBuffer, count, *(unsigned *) v);
		break;
	case _GLuint1:
#ifdef LOGTOFILE
		print("{ %u } ", ui[0]);
#endif
		writeLong(netBuffer, count, ui[0]);
		break;
	case _GLuint2:
#ifdef LOGTOFILE
		print("{ %u, %u } ", ui[0], ui[1]);
#endif
		writeLong(netBuffer, count, ui[0]);
		writeLong(netBuffer, count, ui[1]);
		break;
	case _GLuint3:
#ifdef LOGTOFILE
		print("{ %u, %u, %u } ", ui[0], ui[1], ui[2]);
#endif
		writeLong(netBuffer, count, ui[0]);
		writeLong(netBuffer, count, ui[1]);
		writeLong(netBuffer, count, ui[2]);
		break;
	case _GLuint4:
#ifdef LOGTOFILE
		print("{ %u, %u, %u, %u } ", ui[0], ui[1], ui[2], ui[3]);
#endif
		writeLong(netBuffer, count, ui[0]);
		writeLong(netBuffer, count, ui[1]);
		writeLong(netBuffer, count, ui[2]);
		writeLong(netBuffer, count, ui[3]);
		break;
		
	case _GLclampf:
	case _GLfloat:
#ifdef LOGTOFILE
		print("%f",  *(float *) v);
#endif
		writeFloat(netBuffer, count, *(float *) v);
		break;
	case _GLfloat1:
#ifdef LOGTOFILE
		print(" { %f } ",  f[0]);
#endif
		writeFloat(netBuffer, count, f[0]);
		break;
	case _GLfloat2:
#ifdef LOGTOFILE
		print(" { %f, %f } ", f[0], f[1]);
#endif
		writeFloat(netBuffer, count, f[0]);
		writeFloat(netBuffer, count, f[1]);
		break;
	case _GLfloat3:
#ifdef LOGTOFILE
		print(" { %f, %f, %f } ", f[0], f[1], f[2]);
#endif
		writeFloat(netBuffer, count, f[0]);
		writeFloat(netBuffer, count, f[1]);
		writeFloat(netBuffer, count, f[2]);
		break;
	case _GLfloat4:
#ifdef LOGTOFILE
		print(" { %f, %f, %f, %f } ", f[0], f[1], f[2], f[3]); 
#endif
		writeFloat(netBuffer, count, f[0]);
		writeFloat(netBuffer, count, f[1]);
		writeFloat(netBuffer, count, f[2]);
		writeFloat(netBuffer, count, f[3]);
		break;
	case _GLfloat16:
#ifdef LOGTOFILE
		print("\n");
		print(" { %f, %f, %f, %f } ", f[0], f[4], f[8], f[12]);
		print(" { %f, %f, %f, %f } ", f[1], f[5], f[9], f[13]);
		print(" { %f, %f, %f, %f } ", f[2], f[6], f[10], f[14]);
		print(" { %f, %f, %f, %f } ", f[3], f[7], f[11], f[15]);
#endif
		writeFloat(netBuffer, count, f[0]); writeFloat(netBuffer, count, f[1]); writeFloat(netBuffer, count, f[2]); writeFloat(netBuffer, count, f[3]);
		writeFloat(netBuffer, count, f[4]); writeFloat(netBuffer, count, f[5]); writeFloat(netBuffer, count, f[6]); writeFloat(netBuffer, count, f[7]);
		writeFloat(netBuffer, count, f[8]); writeFloat(netBuffer, count, f[9]); writeFloat(netBuffer, count, f[10]); writeFloat(netBuffer, count, f[11]);
		writeFloat(netBuffer, count, f[12]); writeFloat(netBuffer, count, f[13]); writeFloat(netBuffer, count, f[14]); writeFloat(netBuffer, count, f[15]);
		break;

		
	case _GLboolean:
#ifdef LOGTOFILE
		print("%s",  (*(unsigned char *)v == GL_TRUE) ? "TRUE" : "FALSE");
#endif
		writeByte(netBuffer, count, *(unsigned char *)v);
		break;
	case _GLboolean1:
#ifdef LOGTOFILE
		print(" { %s } ",  (uc[0] == GL_TRUE) ? "TRUE" : "FALSE");
#endif
		writeByte(netBuffer, count, uc[0]);
		break;
	case _GLboolean4:
#ifdef LOGTOFILE
		print(" { %s, %s, %s, %s } ",  (uc[0] == GL_TRUE) ? "TRUE" : "FALSE",
										(uc[1] == GL_TRUE) ? "TRUE" : "FALSE",
										(uc[2] == GL_TRUE) ? "TRUE" : "FALSE",
										(uc[3] == GL_TRUE) ? "TRUE" : "FALSE");
#endif
		writeByte(netBuffer, count, uc[0]);
		writeByte(netBuffer, count, uc[1]);
		writeByte(netBuffer, count, uc[2]);
		writeByte(netBuffer, count, uc[3]);
		break;
		
	/*
	  Print GLenum type from master name list
	*/
		
	case _GLenum:  
		
		i = *(int *) v; 

		writeShort(netBuffer, count, (short)i);

#ifdef LOGTOFILE
		print("%i",i);
		
#endif
		break;
	case _GLbitfield:
		{
			GLboolean first = GL_TRUE;
			
			i = *(int *) v;

			writeShort(netBuffer, count, i);
#ifdef LOGTOFILE
			for (j=0;j<ARY_CNT(GLbitfield_list);j++)
			{
				if ((GLbitfield_list[j].tag&i) != 0)
				{
					if (first == GL_FALSE)
					{
						print(" | ");
					}
					else
					{
						first = GL_FALSE;
					}
					print("%s", GLbitfield_list[j].name);
				}
			}
#endif
		}
		break;
		
#ifdef _WIN32
	case _bufferregionbitfield:
		{
			GLboolean first = GL_TRUE;
			
			i = *(int *) v;

			writeLong(netBuffer, count, i);
#ifdef LOGTOFILE
			for (j=0;j<ARY_CNT(bufferregionbitfield_list);j++)
			{
				if ((bufferregionbitfield_list[j].tag&i) != 0)
				{
					if (first == GL_FALSE)
					{
						print(" | ");
					}
					else
					{
						first = GL_FALSE;
					}
					print("%s", bufferregionbitfield_list[j].name);
				}
			}
#endif
		}
		break;
#endif
	/*
	  (Example: How to dump contents of OpenGL-specific structure)
	*/
		
#ifdef _WIN32
	case PFD:   
		desc = *(PIXELFORMATDESCRIPTOR **) v;
		if(desc != NULL)
		{
			print("\n");
			print("     WORD  nSize = 0x%X\n",desc->nSize); 
			print("     WORD  nVersion = 0x%X\n",desc->nVersion); 
			print("     DWORD dwFlags = "); 
			{
				GLboolean first = GL_TRUE;
				
				for (j=0;j<ARY_CNT(PFD_flags_list);j++)
				{
					if ((PFD_flags_list[j].tag&desc->dwFlags) != 0)
					{
						if (first == GL_FALSE)
						{
							print(" | ");
						}
						else
						{
							first = GL_FALSE;
						}
						print("%s", PFD_flags_list[j].name);
					}
				}
				print("\n");
			}
			print("     BYTE  iPixelType = %s\n", desc->iPixelType==(BYTE)PFD_TYPE_RGBA  ? "PFD_TYPE_RGBA" : "PFD_TYPE_COLORINDEX"); 
			print("     BYTE  cColorBits = %d\n",desc->cColorBits); 
			print("     BYTE  cRedBits = %d\n",desc->cRedBits); 
			print("     BYTE  cRedShift = %d\n",desc->cRedShift); 
			print("     BYTE  cGreenBits = %d\n",desc->cGreenBits); 
			print("     BYTE  cGreenShift = %d\n",desc->cGreenShift); 
			print("     BYTE  cBlueBits = %d\n",desc->cBlueBits); 
			print("     BYTE  cBlueShift = %d\n",desc->cBlueShift); 
			print("     BYTE  cAlphaBits = %d\n",desc->cAlphaBits); 
			print("     BYTE  cAlphaShift = %d\n",desc->cAlphaShift); 
			print("     BYTE  cAccumBits = %d\n",desc->cAccumBits); 
			print("     BYTE  cAccumRedBits = %d\n",desc->cAccumRedBits); 
			print("     BYTE  cAccumGreenBits = %d\n",desc->cAccumGreenBits); 
			print("     BYTE  cAccumBlueBits = %d\n",desc->cAccumBlueBits); 
			print("     BYTE  cAccumAlphaBits = %d\n",desc->cAccumAlphaBits); 
			print("     BYTE  cDepthBits = %d\n",desc->cDepthBits); 
			print("     BYTE  cStencilBits = %d\n",desc->cStencilBits); 
			print("     BYTE  cAuxBuffers = %d\n",desc->cAuxBuffers); 
			print("     BYTE  iLayerType = 0x%X\n",desc->iLayerType); 
			print("     BYTE  bReserved = 0x%X\n",desc->bReserved); 
			print("     DWORD dwLayerMask = 0x%X\n",desc->dwLayerMask); 
			print("     DWORD dwVisibleMask = 0x%X\n",desc->dwVisibleMask); 
			print("     DWORD dwDamageMask = 0x%X\n",desc->dwDamageMask);
		}
		break;

	case LPD:   
		lpd = *(LAYERPLANEDESCRIPTOR **) v;
		if(lpd != NULL)
		{
			print("\n");
			print("     WORD  nSize = 0x%X\n",lpd->nSize); 
			print("     WORD  nVersion = 0x%X\n",lpd->nVersion); 
			print("     DWORD dwFlags = "); 
			{
				GLboolean first = GL_TRUE;
				
				for (j=0;j<ARY_CNT(LPD_flags_list);j++)
				{
					if ((LPD_flags_list[j].tag&lpd->dwFlags) != 0)
					{
						if (first == GL_FALSE)
						{
							print(" | ");
						}
						else
						{
							first = GL_FALSE;
						}
						print("%s", LPD_flags_list[j].name);
					}
				}
				print("\n");
			}
			print("     BYTE  iPixelType = %s\n", lpd->iPixelType==(BYTE)LPD_TYPE_RGBA  ? "LPD_TYPE_RGBA" : "LPD_TYPE_COLORINDEX"); 
			print("     BYTE  cColorBits = %d\n",lpd->cColorBits); 
			print("     BYTE  cRedBits = %d\n",lpd->cRedBits); 
			print("     BYTE  cRedShift = %d\n",lpd->cRedShift); 
			print("     BYTE  cGreenBits = %d\n",lpd->cGreenBits); 
			print("     BYTE  cGreenShift = %d\n",lpd->cGreenShift); 
			print("     BYTE  cBlueBits = %d\n",lpd->cBlueBits); 
			print("     BYTE  cBlueShift = %d\n",lpd->cBlueShift); 
			print("     BYTE  cAlphaBits = %d\n",lpd->cAlphaBits); 
			print("     BYTE  cAlphaShift = %d\n",lpd->cAlphaShift); 
			print("     BYTE  cAccumBits = %d\n",lpd->cAccumBits); 
			print("     BYTE  cAccumRedBits = %d\n",lpd->cAccumRedBits); 
			print("     BYTE  cAccumGreenBits = %d\n",lpd->cAccumGreenBits); 
			print("     BYTE  cAccumBlueBits = %d\n",lpd->cAccumBlueBits); 
			print("     BYTE  cAccumAlphaBits = %d\n",lpd->cAccumAlphaBits); 
			print("     BYTE  cDepthBits = %d\n",lpd->cDepthBits); 
			print("     BYTE  cStencilBits = %d\n",lpd->cStencilBits); 
			print("     BYTE  cAuxBuffers = %d\n",lpd->cAuxBuffers); 
			print("     BYTE  iLayerPlane = 0x%X\n",lpd->iLayerPlane); 
			print("     BYTE  bReserved = 0x%X\n",lpd->bReserved); 
			print("     COLORREF crTransparent = 0x%X\n",lpd->crTransparent); 
		}
		break;
#else
	case GLX_return:
	  if (*v >= 0 && *v < ARY_CNT(GLX_return_codes))
		print(GLX_return_codes[*v]);
	  else
		print("%u", *v);
	  break;

	case GLX_attrib:
	  for (i=0;i<ARY_CNT(GLX_attribs);i++)
		if (GLX_attribs[i].tag == *v)
		  {
			print(GLX_attribs[i].name);
			goto found;
		  }
	  print("%u", *v);
	found:
	  break;

	case GLX_attrib_value_list:
	  attrib_list = *(int **) value;
	  print("{");
	  if (attrib_list) {
		for (attrib = attrib_list; *attrib; attrib++) {
		  print_value(GLX_attrib, attrib);
		  print(",");
		  print("%u,", *++attrib);
		}
		print("None");
	  }
	  print("}");
	  break;

	case GLX_visual_info:
	  xvi = *(XVisualInfo **) v;
	  print("0x%X\n", xvi);
	  print("    Visual       *visual =        0x%X\n", xvi->visual); 
	  print("    VisualID      visualid =      0x%X\n", xvi->visualid); 
	  print("    int           screen =        %i\n", xvi->screen);
	  print("    unsigned int  depth =         %u\n", xvi->depth);
	  print("    int           class =         %s\n", GLX_visual_classes[xvi->class]);
	  print("    unsigned long red_mask =      0x%08X\n", xvi->red_mask);
	  print("    unsigned long green_mask =    0x%08X\n", xvi->green_mask);
	  print("    unsigned long blue_mask =     0x%08X\n", xvi->blue_mask);
	  print("    int           colormap_size = %i\n", xvi->colormap_size);
	  print("    int           bits_per_rgb =  %i\n", xvi->bits_per_rgb);
	  break;
#endif
    }
}

/* Print timestamp */

void write_time(void)
{
    struct timeb tb;
	int    t,h,m,s,c;

    ftime(&tb);
	t = (tb.time * 1000) + tb.millitm - (starttime.time * 1000) - starttime.millitm;
	
	h = t/3600000L;
	m = (t/60000L)%60L;
	s = (t/1000L)%60L;
	c = t%1000L;
	
	print("[%.02d:%.02d:%.02d.%.03d] ",h,m,s,c);

    s = tb.time - starttime.time;
	
	h = s/360L;
	m = (s/60L)%60L;
	s = s%60L;
	c = (tb.millitm + 1000 - starttime.millitm)%1000L;
	
	print("[%.02d:%.02d:%.02d.%.03d] ",h,m,s,c);
}

/* Initialise call counters to zero */
void InitCallCount(void)
{
	int i;

	if (bCountCalls == GL_FALSE)
		return;

	for (i=0; i<FP_LAST; i++)
		nCallCount[i]  = 0;
	extInitCallCount();
}

extern char *GLN[];
extern char *GLN_EXT[];

/* Print call count tables */
void PrintCallCounts(void)
{
	int i;

	if (bCountCalls == GL_FALSE)
		return;

	print("\nCall Counts:\n");

	/* NOTE: Do not print functions that were not called */

	for (i=0; i<FP_LAST; i++) {
		if (nCallCount[i] > 0)
			print(fmt, GLN[i], nCallCount[i]);
	}
	print("\n");

	extPrintCallCounts();

	print("\n");

}

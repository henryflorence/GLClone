#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nl.h"
#include "glut.h"
#include <gl/glu.h>

#if defined WIN32 || defined WIN64
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define sleep(x)    Sleep(1000 * (x))
#endif

//#define readShort(x, y, z)      {z = nlSwaps(*(NLushort *)((NLbyte *)&x[y])); y += 2;}
//#define readLong(x, y, z)       {z = nlSwapl(*(NLulong  *)((NLbyte *)&x[y])); y += 4;}
//#define readFloat(x, y, z)      {z = nlSwapf(*(NLfloat  *)((NLbyte *)&x[y])); y += 4;}
//#define readDouble(x, y, z)     {z = nlSwapd(*(NLdouble *)((NLbyte *)&x[y])); y += 8;}
/* MACRO NAMEs to be completed*/
//used 21 times
#define readByte(x,y,z)		{z = *(NLbyte *)((NLbyte *)&x[y]);y += 1; } //print("%i, ",z);}
//used 6 times
#define readByteV(x,y,z,n)		{z = (NLbyte *)((NLbyte *)&x[y]);y += n;}
//used 81 times
#define readDouble(x,y,z)		{z = *(NLdouble *)((NLbyte *)&x[y]);y += 8; }// print("%f, ",z);}
//used 28 times
#define readDoubleV(x,y,z,n)		{z = (NLdouble *)((NLbyte *)&x[y]);y += 8*n; }// for(int hen = 0; hen < n; hen++) print("%f, ",z[hen]); }
//used 103 times
#define readFloat(x,y,z)		{z = *(NLfloat  *)((NLbyte *)&x[y]);y += 4; }// print("%f, ",z);}
//used 41 times
#define readFloatV(x,y,z,n)		{z = (NLfloat  *)((NLbyte *)&x[y]);y += 4*n; }// for(int hen = 0; hen < n; hen++) print("%f, ",z[hen]);}
//used 179 times
#define readInt(x,y,z)		{z = *(NLushort *)((NLbyte *)&x[y]);y += 2; }//print("%i, ",z);}
//used 56 times
#define readIntV(x,y,z,n)		{z = (NLushort *)((NLbyte *)&x[y]);y += 2*n; }//for(int hen = 0; hen < n; hen++) print("%i, ",z[hen]);}
//used 47 times
#define readLong(x,y,z)		{z = *(NLulong  *)((NLbyte *)&x[y]);y += 4; }//print("%i, ",z);}
//used 1 times
#define readLongV(x,y,z,n)		{z = (NLulong  *)((NLbyte *)&x[y]);y += 4*n;}
//used 243 times
#define readShort(x,y,z)		{z = *(NLushort *)((NLbyte *)&x[y]);y += 2; }//print("%i, ",z);}

FILE *text_file = NULL; 

void parseFunctions();
void print(const char *fmt, ...);
char *GLN[] = 
{
	"glAccum",
	"glAlphaFunc",
	"glAreTexturesResident",
	"glArrayElement",
	"glBegin",
	"glBindTexture",
	"glBitmap",
	"glBlendFunc",
	"glCallList",
	"glCallLists",
	"glClear",
	"glClearAccum",
	"glClearColor",
	"glClearDepth",
	"glClearIndex",
	"glClearStencil",
	"glClipPlane",
	"glColor3b",
	"glColor3bv",
	"glColor3d",
	"glColor3dv",
	"glColor3f",
	"glColor3fv",
	"glColor3i",
	"glColor3iv",
	"glColor3s",
	"glColor3sv",
	"glColor3ub",
	"glColor3ubv",
	"glColor3ui",
	"glColor3uiv",
	"glColor3us",
	"glColor3usv",
	"glColor4b",
	"glColor4bv",
	"glColor4d",
	"glColor4dv",
	"glColor4f",
	"glColor4fv",
	"glColor4i",
	"glColor4iv",
	"glColor4s",
	"glColor4sv",
	"glColor4ub",
	"glColor4ubv",
	"glColor4ui",
	"glColor4uiv",
	"glColor4us",
	"glColor4usv",
	"glColorMask",
	"glColorMaterial",
	"glColorPointer",
	"glCopyPixels",
	"glCopyTexImage1D",
	"glCopyTexImage2D",
	"glCopyTexSubImage1D",
	"glCopyTexSubImage2D",
	"glCullFace",
	"glDeleteLists",
	"glDeleteTextures",
	"glDepthFunc",
	"glDepthMask",
	"glDepthRange",
	"glDisable",
	"glDisableClientState",
	"glDrawArrays",
	"glDrawBuffer",
	"glDrawElements",
	"glDrawPixels",
	"glEdgeFlag",
	"glEdgeFlagPointer",
	"glEdgeFlagv",
	"glEnable",
	"glEnableClientState",
	"glEnd",
	"glEndList",
	"glEvalCoord1d",
	"glEvalCoord1dv",
	"glEvalCoord1f",
	"glEvalCoord1fv",
	"glEvalCoord2d",
	"glEvalCoord2dv",
	"glEvalCoord2f",
	"glEvalCoord2fv",
	"glEvalMesh1",
	"glEvalMesh2",
	"glEvalPoint1",
	"glEvalPoint2",
	"glFeedbackBuffer",
	"glFinish",
	"glFlush",
	"glFogf",
	"glFogfv",
	"glFogi",
	"glFogiv",
	"glFrontFace",
	"glFrustum",
	"glGenLists",
	"glGenTextures",
	"glGetBooleanv",
	"glGetClipPlane",
	"glGetDoublev",
	"glGetError",
	"glGetFloatv",
	"glGetIntegerv",
	"glGetLightfv",
	"glGetLightiv",
	"glGetMapdv",
	"glGetMapfv",
	"glGetMapiv",
	"glGetMaterialfv",
	"glGetMaterialiv",
	"glGetPixelMapfv",
	"glGetPixelMapuiv",
	"glGetPixelMapusv",
	"glGetPointerv",
	"glGetPolygonStipple",
	"glGetString",
	"glGetTexEnvfv",
	"glGetTexEnviv",
	"glGetTexGendv",
	"glGetTexGenfv",
	"glGetTexGeniv",
	"glGetTexImage",
	"glGetTexLevelParameterfv",
	"glGetTexLevelParameteriv",
	"glGetTexParameterfv",
	"glGetTexParameteriv",
	"glHint",
	"glIndexMask",
	"glIndexPointer",
	"glIndexd",
	"glIndexdv",
	"glIndexf",
	"glIndexfv",
	"glIndexi",
	"glIndexiv",
	"glIndexs",
	"glIndexsv",
	"glIndexub",
	"glIndexubv",
	"glInitNames",
	"glInterleavedArrays",
	"glIsEnabled",
	"glIsList",
	"glIsTexture",
	"glLightModelf",
	"glLightModelfv",
	"glLightModeli",
	"glLightModeliv",
	"glLightf",
	"glLightfv",
	"glLighti",
	"glLightiv",
	"glLineStipple",
	"glLineWidth",
	"glListBase",
	"glLoadIdentity",
	"glLoadMatrixd",
	"glLoadMatrixf",
	"glLoadName",
	"glLogicOp",
	"glMap1d",
	"glMap1f",
	"glMap2d",
	"glMap2f",
	"glMapGrid1d",
	"glMapGrid1f",
	"glMapGrid2d",
	"glMapGrid2f",
	"glMaterialf",
	"glMaterialfv",
	"glMateriali",
	"glMaterialiv",
	"glMatrixMode",
	"glMultMatrixd",
	"glMultMatrixf",
	"glNewList",
	"glNormal3b",
	"glNormal3bv",
	"glNormal3d",
	"glNormal3dv",
	"glNormal3f",
	"glNormal3fv",
	"glNormal3i",
	"glNormal3iv",
	"glNormal3s",
	"glNormal3sv",
	"glNormalPointer",
	"glOrtho",
	"glPassThrough",
	"glPixelMapfv",
	"glPixelMapuiv",
	"glPixelMapusv",
	"glPixelStoref",
	"glPixelStorei",
	"glPixelTransferf",
	"glPixelTransferi",
	"glPixelZoom",
	"glPointSize",
	"glPolygonMode",
	"glPolygonOffset",
	"glPolygonStipple",
	"glPopAttrib",
	"glPopClientAttrib",
	"glPopMatrix",
	"glPopName",
	"glPrioritizeTextures",
	"glPushAttrib",
	"glPushClientAttrib",
	"glPushMatrix",
	"glPushName",
	"glRasterPos2d",
	"glRasterPos2dv",
	"glRasterPos2f",
	"glRasterPos2fv",
	"glRasterPos2i",
	"glRasterPos2iv",
	"glRasterPos2s",
	"glRasterPos2sv",
	"glRasterPos3d",
	"glRasterPos3dv",
	"glRasterPos3f",
	"glRasterPos3fv",
	"glRasterPos3i",
	"glRasterPos3iv",
	"glRasterPos3s",
	"glRasterPos3sv",
	"glRasterPos4d",
	"glRasterPos4dv",
	"glRasterPos4f",
	"glRasterPos4fv",
	"glRasterPos4i",
	"glRasterPos4iv",
	"glRasterPos4s",
	"glRasterPos4sv",
	"glReadBuffer",
	"glReadPixels",
	"glRectd",
	"glRectdv",
	"glRectf",
	"glRectfv",
	"glRecti",
	"glRectiv",
	"glRects",
	"glRectsv",
	"glRenderMode",
	"glRotated",
	"glRotatef",
	"glScaled",
	"glScalef",
	"glScissor",
	"glSelectBuffer",
	"glShadeModel",
	"glStencilFunc",
	"glStencilMask",
	"glStencilOp",
	"glTexCoord1d",
	"glTexCoord1dv",
	"glTexCoord1f",
	"glTexCoord1fv",
	"glTexCoord1i",
	"glTexCoord1iv",
	"glTexCoord1s",
	"glTexCoord1sv",
	"glTexCoord2d",
	"glTexCoord2dv",
	"glTexCoord2f",
	"glTexCoord2fv",
	"glTexCoord2i",
	"glTexCoord2iv",
	"glTexCoord2s",
	"glTexCoord2sv",
	"glTexCoord3d",
	"glTexCoord3dv",
	"glTexCoord3f",
	"glTexCoord3fv",
	"glTexCoord3i",
	"glTexCoord3iv",
	"glTexCoord3s",
	"glTexCoord3sv",
	"glTexCoord4d",
	"glTexCoord4dv",
	"glTexCoord4f",
	"glTexCoord4fv",
	"glTexCoord4i",
	"glTexCoord4iv",
	"glTexCoord4s",
	"glTexCoord4sv",
	"glTexCoordPointer",
	"glTexEnvf",
	"glTexEnvfv",
	"glTexEnvi",
	"glTexEnviv",
	"glTexGend",
	"glTexGendv",
	"glTexGenf",
	"glTexGenfv",
	"glTexGeni",
	"glTexGeniv",
	"glTexImage1D",
	"glTexImage2D",
	"glTexParameterf",
	"glTexParameterfv",
	"glTexParameteri",
	"glTexParameteriv",
	"glTexSubImage1D",
	"glTexSubImage2D",
	"glTranslated",
	"glTranslatef",
	"glVertex2d",
	"glVertex2dv",
	"glVertex2f",
	"glVertex2fv",
	"glVertex2i",
	"glVertex2iv",
	"glVertex2s",
	"glVertex2sv",
	"glVertex3d",
	"glVertex3dv",
	"glVertex3f",
	"glVertex3fv",
	"glVertex3i",
	"glVertex3iv",
	"glVertex3s",
	"glVertex3sv",
	"glVertex4d",
	"glVertex4dv",
	"glVertex4f",
	"glVertex4fv",
	"glVertex4i",
	"glVertex4iv",
	"glVertex4s",
	"glVertex4sv",
	"glVertexPointer",
	"glViewport",
#if defined WIN32 || defined WIN64
	"wglChoosePixelFormat",
	"wglCopyContext",
	"wglCreateContext",
	"wglCreateLayerContext",
	"wglDeleteContext",
	"wglDescribeLayerPlane",
	"wglDescribePixelFormat",
	"wglGetCurrentContext",
	"wglGetCurrentDC",
	"wglGetDefaultProcAddress",
	"wglGetLayerPaletteEntries",
	"wglGetPixelFormat",
	"wglGetProcAddress",
	"wglMakeCurrent",
	"wglRealizeLayerPalette",
	"wglSetLayerPaletteEntries",
	"wglSetPixelFormat",
	"wglShareLists",
	"wglSwapBuffers",
	"wglSwapLayerBuffers",
	"wglUseFontBitmapsA",
	"wglUseFontBitmapsW",
	"wglUseFontOutlinesA",
	"wglUseFontOutlinesW",
#else
    "glXChooseVisual",
    "glXCopyContext",
    "glXCreateContext",
    "glXCreateGLXPixmap",
    "glXDestroyContext",
    "glXDestroyGLXPixmap",
    "glXGetConfig",
    "glXGetCurrentContext",
    "glXGetCurrentDrawable",
    "glXIsDirect",
    "glXMakeCurrent",
    "glXQueryExtension",
    "glXQueryVersion",
    "glXSwapBuffers",
    "glXUseXFont",
    "glXWaitGL",
    "glXWaitX",
    "glXGetClientString",
    "glXQueryServerString",
    "glXQueryExtensionsString",
    "glXGetFBConfigs",
    "glXChooseFBConfig",
    "glXGetFBConfigAttrib",
    "glXGetVisualFromFBConfig",
    "glXCreateWindow",
    "glXDestroyWindow",
    "glXCreatePixmap",
    "glXDestroyPixmap",
    "glXCreatePbuffer",
    "glXDestroyPbuffer",
    "glXQueryDrawable",
    "glXCreateNewContext",
    "glXMakeContextCurrent",
    "glXGetCurrentReadDrawable",
    "glXGetCurrentDisplay",
    "glXQueryContext",
    "glXSelectEvent",
    "glXGetSelectedEvent",
    "glXGetContextIDEXT",
    "glXGetCurrentDrawableEXT",
    "glXImportContextEXT",
    "glXFreeContextEXT",
    "glXQueryContextInfoEXT",
    "glXGetProcAddressARB",
#endif
};

#define MAX_CLIENTS 10
NLsocket    serversock;
NLsocket    client;
NLint       clientnum = 0;
NLbyte		buffer[NL_MAX_PACKET_LENGTH];
bool		data = false;

void printErrorExit(void)
{
    NLenum err = nlGetError();

    if(err == NL_SYSTEM_ERROR)
    {
        printf("System error: %s\n", nlGetSystemErrorStr(nlGetSystemError()));
    }
    else
    {
        printf("HawkNL error: %s\n", nlGetErrorStr(err));
    }
    nlShutdown();
    exit(1);
}

void mainServerLoop()
{
    NLchar      string[NL_MAX_STRING_LENGTH];

    //memset(client, 0, sizeof(client));

    NLint   i;

    /* check for a new client */
    NLsocket newsock = nlAcceptConnection(serversock);

    if(newsock != NL_INVALID)
    {
        NLaddress   addr;

        nlGetRemoteAddr(newsock, &addr);
        client = newsock;
        printf("Client %d connected from %s\n", clientnum, nlAddrToString(&addr, string));
        clientnum++;
    }
    else
    {
        if(nlGetError() == NL_SYSTEM_ERROR)
        {
            printErrorExit();
        }
    }
	/*if (clientnum > 0) {
		NLaddress   addr;
		nlGetRemoteAddr(client[clientnum-1], &addr);
		print("Client %d still connected from %s\n", clientnum, nlAddrToString(&addr, string));
	}*/

    /* loop through the clients and read the packets */
    if(nlRead(client, buffer, sizeof(buffer)) > 0)
    {
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //buffer[NL_MAX_PACKET_LENGTH-1] = 0; /* null terminate the char string */
		//glutPostRedisplay();
		parseFunctions();
		//glutSwapBuffers();
	} //else print("nothing to read\n");
}
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
void parseFunctions()
{
	data = false;
	NLushort index;
	NLuint count = 0;
/*DECLARATIONS CREATED BY PERL SCRIPT*/
	GLvoid* pixels;
	HDC a;
	NLbyte* bitmap;
	NLbyte* v_ByteV;
	NLdouble bottom;
	NLdouble left;
	NLdouble right;
	NLdouble top;
	NLdouble x_Double;
	NLdouble y_Double;
	NLdouble zFar;
	NLdouble zFar_Double;
	NLdouble zNear;
	NLdouble zNear_Double;
	NLdouble z_Double;
	NLdouble* m_DoubleV;
	NLfloat alpha;
	NLfloat alpha_Float;
	NLfloat angle;
	NLfloat blue;
	NLfloat blue_Float;
	NLfloat green;
	NLfloat green_Float;
	NLfloat param;
	NLfloat red;
	NLfloat red_Float;
	NLfloat ref;
	NLfloat s;
	NLfloat size;
	NLfloat t;
	NLfloat x;
	NLfloat xmove;
	NLfloat xorig;
	NLfloat y;
	NLfloat ymove;
	NLfloat yorig;
	NLfloat z;
	NLfloat* m;
	NLfloat* params;
	NLfloat* v;
	NLulong border;
	NLulong height;
	NLulong internalformat;
	NLulong level;
	NLulong n;
	NLulong width;
	NLulong x_Long;
	NLulong xoffset;
	NLulong y_Long;
	NLulong yoffset;
	NLushort cap;
	NLushort dfactor;
	NLushort face;
	NLushort flag;
	NLushort format;
	NLushort func;
	NLushort mask;
	NLushort mode;
	NLushort name;
	NLushort pname;
	NLushort sfactor;
	NLushort target;
	NLushort texture;
	NLushort type;
	NLushort* textures;
	NLuint packetCount;

	readInt(buffer,count,index);
	while (count < NL_MAX_PACKET_LENGTH) {
		if(index < 400) print("%s (",GLN[index]);

/*SWITCH CREATED BY PERL SCRIPT*/
		switch(index) {
			case 1: //glAlphaFunc
				readShort(buffer,count,func);
				readFloat(buffer,count,ref);
				print("%i, %f",(short) func, (float) ref);
				glAlphaFunc((GLenum) func,(GLclampf) ref);
			break;
			case 4: //glBegin
				readShort(buffer,count,mode);
				print("%i",(short) mode);
				glBegin((GLenum) mode);
			break;
			case 5: //glBindTexture
				readShort(buffer,count,target);
				readLong(buffer,count,texture);
				//print("%i",(short)target);
				print("%i, %i",(short) target, (GLuint) texture);
				glBindTexture((GLenum) target,(GLuint) texture);
			break;
			case 6: //glBitmap
				readLong(buffer,count,width);
				readLong(buffer,count,height);
				readFloat(buffer,count,xorig);
				readFloat(buffer,count,yorig);
				readFloat(buffer,count,xmove);
				readFloat(buffer,count,ymove);
				print("%i, %i, %f, %f, %f, %f, (bytes)",(long) width, (long) height, (float) xorig, (float) yorig, (float) xmove, (float) ymove);
				if(width * height == 0) {
					print("-----size error------");
					break;
				}
				bitmap = &(buffer[count]);
				count += width * height;
				glBitmap((GLsizei) width,(GLsizei) height,(GLfloat) xorig,(GLfloat) yorig,(GLfloat) xmove,(GLfloat) ymove,(GLubyte*) bitmap);
			break;
			case 7: //glBlendFunc
				readShort(buffer,count,sfactor);
				readShort(buffer,count,dfactor);
				print("%i, %i",(short) sfactor, (short) dfactor);
				glBlendFunc((GLenum) sfactor,(GLenum) dfactor);
			break;
			case 10: //glClear
				readInt(buffer,count,mask);
				print("%i",(int) mask);
				glClear((GLbitfield) mask);
			break;
			case 12: //glClearColor
				readFloat(buffer,count,red_Float);
				readFloat(buffer,count,green_Float);
				readFloat(buffer,count,blue_Float);
				readFloat(buffer,count,alpha_Float);
				print("%f, %f, %f, %f",(float) red_Float, (float) green_Float, (float) blue_Float, (float) alpha_Float);
				glClearColor((GLclampf) red_Float,(GLclampf) green_Float,(GLclampf) blue_Float,(GLclampf) alpha_Float);
			break;
			case 21: //glColor3f
				readFloat(buffer,count,red);
				readFloat(buffer,count,green);
				readFloat(buffer,count,blue);
				print("%f, %f, %f",(float) red, (float) green, (float) blue);
				glColor3f((GLfloat) red,(GLfloat) green,(GLfloat) blue);
			break;
			case 37: //glColor4f
				readFloat(buffer,count,red);
				readFloat(buffer,count,green);
				readFloat(buffer,count,blue);
				readFloat(buffer,count,alpha);
				print("%f, %f, %f, %f",(float) red, (float) green, (float) blue, (float) alpha);
				glColor4f((GLfloat) red,(GLfloat) green,(GLfloat) blue,(GLfloat) alpha);
			break;
			case 38: //glColor4fv
				readFloatV(buffer,count,v,4);
				print("%f, %f, %f, %f",(float) v[0],(float) v[1],(float) v[2],(float) v[3]);
				glColor4fv((GLfloat*) v);
			break;
			case 44: //glColor4ubv
				readByteV(buffer,count,v_ByteV,4);
				//print("%i",*(byte) v_ByteV);
				glColor4ubv((GLubyte*) v_ByteV);
			break;
			case 57: //glCullFace
				readShort(buffer,count,mode);
				print("%i",(short) mode);
				glCullFace((GLenum) mode);
			break;
			case 59: //glDeleteTextures
				readLong(buffer,count,n);
				readIntV(buffer,count,textures,1);
				//print("%i, %i",(long) n, *(int*) textures);
				//glDeleteTextures((GLsizei) n,(GLuint*) textures);
			break;
			case 60: //glDepthFunc
				readShort(buffer,count,func);
				print("%i",(short) func);
				glDepthFunc((GLenum) func);
			break;
			case 61: //glDepthMask
				readShort(buffer,count,flag);
				print("%i",(short) flag);
				glDepthMask((GLboolean) flag);
			break;
			case 62: //glDepthRange
				readDouble(buffer,count,zNear_Double);
				readDouble(buffer,count,zFar_Double);
				print("%f, %f",(double) zNear_Double, (double) zFar_Double);
				glDepthRange((GLclampd) zNear_Double,(GLclampd) zFar_Double);
			break;
			case 63: //glDisable
				readShort(buffer,count,cap);
				print("%i",(short) cap);
				glDisable((GLenum) cap);
			break;
			case 66: //glDrawBuffer
				readShort(buffer,count,mode);
				print("%i",(short) mode);
				glDrawBuffer((GLenum) mode);
			break;
			case 72: //glEnable
				readShort(buffer,count,cap);
				print("%i",(short) cap);
				glEnable((GLenum) cap);
			break;
			case 74: //glEnd
				glEnd();
			break;
			case 96: //glFrustum
				readDouble(buffer,count,left);
				readDouble(buffer,count,right);
				readDouble(buffer,count,bottom);
				readDouble(buffer,count,top);
				readDouble(buffer,count,zNear);
				readDouble(buffer,count,zFar);
				print("%f, %f, %f, %f, %f, %f",(double) left, (double) right, (double) bottom, (double) top, (double) zNear, (double) zFar);
				glFrustum((GLdouble) left,(GLdouble) right,(GLdouble) bottom,(GLdouble) top,(GLdouble) zNear,(GLdouble) zFar);
			break;
			case 102: //glGetError
				glGetError();
			break;
			case 103: //glGetFloatv
				readShort(buffer,count,pname);
				readFloatV(buffer,count,params,1);
				//print("%i, %f",(short) pname, *(float) params);
				glGetFloatv((GLenum) pname,(GLfloat*) params);
			break;
			case 117: //glGetString
				readShort(buffer,count,name);
				print("%i",(short) name);
				glGetString((GLenum) name);
			break;
			case 157: //glLoadIdentity
				glLoadIdentity();
			break;
			case 159: //glLoadMatrixf
				readFloatV(buffer,count,m,16);
				//print("%f",*(float) m);
				glLoadMatrixf((GLfloat*) m);
			break;
			case 174: //glMatrixMode
				readShort(buffer,count,mode);
				print("%i",(short) mode);
				glMatrixMode((GLenum) mode);
			break;
			case 175: //glMultMatrixd
				readDoubleV(buffer,count,m_DoubleV,16);
				//print("%f",*(double) m_DoubleV);
				glMultMatrixd((GLdouble*) m_DoubleV);
			break;
			case 176: //glMultMatrixf
				readFloatV(buffer,count,m,16);
				//print("%f",*(float) m);
				glMultMatrixf((GLfloat*) m);
			break;
			case 183: //glNormal3fv
				readFloatV(buffer,count,v,3);
				//print("%f",*(float) v);
				glNormal3fv((GLfloat*) v);
			break;
			case 189: //glOrtho
				readDouble(buffer,count,left);
				readDouble(buffer,count,right);
				readDouble(buffer,count,bottom);
				readDouble(buffer,count,top);
				readDouble(buffer,count,zNear);
				readDouble(buffer,count,zFar);
				print("%f, %f, %f, %f, %f, %f",(double) left, (double) right, (double) bottom, (double) top, (double) zNear, (double) zFar);
				glOrtho((GLdouble) left,(GLdouble) right,(GLdouble) bottom,(GLdouble) top,(GLdouble) zNear,(GLdouble) zFar);
			break;
			case 199: //glPointSize
				readFloat(buffer,count,size);
				print("%f",(float) size);
				glPointSize((GLfloat) size);
			break;
			case 200: //glPolygonMode
				readShort(buffer,count,face);
				readShort(buffer,count,mode);
				print("%i, %i",(short) face, (short) mode);
				glPolygonMode((GLenum) face,(GLenum) mode);
			break;
			case 205: //glPopMatrix
				glPopMatrix();
			break;
			case 210: //glPushMatrix
				glPushMatrix();
			break;
			case 248: //glRotatef
				readFloat(buffer,count,angle);
				readFloat(buffer,count,x);
				readFloat(buffer,count,y);
				readFloat(buffer,count,z);
				print("%f, %f, %f, %f",(float) angle, (float) x, (float) y, (float) z);
				glRotatef((GLfloat) angle,(GLfloat) x,(GLfloat) y,(GLfloat) z);
			break;
			case 250: //glScalef
				readFloat(buffer,count,x);
				readFloat(buffer,count,y);
				readFloat(buffer,count,z);
				print("%f, %f, %f",(float) x, (float) y, (float) z);
				glScalef((GLfloat) x,(GLfloat) y,(GLfloat) z);
			break;
			case 253: //glShadeModel
				readShort(buffer,count,mode);
				print("%i",(short) mode);
				glShadeModel((GLenum) mode);
			break;
			case 267: //glTexCoord2f
				readFloat(buffer,count,s);
				readFloat(buffer,count,t);
				print("%f, %f",(float) s, (float) t);
				glTexCoord2f((GLfloat) s,(GLfloat) t);
			break;
			case 290: //glTexEnvf
				readShort(buffer,count,target);
				readShort(buffer,count,pname);
				readFloat(buffer,count,param);
				print("%i, %i, %f",(short) target, (short) pname, (float) param);
				glTexEnvf((GLenum) target,(GLenum) pname,(GLfloat) param);
			break;
			case 301: //glTexImage2D
				readShort(buffer,count,target);
				readLong(buffer,count,level);
				readLong(buffer,count,internalformat);
				readLong(buffer,count,width);
				readLong(buffer,count,height);
				readLong(buffer,count,border);
				readShort(buffer,count,format);
				readShort(buffer,count,type);
				// unknown type for: pixels
				pixels = &(buffer[count]);
				count += width * height;
				print("%i, %i, %i, %i, %i, %i, %i, %i, ",(short) target, (long) level, (long) internalformat, (long) width, (long) height, (long) border, (short) format, (short) type );
				glTexImage2D((GLenum) target,(GLint) level,(GLint) internalformat,(GLsizei) width,(GLsizei) height,(GLint) border,(GLenum) format,(GLenum) type,(GLvoid*) pixels);
			break;
			case 302: //glTexParameterf
				readShort(buffer,count,target);
				readShort(buffer,count,pname);
				switch (pname) {
					case GL_TEXTURE_PRIORITY:
						readFloat(buffer,count,param);
						print("%i, %i, %f",(short) target, (short) pname, (float) param);
						glTexParameterf((GLenum) target,(GLenum) pname,(GLfloat) param);
						break;
					default:
						readShort(buffer,count,param);
						print("%i, %i, %i",(short) target, (short) pname, (short) param);
						glTexParameterf((GLenum) target,(GLenum) pname,(GLshort) param);
					break;
				}
			break;
			case 307: //glTexSubImage2D
				readShort(buffer,count,target);
				readLong(buffer,count,level);
				readLong(buffer,count,xoffset);
				readLong(buffer,count,yoffset);
				readLong(buffer,count,width);
				readLong(buffer,count,height);
				readShort(buffer,count,format);
				readShort(buffer,count,type);
				// unknown type for: pixels
				pixels = &buffer[count];
				count += width * height;
				print("%i, %i, %i, %i, %i, %i, %i, %i, ",(short) target, (long) level, (long) xoffset, (long) yoffset, (long) width, (long) height, (short) format, (short) type );
				glTexSubImage2D((GLenum) target,(GLint) level,(GLint) xoffset,(GLint) yoffset,(GLsizei) width,(GLsizei) height,(GLenum) format,(GLenum) type,(GLvoid*) pixels);
			break;
			case 308: //glTranslated
				readDouble(buffer,count,x_Double);
				readDouble(buffer,count,y_Double);
				readDouble(buffer,count,z_Double);
				print("%f, %f, %f",(double) x_Double, (double) y_Double, (double) z_Double);
				glTranslated((GLdouble) x_Double,(GLdouble) y_Double,(GLdouble) z_Double);
			break;
			case 309: //glTranslatef
				readFloat(buffer,count,x);
				readFloat(buffer,count,y);
				readFloat(buffer,count,z);
				print("%f, %f, %f",(float) x, (float) y, (float) z);
				glTranslatef((GLfloat) x,(GLfloat) y,(GLfloat) z);
			break;
			case 312: //glVertex2f
				readFloat(buffer,count,x);
				readFloat(buffer,count,y);
				print("%f, %f",(float) x, (float) y);
				glVertex2f((GLfloat) x,(GLfloat) y);
			break;
			case 321: //glVertex3fv
				readFloatV(buffer,count,v,3);
				print("%f, %f, %f",*(float*) v,(float) v[1],(float) v[2]);
				glVertex3fv((GLfloat*) v);
			break;
			case 335: //glViewport
				readLong(buffer,count,x_Long);
				readLong(buffer,count,y_Long);
				readLong(buffer,count,width);
				readLong(buffer,count,height);
				print("%i, %i, %i, %i",(long) x_Long, (long) y_Long, (long) width, (long) height);
				glViewport((GLint) x_Long,(GLint) y_Long,(GLsizei) width,(GLsizei) height);
			break;
			case 354: //wglSwapBuffers
				// unknown type for: a
				glutSwapBuffers();
			break;
			case 400:
				print("------Start Packet-----\n");
				readShort(buffer,count,packetCount);
				print("packet no: %i\n",packetCount);
			break;
			case 500:
				print("----End Packet-----\n");
				readShort(buffer,count,packetCount);
				print("packet no: %i\n",packetCount);
			return;
		}
/* FINISHED CREATION (<6 days)*/

		if(index < 400) print(") %i\n",count);
		readShort(buffer,count,index);
	}
	
}
void
display(void)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//mainServerLoop();
	//glutSwapBuffers();
}
int main(int argc, char **argv)
{
    NLboolean   isserver = NL_FALSE;
    NLsocket    clientsock;
    NLaddress   addr;
    NLenum      type = NL_RELIABLE; /* Change this to NL_RELIABLE for reliable connection */
    NLchar      string[NL_MAX_STRING_LENGTH];

	//strcpy(server, "127.0.0.1:25000");
    if(!nlInit())
        printErrorExit();


    print("nlGetString(NL_VERSION) = %s\n\n", nlGetString(NL_VERSION));
    print("nlGetString(NL_NETWORK_TYPES) = %s\n\n", nlGetString(NL_NETWORK_TYPES));

	text_file = fopen("output.txt", "w");

    if(!nlSelectNetwork(NL_IP))
        printErrorExit();

    /* create a server socket */
    serversock = nlOpen(25000, type); /* just a random port number ;) */

    if(serversock == NL_INVALID)
        printErrorExit();

    if(!nlListen(serversock))       /* let's listen on this socket */
    {
        nlClose(serversock);
        printErrorExit();
    }
    nlGetLocalAddr(serversock, &addr);
    print("Server address is %s\n", nlAddrToString(&addr, string));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Client display");
	glutReshapeWindow(640,480);
	glutDisplayFunc(display);
	glutIdleFunc(mainServerLoop);
	glutMainLoop();

    nlShutdown();
    return 0;
}


#include "RakPeerInterface.h"
#include "BitStream.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#include "udt.h"
//#include "cc.h"
#include "nl.h"
#include "glut.h"
#include <gl/glu.h>
#include "MessageIdentifiers.h"
#include "RakNetStatistics.h"
#include "RakSleep.h"
#include "cc.h"

#define NO_PACKETS 100
#define LOGFUNC
#define LOGTOFILE

#if defined WIN32 || defined WIN64
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define sleep(x)    Sleep(1000 * (x))
#endif

DWORD WINAPI monitor(LPVOID);
#define PACKET_LENGTH 1400

#ifndef WIN32
void* recvdata(void*);
#else
DWORD WINAPI recvdata(LPVOID);
#endif

//#define readShort(x, y, z)      {z = nlSwaps(*(NLushort *)((NLbyte *)&x[y])); y += 2;}
//#define readLong(x, y, z)       {z = nlSwapl(*(NLulong  *)((NLbyte *)&x[y])); y += 4;}
//#define readFloat(x, y, z)      {z = nlSwapf(*(NLfloat  *)((NLbyte *)&x[y])); y += 4;}
//#define readDouble(x, y, z)     {z = nlSwapd(*(NLdouble *)((NLbyte *)&x[y])); y += 8;}
/* MACRO NAMEs to be completed*/
//used 21 times
#define readByte(x,y,z) {z = *(NLbyte *)((NLbyte *)&x[y]);y += 1; } //#ifdef LOGTOFILE  print("%i, ",z);}
//used 6 times
#define readByteV(x,y,z,n) {z = (NLbyte *)((NLbyte *)&x[y]);y += n;}
//used 81 times
#define readDouble(x,y,z) {z = *(NLdouble *)((NLbyte *)&x[y]);y += 8; }// #ifdef LOGTOFILE  print("%f, ",z);}
//used 28 times
#define readDoubleV(x,y,z,n) {z = (NLdouble *)((NLbyte *)&x[y]);y += 8*n; }// for(int hen = 0; hen < n; hen++) #ifdef LOGTOFILE  print("%f, ",z[hen]); }
//used 103 times
#define readFloat(x,y,z) {z = *(NLfloat *)((NLbyte *)&x[y]);y += 4; }// #ifdef LOGTOFILE  print("%f, ",z);}
//used 41 times
#define readFloatV(x,y,z,n) {z = (NLfloat *)((NLbyte *)&x[y]);y += 4*n; }// for(int hen = 0; hen < n; hen++) #ifdef LOGTOFILE  print("%f, ",z[hen]);}
//used 179 times
#define readInt(x,y,z) {z = *(NLshort *)((NLbyte *)&x[y]);y += 2; }//#ifdef LOGTOFILE  print("%i, ",z);}
//used 56 times
#define readIntV(x,y,z,n) {z = (NLushort *)((NLbyte *)&x[y]);y += 2*n; }//for(int hen = 0; hen < n; hen++) #ifdef LOGTOFILE  print("%i, ",z[hen]);}
//used 47 times
#define readLong(x,y,z) {z = *(NLulong *)((NLbyte *)&x[y]);y += 4; }//#ifdef LOGTOFILE  print("%i, ",z);}
//used 1 times
#define readLongV(x,y,z,n) {z = (NLulong *)((NLbyte *)&x[y]);y += 4*n;}
//used 243 times
#define readShort(x,y,z) {z = *(NLshort *)((NLbyte *)&x[y]);y += 2; }//#ifdef LOGTOFILE  print("%i, ",z);}

NLbyte *buffer;
NLbyte *bigBuffer;
FILE *text_file = NULL; 

using namespace RakNet;
using namespace std;

//RakPeerInterface *client;
UDTSOCKET client;

//GLfloat verts[8][3];  /* Will be filled in with X,Y,Z vertexes. */
/*GLubyte allIndices[] = {0, 1, 2, 3, 3, 2, 6, 7, 7, 6, 5, 4,
  4, 5, 1, 0, 5, 6, 2, 1, 7, 4, 0, 3};
GLfloat normals[] = 
{ -1, 0, 0,
   0, 1, 0, 
   1, 0, 0,
   0, -1, 0, 
   0, 0, -1, 
   0, 0, 1 };*/

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

enum DataMode { None,TexImage2D,TexSubImage2D };

#define MAX_CLIENTS 20
NLsocket    serversock;
NLsocket    NLclient;
NLint       clientnum = 0;
bool		logToFile = false;

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
   sockaddr_storage clientaddr;
   int addrlen = sizeof(clientaddr);
   //static int count = 0;
   UDTSOCKET recver;
   cout << "hiya" << endl;
   while (true)
   {
      if (UDT::INVALID_SOCK == (recver = UDT::accept(client, (sockaddr*)&clientaddr, &addrlen)))
      {
         cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
         return;
      }

      char clienthost[NI_MAXHOST];
      char clientservice[NI_MAXSERV];
      getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
      cout << "new connection: " << clienthost << ":" << clientservice << endl;

      #ifndef WIN32
         pthread_t rcvthread;
         pthread_create(&rcvthread, NULL, recvdata, new UDTSOCKET(recver));
         pthread_detach(rcvthread);
      #else
         //CreateThread(NULL, 0, recvdata, new UDTSOCKET(recver), 0, NULL);
		 recvdata(new UDTSOCKET(recver));
		 //fflush(text_file);
		 //fclose(text_file);
      #endif
		
   }
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
void makeCheckImage(int checkImageWidth, int checkImageHeight, GLubyte* checkImage)
{
    int i, j, r, c;

       for (i = 0; i < checkImageWidth; i++) {
        for (j = 0; j < checkImageHeight; j++) {
            c = ((((i&0x8)==0)^((j&0x8))==0))*255;
            checkImage[i*3+j*checkImageWidth] = (GLubyte) c;
            checkImage[i*3+1+j*checkImageWidth] = (GLubyte) c;
            checkImage[i*3+2+j*checkImageWidth] = (GLubyte) c;
			checkImage[i*3+3+j*checkImageWidth] = (GLubyte) 255;
        }
    }
}
void parseFunctions()
{
	NLshort index;
	int count = 0;

	// used when receiving textures
	static DataMode dataMode = None;
	static GLvoid *pixels = 0;
	static NLbyte *ptr;
	static long nBytes = 0;
	static long rBytes = 0;

/*DECLARATIONS CREATED BY PERL SCRIPT*/
	HDC a;
	unsigned char* bitmap;
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
	NLfloat nx;
	NLfloat ny;
	NLfloat nz;
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
	static NLulong border;
	NLlong elementCount;
	static NLulong height;
	static NLulong internalformat;
	static NLulong level;
	NLulong n;
	NLulong range;
	NLulong stride;
	static NLulong width;
	NLulong x_Long;
	static NLulong xoffset;
	NLulong y_Long;
	static NLulong yoffset;
	NLushort arrayType;
	NLushort cap;
	NLushort dfactor;
	NLushort face;
	NLushort flag;
	static NLushort format;
	NLushort func;
	NLushort light;
	NLushort list;
	NLushort mask;
	NLushort mode;
	NLushort name;
	NLushort pname;
	NLushort p;
	NLushort sfactor;
	static NLushort target;
	NLushort texture;
	static NLushort type;
	NLushort* textures;
	GLuint* textureList;
	int packetCount;
	int packetSize;
	int typeModifier;

	static NLushort indexPtrSize;
	static NLushort vertexPtrSize;
	static NLushort normalPtrSize;

	static GLubyte *indexPtr = (GLubyte*)malloc(24);
	static GLfloat *vertexPtr = (GLfloat*)malloc(24*sizeof(GLfloat));
	static GLfloat *normalPtr = (GLfloat*)malloc(18*sizeof(GLfloat));
	
	if(dataMode != None) {
		int i, tBytes = PACKET_LENGTH - rBytes > 0 ? rBytes : PACKET_LENGTH;
		//memcpy(ptr,buffer,tBytes);
		
		ptr += tBytes;
		rBytes -= tBytes;

		//cout << rBytes << " " << nBytes << " " << (int)ptr << endl;
		if(rBytes <= 0) {
			switch (dataMode) {
			case TexImage2D:
				glTexImage2D((GLenum) target,(GLint) level,(GLint) internalformat,(GLsizei) width,(GLsizei) height,(GLint) border,(GLenum) format,(GLenum) type,(GLvoid*) pixels);
				break;
			case TexSubImage2D:
				glTexSubImage2D((GLenum) target,(GLint) level,(GLint) xoffset,(GLint) yoffset,(GLsizei) width,(GLsizei) height,(GLenum) format,(GLenum) type,(GLvoid*) pixels);
			}
			//printf("%i, %i, %i, %i, %i, %i, %i, %i, \n",(short) target, (long) level, (long) internalformat, (long) width, (long) height, (long) border, (short) format, (short) type );
			free(pixels);
			dataMode = None;
			//cout << endl;
		}
		return;
	}
	//cout << "word up" << endl;
	readInt(buffer,count,index);
	while (count < PACKET_LENGTH) {
		//printf("%i\n", index);
#ifdef LOGFUNC
		if(index < 400 && index > 0) print("%i - %s (",index,GLN[index]);
#endif
/*SWITCH CREATED BY PERL SCRIPT*/
		switch(index) {
			case 1: //glAlphaFunc
				readShort(buffer,count,func);
				readFloat(buffer,count,ref);
#ifdef LOGTOFILE
				print("%i, %f",(short) func, (float) ref);
#endif
				glAlphaFunc((GLenum) func,(GLclampf) ref);
			break;
			case 4: //glBegin
				readInt(buffer,count,mode);
#ifdef LOGTOFILE  
				print("%i",(short) mode);
#endif
				glBegin((GLenum) mode);
			break;
			case 5: //glBindTexture
				readShort(buffer,count,target);
				readLong(buffer,count,texture);
#ifdef LOGTOFILE
				print("%i",(short)target);
				print("%i, %i",(short) target, (GLuint) texture);
#endif
				glBindTexture((GLenum) target,(GLuint) texture);
			break;
			case 6: //glBitmap
				readLong(buffer,count,width);
				readLong(buffer,count,height);
				readFloat(buffer,count,xorig);
				readFloat(buffer,count,yorig);
				readFloat(buffer,count,xmove);
				readFloat(buffer,count,ymove);
#ifdef LOGTOFILE
				print("%i, %i, %f, %f, %f, %f, (bytes)",(long) width, (long) height, (float) xorig, (float) yorig, (float) xmove, (float) ymove);
#endif
				if(width * height == 0) {
					print("-----size error------");
					break;
				}
				//bitmap = &(buffer[count]);
				//count += width * height;
				bitmap = (unsigned char *)malloc(width * height);
				glBitmap((GLsizei) width,(GLsizei) height,(GLfloat) xorig,(GLfloat) yorig,(GLfloat) xmove,(GLfloat) ymove,(GLubyte*) bitmap);
				free (bitmap);
			break;
			case 7: //glBlendFunc
				readShort(buffer,count,sfactor);
				readShort(buffer,count,dfactor);
#ifdef LOGTOFILE
				print("%i, %i",(short) sfactor, (short) dfactor);
#endif
				glBlendFunc((GLenum) sfactor,(GLenum) dfactor);
			break;
			case 10: //glClear
				readInt(buffer,count,mask);
#ifdef LOGTOFILE  
				print("%i",(int) mask);
#endif
				glClear((GLbitfield) mask);
			break;
			case 12: //glClearColor
				readFloat(buffer,count,red_Float);
				readFloat(buffer,count,green_Float);
				readFloat(buffer,count,blue_Float);
				readFloat(buffer,count,alpha_Float);
#ifdef LOGTOFILE  
				print("%f, %f, %f, %f",(float) red_Float, (float) green_Float, (float) blue_Float, (float) alpha_Float);
#endif
				glClearColor((GLclampf) red_Float,(GLclampf) green_Float,(GLclampf) blue_Float,(GLclampf) alpha_Float);
			break;
			/*case 21: //glColor3f
				readFloat(buffer,count,red);
				readFloat(buffer,count,green);
				readFloat(buffer,count,blue);
#ifdef LOGTOFILE
				print("%f, %f, %f",(float) red, (float) green, (float) blue);		
#endif
				glColor3f((GLfloat) red,(GLfloat) green,(GLfloat) blue);
			break;*/
			case 21: //glColor3f
			case 22: //glColor3fv
				readFloatV(buffer,count,v,3);
				//printf("%f",*(float) v);
				glColor3fv((GLfloat*) v);
			break;
			/*case 37: //glColor4f
				readFloat(buffer,count,red);
				readFloat(buffer,count,green);
				readFloat(buffer,count,blue);
				readFloat(buffer,count,alpha);
#ifdef LOGTOFILE
				print("%f, %f, %f, %f",(float) red, (float) green, (float) blue, (float) alpha);
#endif
				glColor4f((GLfloat) red,(GLfloat) green,(GLfloat) blue,(GLfloat) alpha);
			break;*/
			case 37: //glColor4f
			case 38: //glColor4fv
				readFloatV(buffer,count,v,4);
#ifdef LOGTOFILE  
				print("%f, %f, %f, %f",(float) v[0],(float) v[1],(float) v[2],(float) v[3]);
#endif
				glColor4fv((GLfloat*) v);
			break;
			case 44: //glColor4ubv
				readByteV(buffer,count,v_ByteV,4);
#ifdef LOGTOFILE  
//				print("%i",*(byte) v_ByteV);
#endif
				glColor4ubv((GLubyte*) v_ByteV);
			break;
			case 57: //glCullFace
				readShort(buffer,count,mode);
#ifdef LOGTOFILE
				print("%i",(short) mode);
#endif
				glCullFace((GLenum) mode);
			break;
			/*case 59: //glDeleteTextures
				readLong(buffer,count,n);
				readIntV(buffer,count,textures,1);
				//#ifdef LOGTOFILE  print("%i, %i",(long) n, *(int*) textures);
				//glDeleteTextures((GLsizei) n,(GLuint*) textures);
			break;*/
			case 60: //glDepthFunc
				readShort(buffer,count,func);
#ifdef LOGTOFILE
				print("%i",(short) func);
#endif
				glDepthFunc((GLenum) func);
			break;
			case 61: //glDepthMask
				readByte(buffer,count,flag);
#ifdef LOGTOFILE  
				print("%i",(short) flag);
#endif
				glDepthMask((GLboolean) flag);
			break;
			case 62: //glDepthRange
				readDouble(buffer,count,zNear_Double);
				readDouble(buffer,count,zFar_Double);
#ifdef LOGTOFILE  
				print("%f, %f",(double) zNear_Double, (double) zFar_Double);
#endif
				glDepthRange((GLclampd) zNear_Double,(GLclampd) zFar_Double);
			break;
			case 63: //glDisable
				readShort(buffer,count,cap);
#ifdef LOGTOFILE  
				print("%i",(short) cap);
#endif
				glDisable((GLenum) cap);
			break;
			case 66: //glDrawBuffer
				readShort(buffer,count,mode);
#ifdef LOGTOFILE  
				print("%i",(short) mode);
#endif
				glDrawBuffer((GLenum) mode);
			break;
			case 67: //glDrawElements
				readShort(buffer,count,mode);
				readLong(buffer,count,elementCount);
				readShort(buffer,count,type);
				// unknown type for: indices
#ifdef LOGTOFILE
				print("%i, %i, %i, ",(GLenum) mode, (long) elementCount, (GLenum) type );
#endif
				glDrawElements((GLenum) mode,(GLsizei) elementCount,(GLenum) type, (GLvoid*) indexPtr);
			break;
			case 72: //glEnable
				readShort(buffer,count,cap);
#ifdef LOGTOFILE
				print("%i",(short) cap);
#endif
				glEnable((GLenum) cap);
			break;
			case 73: //glEnableClientState
				readShort(buffer,count,arrayType);
#ifdef LOGTOFILE
				print("%i",(GLenum) arrayType);
#endif
				glEnableClientState((GLenum) arrayType);
			break;
			case 74: //glEnd
				glEnd();
			break;
			case 75: //glEndList
				glEndList();
			break;
			case 95: //glFrontFace
				readShort(buffer,count,mode);
#ifdef LOGTOFILE  
				print("%i",(short) mode);
#endif
				glFrontFace((GLenum) mode);
			break;
			case 96: //glFrustum
				readDouble(buffer,count,left);
				readDouble(buffer,count,right);
				readDouble(buffer,count,bottom);
				readDouble(buffer,count,top);
				readDouble(buffer,count,zNear);
				readDouble(buffer,count,zFar);
#ifdef LOGTOFILE  
				print("%f, %f, %f, %f, %f, %f",(double) left, (double) right, (double) bottom, (double) top, (double) zNear, (double) zFar);
#endif
				glFrustum((GLdouble) left,(GLdouble) right,(GLdouble) bottom,(GLdouble) top,(GLdouble) zNear,(GLdouble) zFar);
			break;
			case 97: //glGenLists
				readLong(buffer,count,range);
#ifdef LOGTOFILE
				print("%i",(long) range);
#endif
				glGenLists((GLsizei) range);
			break;
			case 98: //glGenTextures
				readLong(buffer,count,n);
				//readIntV(buffer,count,textures,1);
				textureList = (GLuint *)malloc(n * sizeof(GLuint));
#ifdef LOGTOFILE
				print("%i",(long) n);
#endif
				glGenTextures((GLsizei) n,(GLuint*) textureList);
			break;
			case 102: //glGetError
				glGetError();
			break;
			/*case 103: //glGetFloatv
				readShort(buffer,count,pname);
				readFloatV(buffer,count,params,1);
				//#ifdef LOGTOFILE  print("%i, %f",(short) pname, *(float) params);
				glGetFloatv((GLenum) pname,(GLfloat*) params);
			break;*/
			case 117: //glGetString
				readShort(buffer,count,name);
#ifdef LOGTOFILE
				print("%i",(short) name);
#endif
				glGetString((GLenum) name);
			break;
			case 130: //glIndexPointer
				readShort(buffer,count,type);
				readLong(buffer,count,stride);
				memcpy((NLubyte *)indexPtr, (NLubyte *)&buffer[count],  indexPtrSize);
				count += indexPtrSize;
				// unknown type for: pointer
#ifdef LOGTOFILE
				print("%i, %i, ",(short) type, (long) stride );
#endif
				glIndexPointer((GLenum) type,(GLsizei) stride,(GLvoid*) indexPtr);
				//glIndexPointer(GL_UNSIGNED_BYTE,0,allIndices);
			break;
			case 147: //glLightModelfv
				readShort(buffer,count,pname);
				if(pname == GL_LIGHT_MODEL_AMBIENT) {
					readFloatV(buffer,count,params,4);
				} else {
					readFloatV(buffer,count,params,1);
				}
#ifdef LOGTOFILE
				print("%i",(short) pname);
#endif
				glLightModelfv((GLenum) pname,(GLfloat*) params);
			break;
			case 150: //glLightf
				readShort(buffer,count,light);
				readShort(buffer,count,pname);
				readFloat(buffer,count,param);
#ifdef LOGTOFILE
				print("%i, %i, %f",(short) light, (short) pname, (float) param);
#endif
				glLightf((GLenum) light,(GLenum) pname,(GLfloat) param);
			break;
			case 157: //glLoadIdentity
				glLoadIdentity();
			break;
			case 159: //glLoadMatrixf
				readFloatV(buffer,count,m,16);
//#ifdef LOGTOFILE  
//				print("%f",*(float) m);
//#endif
				glLoadMatrixf(m);
			break;
			case 171: //glMaterialfv
				readShort(buffer,count,face);
				readShort(buffer,count,pname);
				switch (pname) {
					case GL_AMBIENT:
					case GL_DIFFUSE:
					case GL_SPECULAR:
					case GL_EMISSION:
					case GL_AMBIENT_AND_DIFFUSE:
						readFloatV(buffer,count,params,4);
						break;
					case GL_COLOR_INDEXES:
						readFloatV(buffer,count,params,3);
						break;
					case GL_SHININESS:
					default:
						readFloatV(buffer,count,params,1);
						break;
				}
				//printf("%i, %i, %f",(short) face, (short) pname, *(float) params);
				glMaterialfv((GLenum) face,(GLenum) pname,(GLfloat*) params);
			break;
			case 174: //glMatrixMode
				readShort(buffer,count,mode);
#ifdef LOGTOFILE  
				print("%i",(short) mode);
#endif
				glMatrixMode((GLenum) mode);
			break;
			case 175: //glMultMatrixd
				readDoubleV(buffer,count,m_DoubleV,16);
#ifdef LOGTOFILE  
				print("{ %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f }",
					(double) *m_DoubleV,(double) *(m_DoubleV+1),(double) *(m_DoubleV+2),(double) *(m_DoubleV+3),
					(double) *(m_DoubleV+4),(double) *(m_DoubleV+5),(double) *(m_DoubleV+6),(double) *(m_DoubleV+7),
					(double) *(m_DoubleV+8),(double) *(m_DoubleV+9),(double) *(m_DoubleV+10),(double) *(m_DoubleV+11),
					(double) *(m_DoubleV+12),(double) *(m_DoubleV+13),(double) *(m_DoubleV+14),(double) *(m_DoubleV+15));
#endif
				glMultMatrixd((GLdouble*) m_DoubleV);
			break;
			case 176: //glMultMatrixf
				readFloatV(buffer,count,m,16);
				//#ifdef LOGTOFILE  print("%f",*(float) m);
				glMultMatrixf((GLfloat*) m);
			break;
			case 177: //glNewList
				readInt(buffer,count,list);
				readShort(buffer,count,mode);
				//printf("%i, %i",(int) list, (short) mode);
				glNewList((GLuint) list,(GLenum) mode);
			break;
			case 182: //glNormal3f
				//readFloat(buffer,count,nx);
				//readFloat(buffer,count,ny);
				//readFloat(buffer,count,nz);
				//printf("%f, %f, %f",(float) nx, (float) ny, (float) nz);
				//glNormal3f((GLfloat) nx,(GLfloat) ny,(GLfloat) nz);
			//break;
			case 183: //glNormal3fv
				readFloatV(buffer,count,v,3);
				//#ifdef LOGTOFILE  print("%f",*(float) v);
				glNormal3fv((GLfloat*) v);
			break;
			case 188: //glNormalPointer
				readShort(buffer,count,type);
				readLong(buffer,count,stride);
				memcpy((NLbyte *)normalPtr, (NLbyte *)&buffer[count], normalPtrSize);
				count += normalPtrSize;
#ifdef LOGTOFILE
				print("%i, %i, ",(short) type, (long) stride );
#endif
				glNormalPointer((GLenum) type,(GLsizei) stride,(GLvoid*) normalPtr);
				//glNormalPointer(GL_FLOAT,0,normals);
			break;
			case 189: //glOrtho
				readDouble(buffer,count,left);
				readDouble(buffer,count,right);
				readDouble(buffer,count,bottom);
				readDouble(buffer,count,top);
				readDouble(buffer,count,zNear);
				readDouble(buffer,count,zFar);
#ifdef LOGTOFILE  
				print("%f, %f, %f, %f, %f, %f",(double) left, (double) right, (double) bottom, (double) top, (double) zNear, (double) zFar);
#endif
				//glOrtho((GLdouble) left,(GLdouble) right,(GLdouble) bottom,(GLdouble) top,(GLdouble) zNear,(GLdouble) zFar);
				glOrtho((GLdouble) left,1280,1024,(GLdouble) top,(GLdouble) zNear,(GLdouble) zFar);
			break;
			case 199: //glPointSize
				readFloat(buffer,count,size);
#ifdef LOGTOFILE  
				print("%f",(float) size);
#endif
				glPointSize((GLfloat) size);
			break;
			case 200: //glPolygonMode
				readShort(buffer,count,face);
				readShort(buffer,count,mode);
#ifdef LOGTOFILE  
				print("%i, %i",(short) face, (short) mode);
#endif
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
#ifdef LOGTOFILE  
				print("%f, %f, %f, %f",(float) angle, (float) x, (float) y, (float) z);
#endif
				glRotatef((GLfloat) angle,(GLfloat) x,(GLfloat) y,(GLfloat) z);
			break;
			case 250: //glScalef
				readFloat(buffer,count,x);
				readFloat(buffer,count,y);
				readFloat(buffer,count,z);
#ifdef LOGTOFILE  
				print("%f, %f, %f",(float) x, (float) y, (float) z);
#endif
				glScalef((GLfloat) x,(GLfloat) y,(GLfloat) z);
			break;
			case 253: //glShadeModel
				readShort(buffer,count,mode);
#ifdef LOGTOFILE  
				print("%i",(short) mode);
#endif
				glShadeModel((GLenum) mode);
			break;
			case 267: //glTexCoord2f
				readFloat(buffer,count,s);
				readFloat(buffer,count,t);
#ifdef LOGTOFILE  
				print("%f, %f",(float) s, (float) t);
#endif
				glTexCoord2f((GLfloat) s,(GLfloat) t);
			break;
			case 290: //glTexEnvf
				readShort(buffer,count,target);
				readShort(buffer,count,pname);
				readFloat(buffer,count,param);
#ifdef LOGTOFILE  
				print("%i, %i, %f",(short) target, (short) pname, (float) param);
#endif
				glTexEnvf((GLenum) target,(GLenum) pname,(GLfloat) param);
			break;
			case 292: //glTexEnvi
				readShort(buffer,count,target);
				readShort(buffer,count,pname);
				readShort(buffer,count,param);
				//printf("%i, %i, %i",(short) target, (short) pname, (long) param);
				glTexEnvi((GLenum) target,(GLenum) pname,(GLint) param);
			break;
			case 301: //glTexImage2D
				//printf("receiving texture: ");
				readShort(buffer,count,target);
				readLong(buffer,count,level);
				readLong(buffer,count,internalformat);
				readLong(buffer,count,width);
				readLong(buffer,count,height);
				readLong(buffer,count,border);
				readShort(buffer,count,format);
				readShort(buffer,count,type);
				// unknown type for: pixels
				
				typeModifier = 4;
				switch(type) {
					case GL_UNSIGNED_SHORT:
					case GL_SHORT:
						typeModifier = 2;
						break;
					case GL_UNSIGNED_INT:
						typeModifier = 4;
				}
				pixels = malloc(width * height * typeModifier);
				ptr = (NLbyte*)pixels;
				
				for(int i = 0; i < width * height / 10; i++)
					((GLubyte*) pixels)[i*40] = ((GLubyte*) pixels)[i*40+1] = ((GLubyte*) pixels)[i*40+2] = 255;
				
				glTexImage2D((GLenum) target,(GLint) level,(GLint) internalformat,(GLsizei) width,(GLsizei) height,(GLint) border,(GLenum) format,(GLenum) type,(GLvoid*) pixels);
				free(pixels);
				
				//rBytes = nBytes = width * height * typeModifier;
				//dataMode = TexImage2D;
				//count += width * height;
#ifdef LOGTOFILE  
				print("%i, %i, %i, %i, %i, %i, %i, %i, )\n",(short) target, (long) level, (long) internalformat, (long) width, (long) height, (long) border, (short) format, (short) type );
#endif
				//return;
			break;
			case 302: //glTexParameterf
				readShort(buffer,count,target);
				readShort(buffer,count,pname);
				switch (pname) {
					case GL_TEXTURE_PRIORITY:
						readFloat(buffer,count,param);
#ifdef LOGTOFILE  
						print("%i, %i, %f",(short) target, (short) pname, (float) param);
						
#endif
						glTexParameterf((GLenum) target,(GLenum) pname,(GLfloat) param);
						break;
					default:
						readShort(buffer,count,param);
#ifdef LOGTOFILE  
						print("%i, %i, %i",(short) target, (short) pname, (short) param);
#endif
						glTexParameterf((GLenum) target,(GLenum) pname,(GLshort) param);
					break;
				}
			break;
			case 304: //glTexParameteri
				readShort(buffer,count,target);
				readShort(buffer,count,pname);
				switch (pname) {
					case GL_TEXTURE_PRIORITY:
						readLong(buffer,count,param);
						glTexParameteri((GLenum) target,(GLenum) pname,(GLint) param);
						break;
					default:
						readShort(buffer,count,p);
						glTexParameteri((GLenum) target,(GLenum) pname,(GLenum) p);
						break;
				}
				
				//printf("%i, %i, %i",(short) target, (short) pname, (long) param);
				
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
				
				typeModifier = 4;
				switch(type) {
					case GL_UNSIGNED_SHORT:
					case GL_SHORT:
						typeModifier = 2;
						break;
					case GL_UNSIGNED_INT:
						typeModifier = 4;
				}
				pixels = malloc(width * height * typeModifier);
				ptr = (NLbyte*)pixels;
				
				for(int i = 0; i < width * height / 10; i++)
					((GLubyte*) pixels)[i*40] = ((GLubyte*) pixels)[i*40+1] = ((GLubyte*) pixels)[i*40+2] = 255;
				glTexSubImage2D((GLenum) target,(GLint) level,(GLint) xoffset,(GLint) yoffset,(GLsizei) width,(GLsizei) height,(GLenum) format,(GLenum) type,(GLvoid*) pixels);
				free(pixels);

				//rBytes = nBytes = width * height * typeModifier;
				
				//dataMode = TexSubImage2D;
				
				//count += width * height;
#ifdef LOGTOFILE  
				print("%i, %i, %i, %i, %i, %i, %i, %i,  )\n",(short) target, (long) level, (long) xoffset, (long) yoffset, (long) width, (long) height, (short) format, (short) type );
#endif
			return;
			case 308: //glTranslated
				readDouble(buffer,count,x_Double);
				readDouble(buffer,count,y_Double);
				readDouble(buffer,count,z_Double);
#ifdef LOGTOFILE  
				print("%f, %f, %f",(double) x_Double, (double) y_Double, (double) z_Double);
#endif
				glTranslated((GLdouble) x_Double,(GLdouble) y_Double,(GLdouble) z_Double);
			break;
			case 309: //glTranslatef
				readFloat(buffer,count,x);
				readFloat(buffer,count,y);
				readFloat(buffer,count,z);
#ifdef LOGTOFILE  
				print("%f, %f, %f",(float) x, (float) y, (float) z);
#endif
				glTranslatef((GLfloat) x,(GLfloat) y,(GLfloat) z);
			break;
			case 312: //glVertex2f
				readFloat(buffer,count,x);
				readFloat(buffer,count,y);
#ifdef LOGTOFILE  
				print("%f, %f",(float) x, (float) y);
#endif
				glVertex2f((GLfloat) x,(GLfloat) y);
			break;
			case 320: //glVertex3f falls through to:
			case 321: //glVertex3fv
				readFloatV(buffer,count,v,3);
#ifdef LOGTOFILE  
				print("%f, %f, %f",*(float*) v,(float) v[1],(float) v[2]);
#endif
				glVertex3fv((GLfloat*) v);
			break;
			case 334: //glVertexPointer
				readLong(buffer,count,size);
				readShort(buffer,count,type);
				readLong(buffer,count,stride);
				memcpy((NLbyte *)vertexPtr, (NLbyte *)&buffer[count], vertexPtrSize);
				count += vertexPtrSize;
#ifdef LOGTOFILE  
				print("%i, %i, %i, ",(long) size, (short) type, (long) stride );
#endif
				glVertexPointer((GLint) size,(GLenum) type,(GLsizei) stride,(GLvoid*) vertexPtr);
				//glVertexPointer(3, GL_FLOAT, 0, verts);
			break;
			case 335: //glViewport
				readLong(buffer,count,x_Long);
				readLong(buffer,count,y_Long);
				readLong(buffer,count,width);
				readLong(buffer,count,height);
#ifdef LOGTOFILE  
				print("%i, %i, %i, %i",(long) x_Long, (long) y_Long, (long) width, (long) height);
#endif
				glViewport((GLint) x_Long,(GLint) y_Long,1280,1024);
				//glViewport((GLint) x_Long,(GLint) y_Long,(GLsizei) width,(GLsizei) height);
			break;
			case 354: //wglSwapBuffers
				// unknown type for: a
#ifdef PACKETCOUNT
				printf("swap ");
#endif
				//print("swap ");
				glutSwapBuffers();
			break;
			case 400:
				//readShort(buffer,count,packetCount);
#ifdef LOGFUNC
				print("------Start Packet-----\n");
				//print("packetCount: %i\n",packetCount);
#endif

				//printf("%i ",packetCount);

				//printf("start packet: %i\n",packetCount);
				//count = (count > PACKET_LENGTH ? count - PACKET_LENGTH : 0);
			break;
			case 401: //pointer array sizes
				readShort(buffer,count,vertexPtrSize);
				readShort(buffer,count,normalPtrSize);
				readShort(buffer,count,indexPtrSize);
			break;
			case 500:
				//readShort(buffer,count,packetCount);
				//readShort(buffer,count,packetSize);
#ifdef LOGFUNC
				print("----End Packet-----\n");
				//print("packet no: %i, sent reason: %i\n\n",packetCount,packetSize);
				//printf("finish packet: %i\n",packetCount);
#endif
			return;
		}
/* FINISHED CREATION (<6 days)*/
#ifdef LOGFUNC
		if(index < 400 && index > 0) print(") %i\n",count);
#endif
		//fflush(text_file);
		readInt(buffer,count,index);
	}
	return;
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
	text_file = fopen("output.txt", "w");
	
	// print("file opened\n");
	// use this function to initialize the UDT library
	UDT::startup();

	struct addrinfo hints, *local, *peer;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_socktype = SOCK_DGRAM;

	string service("9000");
	if (2 == argc)
      service = argv[1];

	if (0 != getaddrinfo(NULL, service.c_str(), &hints, &local))
	{
		cout << "incorrect network address.\n" << endl;
		return 0;
	}

	client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);
	//UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
    //UDT::setsockopt(client, 0, UDT_MSS, new int(9000), sizeof(int));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10000000), sizeof(int));
    //UDT::setsockopt(client, 0, UDP_SNDBUF, new int(10000000), sizeof(int));

	#ifdef WIN32
      UDT::setsockopt(client, 0, UDT_MSS, new int(1052), sizeof(int));
	#endif

	if (UDT::ERROR == UDT::bind(client, local->ai_addr, local->ai_addrlen))
	{
      cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
	}
	
	freeaddrinfo(local);

	cout << "client is ready at port: " << service << endl;

	if (UDT::ERROR == UDT::listen(client, 10))
	{
      cout << "listen: " << UDT::getlasterror().getErrorMessage() << endl;
      return 0;
	}
	buffer = bigBuffer = (NLbyte *)malloc(NO_PACKETS * PACKET_LENGTH);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutGameModeString("1280x1024:24@60");
	glutEnterGameMode();
	//glutCreateWindow("Client display");
	//glutReshapeWindow(640,480);
	glutDisplayFunc(display);
	glutIdleFunc(mainServerLoop);
	//mainServerLoop();

	//fclose(text_file);
	glutMainLoop();

	UDT::close(client);

	// use this function to release the UDT library
	UDT::cleanup();

	return 1;
}

#ifndef WIN32
void* recvdata(void* usocket)
#else
DWORD WINAPI recvdata(LPVOID usocket)
#endif
{
   UDTSOCKET recver = *(UDTSOCKET*)usocket;
   delete (UDTSOCKET*)usocket;
   static int count = 0;
   static int pcktsRcvd = 0;
   static int pcktsPrsd = 0;
   //NLbyte *curBuffer = bigBuffer + pcktsRcvd * PACKET_LENGTH;

   while (true)
   {
      int rsize = 0;
      int rs;
	  buffer = bigBuffer;
      while (rsize < PACKET_LENGTH)
      {
         if (UDT::ERROR == (rs = UDT::recv(recver, buffer + rsize, PACKET_LENGTH - rsize, 0)))
         {
            cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
            break;
         }

         rsize += rs;
      }
	 //pcktsRcvd = pcktsRcvd  + 1; // % NO_PACKETS;

	  //if(pcktsRcvd - pcktsPrsd == 2) {
		//cout << "parsing buffer" << endl;
	  //printf("%i ", count++);
	  parseFunctions(); //bigBuffer + pcktsPrsd * PACKET_LENGTH);
		
		//pcktsPrsd = pcktsPrsd + 1; //) % NO_PACKETS;
	  //}
	  
      if (rsize < PACKET_LENGTH) //|| pcktsRcvd == 50)
         break;
   }

   //delete [] data;

   UDT::close(recver);
   cout << "Socket closed" << endl;
   #ifndef WIN32
      return NULL;
   #else
      return 0;
   #endif
}

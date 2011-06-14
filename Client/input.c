/* input.c
 *
 * GLTrace OpenGL debugger/trace utlility
 * Version:  2.3
 * Copyright (C) 1999-2002
 *		Phil Frisbie, Jr. (phil@hawksoft.com)
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

#include <stdlib.h>
#include <ctype.h>
#include "gltrace.h"
#include "input.h"

#if defined WIN32 || defined WIN64
#  define ISKEYDOWN(x) ((GetAsyncKeyState(x) & 0x8000) != 0)
#else
#  define XK_MISCELLANY
#  include <X11/keysymdef.h>
#  define ISKEYDOWN(x)  (0)
#  define VK_SHIFT   XK_Shift_L
#  define VK_MENU    XK_Menu
#  define VK_CONTROL XK_Control_L
#  define VK_TAB     XK_Tab
#  define VK_UP      XK_Up
#  define VK_DOWN    XK_Down
#  define VK_LEFT    XK_Left
#  define VK_RIGHT   XK_Right
#  define VK_INSERT  XK_Insert
#  define VK_DELETE  XK_Delete
#  define VK_HOME    XK_Home
#  define VK_END     XK_End
#  define VK_PRIOR   XK_Prior
#  define VK_NEXT    XK_Next
#  define VK_BACK    XK_BackSpace
#  define VK_RETURN  XK_Return
#  define VK_F1      XK_F1
#  define VK_F2      XK_F2
#  define VK_F3      XK_F3
#  define VK_F4      XK_F4
#  define VK_F5      XK_F5
#  define VK_F6      XK_F6
#  define VK_F7      XK_F7
#  define VK_F8      XK_F8
#  define VK_F9      XK_F9
#  define VK_F10     XK_F10
#  define VK_F11     XK_F11
#  define VK_F12     XK_F12
#endif

static GLboolean option = GL_FALSE;
static GLboolean oneframe = GL_FALSE;
static GLboolean fps = GL_TRUE;
static GLboolean on_off = GL_FALSE;

static int option_key = '1';
static int oneframe_key = '2';
static int fps_key = '3';
static int on_off_key = '4';

static int translateKey(char *key)
{
    /* check for single char */
    if(strlen(key) == 1)
    {
        /* convert to upper for consistancy */
        return toupper(*key);
    }
    if(strcmp(key, "shift") == 0)
    {
        return VK_SHIFT;
    }
    if(strcmp(key, "alt") == 0)
    {
        return VK_MENU;
    }
    if(strcmp(key, "ctrl") == 0)
    {
        return VK_CONTROL;
    }
    if(strcmp(key, "tab") == 0)
    {
        return VK_TAB;
    }
    if(strcmp(key, "up") == 0)
    {
        return VK_UP;
    }
    if(strcmp(key, "down") == 0)
    {
        return VK_DOWN;
    }
    if(strcmp(key, "left") == 0)
    {
        return VK_LEFT;
    }
    if(strcmp(key, "right") == 0)
    {
        return VK_RIGHT;
    }
    if(strcmp(key, "insert") == 0)
    {
        return VK_INSERT;
    }
    if(strcmp(key, "delete") == 0)
    {
        return VK_DELETE;
    }
    if(strcmp(key, "home") == 0)
    {
        return VK_HOME;
    }
    if(strcmp(key, "end") == 0)
    {
        return VK_END;
    }
    if(strcmp(key, "pageup") == 0)
    {
        return VK_PRIOR;
    }
    if(strcmp(key, "pagedn") == 0)
    {
        return VK_NEXT;
    }
    if(strcmp(key, "backsp") == 0)
    {
        return VK_BACK;
    }
    if(strcmp(key, "enter") == 0)
    {
        return VK_RETURN;
    }
    if(strcmp(key, "F1") == 0)
    {
        return VK_F1;
    }
    if(strcmp(key, "F2") == 0)
    {
        return VK_F2;
    }
    if(strcmp(key, "F3") == 0)
    {
        return VK_F3;
    }
    if(strcmp(key, "F4") == 0)
    {
        return VK_F4;
    }
    if(strcmp(key, "F5") == 0)
    {
        return VK_F5;
    }
    if(strcmp(key, "F6") == 0)
    {
        return VK_F6;
    }
    if(strcmp(key, "F7") == 0)
    {
        return VK_F7;
    }
    if(strcmp(key, "F8") == 0)
    {
        return VK_F8;
    }
    if(strcmp(key, "F9") == 0)
    {
        return VK_F9;
    }
    if(strcmp(key, "F10") == 0)
    {
        return VK_F10;
    }
    if(strcmp(key, "F11") == 0)
    {
        return VK_F11;
    }
    if(strcmp(key, "F12") == 0)
    {
        return VK_F12;
    }
    return 0;
}

void updateKeyState(void)
{
    static GLboolean oneframe_toggle = GL_FALSE;
    static GLboolean fps_toggle = GL_FALSE;
    static GLboolean on_off_toggle = GL_FALSE;

    /* toggle oneframe key, only valid for one frame */
    if(ISKEYDOWN(oneframe_key))
    {
        /* key is down, but was it up last frame? */
        if(oneframe_toggle == GL_FALSE)
        {
            /* yes, so set to true for this frame */
            oneframe_toggle = GL_TRUE;
            oneframe = GL_TRUE;
        }
        else
        {
            /* no, so set to false and wait for key to be released */
            oneframe = GL_FALSE;
        }
    }
    else
    {
        oneframe_toggle = GL_FALSE;
    }

    option = (GLboolean)(ISKEYDOWN(option_key));
    /* toggle on_off key */
    if(ISKEYDOWN(fps_key))
    {
        if(fps_toggle == GL_FALSE)
        {
            fps_toggle = GL_TRUE;
            fps = (GLboolean)(fps == GL_TRUE? GL_FALSE:GL_TRUE);
        }
    }
    else
    {
        fps_toggle = GL_FALSE;
    }
    /* toggle on_off key */
    if(ISKEYDOWN(on_off_key))
    {
        if(on_off_toggle == GL_FALSE)
        {
            on_off_toggle = GL_TRUE;
            on_off = (GLboolean)(on_off == GL_TRUE? GL_FALSE:GL_TRUE);
        }
    }
    else
    {
        on_off_toggle = GL_FALSE;
    }

}

GLboolean setKey(int function, char *key)
{
    switch (function) {
    case GLT_OPTION:
        option_key = translateKey(key);
        break;
    case GLT_ONEFRAME:
        oneframe_key = translateKey(key);
        break;
    case GLT_FPS:
        fps_key = translateKey(key);
        break;
    case GLT_ON_OFF:
        on_off_key = translateKey(key);
        break;
    default:
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean getKeyState(int function)
{
    switch (function) {
    case GLT_OPTION:
        return option;
        break;
    case GLT_ONEFRAME:
        return oneframe;
        break;
    case GLT_FPS:
        return fps;
        break;
    case GLT_ON_OFF:
        return on_off;
        break;
    default:
        return GL_FALSE;
    }
}

void setKeyState(int function, GLboolean state)
{
    switch (function) {
    case GLT_OPTION:
        option = state;
        break;
    case GLT_ONEFRAME:
        oneframe = state;
        break;
    case GLT_FPS:
        fps = state;
        break;
    case GLT_ON_OFF:
        on_off = state;
        break;
    default:
        break;
    }
}


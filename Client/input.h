/* input.h             -*- mode: C; tab-width: 4 -*-
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

#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#define GLT_OPTION	    0
#define GLT_ONEFRAME    1
#define GLT_FPS		    2
#define GLT_ON_OFF	    3

void updateKeyState(void);
GLboolean setKey(int function, char *key);
GLboolean getKeyState(int function);
void setKeyState(int function, GLboolean state);

#ifdef __cplusplus
}
#endif

#endif /* INPUT_H */

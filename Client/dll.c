/* dll.c, portable shared library loader
 *
 * Copyright (C) 1997-2002
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

#include "dll.h"

#if defined WIN32 || defined WIN64

#ifdef _MSC_VER
#pragma warning (disable:4201)
#pragma warning (disable:4214)
#pragma warning (disable:4115)
#pragma warning (disable:4514)
#pragma warning (disable:4127)
#endif /* _MSC_VER */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _MSC_VER
#pragma warning (default:4201)
#pragma warning (default:4214)
#pragma warning (default:4115)
#endif /* _MSC_VER */

void *dllLoad(char *name)
{
	HMODULE handle;

	handle = LoadLibrary(name);
	return (void *)handle;
}

FUNCTION dllGetFunction(void *handle, char *name)
{
	return (FUNCTION)GetProcAddress((HMODULE)handle, name);
}

void dllUnload(void *handle)
{
	if(handle)
		(void)FreeLibrary((HMODULE)handle);
}

#else
/* assume Linux or Solaris */

#include <dlfcn.h>

void *dllLoad(char *name)
{
	void *handle;

	handle=dlopen(name, RTLD_LAZY | RTLD_LOCAL);
	return handle;
}

FUNCTION dllGetFunction(void *handle, char *name)
{
	return dlsym(handle, name);
}

void dllUnload(void *handle)
{
	if(handle)
		dlclose(handle);
}

#endif

 _________________________________________________________________________

 GLTrace

 Win32 module to redirect calls from OPENGL32.DLL to specified provider DLL, 
 with API function trace to OutputDebugString() or text file

 Please forward enhancements and bug reports to Phil Frisbie Jr. at
 Hawk Software (phil@hawksoft.com)
 
 _________________________________________________________________________

 Instructions:

    - If original OPENGL32.DLL resides in application directory, rename or
      move it to avoid conflicts with this copy of OPENGL32.DLL

    - After renaming original OPENGL32.DLL (if necessary), copy 
      OPENGL32.DLL and GLTRACE.INI into target application directory 
      (e.g, C:\QUAKE)

    - Modify GLTRACE.INI's [Implementation] section to point to renamed or
      moved copy of original OPENGL32.DLL (e.g., the 3Dfx GLQuake library)

    - Modify other GLTRACE.INI sections, if desired, in accordance with
      instructions in GLTRACE.INI comment header

    - Run the application under test (e.g., GLQUAKE.EXE) and observe
      output trace

  On Linux:

    - copy GLTRACE.INI to the current directory

    - if your original libGL.so is not installed in /usr/lib, modify 
      GLTRACE.INI's [Implementation] section to point to it

    - set LD_LIBRARY_PATH to where you installed gltrace's libGL.so

    - verify the gltrace libGL actually gets used using "ldd yourprogram"
      (possibly it's linked to libGL.so.1 instead of libGL.so)

    - run your program

    - BUGS: keyboard operation is not yet implemented
 _________________________________________________________________________

 Latest updates:

Version 2.3 alpha 2
  Merged in preliminary GLX support. NOTE: Key input does not yet work

  Added DisableExt option to disable extensions


Version 2.3 alpha
  Ported to 99% ANSI C with some POSIX functions

  Moved all OpenGL extensions to ext.h

  Added dll.c to wrap the dynamic module code for portability

  Added input.c to wrap the key press code for portability, and also support user specified keys

  If no output file is specified, logging now defaults to stdout

  Added user specified keys

  Added one frame key to capture all calls for one full rendering frame

  Added on-off key to toggle normal logging on and off

  Fixed an extension bug

  Added FPS counter with user selectable color and toggle on and off key

Version 2.2
  Several bug fixes.

  Cleaned out old, no longer used code.

  Added more extensions. ARB extensions 1, and 3 through 10 are completed.

Version 2.2 beta
  Speed much faster, especially when verbose logging!

  Merged in Keith Harrison's call counting code.

  Added the Verbose and LogTime options.

  Works on Windows 2000 now.

Version 2.0
  Fixed various bugs from original GLTRACE code.

  Added some support to trace extensions.

  Broke the single .cpp file into several smaller .c files

 _________________________________________________________________________

 Future plans:

  Port to Linux/Solaris

  Add more extensions

  Bring up to OpenGL 1.2 standard

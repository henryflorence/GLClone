use Switch;
if(!$ARGV[0]) { $filename = "defs.txt"; }
else { $filename = $ARGV[0]; }
print $filename, "\n";

open (FILE, $filename) or die "no file\n";

#copied and pasted from glTrace, index in this array is used as reference in packet
@funcIndex =
(
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
     # "glXGetProcAddressARB", - byzantine function definition
);

#read file retrieve function arguments store in hash
LINE: while (<FILE>) {
	#get next line of file, then rewind
	my $pos = tell;
 	$next = <FILE>;
 	seek FILE, $pos, 0;

	#skip non functiuon definitions
	next LINE if !(/.*?\(.*?\)/ && $next =~ /^\{/);
	
	($namepart, $parampart) = split / \(/;
	
	#specific to glTrace - I think
	$namepart =~ s/\GLAPIENTRY\ //;
	
	$name = (split / /, $namepart)[-1];
	
	#loose trailing )\n
	$parampart = substr $parampart, 0, -3;
	
	@params = split /\, /, $parampart;
	
	$functions{$name} = [@params];
}
close(FILE);

#open FILE, ">args.txt";
#foreach $arg (sort { $argCounts{$a} <=> $argCounts{$b} } keys %argCounts) { print FILE "\"$arg\" => \"\",\n"; }
#close FILE;

#created using 3 code lines above
%bufferTypes = (
"Font" => "",
"long*" => "LongV",
"GLXWindow" => "",
"Window" => "",
"GLXPbuffer" => "",
"LPLAYERPLANEDESCRIPTOR" => "",
"BOOL" => "Short",
"GLXContextID" => "",
"GLclampf*" => "FloatV",
"GLvoid**" => "",
"LPPIXELFORMATDESCRIPTOR" => "",
"Bool" => "Short",
"LPCSTR" => "",
"PIXELFORMATDESCRIPTOR*" => "",
"COLORREF*" => "",
"LPGLYPHMETRICSFLOAT" => "",
"GLXPixmap" => "",
"Pixmap" => "",
"long" => "Long",
"GLclampd" => "Double",
"XVisualInfo*" => "",
"GLbyte*" => "Byte",
"GLboolean*" => "Short",
"GLbitfield" => "Int",
"FLOAT" => "Float",
"UINT" => "Int",
"GLushort*" => "Short",
"GLclampf" => "Float",
"GLXFBConfig" => "",
"GLboolean" => "Short",
"HGLRC" => "",
"GLubyte*" => "ByteV",
"GLXDrawable" => "",
"GLubyte" => "Byte",
"GLushort" => "Short",
"GLuint*" => "IntV",
"void" => "",
"GLbyte" => "Byte",
"GLXContext" => "",
"DWORD" => "",
"int*" => "IntV",
"GLvoid*" => "",
"GLshort*" => "ShortV",
"HDC" => "",
"GLuint" => "Int",
"GLdouble*" => "DoubleV",
"int" => "Int",
"GLint*" => "IntV",
"Display*" => "",
"GLfloat*" => "FloatV",
"GLshort" => "Short",
"GLsizei" => "Long",
"GLdouble" => "Double",
"GLfloat" => "Float",
"GLint" => "Long",
"GLenum" => "Short",
);

%casts = (
"Short" => "NLushort",
"Byte" => "NLbyte",
"Long" => "NLulong",
"Double" => "NLdouble",
"Float" => "NLfloat",
"Int" => "NLushort",
);

%format = (
"Short" => "%i",
"Byte" => "%i",
"Long" => "%i",
"Double" => "%f",
"Float" => "%f",
"Int" => "%i",
);
sub NLcast {
	my $z = $bufferTypes{$_[0]};
	$z =~ s/V$//;
	my $y = $casts{$z};
	$y .= "*" if ($bufferTypes{$_[0]} =~ m/V$/);
	return $y;
}
#output casts
foreach (keys %bufferTypes) { print "$_ => $bufferTypes{$_} => ", (NLcast $_), "\n" if($bufferTypes{$_}); }

#parse arguments -> remove const, spot pointers, spot v, 2v, 3v functions
foreach $f (keys %functions) { 
	$args_ref = $functions{$f};
	@args = @$args_ref;
	
	$types{$f} = [];
	$names{$f} = [];
	foreach $arg (@args) {
		$arg =~ s/(const\ )|(CONST\ )|(unsigned\ )//;
		$arg =~ s/\ \*/\*\ /;
		
		($type, $name) = split / /, $arg;
		
		if(!($type eq "void")) {
			#check for collisions
			if(exists ($declarations{$name}) && $declarations{$name} ne $type) 
			{ $name = $name."_".$bufferTypes{$type} }
			$declarations{$name} = $type;			
		}
		
		push @{ $types{$f}}, $type;
		push @{ $names{$f}}, $name;
		$argCounts{$type}++;
	}
	$num = 0;
	if ($f =~ m/v$/) {
		$num = $f;
		$num =~ s/\D//g;
		if ($num eq "") { $num = 1 };
		#list the v functions, pointer type and number
		#print "$f - $num @{ $types{$f}}[-1]\n" 
	};
	$numv{$f} = $num;
	#print "$f - @{ $types{$f}}\n";
}

#prints counts of all the arguments by type
#foreach $arg (sort { $argCounts{$a} <=> $argCounts{$b} } keys %argCounts) { print "$arg - $argCounts{$arg}\n"; }

#print out all stored function info
#foreach $f (keys %functions) {	print "$f - @{ $types{$f}} - v:$numv{$f} - @{ $names{$f}}\n"; }

#for($i = 0; $i < @funcIndex; $i++) { 
	#missing function defs
	#if($numv{$funcIndex[$i]} eq "") { print "$i - $funcIndex[$i]\n"; }
	#info for buffer reader
	#print "$i - ", $f = $funcIndex[$i], " - @{ $types{$f}} - v:$numv{$f} - @{ $names{$f}}\n";
#}
sub NLcastF { return NLcast($_[0]) || $_[0]; }
open (FILE, ">out.txt");
$t = "\t\t\t";
print FILE "/*DECLARATIONS CREATED BY PERL SCRIPT*/\n";
print FILE "\t", NLcastF($declarations{$_}), " $_;\n" foreach (sort {
		NLcastF($declarations{$a}) cmp NLcastF($declarations{$b}) or $a cmp $b;
	} keys %declarations);
print FILE "\n\treadInt(buffer,count,index);";
print FILE "\n\twhile (index < 500 && count < NL_MAX_PACKET_LENGTH) {";
print FILE "\n\t",'printf("%s (",GLN[index]);',"\n";
print FILE "\n/*SWITCH CREATED BY PERL SCRIPT*/\n";
print FILE "\t\tswitch(index) {\n";

CASELINE: for($i = 0; $i < @funcIndex; $i++) { 
	next CASELINE if (!$names{$f = $funcIndex[$i]});
	print FILE $t,"case $i: //$f\n";
	
	$formatString = "";
	$formatArgs = "";
	for($j = 0; $j < @{ $types{$f}}; $j++) {
		# silently loose unknown argument types
		$type = $bufferTypes{$types{$f}[$j]};
		if($type ne "") {
			if($type =~ m/V$/) { 
				if($numv{$f} > 0) { $vpart = ','.$numv{$f}; }
				else { $vpart = ',1' } #function name did not include 3v, 2v etc
			} else { $vpart = ''; }
			$macroName = "read".$type;
			print FILE $t,"\tread",$type,"(buffer,count,",$names{$f}[$j],"$vpart);\n";
			$macroNames{$macroName}++;
			
			#generate format string for logging
			$isPtr = $type =~ s/V$//;
			$formatString .= $format{$type};
			$formatArgs .= ($isPtr ? "*" : "") . "(".lc(substr($type, 0, 1)).substr($type,1).") $names{$f}[$j]"; 
			
			$formatString .= ", " if($j < @{ $types{$f}} - 1);
			$formatArgs .= ", " if($j < @{ $types{$f}} - 1);
		} else {
			print FILE $t,"\t// unknown type for: $names{$f}[$j]\n";
		}
	}
	#$paramString = join(',', @{ $names{$f}});
	$paramString = "";
	for($j = 0; $j < @{ $names{$f}}; $j++) {
		$paramString .= "(@{ $types{$f}}[$j]) @{ $names{$f}}[$j]";
		$paramString .= "," if($j < @{ $names{$f}} - 1);
	}
    print FILE "#ifdef LOGTOFILE\n" if($formatString ne "");
	print FILE $t,"\tprint(\"$formatString\",$formatArgs);\n" if($formatString ne "");
    print FILE "#endif\n" if($formatString ne "");
	print FILE $t,"\t$f($paramString);\n";
	#"$i - ", $f = $funcIndex[$i], " - @{ $types{$f}} - v:$numv{$f} - @{ $names{$f}}\n";
	print FILE $t,"break;\n";
}
print FILE "\t\t}\n";
print FILE "/* FINISHED CREATION (<6 days)*/\n";
print FILE "/* MACRO NAMEs to be completed*/\n";
foreach (sort keys %macroNames) {
	print FILE "//used $macroNames{$_} times\n";
	$vpart = $_ =~ m/V$/ ? ",n" : "";
	print FILE "#define $_(x,y,z$vpart)\t\t{z = 0;y += 1}\n";
}
close (FILE);

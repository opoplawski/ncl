.\"
.\"	$Id: ncarg_cbind.m,v 1.2 1993-04-05 17:12:54 haley Exp $
.\"
.TH NCARG_CBIND 3NCARG "February 1993" NCAR "NCAR GRAPHICS"
.SH NAME
NCAR Graphics C-binding - Description of how to use the NCAR Graphics
C-binding.
.SH SYNOPSIS
The NCAR Graphics C-binding consists of a collection of C routines
that allow you to call NCAR Graphics Fortran routines from a C program
without having to worry about the C to Fortran interface yourself.
For detailed information about the C-binding for a particular routine,
please see the man page for the corresponding Fortran routine.
.sp
A C-binding has been created for every user entry point in the NCAR
Graphics utilities with the exception of some of the obsolete
utilities listed below.  A GKS level 0A C-binding has also been
created which adheres to the ISO/IEC standard.  
.sp
From here on, a \fIutility\fP C-binding refers to a C-binding for the 
NCAR Graphics utilities (AREAS, CONPACK, etc.).
.SH NAMING CONVENTION
For all of the utility user entry points, the name of the C-binding
function is the same as the Fortran routine name with a "c_"
prepended.  For example, the name of the C-binding for the CONPACK
routine \fBCPBACK\fP is \fBc_cpback\fP.  This naming convention does
not apply to the GKS C-bindings.  Instead, the C-binding names are
more descriptive, like \fBgset_fill_colr_ind\fP for which is
equivalent to the Fortran routine \fBGSFACI\fP.  Please see the man
page for the GKS Fortran routine to get the name of the corresponding
C-binding name.
.sp
The C programming language is case sensitive, so for programming
convenience the utility C-binding names and their arguments are all in
lower case, while the Fortran routines and their arguments will be
referred to in upper-case.
.SH ARGUMENT LISTS
The argument list of each utility C-binding corresponds with the
argument list of the corresponding Fortran routine except in some 
cases where multi-dimensioned arrays and/or character strings are
involved.  These exceptions will be described in separate
sections.
.sp
The C-binding argument list being similar to the Fortran argument
list makes using the C-bindings easier for people who already know 
the calling sequence of a particular Fortran routine.  For example, 
the NCAR Graphics routine \fBPLCHHQ\fP has the following argument list:
.sp
.nf
PLCHHQ(REAL X, REAL Y, CHARACTER*CHRS(*), REAL SZ, REAL ANG, REAL CNTR)
.fi
.sp
and the corresponding C-binding \fBc_plchhq\fP has the same type of
argument list:
.sp
.nf
c_plchhq(float x, float y, char *chrs, float sz, float ang, float cntr)
.fi
.sp
.SH ARRAYS
One of the exceptions to the utility C-binding argument lists has to do 
with multi-dimensioned arrays.  In Fortran, arrays are stored in
column-major order, while in C they are stored in row-major order.
This means that the subscripts of the array need to be switched before 
sending it to a Fortran routine from a C program.
.sp
As an example, the \fBCONPACK\fP routine \fBCPRECT\fP takes as one of
its arguments a two-dimensional array, \fIZDAT\fP.  If the Fortran
dimensions of \fIZDAT\fP are to be 25 x 14, then in the C program you
must dimension \fIZDAT\fP to be 14 x 25 before calling the C-binding
\fBc_cprect\fP.  The next three arguments of \fBCPRECT\fP describe the
array in question.  The first of these three arguments, \fIKZDT\fP, is
the first dimension of the array \fIZDAT\fP as it is declared in the
calling program.  For \fBCPRECT\fP this value would be 25, while for
the C-binding \fBc_cprect\fP, it would be 14.  The second and third
of these three arguments (\fIMZDT\fP and \fINZDT\fP) specify the
number of elements in each row and column (respectively) to be contoured.
If you only want to contour 20 x 10 of the array \fIZDAT\fP, then
\fIMZDT\fP and \fINZDT\fP should be 20 and 10 respectively in
\fBCPRECT\fP.  For \fBc_cprect\fP, the values would be 10 and 20
respectively.
.SH CHARACTER STRINGS
Another exception to the argument lists for the utility C-bindings has
to do with routines that return character strings.  The NCAR Graphics
routines that return strings do not have a string length as part of
their argument lists, so you must pass an extra argument to the
C-binding which specifies the maximum length of the string.  Also,
all input strings passed to the C-bindings must be null-terminated!
.sp
For example, in the routine \fBPCGETC\fP, you pass a parameter name
and it returns the value of that parameter which in this case is a
string.  The two arguments to \fBPCGETC\fP are \fIWHCH\fP and
\fICVAL\fP, where \fIWHCH\fP is the parameter name and \fICVAL\fP is
the string to be returned.  Since the C-binding \fBc_pcgetc\fP needs
to know the length of \fIcval\fP, an extra argument of type "int" must
be passed.  Thus, the arguments for \fBc_pcgetc\fP would be
\fIwhch\fP, \fIcval\fP, and \fIlen\fP, where \fIlen\fP is the length
of \fIcval\fP as it is declared in the C program.  In any case of
having to add an extra argument for the string length, the extra
argument will always be the last one in the list.  If more than one
string length argument needs to be added, then each one should be added 
at the end of the argument list in the order that their corresponding
strings appear.
.SH FUNCTION PROTOTYPES
The C-bindings are intended to be ANSI C compliant.  To get the
correct function prototypes for the utility C-bindings, you can
include <ncarg/ncargC.h>.  For the GKS C-bindings, include
<ncarg/gks.h>.  In some cases, it may be necessary to typecast the
arguments in the utility C-bindings to get the prototypes correct.
For example, the C-bindings do not distinguish between singly and
multiply dimensioned arrays, so if you are passing a multiply dimensioned
float array, you may need to typecast it as a (float *) if this is not how
it is declared in the main program.
.SH COMPILING YOUR PROGRAM
To compile your NCAR Graphics C program with the C-bindings, use the
NCARG application \fBncargcc\fP.  \fBncargcc\fP will take care of loading in
the necessary C/Fortran interface libraries as well as the NCAR
Graphics C and Fortran libraries.  If you do not wish to use
\fBncargcc\fP, then you can just run it with no arguments to see what
the necessary libraries are, and then put this information your
Makefile or whatever else you are using to compile and link your program.
.SH EXAMPLES
A few examples of C programs that call the NCAR Graphics C-bindings
have been provided for your convenience.  You may be familiar with the
output from these C programs as they were modeled after the Fortran
programs that you can get with the \fBncargex\fP command.  To copy
any one of these C programs in your directory, and then compile, link, 
and run it,  type \fBncargcex\fP \fIxxxx\fP, where \fIxxxx\fP is the 
name of the example you wish to look at.  The following examples are 
available:
.sp
.nf
.in .5i
c_agex07 c_cbex01 c_colcon c_eezmpa c_elblba
c_epltch c_mpex05 c_sfex02 c_slex01
.sp
If you want to generate all of these examples, type \fBncargcex -A\fP.
\fBncargcex\fP uses \fBncargcc\fP to compile and link the C-binding
example programs.
.sp
.in -.5i
.fi
.SH OBSOLETE ROUTINES
C-bindings have not been provided for user entry points in the 
following obsolete utilities:
.sp
.in .5i
CONRAN, CONREC, CONTERP, HAFTON, ISOSFHR, PWRITX, PWRITY
.in -.5i
.SH CAVEATS
You may not be able to pass a NULL pointer in place of a character string.
If you do not wish to pass a string, just pass "", which is a valid
character string of length zero.
.SH SEE ALSO
Online: 
ncargcc (1NCARG), ncargex (1NCARG), ncarg_gks_cbind (3NCARG), 
man pages for any of the user entry points and/or GKS routines.
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version; User's Guide for 
NCAR GKS-0A Graphics
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved


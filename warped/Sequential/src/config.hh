#ifndef CONFIG_HH
#define CONFIG_HH

//-*-c++-*-
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT
// WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND,
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
//
// IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
// WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
// REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR
// DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL
// DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM
// (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED
// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF
// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER
// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
// $Id: config.hh,v 1.1 1998/12/22 20:35:49 dmadhava Exp $
//
//
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
//The following section allows the configuration of the warped kernel.  In
//each section, several choices are available.  For each category, two
//symbols must be defined.  The first symbol must be defined to be the
//name of the class being instantiated.  The second symbol is the class
//name, all in uppercase.  This is to allow the appropriate header files
//to be included.
//----------------------------------------------------------------------------


// This flag is used to check if the compiler has an inbuilt boolean type.
// If the compiler does not (as in SunPro, CC), comment the following
// line.
#ifndef HAS_BOOLEAN
#define HAS_BOOLEAN
#endif

// Diplays the simulation configuration i.e. which optimizations/algorithms
// are being used for that simulation run. The configuration is displayed
// at the begining of the simulation
//#define DISPLAY_SIM_CONFIG


#ifndef ASSERT
#ifdef DEVELOPER_ASSERTIONS
#include <cstdio>
#include <cstdlib>
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif
#endif

#endif


//-*-c++-*-
#ifndef SIMPLE_STACK_HH
#define SIMPLE_STACK_HH
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// Author: Dale Martin (dmartin@thor.ece.uc.edu)
// Name: SimpleStack.hh
// Description: Header file for a simple stack template
//
// $Id: SimpleStack.hh,v 1.1 1998/12/01 14:53:51 dmartin Exp $
//
//      This library is free software; you can redistribute it and/or
//      modify it under the terms of the GNU Library General Public
//      License as published by the Free Software Foundation; either
//      version 2 of the License, or (at your option) any later version.
//     
//      This library is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//      Library General Public License for more details.
//     
//      You should have received a copy of the GNU Library General Public
//      License along with this library; if not, write to the Free
//      Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// 
//
// Change Log
//---------------------------------------------------------------------------
// $Log: SimpleStack.hh,v $
// Revision 1.1  1998/12/01 14:53:51  dmartin
// Moved all of the "utils" to the common subdirectory.  Changed some
// #includes to reflect this.
//
// Started an "install" target for the Sequential kernel.
//
// Is anyone trying these changes, btw?  I've not heard any complaints or
// otherwise, and that frightens me a little :-)
//
// Revision 1.1  1998/03/07 21:11:58  dmadhava
// Moved the files in the old "warped" directory down by a level. The new
// hierarchy is
//
//       warped
//         |
//         +----- TimeWarp (The files in warped are now here)
//         |
//         +----- Sequential
//         |
//         +----- NoTime
//
// Revision 1.1.1.1  1995/08/14 20:23:18  dmartin
// The warped system.
//
// Revision 1.1  1995/07/05 16:36:17  dmartin
// Some utility classes to use with warped.
//
// Revision 1.3  1995/01/26  12:10:48  dmartin
// Changed header comments.
//
// Revision 1.2  1995/01/25  22:09:31  dmartin
// Took out all references to numElements and size() to make this a truly
// simple stack.
//
// Revision 1.1  1995/01/25  22:03:32  dmartin
// Cool Stack Class
//
//---------------------------------------------------------------------------
static const char* simple_stack_hh_rcsid = "$Id: SimpleStack.hh,v 1.1 1998/12/01 14:53:51 dmartin Exp $";

template < class Element >
struct Container {
  Container(){};
  ~Container(){};
  Container< Element >* next;
  Element data;
  Container( Element & element ) : data(element){};
};

template < class Element >
class SimpleStack {

public:
  SimpleStack();
  ~SimpleStack();
  Element pop( );
  void push(Element);
  
private:
  Container< Element > * handle;
  static Container< Element > * freeList;
};
#include "SimpleStack.cc"
#endif

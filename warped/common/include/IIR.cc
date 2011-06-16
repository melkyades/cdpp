//-*-c++-*-
#ifndef IIR_CC
#define IIR_CC
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// Author: Dale Martin
// Name: IIR.cc
// Description: template class for IIR filter
//
// $Id: IIR.cc,v 1.1 1998/12/01 14:53:49 dmartin Exp $
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
// $Log: IIR.cc,v $
// Revision 1.1  1998/12/01 14:53:49  dmartin
// Moved all of the "utils" to the common subdirectory.  Changed some
// #includes to reflect this.
//
// Started an "install" target for the Sequential kernel.
//
// Is anyone trying these changes, btw?  I've not heard any complaints or
// otherwise, and that frightens me a little :-)
//
// Revision 1.1  1998/03/07 21:11:55  dmadhava
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
// Revision 1.4  1997/11/11 20:49:41  cyoung
// Added functions to allow the filter to be examined and modified
// (using functions to get and set the current weights and average).
//
// Revision 1.3  1995/08/17 17:18:11  tmcbraye
// Modified to compile cleanly with -Wall.
//
// Revision 1.2  1995/08/15 20:45:25  dmartin
// Update with Chris's changes.
//
// Revision 1.1.1.1  1995/08/14 20:23:10  dmartin
// The warped system.
//---------------------------------------------------------------------------

// IIR.cc
// This class implements a first order IIR filter.  To use it,
// #include "IIR.cc" into whatever file you want to have the
// class in.  Then you need a statement similar to the following
// for a declaration:
//       IIR<float> filter(.25,.75)
// Replace "float" with whatever input/output type you want to
// use, and ".25" with the weight you want for the old average,
// and ".75" with your weight for the new data.  The weights must
// add up to 1.0, or you will get an error message when the
// filter is initialized.

#include <iostream>

template <class Type> class IIR {
public:
  IIR(float old_weight_in = .4, float new_weight_in = .6);
  inline void update(Type);
  inline Type getAvg();
  inline void clear() {running_avg = 0;};
  inline void set_weights(float, float);
  inline void set_weights_and_avg( float, float, Type);
  inline float get_old_weight() { return old_weight; };
  inline float get_new_weight() { return new_weight; };
private:
  float old_weight, new_weight;
  Type running_avg, new_avg;
};


// This is the constructor code for the class IIR.  The
// arguments passed in are the coeffiecents of the filter.
// They are floats and they need to add up to 1.0, if you don't
// want an error message, and you weant your filter definition to
// make sense.
template <class Type>
IIR<Type>::IIR(float old_weight_in, float new_weight_in) {
  new_weight = new_weight_in;
  old_weight = old_weight_in;
  if (new_weight + old_weight != 1.0) {
    cerr << "The weights entered into IIR don't add up to 1!" << endl;
  }
  running_avg = 0;
  new_avg = 0;
}

template <class Type>
inline void
IIR<Type>::set_weights_and_avg(float old_weight_in, float new_weight_in, 
			       Type avg) {
  new_weight = new_weight_in;
  old_weight = old_weight_in;
  if (new_weight + old_weight != 1.0) {
    cerr << "The weights entered into IIR don't add up to 1!" << endl;
  }
  running_avg = avg;
  new_avg = 0;

}



// This is used to reset the weights to other values
template <class Type>
inline void IIR<Type>::set_weights(float newOld, float newNew) {
  new_weight = newNew;
  old_weight = newOld;
}


// This is the method to enter new data into the IIR filter.  The
// argument is the actual data, and should be whatever type the
// template class was declared with.  This function returns nothing.
template <class Type>
inline void IIR<Type>::update(Type new_sample) {
  running_avg = (Type)(old_weight*running_avg + new_weight*new_sample);
}


// This is the method to read the output of the filter.  It returns the
// running average in the type the the template was declared with.

template <class Type>
inline Type IIR<Type>::getAvg() {
  return(running_avg);
}
#endif
  

#ifndef FIR_CC
#define FIR_CC

/*
 Copyright (c) 1994 Ohio Board of Regents and the University of
 Cincinnati.  All Rights Reserved.


 Change Log
------------------------------------------------------------------
 $Log: FIR.cc,v $
 Revision 1.3  1999/07/16 17:57:58  dmartin
 Removed a debug statement.

 Revision 1.2  1999/07/16 17:49:39  dmartin
 Fixed a bug introduced in 1995 that was making the results of this filter
 incorrect.  Fixed it in a manner so it's still "rollover safe" in terms
 of number of data points entered.

 Revision 1.1  1998/12/01 14:53:48  dmartin
 Moved all of the "utils" to the common subdirectory.  Changed some
 #includes to reflect this.

 Started an "install" target for the Sequential kernel.

 Is anyone trying these changes, btw?  I've not heard any complaints or
 otherwise, and that frightens me a little :-)

 Revision 1.1  1998/03/07 21:11:52  dmadhava
 Moved the files in the old "warped" directory down by a level. The new
 hierarchy is

       warped
         |
         +----- TimeWarp (The files in warped are now here)
         |
         +----- Sequential
         |
         +----- NoTime

 Revision 1.5  1997/11/13 23:37:37  gsharma
 Modified some code to make FIR.cc CC compiler compliant.

 Revision 1.4  1995/10/03 15:18:43  cyoung
 bug fix on the FIR. (fixed a problem with it overflowing).

 Revision 1.3  1995/08/17 17:18:13  tmcbraye
 Modified to compile cleanly with -Wall.

 Revision 1.2  1995/08/15 20:45:27  dmartin
 Update with Chris's changes.

 Revision 1.2  1995/08/15 16:42:59  cyoung
 Bug fix that could cause a bad conversion for unsigned ints.

 Revision 1.1.1.1  1995/08/14 20:23:09  dmartin
 The warped system.
------------------------------------------------------------------
*/

// FIR.cc
// This template class implements an nth order FIR filter, where n
// is specified by the user.  To use it, #include "FIR.cc" at the top of
// the file you'd like to use it in.  Then include a declaration similar
// to the following:
// FIR<double> filter(3)
//
// Replace "double" with whatever type you'd like it to input and output,
// and "3" with whatever order filter you'd like to use.  If you specify
// a 0 or a 1 for a filter order, an error message is generated.

#include <iostream.h>

template <class Type> class FIR 
{

public:
  FIR(int n = 1000);
  ~FIR();
  inline Type getAvg();
  inline void clear();
  inline int numDatapoints() { return(num_valid); };
  inline void update(Type);
  
private:
  unsigned int size, num_valid, index;
  Type current_sum;
  Type *fir_array;
};


// This is the constructor code for the class FIR. The integer passed into
// it is the order of the filter.
template <class Type>
inline FIR<Type>::FIR(int n) { 
  if (n <= 1) {
    cerr << "A window size of " << n <<" makes NO sense!" << endl;
  }

  size = n;
  num_valid = 0;
  current_sum = 0;
  index = 0;

  fir_array = new Type[size];
  for (register unsigned int i = 0; i < size;i++) {
    fir_array[i] = 0;
  }
}


// This is the destructor method for the class FIR.  It deletes the
// dynamically created array pointed at by "fir_array."
template <class Type>
inline FIR<Type>::~FIR() {
  delete [] fir_array;
}


template <class Type>
inline void 
FIR<Type>::clear() {
  num_valid = 0;
  for (register unsigned int i = 0; i < size; i++) {
    fir_array[i] = 0;
  }
}


// This is the method which calculates and returns the current
// output of the filter.  If the filter isn't full yet, then the
// average is computed on the valid data only.  The return type is
// whatever the template was initialized with.  

template <class Type>
inline 
Type FIR<Type>::getAvg() {
  if( num_valid < size ) {
    if( num_valid != 0 ){
      return ( current_sum / num_valid );
    }
    else{
      return 0;
    }
  }
  else{
    return ( current_sum / (Type)size );
  }
}


// This is the method that updates the current state of the filter with
// a new data point.  The type of the new data should be the same as 
// the type that the filter was initialized with.  The variable
// num_valid will overflow if this method is called more than 2*INT_MAX
// times.  To avoid an extra computation, this check isn't made by the 
// method.
template <class Type>
inline void FIR<Type>::update(Type new_element) {
  current_sum = current_sum + new_element;
  current_sum = current_sum - fir_array[index];
  fir_array[index] = new_element;
  if (num_valid < size){
    num_valid++;
  }
  index++;
  if(  index == size ){
    index = 0;
  }
}
#endif

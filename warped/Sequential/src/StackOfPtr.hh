#ifndef __STACKOFPTR_HH__
#define __STACKOFPTR_HH__

#include <iostream>

template <class Element>
class StackOfPtr {

private:
  Element** ptrArray;
  int ptrArrayIndex;
  int size;

public:

  inline StackOfPtr() {
    ptrArray = new Element* [1000];
    ptrArrayIndex = 0;
    size = 1000;
  }

  inline ~StackOfPtr() {
    delete [] ptrArray;
  }

  inline void insert(Element* ptr) {
    if(ptrArrayIndex == size) {
      Element** tmpPtrArray = new Element* [size*2];
      size = size*2;
      for(int i=0; i < size; i++) {
	tmpPtrArray[i] = ptrArray[i];
      }
      delete [] ptrArray;
      ptrArray = tmpPtrArray;
    }
    ptrArray[ptrArrayIndex] = ptr;
    ptrArrayIndex++;
  }

  inline Element* remove() {

    Element* ptr = NULL;

    if (ptrArrayIndex > 0) {
      ptrArrayIndex--;
      ptr = ptrArray[ptrArrayIndex];
      ptrArray[ptrArrayIndex] = NULL;
    }
    else {
      return NULL;
    }

    return ptr;
  }

  inline Element* top() const {
    return ptrArray[ptrArrayIndex-1];
  }
};

#endif //__STACKOFPTR_HH__

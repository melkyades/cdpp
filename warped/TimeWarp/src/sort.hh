#ifndef SORT_HH
#define SORT_HH

#ifndef QUICKSORT_CUT_OFF	// defines where quick sort stops and 
#define QUICKSORT_CUTOFF 10	// Insertion Sort takes over.
#endif

#define MAX_TIME 25		// 
#define MAX_ELEMS 25

#include <iostream.h>
#include <stdlib.h>
#include "BasicEvent.hh"

void
swap(BasicEvent*& a,BasicEvent*& b) {

  BasicEvent* temp = a;
  a = b;
  b = temp;
};
// Shuffles the elements in the array such that, all elements <= elem
// lies before elem.

int 
partition(BasicEvent* inpArray[], int numElem) {
  BasicEvent* part = inpArray[0];
  int i,j;

  i = -1;
  j = numElem;
  while(i<j) {
    do 
      j--; while(part->recvTime < inpArray[j]->recvTime);
    do 
      i++; while(inpArray[i]->recvTime < part->recvTime);
    if(i < j)
      swap(inpArray[i],inpArray[j]);
  }
  return j+1;
};

// Prepares the array for Partition by the partiton algorithm

void 
medianOfThree(BasicEvent *inpArray[], int numElem) {
  if (numElem >= 3) {
    BasicEvent* begin = inpArray[0];
    BasicEvent* middle = inpArray[numElem/2];
    BasicEvent* end = inpArray[numElem -1];

    if(begin->recvTime < middle->recvTime) {
      if(middle->recvTime < end->recvTime)
	swap(begin,middle);	// middle is the median
      else 
	if(begin->recvTime < end->recvTime)
	  swap(begin,end);	// end is the median
    }
    else {
      if(middle->recvTime < end->recvTime) {
	if(begin->recvTime >= end->recvTime)
	  swap(begin,end);
      }
      else 
	swap(begin,middle);
    }
  }
};

// Used by Quick sort to sort the array
int
qSortHelper(BasicEvent* inpArray[], int numElem) {
  int left = 0;
  while(numElem > 1) {
    medianOfThree(inpArray+left,numElem);
    int part = partition(inpArray+left,numElem);
    qSortHelper(inpArray+left,part);
    left += part;
    numElem -= part;
  }
};
  
void 
insertionSort(BasicEvent* inpArray[],int numElem) {
  int i,j;

  for(i=1;i<numElem;i++) {
    if(inpArray[i]->recvTime < inpArray[i-1]->recvTime) {
      BasicEvent* temp = inpArray[i];
      for(j=i;j && temp->recvTime<inpArray[j-1]->recvTime;j--)
	inpArray[j] = inpArray[j-1];
      inpArray[j] = temp;
    }
  }
};

// Sorts the array inpArray with numElem elements using Quck sort
// Or insertion sort (if the array is small)
// Assume the elements in InpArray are BasicEvents
// Returns an array of sorted BasicEvents 

void 
sortArray(BasicEvent* inpArray[],int numElem) {
  if(numElem > QUICKSORT_CUTOFF)
    qSortHelper(inpArray,numElem);
  else 
    insertionSort(inpArray,numElem);
};


void
createArray(BasicEvent *array[],int arrayLen) {
  int i;

  srand(MAX_TIME);
  for(i=0;i<arrayLen;i++) {
    array[i] = new BasicEvent;
    array[i]->recvTime = rand();
  }
};


void 
printArray(BasicEvent* array[],int arrayLen) {
  int i;
  for(i=0;i<arrayLen;i++)
    cout << "Array[" << i << "]" << " = " << array[i]->recvTime << endl;
  cout.flush();
};

#include "sort.cc"  
#endif


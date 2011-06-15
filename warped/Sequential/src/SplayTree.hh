#ifndef _SPLAY_TREE_HH_
#define _SPLAY_TREE_HH_

// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.

// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 

// IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING 
// WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR 
// REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR 
// DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL 
// DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM 
// (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED 
// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF 
// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER 
// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

// Authors: Philip A. Wilsey            phil.wilsey@uc.edu
//          Malolan Chetlur             mal@ececs.uc.edu
//---------------------------------------------------------------------------
#include "InputQ.hh"
#include "TreeElement.hh"
#include "StackOfPtr.hh"

class BasicEvent;

class SplayTree : public InputQ {

public:

  SplayTree();
  ~SplayTree();

  void insert(BasicEvent*);
  BasicEvent* getEvent();
  BasicEvent* peekEvent();
  void garbageCollect(VTime* gTime);

private:

  //Tree Element Denoting the root of the SplayTree
  TreeElement* root;

  //Tree Element denoting the current Element that is with the
  //Least Time and is the current (next) element to be processed
  TreeElement* current;

  StackOfPtr<TreeElement> processedElements;

  //Number of elements in the Q
  int numberOfElements;

  //Private functions...
  void splay(TreeElement* Event);

  TreeElement* rotateLeft(TreeElement* treeItem);
  TreeElement* rotateRight(TreeElement* treeItem);

};

#endif

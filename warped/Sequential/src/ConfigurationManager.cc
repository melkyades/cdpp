#include "ConfigurationManager.hh"
#include "SplayTree.hh"
#include "SequentialInputQueue.hh"

int ConfigurationManager::inputQConfigFileOpen = 0;

ifstream ConfigurationManager::inputQConfigFile;

//This function gets the Q implementation for the whole of Simulation kernel
//Assumes Single Q is used by all objects
//Interface got to be changed when seperate Q's are used by different objects

//Currently the Q implementation is hardcoded. 
//Sooner a test file or command line will be interpreted to decide between
//several Q implementations.

InputQ* 
ConfigurationManager::getQImplementation() {

  if(inputQConfigFileOpen == 0) {
    inputQConfigFileOpen = 1;
    openInputQConfigFile();
  }

  int qType = -1;

  if(inputQConfigFile.good()) {
    inputQConfigFile >> qType;
  }

  switch (qType) {

  case LINEARQ:
    return new SequentialInputQueue;
    break;

  case SPLAYTREE:
    return new SplayTree;
    break;

  default:
    //Splay tree is chosen as the default implementation
    return new SplayTree;
    break;
  };
  //I should not be here
  return NULL;
}

void
ConfigurationManager::openInputQConfigFile() {
  inputQConfigFile.open("InputQConfig");
  if(inputQConfigFile.bad() || inputQConfigFile.fail()) {
    //If not able to find the file, default SplayTree Q implementation 
    //is assumed
  }
}

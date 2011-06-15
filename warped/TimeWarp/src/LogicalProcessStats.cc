#include "LogicalProcessStats.hh"

#if (defined(MESSAGE_AGGREGATION) && defined(STATS))

int logicalProcessStats::window_size_exhausted = 0;
int logicalProcessStats::waited_too_long = 0;
int logicalProcessStats::waited_long_trigger = 0;
int logicalProcessStats::received_control_message = 0;
int logicalProcessStats::received_anti_message = 0;
int logicalProcessStats::write_buffer_full = 0;
int logicalProcessStats::got_roll_back = 0;
VTime logicalProcessStats::overoptimistic_lookahead = 0;
int logicalProcessStats::lookaheadCount = 0;

#endif

logicalProcessStats::logicalProcessStats(int numberOfObjects){
  int i;
  totalEventsProcessed = 0;
  totalEventsCommitted = 0;
  totalEventsUndone = 0;
  objectNum = 0;
  eventIds = new int[numberOfObjects];
  for(i=0;i < 22; i++){
    kernelMsgs[i] = 0;
  }
  
  numPhyMsgsRecv  = 0;
  numPhyMsgsSent  = 0;
  numApplMsgsRecv = 0;
  numApplMsgsSent = 0;
#if (defined(MESSAGE_AGGREGATION) && defined(STATS))
  for(i=0; i < MAX_AGGREGATE_SIZE; i++){
    statsArray[i] = 0;
  }
#endif
  
};


void
logicalProcessStats::printNumberOfKernelMessages(ostream* statsFile){
    *statsFile << "-----------------------------------------------------------------------" << endl;    
    *statsFile << " *** Kernel Messages Sent/Received through MPI ***" << endl;
    *statsFile << "-----------------------------------------------------------------------" << endl;
    *statsFile <<"TERMN EVENTMSGS LGVTMSG LLGVTMG NEWGVT GVTACK LPACK  RCHKPT" 
	       << endl;
    *statsFile << "-----------------------------------------------------------------------" << endl;
    for(int i=1; i < 22; i++){
      switch(i) {
      case 3:
	*statsFile << "[" << setw(3) << kernelMsgs[i] << "] ";
	break;
      case 5:
	*statsFile << "[" << setw(7) << kernelMsgs[i] << "] ";
	break;
      case 6:
      case 7:
	*statsFile << "[" << setw(5) << kernelMsgs[i] << "] ";
	break;
      case 8:
      case 9:
      case 10:
	*statsFile << "[" << setw(4) << kernelMsgs[i] << "] ";
	break;
#ifdef MATTERNGVTMANAGER
      case 13:
	*statsFile << endl << "Number of MATTERNGVTMSG : " << kernelMsgs[i] 
		  << endl;
	break;
      case 14:
        *statsFile << "Number of RESTORECKPTMSG : " << kernelMsgs[i] << endl;
	break;
      case 20:
	*statsFile << "Number of EXTAGENTMSG : " << kernelMsgs[i] << endl;
	break;
#else
      case 13:
	*statsFile << "[" << setw(4) << kernelMsgs[i] << "]" << endl;
	*statsFile << "-----------------------------------------------------------------------" << endl;
	break;
#endif
      default:
	break;
      }
    }
  };

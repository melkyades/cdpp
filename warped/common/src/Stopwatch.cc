#ifndef STOPWATCH_CC
#define STOPWATCH_CC
#include "Stopwatch.hh"

  ////////////////////////////////
  //  Private Member Functions  //
  ////////////////////////////////

inline double 
Stopwatch::timeDifference(timeval& start, timeval& stop){
  double d1, d2;
  d1 = (double)start.tv_sec + 1e-6*((double)start.tv_usec);
  d2 = (double)stop.tv_sec + 1e-6*((double)stop.tv_usec);
  // return result in nanoseconds
  return ( (d2-d1)*1e9 );
}

  ////////////////////
  //  Constructors  //
  ////////////////////

Stopwatch::Stopwatch(){
  started = false;
  stopped = false;
  lap = false;
}

Stopwatch::~Stopwatch(){
}

  ////////////////////////
  //  Member Functions  //
  ////////////////////////

void 
Stopwatch::start(){
  started = true;
  stopped = false;
  lap = false;
#if defined(__SVR4) && defined(sun)
    startValue = gethrtime(); // call solaris timer
#else
  gettimeofday(&start_tv,&start_tz);
#endif
}

void 
Stopwatch::stop(){
  if(started == false) {
    if(stopped == true) cerr << "Stopwatch has already been stopped.\n";
    else cerr << "Stopwatch has not been started yet.\n";
  }
  else {
#if defined(__SVR4) && defined(sun)
    stopValue = gethrtime(); // call solaris timer
#else
    gettimeofday(&stop_tv,&stop_tz);
#endif
    stopped = true;
    started = false;
    lap = false;
  }
}

#if defined(__SVR4) && defined(sun)
void
Stopwatch::reset(){
  stopped = true;
  started = false;
  lap = false;
  startValue = stopValue;
}

double
Stopwatch::elapsed(){
  if(stopped == true) {
    return ( double(stopValue - startValue) );
  }
  else if(started == true) {
    tempValue = gethrtime();
    return ( double(tempValue - startValue) );
  }
  else {
    cerr << "Stopwatch has not been started yet.\n";
    return (double) 0;
  }
}

double 
Stopwatch::lapTime(){
  if (started == true) {
    tempValue = gethrtime();
    tempValue2 = lapValue;
    lapValue = tempValue;
    if (lap == true) {
      return ( double(tempValue - tempValue2) );
    }
    else {
      lap = true;
      return ( double(tempValue - startValue) );
    }
  } else {
    cerr << "Stopwatch has not been started yet.\n";
    return (double) 0;
  }
}
#else
void
Stopwatch::reset(){
  stopped = true;
  started = false;
  lap = false;
  start_tv = stop_tv;
}

double 
Stopwatch::elapsed(){
  if(stopped == true) {
    return timeDifference(start_tv,stop_tv);
  }
  else if(started == true) {
    gettimeofday(&temp_tv,&temp_tz);
    return timeDifference(start_tv,temp_tv);
  }
  else {
    cerr << "Stopwatch has not been started yet.\n";
    return (double) 0;
  }
}

double 
Stopwatch::lapTime(){
  if (started == true) {
      gettimeofday(&temp_tv,&temp_tz);
      temp2_tv = lap_tv;
      lap_tv = temp_tv; // Time zone does not need to be modified.
    if (lap == true) {
      return timeDifference(temp2_tv,temp_tv);
    }
    else {
      lap = true;
      return timeDifference(start_tv,temp_tv);
    }
  } else {
    cerr << "Stopwatch has not been started yet.\n";
    return (double) 0;
  }
}
#endif
#endif

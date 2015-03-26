

//
// DLL-to-CycleComputer class interface declarations
//
// Author: Leander Rodriguez
//


#ifndef  cycleComputer_interface_H
#define  cycleComputer_interface_H

#include <windows.h>


extern "C"
{
  int     __declspec(dllexport)  createCycleComputer( int  tireSize );
  int     __declspec(dllexport)  destroyCycleComputer( int  handle );
  int     __declspec(dllexport)  tickSec( int  handle );
  int     __declspec(dllexport)  tickTire( int  handle );
  double  __declspec(dllexport)  distance( int  handle );
  double  __declspec(dllexport)  getVelocity( int  handle, int  type );
  int     __declspec(dllexport)  timeElapsed( int  handle );
  int     __declspec(dllexport)  timeTrip( int  handle );
  int     __declspec(dllexport)  resetCycleComputer( int  handle );
}


#endif

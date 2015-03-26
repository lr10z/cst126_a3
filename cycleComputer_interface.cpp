

//
// DLL-to-CycleComputer class interface file
//
// Author: Leander Rodriguez
//
// ---
//
// These interface routines provide the mappings
// between the DLL function interface and the CycleComputer
// class interface. Also, a map container is used to
// associate a game handle with each game in progress.
// This allows multiple instances of the game to be
// played simultaneously on the same computer.
//


//#include  <time.h>
#include  <map>
#include  "cycleComputer.h"
#include  "cycleComputer_interface.h"


typedef  std::map<int, CycleComputer>  CycleComputerContainer;


namespace
{
  //
  // Cycle computer container and handle tracker
  //
  CycleComputerContainer  computers;
  int                     lastHandle = 0;
}


//
// Constructor interface
//
int  createCycleComputer( int  tireSize )
{
  int  handle  =  ++lastHandle;

  computers.insert( std::pair<int, CycleComputer>( handle,
                                                   CycleComputer( tireSize ) ) );

  return  handle;
}


//
// Destructor interface
//
int  destroyCycleComputer( int  handle )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter != computers.end() )
    computers.erase( iter );

  return 0;
}


//
// Interface to the CycleComputer::tickSec() member function
//
int  tickSec( int  handle )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter != computers.end() )
  {
    iter->second.tickSecond();
  }

  return 0;
}


//
// Interface to the CycleComputer::tickTire() member function
//
int  tickTire( int  handle )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter != computers.end() )
  {
    iter->second.tickTireRotation();
  }

  return 0;
}


//
// Interface to the CycleComputer::distance() member function
//
double  distance( int  handle )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter == computers.end() )
    return  0.0;

  return  iter->second.distanceTraveled();
}


//
// Interface to the CycleComputer::getVelocity() member function
//
double  getVelocity( int  handle, int  type )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter == computers.end() )
    return  0.0;

  return  iter->second.velocity( CycleComputer::VelocityType(type) );
}


//
// Interface to the CycleComputer::timeElapsed() member function
//
int  timeElapsed( int  handle )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter == computers.end() )
    return  0;

  return  iter->second.elapsedTime();
}


//
// Interface to the CycleComputer::tripTime() member function
//
int  timeTrip( int  handle )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter == computers.end() )
    return  0;

  return  iter->second.tripTime();
}


//
// Interface to the CycleComputer::tickSec() member function
//
int  resetCycleComputer( int  handle )
{
  CycleComputerContainer::iterator  iter  =  computers.find( handle );

  if  (iter != computers.end() )
  {
    iter->second.resetTrip();
  }

  return 0;
}

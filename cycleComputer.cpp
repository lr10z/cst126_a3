//
// CST 126 - Assignment #3
//
// Author: Leander Rodriguez
//
// Definitions for CycleComputer class
//
// File: cycleComputer.cpp
//


#include  "cycleComputer.h"


//
// Some useful constants and macros
//
#define  PI                    3.1415927
#define  RADIUS(diameter)      ( (diameter) / 2.0 )
#define  FEET_PER_MILE         5280.0
#define  INCHES_PER_FOOT       12.0
#define  INCHES_PER_MILE       ( INCHES_PER_FOOT * FEET_PER_MILE )
#define  SECONDS_PER_HOUR      3600.0


//
// Constructor
//
CycleComputer::CycleComputer( double  diameter )
{
  m_circumference  =  2.0 * PI * RADIUS( diameter );   // in inches

  m_circumference  /=  INCHES_PER_MILE;   // in miles

  //
  // All other member variables are initialized by the resetTrip() function
  //
  resetTrip();
}


//
// Member function: tickSecond()
//
// Records the passage of a second in time.
//
void  CycleComputer::tickSecond()
{
  //
  // Increment the elapsed time counter.
  //
  ++m_numSecondsElapsed;


  //
  // If there was at least one tire rotation during the
  // previous second (meaning the bicycle is moving), increment the
  // trip time, and add the rotations during the previous second to the
  // total trip rotations (so the distance traveled can be calculated
  // in another member function).
  //
  if  ( m_numRotationsThisSecond  !=  0 )
  {
    ++m_numSecondsTrip;

    m_numRotationsThisTrip    +=  m_numRotationsThisSecond;
  }


  //
  // Calculate current velocity
  //
  m_curMPH  =  (m_numRotationsThisSecond * m_circumference) / (1 / SECONDS_PER_HOUR);

  //
  // Update the max mph if it's less than the current mph
  //
  if  (m_curMPH > m_maxMPH)  
    m_maxMPH  =  m_curMPH;

  //
  // Reset the rotations counter so the rotations in the next second can be tracked.
  //
  m_numRotationsThisSecond   =  0;
}


//
// Member function: tickTireRotation()
//
// Records the occurance of a tire rotation.
//
void  CycleComputer::tickTireRotation()
{
  ++m_numRotationsThisSecond;
}


//
// Member function: distanceTraveled()
//
// Returns the distrance traveled so far during the trip.
//
double  CycleComputer::distanceTraveled()
{
  double  distance  =  m_numRotationsThisTrip * m_circumference;  // in miles

  return  distance;
}


//
// Member function: velocity()
//
// Returns either the current, average, or maximum velocity (mph)
//
double  CycleComputer::velocity( VelocityType  type )
{
  switch( type )
  {
    case  current_  :  return  m_curMPH;           break;
    case  average_  :  return  averageVelocity();  break;
    case  maximum_  :    return  m_maxMPH;         break;
  }

  return  0.0;
}


//
// Member function: averageVelocity()
//
// Returns the average velocity (mph) for the current trip.
// This is calculated as trip distance divided by the
// trip time.
//
double  CycleComputer::averageVelocity()
{
  if  (tripTime() == 0)
  {
    //
    //  If hours is 0, then there's no distance, so just return
    //  0 for the average.  This is done to avoid a divide by
    //  zero problem.
    //
    return  0.0;
  }

  return  distanceTraveled() / (tripTime() / SECONDS_PER_HOUR);
}


//
// Member function: elapsedTime()
//
// Returns the time (in seconds) since the last
// reset.
//
long  CycleComputer::elapsedTime()
{
  return  m_numSecondsElapsed;
}


//
// Member function: tripTime()
//
// Returns the time (in seconds) for the current trip
// during which the tires were rotating.
//
long  CycleComputer::tripTime( void )
{
  return  m_numSecondsTrip;
}


//
// Member function: resetTrip()
//
// Initializes all trip-specific member data
//
void  CycleComputer::resetTrip( void )
{
  m_numSecondsElapsed       =  0;
  m_numSecondsTrip          =  0;
  m_numRotationsThisSecond  =  0;
  m_numRotationsThisTrip    =  0;
  m_maxMPH                  =  0;
  m_curMPH                  =  0;
}

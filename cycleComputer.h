
//
// CST 126 - Assignment #3
//
// Author: Leander Rodriguez
//
// Declaration for CycleComputer class
//
// File: cycleComputer.h
//

#ifndef  cycleComputer_H
#define  cycleComputer_H


//
// CycleComputer class declaration
//
class CycleComputer
{
  public  :

    enum  VelocityType   { current_, average_, maximum_ };

    CycleComputer( double  diameter );

    void  tickSecond();
    void  tickTireRotation();

    double  distanceTraveled();
    double  velocity( VelocityType  type );

    long    elapsedTime();
    long    tripTime();

    void  resetTrip();

  private  :

    double  m_circumference;
    long    m_numSecondsElapsed;
    long    m_numSecondsTrip;
    long    m_numRotationsThisSecond;
    long    m_numRotationsThisTrip;
    double  m_curMPH;
    double  m_maxMPH;

    double  averageVelocity();
};

#endif

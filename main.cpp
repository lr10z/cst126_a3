
//
// CST 126 - Assignment #3
//
// Cycle Computer test program
//
//

#define   _CRT_SECURE_NO_WARNINGS


#include  <iostream>
#include  <fstream>
#include  <cstring>
#include  <iomanip>
#include  <cassert>
#include  <cmath>
#include  <cstdlib>
#include  <ctime>
#include  <sstream>
#include  "CycleComputer.h"


using  namespace  std;


//
// Macro constants
//
#define  FILE_NAME_SZ         256
#define  INPUT_BUFFER_SZ      256
#define  SCRIPT_COMMENT_CHAR  '\''
#define  SCRIPT_DELIM         ": \t\n"
#define  SCRIPT_TOK_RESET     "RESET"
#define  SCRIPT_TOK_SECOND    "SECONDS"
#define  SCRIPT_TOK_SHOW      "SHOW"
#define  SCRIPT_TOK_ROTATION  "ROTATIONS"
#define  SCRIPT_TOK_PAUSE     "PAUSE"
#define  SCRIPT_TOK_LOGGING   "LOGGING"

#define  SECS_PER_HOUR  3600
#define  SECS_PER_MIN   60

#define  LOG_PREFIX           "cc_log_"
#define  LOG_SUFFIX           ".csv"

#define  ON_OFF_DISP(b)  ((b) ? "ON" : "OFF")

#define  BREAK_STR  cout  <<  endl  <<  "**********"  <<  endl  <<  endl


//
// Macro to create functions for fetching values from a user.
//         nm - the function name
//       type - the type for the function to get
//     prompt - the prompt displayed to the user
//
#define  GET_FUNC(nm, type, prompt)                      \
            static  type  nm(void)                       \
            {                                            \
              type  value;                               \
                                                         \
              BREAK_STR;                                 \
              cout  <<  "Enter "  <<  prompt  <<  ": ";  \
                                                         \
              cin  >>  value;                            \
                                                         \
              return  value;                             \
            }


//
// Macro to display an error message for token processing
//         ln - line number
//        tok - token string
//        msg - descriptive error message
//
#define  ERROR(ln, tok, msg)                                              \
              {                                                           \
                cout  <<  "*** Script error - tok: "  <<  tok  <<  endl;  \
                cout  <<  "*** "  msg  " @ line #"  <<  ln  <<  endl;     \
              }


//
// Macro to test for a valid parameter for a token
//        tok - token string
//      param - parameter string
//         ln - line number
//
#define  VERIFY_PARAM_FOR_TOK( tok, param, ln )          \
              if  ( ! param )                            \
              {                                          \
                ERROR( ln, tok, "Missing parameter" );   \
                return;                                  \
              }


//
// Enum for menu options
//
enum  MenuOption  {
                    mnuMIN_  =  1,
                      mnuShowState_ = mnuMIN_,
                      mnuTickSingle_,
                      mnuTickMultiple_,
                      mnuSetRotation_,
                      mnuResetTrip_,
                      mnuSetDiameter_,
                      mnuExeScript_,
                      mnuToggleLog_,
                      mnuQuit_,
                    mnuMAX_  =  mnuQuit_
                  };


//
// Logging tracking variables
//
namespace
{
  bool      logEnabled  =  false;
  ofstream  logStrm;
};


//
// Macro expansions for functions to get values from the user
//
GET_FUNC(getDiameter, double, "tire diameter (inches)" )
GET_FUNC(getRotations, int, "tire rotations per second" )
GET_FUNC(getNumSeconds, int, "number of seconds" )


//
// Function to trucate a real number (value) to a specified
// number of places past the decimal (digits).
//
static  double  truncate( double  value, int  digits )
{
  return  int(value * pow(10.0, digits) ) / pow(10.0, digits);
}


//
// Function to take a large amount of seconds (eg. elapse time) and split it into
// its hours, minutes, and seconds components.
//
static  void  splitTime( long  totalSeconds, long  &hours, long  &minutes, long  &seconds )
{
  hours    =  totalSeconds / SECS_PER_HOUR;
  minutes  =  totalSeconds % SECS_PER_HOUR / SECS_PER_MIN;
  seconds  =  totalSeconds % SECS_PER_HOUR % SECS_PER_MIN;
}


//
// Convert a time (in seconds) to an HH:MM:SS string.
//
static  string  timeStr( long  totalSeconds )
{
  long  hours;
  long  minutes;
  long  seconds;

  splitTime( totalSeconds, hours, minutes, seconds );

  stringstream  timeSS;

  timeSS  <<  setfill('0')
          <<  setw(2)  <<  hours    <<  ':'
          <<  setw(2)  <<  minutes  <<  ':'
          <<  setw(2)  <<  seconds;

  string  timeStr;
  timeSS  >>  timeStr;

  return  timeStr;
}


//
// Log the current state of the cycle computer.  This will call all
// time, distance, and velocity member functions.
//
static  void  logState( CycleComputer  *ccp, int  numRotationsPerSec, double  tireDiameter )
{
  if  (! logEnabled)  return;

  logStrm  <<  truncate( tireDiameter , 2 )                             <<  ','
           <<  numRotationsPerSec                                       <<  ','
           <<  timeStr( ccp->elapsedTime() )                            <<  ','
           <<  timeStr( ccp->tripTime() )                               <<  ','
           <<  truncate( ccp->distanceTraveled(), 2 )                   <<  ','
           <<  truncate( ccp->velocity( CycleComputer::current_ ), 2 )  <<  ','
           <<  truncate( ccp->velocity( CycleComputer::average_ ), 2 )  <<  ','
           <<  truncate( ccp->velocity( CycleComputer::maximum_ ), 2 )
           <<  endl;
}


//
// Generate a date/time string of the format HH:MM:SS
//
static  string  generateDateTimeString()
{
  time_t  curTime  =  time( 0 );     // fetch current time

  struct  tm  *timeInfo  =  localtime( &curTime );

  stringstream  dateTimeSS;

  dateTimeSS  <<  setfill('0')
              <<  setw(2)  <<  (timeInfo->tm_mon + 1)
              <<  setw(2)  <<  timeInfo->tm_mday
              <<  setw(2)  <<  (timeInfo->tm_year - 100)
              <<  '_'
              <<  setw(2)  <<  timeInfo->tm_hour
              <<  setw(2)  <<  timeInfo->tm_min
              <<  setw(2)  <<  timeInfo->tm_sec;

  string  dateTimeStr;
  dateTimeSS  >>  dateTimeStr;

  return  dateTimeStr;
}


//
// Close the log file one is open
//
static  void  closeLogFile()
{
  if  (! logEnabled)  return;

  logStrm.close();

  logEnabled  =  false;
}


//
// Opens a file for logging.  If one is already open, it will be closed and
// a new one is opened.
//
static  void  openLogFile( double  diameter )
{
  //
  // Close the current log file if one is open
  //
  if  (logEnabled)  closeLogFile();


  //
  // Generate the log file name, which includes the current date/time
  //
  string   dateTimeStr  =  generateDateTimeString();

  string  logFileName  =  LOG_PREFIX + dateTimeStr + LOG_SUFFIX;


  //
  // Open the log file
  //
  logStrm.clear();
  logStrm.open( logFileName );

  if  ( ! logStrm )
  {
    cout   <<  "*** Error opening log file '"  <<  logFileName  <<  '\''  <<  endl;
    return;
  }


  //
  // Write column headers to the log file
  //
  logStrm  <<  "Tire Diameter,Rotations Per Second,Elapsed Time, Trip Time,Distance Traveled,"
               "Current Velocity, Average Velocity, Maximum Velocity"
           <<  endl;

  logEnabled  =  true;
}


//
// Function to display the menu and prompt the user for a menu choice.
// The user is re-prompted until a valid menu choice is entered.
// A valid menu choice number is returned.
//
static  MenuOption  getChoice(int  numRotationsPerSecond)
{
  //
  // Display the menu
  //
  cout  <<  endl
        <<  "Menu"                                          <<  endl
        <<  "----"                                          <<  endl
        <<  "   1. Show Computer State"                     <<  endl
        <<  "   2. Tick second (single)"                    <<  endl
        <<  "   3. Tick second (multiple)"                  <<  endl
        <<  "   4. Set rotations per sec (current: "        <<  numRotationsPerSecond  <<  ')'  <<  endl
        <<  "   5. Reset trip"                              <<  endl
        <<  "   6. Change diameter (causes a trip reset)"   <<  endl
        <<  "   7. Execute script"                          <<  endl
        <<  "   8. Toggle logging (currently: "             <<  ON_OFF_DISP(logEnabled)  <<  ')'  <<  endl
        <<  "   9. Quit"                                    <<  endl;


  //
  // Prompt until a value menu choice is entered
  //

  int  choice;

  while(true)
  {
    cout  <<  "Choice: ";

    cin  >>  choice;

    if  (choice >= mnuMIN_  && choice <= mnuMAX_ )  break;

    cout  <<  "*** Invalid entry!"  <<  endl  <<  endl;
  }

  return  MenuOption(choice);
}


//
// Display the current state of the cycle computer
//
static  void  showState( CycleComputer  *ccp, double  diameter )
{
  BREAK_STR;

  //
  // Display tire diameter
  //
  cout  <<  "     Tire diameter:  "
        <<  truncate( diameter , 2 )
        <<  " inches"
        <<  endl;

  //
  // Display elapsed time
  //
  cout  <<  "      Elapsed time:  "  <<  timeStr( ccp->elapsedTime() );


  //
  // Display trip time
  //
  cout  <<  endl
        <<  "         Trip time:  "  <<  timeStr( ccp->tripTime() );


  //
  // Display velocity
  //
  cout  <<  endl
        <<  "          Distance:  "
        <<  truncate( ccp->distanceTraveled(), 2 )
        <<  " miles"
        <<  endl;

  //
  // Display current velocity
  //
  cout  <<  "  Current velocity:  "
        <<  truncate( ccp->velocity( CycleComputer::current_ ), 2 )
        <<  " mph"
        <<  endl;

  //
  // Display average velocity
  //
  cout  <<  "  Average velocity:  "
        <<  truncate( ccp->velocity( CycleComputer::average_ ), 2 )
        <<  " mph"
        <<  endl;

  //
  // Display maximum velocity
  //
  cout  <<  "  Maximum velocity:  "
        <<  truncate( ccp->velocity( CycleComputer::maximum_ ), 2 )
        <<  " mph"
        <<  endl;
}


//
// Creates a dynamic CycleComputer object
//
static  CycleComputer  *createCycleComputer( double  diameter )
{
  CycleComputer  *ccp  =  new  CycleComputer( diameter );

  if  (! ccp)
  {
    cout  <<  "Error allocating CycleComputer object"  <<  endl;
    exit( EXIT_FAILURE );
  }

  return  ccp;
}


//
// Destroys a dynamic CycleComputer object
//
static  void  destroyCycleComputer( CycleComputer  *ccp )
{
  delete  ccp;
}


//
// Processes a single second of time.  This includes calling the
// the appropriate number of rotation ticks and a single second tick
// for the CycleComputer object.
//
static  void  doTickSecond( CycleComputer  *ccp, int  numRotationsPerSec, double  tireDiameter )
{
  for( int  i = 0;  i < numRotationsPerSec;  ++i )
    ccp->tickTireRotation();

  ccp->tickSecond();

  logState( ccp, numRotationsPerSec, tireDiameter );
}


//
// Processes multiple second of time.
//
static  void  doMultiTickSecond( CycleComputer  *ccp, int  numSeconds, int  numRotationsPerSec, double  tireDiameter )
{
  for( int  i = 0;  i < numSeconds;  ++i )
    doTickSecond( ccp, numRotationsPerSec, tireDiameter );
}


//
// Prompt file a filename and open the file for input
//
static  bool  openFile( ifstream  &in )
{
  char  filename[FILE_NAME_SZ];

  cout  <<  endl  <<  "Enter script filename: ";

  cin  >>  filename;

  in.open( filename );

  if  ( in.fail() )
  {
    cout  <<  "Error opening file: '"  <<  filename  <<  '\''  <<  endl;
    return  false;
  }

  return  true;
}


//
// Execute a script
//
static  void  executeScript( CycleComputer  *ccp, int  &numRotationsPerSec, double  tireDiameter )
{
  //
  // Open the file
  //

  ifstream  in;

  if  (! openFile( in ) )  return;


  //
  // Process each line of the file
  //

  char   buffer[INPUT_BUFFER_SZ];


  for( int  lineNumber  =  1;  ! in.eof();  ++lineNumber )
  {
    //
    // Read a line from the file
    //
    in.getline( buffer, INPUT_BUFFER_SZ );

    //
    // Parse the first token on the line
    //
    char  *tok  =  strtok( buffer, SCRIPT_DELIM );


    //
    // Skip processing the line if it's a blank line or if there's
    // a comment character at the beginning of the line
    //
    if  ( ! tok  ||  tok[0]  ==  SCRIPT_COMMENT_CHAR  )  continue;


    //
    // Parse a parameter token
    //
    char  *param  =  strtok( 0, SCRIPT_DELIM );


    //
    // Process the command
    //


    if  ( ! strcmp( tok, SCRIPT_TOK_SECOND ) )
    {
      //
      // SECOND:#
      //
      VERIFY_PARAM_FOR_TOK( tok, param, lineNumber );
      doMultiTickSecond( ccp, atoi(param), numRotationsPerSec, tireDiameter );
    }
    else  if  ( ! strcmp( tok, SCRIPT_TOK_ROTATION ) )
    {
      //
      // ROTATION:#
      //
      VERIFY_PARAM_FOR_TOK( tok, param, lineNumber );
      numRotationsPerSec  =  atoi(param);
    }
    else  if  ( ! strcmp( tok, SCRIPT_TOK_RESET ) )
    {
      //
      // RESET
      //
      ccp->resetTrip();
    }
    else  if  ( ! strcmp( tok, SCRIPT_TOK_SHOW ) )
    {
      //
      // SHOW
      //
      showState( ccp, tireDiameter );
    }
    else  if  ( ! strcmp( tok, SCRIPT_TOK_PAUSE ) )
    {
      //
      // PAUSE
      //
      cout <<  endl;
      system("pause");
    }
    else  if  ( ! strcmp( tok, SCRIPT_TOK_LOGGING ) )
    {
      //
      // LOGGING:ON
      // LOGGING:OFF
      //
      VERIFY_PARAM_FOR_TOK( tok, param, lineNumber );
      string  paramStr(param);

      if  ( paramStr == "ON" )   openLogFile( tireDiameter );
      if  ( paramStr == "OFF" )  closeLogFile();
    }
    else
    {
      //
      // It's an invalid token
      //
      ERROR(lineNumber, tok, "Invalid token");
    }
  }
}


//
// Main program
//
int  main()
{
  bool      done                =  false;
  int       numRotationsPerSec  =  0;
  ofstream  scriptRecorder;


  //
  // Fetch the tire diameter and create the CycleComputer object
  //
  double          tireDiameter  =  getDiameter();
  CycleComputer  *ccp           =  createCycleComputer(tireDiameter);


  //
  // Display the initial state of the cycle computer
  //
  showState( ccp, tireDiameter );


  //
  // Loop until user select the quit menu option
  //
  while(! done)
  {

    switch( getChoice( numRotationsPerSec ) )
    {

      case  mnuShowState_     :  showState( ccp, tireDiameter );
                                 break;

      case  mnuTickSingle_    :  doTickSecond( ccp, numRotationsPerSec, tireDiameter );
                                 break;

      case  mnuTickMultiple_  :  doMultiTickSecond( ccp, getNumSeconds(), numRotationsPerSec, tireDiameter );
                                 break;

      case  mnuSetRotation_   :  numRotationsPerSec  =  getRotations();
                                 break;

      case  mnuResetTrip_     :  ccp->resetTrip();
                                 showState( ccp, tireDiameter );
                                 break;

      case  mnuSetDiameter_   :  destroyCycleComputer(ccp);
                                 tireDiameter  =  getDiameter();
                                 ccp  =  createCycleComputer(tireDiameter);
                                 showState( ccp, tireDiameter );
                                 break;

      case  mnuExeScript_     :  executeScript(ccp, numRotationsPerSec, tireDiameter);
                                 break;

      case  mnuToggleLog_     :  logEnabled  ?  closeLogFile()  :  openLogFile( tireDiameter );
                                 break;

      case  mnuQuit_          :  done  =  true;
                                 break;

      default  :  assert(false);
    }
  }

  return  0;
}

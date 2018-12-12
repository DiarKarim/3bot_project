/******************************************************************************/
/*                                                                            */
/* MODULE  : 3BOT-OculusMotor.cpp                                             */
/*                                                                            */
/* PURPOSE : 3BOT and Oculus Rift HMD demo using OCULUS module in MOTOR.LIB   */
/*                                                                            */
/* DATE    : 05/Dec/2014                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 05/Dec/2014 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 12/Jan/2015 - Conversion to use new OCULUS module in MOTOR.LIB   */
/*                                                                            */
/* V1.2  JNI 21/Jan/2015 - OCULUS module uses traditional coordinate system.  */
/*                                                                            */
/******************************************************************************/

//#define  OS_WIN32
//#define  OVR_OS_WIN32

#define MODULE_NAME "3BOT-OculusMotor"

/******************************************************************************/

#include <motor.h>

#include "stdio.h"
/******************************************************************************/

TIMER_Interval  GraphicsDrawLatencyTimer("GraphicsDrawLatency");
TIMER_Frequency GraphicsDrawFrequencyTimer("GraphicsDrawFrequency");
TIMER_Frequency GraphicsIdleFrequencyTimer("GraphicsIdleFrequency");

/******************************************************************************/

STRING RobotName="";
int    RobotID=ROBOT_INVALID;
matrix RobotPosition(3,1);             // cm
matrix RobotPositionRaw(3,1);          // cm
matrix RobotPositionOffset(3,1);       // cm
matrix RobotVelocity(3,1);             // cm/sec
matrix RobotForces(3,1);               // N
double RobotForceMax=40.0;             // N
double RobotSpringConstant=-15.0;      // N/cm

TIMER_Frequency RobotLoopFrequencyTimer("RobotLoopFrequency");
TIMER_Interval  RobotLoopLatencyTimer("RobotLoopLatency");

matrix SpherePosition(3, 1); // {{ 0, 0 }, { 1, 2 }, { 2, 4 }};;
matrix SpherePosition2(3, 1);
double SphereRadius = 2.0; // cm
int    SphereColor = YELLOW;
BOOL   SphereInsideFlag=FALSE;

double CursorRadius = 2.0; // cm
int    CursorColor = RED;
double angles[3] = { 0.1, 0.1, 0.1 };

/******************************************************************************/

void ProgramExit( void );

/******************************************************************************/

void GraphicsTimingResults( void )
{
    printf("----------------------------------------------------------------\n");

    GraphicsDrawFrequencyTimer.Results();
    GraphicsDrawLatencyTimer.Results();
    GraphicsIdleFrequencyTimer.Results();

    GRAPHICS_TimingResults();
    GRAPHICS_AdaptiveDisplayResults();

    printf("----------------------------------------------------------------\n");
}

/******************************************************************************/

void GraphicsIdle( void )
{
    GraphicsIdleFrequencyTimer.Loop();
}

/******************************************************************************/

void GraphicsDraw( void )
{
    GraphicsDrawFrequencyTimer.Loop();
    GraphicsDrawLatencyTimer.Before();

    // Fixed object.
    GRAPHICS_WireSphere(&SpherePosition,SphereRadius,SphereColor);
	
	GRAPHICS_WireSphere(&SpherePosition2, SphereRadius, RED);
	//GRAPHICS_Cube(&SpherePosition, 4.0); 
	//GRAPHICS_Cuboid(&SpherePosition,300.0, 50.0, 5.0);
	//GRAPHICS_Cuboid()
	//GRAPHICS_Ring(&SpherePosition, 10.0, 3.5); 
	

    // Robot cursor.
    GRAPHICS_Sphere(&RobotPosition,CursorRadius,CursorColor);

    GraphicsDrawLatencyTimer.After();
}

/******************************************************************************/

void GraphicsKeyboard( BYTE key, int x, int y )
{
    // Process keyboard input.
	if (key == SPACE)
	{
		printf("Space button pressed...");
	}


    if( key == ESC )
    {
        ProgramExit();
    }

	printf("Key code pressed: %c\n", key);
}

/******************************************************************************/

void GraphicsStart( void )
{
    // Start the graphics system.
    if( !GRAPHICS_GraphicsStart() )
    {
        printf("Cannot start graphics system.\n");
        return;
    }

    // Register call-back functions and pass control to graphics system.
    GRAPHICS_MainLoop(GraphicsKeyboard,GraphicsDraw,GraphicsIdle);
}

/******************************************************************************/

void RobotForcesFunction( matrix &P, matrix &V, matrix &F )
{
int i;
static matrix D(3,1);
static double d,e;

    RobotLoopFrequencyTimer.Loop();
    RobotLoopLatencyTimer.Before();

    RobotPositionRaw = P;
    RobotPosition = P;
    RobotVelocity = V;
    RobotForces.zeros();

    // Calculate robot position in graphics co-orindate system.
	D = RobotPosition - SpherePosition;
    d = norm(D); // Magnitude
    e = d - SphereRadius; // Scalar encroachment into surface of sphere.

    if( ROBOT_JustActivated(RobotID) )
    {
        SphereInsideFlag = (e < 0);
    }

    if( SphereInsideFlag ) // If the cursor is inside the target ...
    {
        e = d - (SphereRadius-CursorRadius);
    }
    else // ... or if the cursor is outside the target 
    {
        e = d - (SphereRadius+CursorRadius); 
    }

    if( (SphereInsideFlag && (e > 0.0)) || (!SphereInsideFlag && (e < 0.0)) )
    {
        RobotForces = RobotSpringConstant * e * (D/d); // PID controller but only position based i.e. Position/P controller 
    }

    RobotForces.clampnorm(RobotForceMax);

    F = RobotForces; 

    RobotLoopLatencyTimer.After();

}


void RobotForcesFunction2(matrix &P, matrix &V, matrix &F)
{
	//cdasdas
	//matrix JT = ROBOT_JT[RobotID];
	//matrix ap(3, 1);
	//matrix aa(3, 1);
	//matrix av(3, 1);
	//ROBOT_Angles(ap, av, aa);

	//printf("-----------\n\n\n\n");
	//for (int i = 1; i <= 3; i++){
	//	printf("%lf %lf %lf\n", JT(i, 1), JT(i, 1), JT(i, 1));
	//}
	//printf("-----------\n\n\n\n");

	//int i;
	static matrix D(3, 1);
	static double d, e;

	RobotLoopFrequencyTimer.Loop();
	RobotLoopLatencyTimer.Before();

	RobotPositionRaw = P;
	RobotPosition = P;
	RobotVelocity = V;
	RobotForces.zeros();






	//Calculate command
	D = RobotPosition - SpherePosition;
	RobotForces = RobotSpringConstant*D;



	



	//Set command to robot
	RobotForces.clampnorm(RobotForceMax);
	F = RobotForces;
	//Dont do anything after that
	RobotLoopLatencyTimer.After();





}

/******************************************************************************/

BOOL RobotStart( void )
{
BOOL ok=FALSE;

    if( (RobotID=ROBOT_Open(RobotName)) == ROBOT_INVALID )
    {
        printf("Cannot open: \n",RobotName);
    }
    else
    if( !ROBOT_Start(RobotID,RobotForcesFunction2) )
    {
        printf("Cannot start: %s\n",RobotName);
    }
    else
    {
        ok = TRUE;
        printf("Robot started: %s\n",RobotName);
    }

    return(ok);
}

/******************************************************************************/

void RobotStop( void )
{
    ROBOT_Stop(RobotID);
    ROBOT_Close(RobotID);
}

/******************************************************************************/

void RobotTimingResults( void )
{
    RobotLoopFrequencyTimer.Results();
    RobotLoopLatencyTimer.Results();
}

/******************************************************************************/

void ProgramExit( void )
{
    // Stop and close the 3BOT and the Oculus HMD.
    RobotStop();
    GRAPHICS_Stop();

    // Timing results.
    RobotTimingResults();
    GraphicsTimingResults();

    // Exit program.
    exit(0);
}

/******************************************************************************/

void Usage( void )
{
    printf("Usage:\n\n");
    printf("%s /R:RobotName\n",MODULE_NAME);

    exit(0);
}

/******************************************************************************/

BOOL Parameters( int argc, char *argv[] )
{
BOOL ok;
char *data;
int i;

    for( ok=TRUE,i=1; ((i < argc) && ok); i++ )
    {
		//char test = CMDARG_code(argv[i], &data);
		//printf("%d:%s:%c", i, argv[i], test);
		//getchar();
        switch( CMDARG_code(argv[i],&data) )
        {
			
            case 'R' :
               ok = CMDARG_data(RobotName,data,STRLEN);
               break;

            case '?' :
               Usage();
               break;

            default :
               //ok = FALSE;
               break;
        }

        if( !ok )
        {
            printf("Invalid argument: %s\n",argv[i]);
        }
    }

    if( ok && STR_null(RobotName) )
    {
        printf("Robot name not specified.\n");
        ok = FALSE;
    }

    if( !ok )
    {
        Usage();
    }

    return(ok);
}

/******************************************************************************/

void main( int argc, char *argv[] )
{

	if (!Parameters(argc, argv))
    {
        exit(0);
    }

    // Position of sphere in simulation...
    SpherePosition.zeros();
	SpherePosition2.zeros();
	printf("%d", SpherePosition);
	
	SpherePosition(1, 1) -= 10.0;
	SpherePosition(2, 1) = 0.0;
	SpherePosition(3,1) -= 10.0;

	SpherePosition2(1, 1) = 10.0;
	SpherePosition2(2, 1) = 0.0;
	SpherePosition2(3, 1) -= 10.0;
	

	//printf ("%d",SpherePosition);

    if( !RobotStart() )
    {
        exit(0);
    }

    // Start graphics.
    GraphicsStart();
}

/******************************************************************************/


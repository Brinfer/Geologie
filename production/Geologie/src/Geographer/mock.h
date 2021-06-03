/**
 * @file mock.h
 *
 * @version 1.0
 * @date 03/06/21
 * @author Nathan BRIENT
 * @copyright BSD 2-clauses
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Define 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** 
 * @def MOCK_H
 * @def ROBOT_HEIGHT_MM
 */

#ifndef MOCK_H
#define MOCK_H 

typedef int CalibrationPositionId;

typedef int CalibrationData;

typedef int BeaconData;

typedef int Position;

typedef int ProcessorAndMemoryLoad;

typedef int ExperimentalPosition;

typedef int CalibrationPosition;

typedef int ExperimentalTraject;

typedef int ConnectionState;


extern int Receiver_ask4StartReceiver();

extern int Bookkeeper_ask4StartBookkeeper();

extern int ProxyLoggerMOB_setSexperimentalTrajects(ExperimentalTraject experimentalTrajects[]);

extern int ProxyLoggerMOB_setSexperimentalPositions(Position experimentalPositions[]);

extern int ProxyLoggerMOB_setCalibrationData(CalibrationData calibrationData[]);

extern int ProxyGUI_setCalibrationPositions(Position calibrationPosition[]);

extern int ProxyGUI_signalEndCalibration(Position calibrationPosition[]);

extern int Scanner_ask4UpdateAttenuationCoefficient(Position calibrationPosition);

extern int Scanner_ask4AverageCalcul();



#endif 

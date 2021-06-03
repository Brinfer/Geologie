#include "mock.h"

extern int Receiver_ask4StartReceiver();

extern int Bookkeeper_ask4StartBookkeeper();

extern int ProxyLoggerMOB_setSexperimentalTrajects(ExperimentalTraject experimentalTrajects[]);

extern int ProxyLoggerMOB_setSexperimentalPositions(Position experimentalPositions[]);

extern int ProxyLoggerMOB_setCalibrationData(CalibrationData calibrationData[]);

extern int ProxyGUI_setCalibrationPositions(Position calibrationPosition[]);

extern int ProxyGUI_signalEndCalibration(Position calibrationPosition[]);

extern int Scanner_ask4UpdateAttenuationCoefficient(Position calibrationPosition);

extern int Scanner_ask4AverageCalcul();
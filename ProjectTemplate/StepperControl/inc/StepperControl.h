/* 
* StepperControl.h
*
* Created: 12/14/2023 6:02:50 PM
* Author: user
*/


#ifndef __STEPPERCONTROL_H__
#define __STEPPERCONTROL_H__
#include "ClearCore.h"

enum {
	STEPPER_MOTOR_STATE_DISABLED = 0x01,
	STEPPER_MOTOR_STATE_MOVING_DIR,
	STEPPER_MOTOR_STATE_STOPPED_DIR,
	STEPPER_MOTOR_STATE_MOVING_POS,
	STEPPER_MOTOR_STATE_STOPPED_POS,
	STEPPER_MOTOR_STATE_FAULTED,
};

enum {	
	STEPPER_DIRECTION_FWD = 0x01,
	STEPPER_DIRECTION_REV
};



class StepperControl
{
//variables
public:
protected:
private:
	MotorDriver* _mtr;
	int32_t _velLim = 4500; // pulses per sec
	int32_t _accelLim = 10000; // pulses per sec^2
	int32_t _pos = 0;
	uint8_t _homeDir = STEPPER_DIRECTION_FWD;
	uint8_t _state;

//functions
public:
	StepperControl();
	~StepperControl();
	void Begin(MotorDriver* connector, uint8_t homeDirection);
	int MoveTo(int32_t pos);
	int32_t GetPos(){return _pos;}
	uint8_t GetState(){return _state;}
	void Home();
	void Homed();
protected:
private:
	StepperControl( const StepperControl &c );
	StepperControl& operator=( const StepperControl &c );

}; //StepperControl

#endif //__STEPPERCONTROL_H__

/* 
* StepperControl.cpp
*
* Created: 12/14/2023 6:02:49 PM
* Author: user
*/


#include "StepperControl.h"

// default constructor
StepperControl::StepperControl()
{
} //StepperControl

// default destructor
StepperControl::~StepperControl()
{
} //~StepperControl

void StepperControl::Begin(MotorDriver* connector, uint8_t homeDirection)
{
	_mtr = connector;
	_homeDir = homeDirection;
	MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);
	MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL, Connector::CPM_MODE_STEP_AND_DIR);
	_state = STEPPER_MOTOR_STATE_STOPPED_DIR;
	_mtr->PolarityInvertSDEnable(true);
	_mtr->VelMax(_velLim);
	_mtr->AccelMax(_accelLim);
	_mtr->EnableRequest(true);
	_mtr->Move(6000);

}
int StepperControl::MoveTo(int32_t pos)
{
	if(!_state==STEPPER_MOTOR_STATE_STOPPED_POS){
		return -1;
	}
	_state = STEPPER_MOTOR_STATE_MOVING_POS;
	_mtr->Move(pos, StepGenerator::MOVE_TARGET_ABSOLUTE);
	return 0;
	
}

void StepperControl::Home()
{
	_state = STEPPER_MOTOR_STATE_MOVING_DIR;
	int32_t vel = _velLim / 10;
	if (_homeDir == STEPPER_DIRECTION_REV){
		vel *= -1;
	}
	_mtr->MoveVelocity(vel);
	//call Homed method on hitting switch
}

/*
 * Hit the home switch
 */
void StepperControl::Homed()
{
	_mtr->MoveVelocity(0);
	_state = STEPPER_MOTOR_STATE_STOPPED_POS;
}
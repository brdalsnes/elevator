#pragma once

#include <stdlib.h>
#include <deque>
#include "elev.h"
#include "OrderManager.h"

typedef enum { RUN, IDLE, OPEN, EMERGENCY }state;

class Elevator{
	private:
		std::deque<int> orders;
		state currentState;
		int currentFloor;
		tag_elev_motor_direction direction;

	public:
		Elevator();
		void driveToFloor(int);
		void stopAtFloor(int);
		int getNextOrder();
		void toRun();
		void toIdle();
		void toOpen();
		void toEmergency();
};
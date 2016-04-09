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
		void run();
		void driveToFloor();
		void stopAtFloor();
		int getNextOrder();
		void addOrder(int);
		void toRun();
		void toIdle();
		void toOpen();
		void toEmergency();
};
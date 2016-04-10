#pragma once

#include <stdlib.h>
#include <deque>
#include "elev.h"
#include "OrderManager.h"

typedef enum { RUN, IDLE, OPEN, EMERGENCY }state;
typedef enum { UP, DOWN }dir_state;

class Elevator{
	private:
		std::deque<int> orders;
		std::deque<int> ordersOnHoldUp;
		std::deque<int> ordersOnHoldDown;
		state currentState;
		dir_state direction;
		int currentFloor;

	public:
		Elevator();
		void run();
		bool driveToFloor();
		void stopAtFloor();
		int getNextOrder();
		state getCurrentState();
		void addOrder(int, elev_button_type_t);
		bool notInQue(int, std::deque<int>);
		void extendOrders(std::deque<int>);
		void toRun();
		void toIdle();
		void toOpen();
		void toEmergency();
		void printOrders();
		inline const char* toString(dir_state);
};
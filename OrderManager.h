#pragma once

#include <stdlib.h>
#include <deque>
#include "elev.h"
#include "Elevator.h"

enum State{MASTER, SLAVE};

class OrderManager{
private:
	std::deque<Elevator> elevators;
	int nElevators;

public:
	unsigned int buttonMatrix[4][3];
	int nextOrder;

	OrderManager(int);
	void run();
	void manage();
	void backUp();
	void addElevators(int);
	inline const char* toString(state);
};
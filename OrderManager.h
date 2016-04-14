#pragma once

#include <stdlib.h>
#include <deque>
#include <string>
#include "elev.h"
#include "Elevator.h"

enum State{MASTER, SLAVE};

class OrderManager{
private:
	std::deque<Elevator> elevators;
	int nElevators;

public:
	unsigned int buttonMatrix[4][3];
	unsigned int buffer[4][3];
	int nextOrder;
	std::string msg;
	void code();
	void decode();
	OrderManager(int);
	void listen();
	void manage();
	void addElevators(int);
	inline const char* toString(state);
};
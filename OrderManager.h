#pragma once

#include <stdlib.h>
#include <deque>
#include <string>
#include "elev.h"
#include "Elevator.h"

enum State{MASTER, SLAVE};

class OrderManager{
private:
	int nElevators;

public:
	State current_state;
	std::deque<Elevator> elevators;
	unsigned int buttonMatrix[4][3];
	int nextOrder;
	std::string smsg;
	void code();
	void decode(std::string);
	OrderManager(int);
	void listen();
	void listen1();
	void manage();
	void addElevators(int);
	inline const char* toString(state);
};
#pragma once

#include <stdlib.h>
#include <deque>
#include "Elevator.h"

class OrderManager{
private:
	std::deque<int> orders[3];
	int currentFloor[3] = {0, 0, 0};
	unsigned int buttonMatrix[4][3];
	int nElevators;
	int nextOrder;

public:
	OrderManager(int);
	void run();
	void setQue();
	bool notInQue(int);
	int getNextOrder();
	void scanOrders();
};
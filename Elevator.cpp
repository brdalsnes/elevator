#include "OrderManager.h"
#include "Elevator.h"
#include "elev.h"
#include "channels.h"
#include "io.h"
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <unistd.h>
#include <deque>
#include <algorithm>

using namespace std;

Elevator::Elevator(){
	currentState = RUN;
	direction = UP;
	orders.push_back(0);
}

void Elevator::run(){
	//Elevator state
	switch(currentState){
		case RUN:
			if(elev_get_stop_signal()){
				toEmergency();
			}
			else if(driveToFloor()){
				toOpen();
			}
			break;

		case IDLE:
			if(elev_get_stop_signal()){
				toEmergency();
			}
			else if(orders.size() > 0){
				toRun();
			}
			else{
				toIdle();
			}
			break;

		case OPEN:
			if(elev_get_stop_signal()){
				toEmergency();
			}
			else{
				toIdle();
			}
			break;

		case EMERGENCY:
			break;
	}
}

void Elevator::toRun(){
	switch(currentState){
		case RUN:
			break;

		case IDLE:
			break;

		case OPEN:
			driveToFloor();
			elev_set_door_open_lamp(0);
			break;

		case EMERGENCY:
			driveToFloor();
			elev_set_stop_lamp(0);
			break;
	}
	currentState = RUN;
}

void Elevator::toIdle(){
	switch(currentState){
		case RUN:
			elev_set_motor_direction(DIRN_STOP);
			break;

		case IDLE:
			setDirectionState();
			break;

		case OPEN:
			elev_set_door_open_lamp(0);
			break;

		case EMERGENCY:
			elev_set_stop_lamp(0);
			break;
	}
	currentState = IDLE;
}

void Elevator::toOpen(){
	switch(currentState){
		case RUN:
			stopAtFloor();
			setDirectionState();
			break;

		case IDLE:
			elev_set_door_open_lamp(1);
			break;

		case OPEN:
			//
			break;

		case EMERGENCY:
			elev_set_door_open_lamp(1);
			elev_set_stop_lamp(0);
			break;
	}
	currentState = OPEN;
}

void Elevator::toEmergency(){
	switch(currentState){
		case RUN:
			elev_set_motor_direction(DIRN_STOP);
			elev_set_stop_lamp(1);
			break;

		case IDLE:
			elev_set_stop_lamp(1);
			break;

		case OPEN:
			elev_set_stop_lamp(1);
			break;

		case EMERGENCY:
			break;
	}
	currentState = EMERGENCY;
}

bool Elevator::driveToFloor(){
	int destionationFloor = getNextOrder();
	int startFloor = elev_get_floor_sensor_signal();

	while(elev_get_floor_sensor_signal() != destionationFloor){
		destionationFloor = getNextOrder();
		sortOrders();

		int tempFloor = elev_get_floor_sensor_signal();
        if(tempFloor != -1){
            elev_set_floor_indicator(tempFloor);
            currentFloor = tempFloor;
        }

        if(startFloor < destionationFloor){
            elev_set_motor_direction(DIRN_UP);
        }
        else if(startFloor > destionationFloor){
            elev_set_motor_direction(DIRN_DOWN);
        }
    }
    elev_set_motor_direction(DIRN_STOP);
    orders.pop_front();
    
    return true;
}

void Elevator::stopAtFloor(){
	currentState = OPEN;
    elev_set_floor_indicator(elev_get_floor_sensor_signal());
    elev_set_door_open_lamp(1);
    sleep(2);
    elev_set_door_open_lamp(0);
    //printOrders("Orders", orders);
    //printOrders("UpOrders", ordersOnHoldUp);
    //printOrders("DownOrders", ordersOnHoldDown);
    //printf("\n");
}

void Elevator::addOrder(int newOrder, elev_button_type_t buttonType){
	//printf("%s\n", toString(direction));
	if(newOrder != elev_get_floor_sensor_signal()){
		switch(buttonType){
			case BUTTON_CALL_UP:
				if(notInQue(newOrder, ordersOnHoldUp) && direction != UP){
					ordersOnHoldUp.push_back(newOrder);
				}
				else if(notInQue(newOrder, orders) && notInQue(newOrder, ordersOnHoldUp)){
					orders.push_back(newOrder);
				}
				break;

			case BUTTON_CALL_DOWN:
				if(notInQue(newOrder, ordersOnHoldDown) && direction != DOWN){
					ordersOnHoldDown.push_back(newOrder);
				}
				else if(notInQue(newOrder, orders) && notInQue(newOrder, ordersOnHoldDown)){
					orders.push_back(newOrder);
				}
				break;

			case BUTTON_COMMAND:
				if(notInQue(newOrder, orders)){
					orders.push_back(newOrder);
				}
				break;
		}
	}
}

int Elevator::getNextOrder(){
	if(orders.size() > 0){
		int nextOrder = orders.front();
		return nextOrder;
	}
	else{
		return currentFloor;
	}
}

state Elevator::getCurrentState(){
	return currentState;
}

bool Elevator::notInQue(int newOrder, deque<int> que){
	for(int i = 0; i < que.size(); i++){
		if(que[i] == newOrder){
			return false;
		}
	}
	return true;
}

void Elevator::extendOrders(deque<int> ext){
	for(int i = 0; i < ext.size(); i++){
		if(notInQue(ext[i], orders)){
			orders.push_back(ext[i]);
		}
	}
}

void Elevator::setDirectionState(){
	if(currentFloor == 0 || (!orders.size() && ordersOnHoldUp.size())){
		//Enter UP
		direction = UP;
		extendOrders(ordersOnHoldUp);
		ordersOnHoldUp.clear();
	}
	else if(currentFloor == 3 || (!orders.size() && ordersOnHoldDown.size())){
		//Enter DOWN
		direction = DOWN;
		extendOrders(ordersOnHoldDown);
		ordersOnHoldDown.clear();
	}
}

void Elevator::sortOrders(){
	if(direction == UP){
		sort(orders.begin(), orders.end());
	}
	else if(direction == DOWN){
		sort(orders.rbegin(), orders.rend());
	}
}

int Elevator::getDirectionIndex(){
	switch(direction){
		case UP:
			return 0;
			break;
		case DOWN:
			return 1;
			break;
		default:
			return 2;
			break;
	}
}

void Elevator::printOrders(string word, deque<int> que){
	printf("%s: [", word.c_str());
	for(int i = 0; i < que.size(); i++){
		printf("%i, ", que[i]);
	}
	printf("]\n");
}

inline const char* Elevator::toString(dir_state s){
	switch(s){
		case UP: return "UP";
		case DOWN: return "DOWN";
		default: return "Unknown";
	}
}
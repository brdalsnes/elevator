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
	currentState = DOWN;
	direction = DOWN;
	orders.push_back(0);
}

void Elevator::run(){
	//printf("%s\n", toString(direction));
	//Elevator state
	switch(currentState){
		case UP:
			if(driveToFloor()){ //Sexy
				toOpen();
			}
			break;

		case DOWN:
			if(driveToFloor()){
				toOpen();
			}
			break;

		case IDLE:
			if((getNextOrder() > currentFloor) || ordersOnHoldUp.size()){
				//printf("UP\n");
				toUp();
			}
			else if((getNextOrder() < currentFloor) || ordersOnHoldDown.size()){
				//printf("DOWN\n");
				toDown();
			}
			break;

		case OPEN:
			toIdle();
			break;
	}
}

void Elevator::toUp(){
	switch(currentState){
		case UP:
			toUp();
			break;

		case DOWN:
			//Never happens
			break;

		case IDLE:
			extendOrders(ordersOnHoldUp);
			ordersOnHoldUp.clear();
			break;

		case OPEN:
			driveToFloor();
			elev_set_door_open_lamp(0);
			break;

	}
	direction = UP;
	currentState = UP;
}

void Elevator::toDown(){
	switch(currentState){
		case UP:
			//Never happens
			break;

		case DOWN:
			toDown();
			break;

		case IDLE:
			extendOrders(ordersOnHoldDown);
			ordersOnHoldDown.clear();
			break;

		case OPEN:
			driveToFloor();
			elev_set_door_open_lamp(0);
			break;

	}
	direction = DOWN;
	currentState = DOWN;
}

void Elevator::toIdle(){
	switch(currentState){
		case UP:
			break;

		case DOWN:
			break;

		case IDLE:
			toIdle();
			break;

		case OPEN:
			elev_set_door_open_lamp(0);
			break;

	}
	currentState = IDLE;
}

void Elevator::toOpen(){
	switch(currentState){
		case UP:
			stopAtFloor();
			break;

		case DOWN:
			stopAtFloor();
			break;

		case IDLE:
			elev_set_door_open_lamp(1);
			break;

		case OPEN:
			//
			break;
	}
	currentState = OPEN;
}

bool Elevator::driveToFloor(){
	int destionationFloor = getNextOrder();
	int startFloor = elev_get_floor_sensor_signal();

	while(elev_get_floor_sensor_signal() != destionationFloor){
		destionationFloor = getNextOrder(); //Update floor
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
    printOrders("Orders", orders);
    printOrders("UpOrders", ordersOnHoldUp);
    printOrders("DownOrders", ordersOnHoldDown);
    printf("\n");
}

void Elevator::addOrder(int newOrder, elev_button_type_t buttonType){
	if(newOrder != elev_get_floor_sensor_signal()){
		switch(buttonType){

			case BUTTON_CALL_UP:
				if((!orders.size() || (direction == UP)) && notInQue(newOrder, orders)){
					orders.push_back(newOrder);
				}
				else if(direction == DOWN){
					ordersOnHoldUp.push_back(newOrder);
				}
				break;

			case BUTTON_CALL_DOWN:
				if((!orders.size() || (direction == DOWN)) && notInQue(newOrder, orders)){
					orders.push_back(newOrder);
				}
				else if(direction == UP){
					ordersOnHoldDown.push_back(newOrder);
				}
				break;

			case BUTTON_COMMAND:
				printf("%i\n", newOrder);
				if(notInQue(newOrder, orders)){
					orders.push_back(newOrder);
				}
				else if((newOrder <= currentFloor) && (direction == DOWN) && notInQue(newOrder, orders)){
					orders.push_back(newOrder);
				}
				else if((newOrder >= currentFloor) && (direction == DOWN) && notInQue(newOrder, ordersOnHoldUp)){
					ordersOnHoldUp.push_back(newOrder);
				}
				else if((newOrder <= currentFloor) && (direction == UP) && notInQue(newOrder, ordersOnHoldDown)){
					ordersOnHoldDown.push_back(newOrder);
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

int Elevator::getCurrentFloor(){
	return currentFloor;
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

void Elevator::sortOrders(){
	if(currentState == UP){
		sort(orders.begin(), orders.end()); //Sort acending
	}
	else if(currentState == DOWN){
		sort(orders.rbegin(), orders.rend()); //Sort decending
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

inline const char* Elevator::toString(state s){
	switch(s){
		case UP: return "UP";
    	case DOWN: return "DOWN";
		case IDLE: return "IDLE";
		case OPEN: return "OPEN";
		default: return "Unknown";
	}
}
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

Elevator::Elevator(){
	currentState = IDLE;
	currentFloor = 0;
	direction = DIRN_DOWN;
}

void Elevator::run(){
	switch(currentState){
		case RUN:
			if(elev_get_stop_signal()){
				toEmergency();
			}
			else{
				driveToFloor();
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
				stopAtFloor();
				toIdle();
			}
			break;

		case EMERGENCY:
			break;
	}
	currentState = RUN;
}

void Elevator::toRun(){
	switch(currentState){
		case RUN:
			break;

		case IDLE:
			driveToFloor();
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
			elev_set_motor_direction(DIRN_STOP);
			stopAtFloor();
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

void Elevator::driveToFloor(){
	int destionationFloor = getNextOrder();
	int startFloor = elev_get_floor_sensor_signal();

	while(elev_get_floor_sensor_signal() != destionationFloor){
		//destionationFloor = getNextOrder();

		int tempFloor = elev_get_floor_sensor_signal();
        if(tempFloor != -1){
            elev_set_floor_indicator(tempFloor);
        }

        if(startFloor < destionationFloor){
            elev_set_motor_direction(DIRN_UP);
        }
        else if(startFloor > destionationFloor){
            elev_set_motor_direction(DIRN_DOWN);
        }
    }
    currentFloor = destionationFloor;
}

void Elevator::stopAtFloor(){
    elev_set_floor_indicator(elev_get_floor_sensor_signal());
    elev_set_door_open_lamp(1);
    sleep(3);
    elev_set_door_open_lamp(0);
}

void Elevator::addOrder(int newOrder){
	orders.push_back(newOrder);
}

int Elevator::getNextOrder(){
	int nextOrder = orders.front();
	orders.pop_front();
	return nextOrder;
}
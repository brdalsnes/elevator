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

using namespace std;

OrderManager::OrderManager(int nElevators){
	this->nElevators = nElevators;
	addElevators(nElevators);
	nextOrder = 0;

	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){
			buttonMatrix[i][j] = 0;
		}
	}
}

void OrderManager::run(){
	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){
			//Listen for orders
			if(elev_get_button_signal((elev_button_type_t)j, i)){
				buttonMatrix[i][j] = 1;
			}
			//Set lights
			elev_set_button_lamp((elev_button_type_t)j, i, buttonMatrix[i][j]);

			//Add and clear
			if(buttonMatrix[i][j]){
				elevators[0].addOrder(i, (elev_button_type_t)j);
				//printf("%s\n", toString(elevators[0].getCurrentState()));
				if(elevators[0].getCurrentState() == OPEN){
					buttonMatrix[elev_get_floor_sensor_signal()][j] = 0;
				}
			}
		}
	}
} 

void OrderManager::manage(){
	elevators[0].run(); //Temporerally
}

void OrderManager::addElevators(int nElevators){
	for(int i = 0; i < nElevators; i++){
		Elevator newElevator;
		elevators.push_back(newElevator);
	}
}

inline const char* OrderManager::toString(state s){
	switch(s){
		case RUN: return "RUN";
		case IDLE: return "IDLE";
		case OPEN: return "OPEN";
		case EMERGENCY: return "EMERGENCY";
		default: return "Unknown";
	}
}
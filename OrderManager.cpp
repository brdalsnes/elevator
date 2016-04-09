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
		}
	}
	findNextOrder();
}

void OrderManager::manage(){
	printf("Next: %i\n", nextOrder);
	elevators[0].driveToFloor(nextOrder); //Temporerally
}

void OrderManager::addElevators(int nElevators){
	for(int i = 0; i < nElevators; i++){
		Elevator newElevator;
		elevators.push_back(newElevator);
	}
}

void OrderManager::findNextOrder(){
	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){
			if(buttonMatrix[i][j]){
				nextOrder = i;
				clearOrders(i);
			}
		}
	}
}

void OrderManager::clearOrders(int i){
	for(int k = 0; k < N_BUTTONS; k++){
		buttonMatrix[i][k] = 0;
	}
}
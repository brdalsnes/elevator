#include "OrderManager.h"
#include "Elevator.h"
#include "Timer.h"
#include "elev.h"
#include "channels.h"
#include "io.h"
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <unistd.h>
#include <deque>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define DEFAULT_PORT 20019
#define BROADCAST_IP "127.0.0.1"

using namespace std;

OrderManager::OrderManager(int nElevators){
	this->nElevators = nElevators;
	addElevators(nElevators);
	nextOrder = 0;

	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){
			buttonMatrix[i][j] = 0;
            buffer[i][j] = 0;
		}
	}
}

void OrderManager::addElevators(int nElevators){
	for(int i = 0; i < nElevators; i++){
		Elevator newElevator;
		elevators.push_back(newElevator);
	}
}

void OrderManager::listen(){
    //printf("%s\n", toString(elevators[0].getCurrentState()));

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
					int index = elevators[0].getDirectionIndex();
					int currentFloor = elev_get_floor_sensor_signal();
					buttonMatrix[currentFloor][index] = 0;
					buttonMatrix[currentFloor][2] = 0;
				}
			}
		}
	}
} 

void OrderManager::manage(){
	elevators[0].run(); //Temporerally
}

void OrderManager::code(){
    int n = 0;
    msg = "S";
    for(int i = 0; i < N_FLOORS; i++){
        for(int j = 0; j < N_BUTTONS; j++){
            msg += to_string(buttonMatrix[i][j]);
            n++;
        }
    }
    //printf("CODE: %s\n", msg.c_str());
}

void OrderManager::decode(){
    int n = 1;
    for(int i = 0; i < N_FLOORS; i++){
        for(int j = 0; j< N_BUTTONS; j++){
            buffer[i][j] = msg[n] - 48;
            n++;
        }
    }
    msg = "";
    //printf("DECODED: %i%i%i%i%i%i%i%i%i%i%i%i\n",buffer[0][0],buffer[0][1],buffer[0][2],buffer[1][0],buffer[1][1],buffer[1][2],buffer[2][0],buffer[2][1],buffer[2][2],buffer[3][0],buffer[3][1],buffer[3][2] );
}

inline const char* OrderManager::toString(state s){
    switch(s){
        case UP: return "UP";
        case DOWN: return "DOWN";
        case IDLE: return "IDLE";
        case OPEN: return "OPEN";
        default: return "Unknown";
    }
}
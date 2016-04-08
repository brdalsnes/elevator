#include "OrderManager.h"
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

			//Set que
			if(buttonMatrix[i][j] && notInQue(i)){
				orders[0].push_back(i);

				//Temporerally removes all orders on the floor where it stops
				for(int k = 0; k < N_BUTTONS; k++){
					buttonMatrix[i][k] = 0;
				}
			}	
		}
	}
}

void OrderManager::scanOrders(){
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

    stopAtFloor(destionationFloor);
	
    currentFloor[0] = destionationFloor;
}

bool OrderManager::notInQue(int floorCheck){
	for(int i = 0; i < orders[0].size(); i++){
		if(orders[0][i] == floorCheck){
			return false;
		}
	}
	return true;
}

int OrderManager::getNextOrder(){
	if(orders[0].size() > 0){
		int nextOrder = orders[0].front();
		orders[0].pop_front();
		return nextOrder;
	}
	else{
		return currentFloor[0];
	}
}

void OrderManager::stopAtFloor(int destionationFloor){
	elev_set_motor_direction(DIRN_STOP);
    elev_set_floor_indicator(destionationFloor);
    elev_set_door_open_lamp(1);
    sleep(3);
    elev_set_door_open_lamp(0);
}
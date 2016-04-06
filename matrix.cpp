#include "matrix.h"
#include "elev.h"
#include "channels.h"
#include "io.h"
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <deque>

using namespace std;

unsigned int orders[4][3];
deque<int> priority;

void initMat(){
	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){
			orders[i][j] = 0;
		}
	}
}

void listen(){
	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){

			if(elev_get_button_signal((elev_button_type_t)j, i)){
				orders[i][j] = 1;

				for(int i = 0; i < priority.size(); i++){
            		printf("%i", priority[i]);
        		}
			}	
		}
	}
}

void lamp(){
	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){
			elev_set_button_lamp((elev_button_type_t)j, i, orders[i][j]);
		}
	}
}

bool notInQue(int floor){
	for(int i = 0; i < priority.size(); i++){
		if(priority[i] == floor){
			return false;
		}
	}
	return true;
}

void setQue(){
	for(int i = 0; i < N_FLOORS; i++){
		for(int j = 0; j < N_BUTTONS; j++){
			if(orders[i][j] && notInQue(i)){
				priority.push_back(i);

				for(int k = 0; k < N_BUTTONS; k++){
					orders[i][k] = 0;
				}
			}
		}
	}
}

void driveToFloor(){
	
	if(priority.size() > 0){
		int destionationFloor = priority.front();
		priority.pop_front();
	

    	int startFloor = elev_get_floor_sensor_signal();

    	while(elev_get_floor_sensor_signal() != destionationFloor){

        	if(elev_get_floor_sensor_signal() != -1){
            	elev_set_floor_indicator(elev_get_floor_sensor_signal());
        	}

        	if(startFloor < destionationFloor){
            	elev_set_motor_direction(DIRN_UP);
        	}
        	else if(startFloor > destionationFloor){
            	elev_set_motor_direction(DIRN_DOWN);
        	}
    	}
    	elev_set_motor_direction(DIRN_STOP);

    	elev_set_floor_indicator(destionationFloor);
    }

}


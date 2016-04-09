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

void Elevator::driveToFloor(int destionationFloor){
	int startFloor = elev_get_floor_sensor_signal();

	while(elev_get_floor_sensor_signal() != destionationFloor){
		printf("Dest: %i\n", destionationFloor);
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
    currentFloor = destionationFloor;
}

void Elevator::stopAtFloor(int destionationFloor){
	elev_set_motor_direction(DIRN_STOP);
    elev_set_floor_indicator(destionationFloor);
    elev_set_door_open_lamp(1);
    sleep(3);
    elev_set_door_open_lamp(0);
}


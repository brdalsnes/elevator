
#include <stdio.h>
#include <thread>
#include "elev.h"
#include "Elevator.h"
#include "OrderManager.h"

OrderManager orderManager(1);

void stop(){
	if (elev_get_stop_signal()) {
            elev_set_motor_direction(DIRN_STOP);
        }
}

void drive(){
	while(true){
		orderManager.scanOrders();
	}
}

void listen(){
	while(true){
		stop();
		orderManager.run();
	}
}


int main() {
    elev_init();
    printf("PROGRAM STARTED");

    std::thread t1(drive);
    std::thread t2(listen);

    t1.join();
    t2.join();

}
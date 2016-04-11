
#include <stdio.h>
#include <thread>
#include "elev.h"
#include "OrderManager.h"
#include "Elevator.h"

OrderManager orderManager(1);

void listen(){
	while(true){
		orderManager.run();
	}
}

void drive(){
	while(true){
		orderManager.manage();
	}
}

void backUp(){
	orderManager.backUp();
}

int main() {
	elev_init();
	printf("PROGRAM STARTED");

    std::thread t1(listen);
    std::thread t2(drive);
    std::thread t3(backUp);

    t1.join();
    t2.join();
    t3.join();
}
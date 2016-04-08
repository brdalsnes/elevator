
#include <stdio.h>
#include <thread>
#include "elev.h"
#include "OrderManager.h"

OrderManager orderManager(1);

void listen(){
	while(true){
		orderManager.run();
	}
}

void drive(){
	while(true){
		orderManager.scanOrders();
	}
}

int main() {
	elev_init();
	printf("PROGRAM STARTED");

    std::thread t1(listen);
    std::thread t2(drive);

    t1.join();
    t2.join();

}
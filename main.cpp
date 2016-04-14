
#include <stdio.h>
#include <thread>
#include "elev.h"
#include "OrderManager.h"
#include "Elevator.h"
#include "sendrec.h"

OrderManager orderManager(1);
struct code_message packet;

void listenForOrders(){
	while(true){
		orderManager.listen();
	}
}

void drive(){
	while(true){
		orderManager.manage();
	}
}

void Sender(){
	while(true){
		udp_Sender("Hello moskus\n", 20022);
		usleep(500000);
	}
}

void Reciever(){
	while(true){
		packet = udp_Reciever();
		printf("Rec Message: %s\n", packet.data);
	}
}

int main() {
	elev_init();
	udp_init(20020);
	printf("PROGRAM STARTED\n");

    std::thread t1(listenForOrders);
    std::thread t2(drive);
    //std::thread t3(Sender);
    //std::thread t4(Reciever);


    t1.join();
    t2.join();
    //t3.join();
    //t4.join();
}
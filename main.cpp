
#include <stdio.h>
#include <thread>
#include "elev.h"
#include "OrderManager.h"
#include "Elevator.h"
#include "sendrec.h"
#include "Timer.h"
#include <iostream>

OrderManager orderManager(1);
struct code_message packet;

void listenForOrders(){
	while(true){
		orderManager.listen();
	}
}

void drive(){
	while(true){
		if(orderManager.current_state == SLAVE){
			orderManager.manage();
		}
	}
}

void Sender(){
	while(true){
		switch(orderManager.current_state){
			case MASTER:
				usleep(250000);
				orderManager.code();
				udp_Broadcaster(orderManager.smsg);
				break;
			case SLAVE:
				orderManager.code();
				udp_Sender(orderManager.smsg, MASTERPORT, packet.rip);
				usleep(250000);
				break;
		}
	}
}

void Reciever(){
	while(true){
		switch(orderManager.current_state){
			case MASTER:
				packet = udp_Reciever();
				//printf("Message: %s\n", packet.data);
				orderManager.decode(packet.data);
				printf("CF: %i, SI: %i, DI %i\n", orderManager.elevators[0].currentFloor, orderManager.elevators[0].stateIndex, orderManager.elevators[0].directionIndex);
				break;
			case SLAVE:
				packet = udp_recieve_broadcast();
				//printf("Message: %s\n", packet.data);
				orderManager.decode(packet.data);
				break;
		}
	}
}

int main() {
	elev_init();
	udp_init(MASTERPORT);
	printf("PROGRAM STARTED\n");
	/*int nBytes = 0;
	Timer timer;
	timer.start();
	current_state = MASTER;
	while(!timer.is_time_out(3)){
		nBytes = Bytes();
		if(nBytes > 0){
			current_state = SLAVE;
			packet = udp_recieve_broadcast();
		}
	}
	printf("complete");*/
	
	int input;

	printf("I AM\n");

	std::cin >> input;

	switch(input){
		case 1:
			orderManager.current_state = MASTER;
			break;
		case 2:
			orderManager.current_state = SLAVE;
			break;
	}

    std::thread t1(listenForOrders);
    std::thread t2(drive);
    std::thread t3(Sender);
    std::thread t4(Reciever);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
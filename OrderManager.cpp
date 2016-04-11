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
		}
	}
}

void OrderManager::addElevators(int nElevators){
	for(int i = 0; i < nElevators; i++){
		Elevator newElevator;
		elevators.push_back(newElevator);
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

void OrderManager::backUp(){
	char buffer[1024];
  	int nBytes = 0;

  	int sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  	if(sock < 0){
    	printf("Socket error: %i\n", sock);
  	}

  	int broadcastEnable = 1;

  	struct sockaddr_in sa_in;
  	struct sockaddr_in sa_out;


  	/*Configure settings in address struct*/
  	sa_in.sin_family = AF_INET;
  	sa_in.sin_port = htons(DEFAULT_PORT);
  	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);

  	sa_out.sin_family = AF_INET;
  	sa_out.sin_port = htons(DEFAULT_PORT);
  	//sa_out.sin_addr.s_addr = inet_addr(BROADCAST_IP);
  	sa_out.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  	bind(sock, (struct sockaddr *)&sa_in, sizeof sa_in);

  	State current_state = SLAVE;
  	Timer timer;
  	timer.start();

  	int currentFloor;

    while(true){
    	switch(current_state){
    	case MASTER:
        	sprintf(buffer, "I am your master! %i", elevators[0].getCurrentFloor());
        	//printf("sendto: %i\n", sendto(sock,buffer,1024,0,(struct sockaddr *)&sa_out, sizeof sa_out));
        	//printf("ERROR: %s\n", strerror(errno));
        	sendto(sock,buffer,1024,0,(struct sockaddr *)&sa_out, sizeof sa_out);
        	break;
    	case SLAVE:
        	nBytes = recv(sock,buffer,1024,0);
        	if (nBytes > 0 && !strncmp(buffer, "I am your master!", 17)){
            	timer.start();
        		currentFloor = atoi((&buffer[17]));
        		nBytes = 0;
        	}
        	else if(timer.is_time_out(3)){
        		current_state = MASTER;
        		printf("Hello MASTER\n");
        		close(sock);
        		system("gnome-terminal -e './elevator'");
        		sleep(1);
        		sock = socket(PF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
        		printf("C: %i\n", currentFloor);

        		//Enable UDP broadcast
        		int ret=setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        		if(ret){printf("Error: setsockopt call failed"); }

      		}
      		break;
    	}
  	}
    close(sock);
}

inline const char* OrderManager::toString(state s){
	switch(s){
		case RUN: return "RUN";
		case IDLE: return "IDLE";
		case OPEN: return "OPEN";
		case EMERGENCY: return "EMERGENCY";
		default: return "Unknown";
	}
}
#pragma once

#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<string>

#define BUFLEN 512  //Max length of buffer
#define PORT 20022   //The port on which to listen for incoming data
#define OURADDRESS "129.241.187.155"
#define SADDRESS "129.241.187.155"

struct code_message{
	char * rip;
	int port;
	char * data;
	int length;
};

void die(char *);

void udp_init(int port);
int udp_Sender(std::string message, int port);
struct code_message udp_Reciever();
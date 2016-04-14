#include "sendrec.h"

int lsocket;

void die(char * s)
{
    perror(s);
    exit(1);
}

void udp_init(int port){

    if ((lsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    struct sockaddr_in si_me;
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    //?si_me.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, OURADDRESS, &si_me.sin_addr);

    if( bind(lsocket, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
}

int udp_Sender(std::string message, int port)
{
    struct sockaddr_in si_me;
    char sbuff[BUFLEN];
    strcat(sbuff, message.c_str());
    //create a UDP socket
     
    // zero out the structur
    
    //memset((char *) &si_other, 0, sizeof(si_other));
    
    /*si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    inet_pton(AF_INET, SADDRESS, &si_other.sin_addr);*/
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    inet_pton(AF_INET, OURADDRESS, &si_me.sin_addr);
    
    if (sendto(lsocket, sbuff, BUFLEN, 0, (struct sockaddr*) &si_me, sizeof(si_me)) == -1)
    {
        die("sendto()");
        
    }
      //printf("Sendt packet to %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data from sender: %s\n" , sbuff);    
 
    return 0;
}

struct code_message udp_Reciever()
{
    struct sockaddr_in si_me;
    int recv_len;
    socklen_t slen = sizeof(si_me); //from si_other to si_me
    char rbuff[BUFLEN];
    struct code_message code;
    
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
    // memset((char *) &si_other, 0, sizeof(si_other));
    
    /*si_other.sin_family = AF_INET;
    si_other.sin_port = PORT;
    inet_pton(AF_INET, SADDRESS, &si_other.sin_addr);/
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    inet_pton(AF_INET, OURADDRESS, &si_me.sin_addr);*/
     
    //keep listening for data
    printf("Waiting for data...");
      
    memset(&rbuff[0], 0, sizeof(rbuff)); 
    //try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(lsocket, rbuff, BUFLEN, 0, (struct sockaddr *)&si_me, &slen)) == -1)
    {
        die("recvfrom()");
    }
    code.data = (char *) malloc(sizeof(char)*recv_len);
    memcpy(code.data,rbuff, sizeof(*code.data));
    code.rip = inet_ntoa(si_me.sin_addr);
    code.port = si_me.sin_port;

    //print details of the client/peer and the data received
    //printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Data received: %s\n" , rbuff);

 
    return code;
}
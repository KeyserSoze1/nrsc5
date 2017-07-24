#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> //memset

#include "ext_com.h"

#include "input.h"

#define BUFLEN 32  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

void *msg_listener(void *input)
{
    input_t *st = input;
    struct sockaddr_in si_me, si_other;
     
    int s, recv_len;
    unsigned int slen = sizeof(si_other);
    unsigned char buf[BUFLEN];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Unable to create UDP Socket.\n");
        exit(1);
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        printf("Unable to bind to UDP port.\n");
        exit(1);
    }

    printf("Listening for program change...");

    //keep listening for data
    while(1)
    {    
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            printf("No data received.\n");
        }
         
        if (recv_len > 1)
            continue;

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Program being changed to: %d\n" , buf[0]);
         
        input_set_program(st, buf[0]);
    }
 
    close(s);

    exit(0);
}

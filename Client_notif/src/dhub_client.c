#include "dhub_client.h"
#include <time.h>
#include <unistd.h>

#define RANDOM_VALUE_LIMIT 50
#define SEND_INTERVAL_IN_US 1000 * 100

float generate_random_float()
{
    srand(time(NULL));
    float ret = ((float)rand()/(float)RAND_MAX) * RANDOM_VALUE_LIMIT;
    return ret;
}

int main()
{
    int i;
    int clientId;
    char clientQName[MAX_MESSAGE_SIZE];
    mqd_t serverQ, clientQ;
    struct dataPoint data;
    zMessage msgSend;
    zMessage msgRecv;
    zNotification msgNotif;

    serverQ = mq_open(SERVER_Q_NAME, O_RDWR);
    if(serverQ == -1)
    {
        perror("Cannot open server Q. Is it running?");
        exit(1);
    }
    else
    {
        printf("Connected to Server Message Q\n");
    }

    // Send register request to Server
    memset(&msgSend, 0, sizeof(zMessage));
    snprintf(msgSend.operation, MAX_MESSAGE_SIZE, "register-client");
    if (mq_send(serverQ, (char*)&msgSend, sizeof(zMessage), 0) == -1)
    {
        perror("Cannot send register command to Server");
        exit(1);
    }
    else
    {
        printf("Successfully Sent register Command\n");
        
        // Receive client Q name
        if (mq_receive(serverQ, (char*)&msgRecv, sizeof(zMessage), NULL) == -1)
        {
            perror("Cannot receive client Q name");
        }
        else
        {
            printf("Got client name: %s\n", msgRecv.clientQName);
        }

        //Open the client Q
        clientQ = mq_open(msgRecv.clientQName, O_RDWR);
        if (clientQ == -1)
        {
            perror("Cannot open the client Q sent by the server");
            exit(1);
        }
        else
        {
            strcpy(clientQName, msgRecv.clientQName);
            printf("Successfully connected to client Q\n");
        }
    }

    // Register for notification
    memset(&msgSend, 0, sizeof(zMessage));
    snprintf(msgSend.operation, MAX_MESSAGE_SIZE, "register-notification");
    if (mq_send(serverQ, (char*)&msgSend, sizeof(zMessage), 0) == -1)
    {
        perror("Cannot Send notification request to Server");
    }
    else
    {
        printf("Successfully registered for notification\n");
    }


    while(1)
    {
        usleep(SEND_INTERVAL_IN_US);
    }
    return 0;
}
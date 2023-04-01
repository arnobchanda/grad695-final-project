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

    while(1)
    {
        // Send random stuff to DHUB
        memset(&msgSend, 0, sizeof(zMessage));
        strcpy(msgSend.clientQName, clientQName);
        snprintf(msgSend.operation, MAX_MESSAGE_SIZE, "update-request");
        msgSend.data.current = generate_random_float();
        msgSend.data.temperature = generate_random_float();
        msgSend.data.voltage = generate_random_float();

        if (mq_send(serverQ, (char*)&msgSend, sizeof(zMessage), 0) == -1)
        {
            perror("Cannot send data to DHub Server");
        }
        else
        {
            printf("Successfully sent data to DHub Server\n");
        }

        // Wait for the update Ack
        memset(&msgRecv, 0, sizeof(zMessage));
        if (mq_receive(serverQ, (char*)&msgRecv, sizeof(zMessage), NULL) == -1)
        {
            perror("Did not get update ack from DHub");
        }
        else
        {
            if (strcmp(msgRecv.operation, "update-ack") != 0)
            {
                perror("Did not receive proper command");
                printf("Command Received: %s\n", msgRecv.operation);
                continue;
            }
            else
            {
                printf("Got update ack from DHub\n");
            }
        }

        usleep(SEND_INTERVAL_IN_US);
    }
    return 0;
}
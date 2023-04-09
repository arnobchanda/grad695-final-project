#include "dhub_client.h"
#include <time.h>
#include <unistd.h>

#define SEND_INTERVAL_IN_US 1000 * 100

int main()
{
    int i;
    int clientId;
    char clientQName[MAX_MESSAGE_SIZE];
    char notifQName[MAX_MESSAGE_SIZE];
    mqd_t serverQ, clientQ, notifQ;
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

        // Copy this client's ID
        clientId = msgRecv.clientId;
    }

    usleep(SEND_INTERVAL_IN_US);
    // Register for notification
    memset(&msgSend, 0, sizeof(zMessage));
    msgSend.clientId = clientId;
    snprintf(msgSend.operation, MAX_MESSAGE_SIZE, "register-notification");

    // Notification Subscription
    msgSend.notif.dataPointSubscribed[0] = 1;

    if (mq_send(serverQ, (char*)&msgSend, sizeof(zMessage), 0) == -1)
    {
        perror("Cannot Send notification request to Server");
    }
    else
    {
        printf("Sent notification register request to server\n");
    }

    memset(&msgRecv, 0, sizeof(zMessage));
    if(mq_receive(serverQ, (char*)&msgRecv, sizeof(zMessage), NULL) == -1)
    {
        perror("Cannot receive notification name");
    }
    else
    {
        printf("Got notification Q name: %s\n",msgRecv.notif.notificationQName);
    }

    strcpy(notifQName, msgRecv.notif.notificationQName);
    notifQ = mq_open(notifQName, O_RDONLY);
    if (notifQ == -1)
    {
        perror("Cannot connect to notification Q\n");
    }
    else
    {
        printf("Successfully connected to notification Q\n");
    }

    //wait for notification
    while(1)
    {
        memset(&msgSend, 0, sizeof(msgSend));
        if(mq_receive(notifQ, (char*)&msgRecv, sizeof(zMessage), NULL) != -1)
        {
            printf("Got notification from server\n");
            for (int i = 0; i<MAX_NUM_DATA_POINTS; i++)
            {
                if(msgRecv.notif.dataPointNotif[i] == 1)
                {
                    printf("Notification received for Message Id: %d\n",i);

                    //Send a get-request to the Server
                    snprintf(msgSend.operation, MAX_MESSAGE_SIZE, "get-request");
                    msgSend.clientId = clientId;
                    mq_send(serverQ, (char*)&msgSend, sizeof(zMessage), 0);

                    // Wait for the data points from Server
                    memset(&msgRecv, 0, sizeof(msgRecv));
                    mq_receive(clientQ, (char*)&msgRecv, sizeof(zMessage), NULL);

                    // Print values
                    switch(i)
                    {
                        case 0:
                            printf("Current: %f\n", msgRecv.data.current);
                            break;
                        case 1:
                            printf("voltage: %f\n", msgRecv.data.voltage);
                            break;
                        case 3:
                            printf("Temperature: %f\n", msgRecv.data.temperature);
                            break;
                        default:
                            break;
                    }

                }
            }
        }
        usleep(SEND_INTERVAL_IN_US);
    }
    return 0;
}
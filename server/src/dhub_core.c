#include "dhub_core.h"
#include <signal.h>
#include <unistd.h>

#define DHUB_CORE_THROTTLE 1000 * 10

zClient clients[MAX_NUM_CLIENTS] = {0};
static zData database = {0};

int checkOpenMessageQ()
{
    int ret;
    ret = mq_unlink(SERVER_Q_NAME);
    if (ret < 0)
    {
        printf("Warning: %d (%s) on server message Q unlink\n", errno, strerror(errno));
    }
    return ret;
}

void printDatabase(void)
{
    printf("\n |---------DATABASE----------|\n");
    printf("|Voltage     | %f      |\n", database.voltage);
    printf("|Current     | %f      |\n", database.temperature);
    printf("|Temperature | %f      |\n", database.temperature);
    printf("|--------------------------|\n");
}

void disconnect_handler(int signum)
{
    for (int i=0; i<MAX_NUM_CLIENTS; i++)
    {
        if ( (clients[i].clientQ != 0) && (sizeof(clients[i].clientMsgQName) > 1))
        {
            mq_unlink(clients[i].clientMsgQName);
            printf("\nClosing Message Q for: %s\n",clients[i].clientMsgQName);
        }
    }
    printf("Closing Message Q for DHub Core\n");
    mq_unlink(SERVER_Q_NAME);

    printf("Exiting ...\n");
    exit(0);
}

int main()
{
    int i,j;
    int numClients = 0;
    mqd_t serverQ;
    char clientQName[MAX_MESSAGE_SIZE];
    struct mq_attr serverQAttributes;
    struct mq_attr notifQAttributes;
    
    serverQAttributes.mq_flags = 0;
    serverQAttributes.mq_maxmsg = 10;
    serverQAttributes.mq_msgsize = sizeof(zMessage);
    serverQAttributes.mq_curmsgs = 0;

    // checkOpenMessageQ();

    serverQ = mq_open(SERVER_Q_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &serverQAttributes);

    if (serverQ == -1)
    {
        perror("Cannot open Server Message Q");
        exit(1);
    }
    else
    {
        printf("DHUB Core Server Started\n");
    }

    while (1)
    {
        zMessage msgSend;
        zMessage msgRecv;
        mqd_t clientQ;
        char clientQName[MAX_MESSAGE_SIZE];

        signal(SIGINT, disconnect_handler);

        memset(&msgRecv, 0, sizeof(msgRecv));

        // Get a message from a client
        if (mq_receive(serverQ, (char*)&msgRecv, sizeof(zMessage), NULL) == -1)
        {
            perror("Error Receiving Message");
            continue;
        }
        else
        {
            printf("Received Message: %s\n",msgRecv.operation);
        }

        //Check if this is a register request
        if(strcmp(msgRecv.operation, "register-client") == 0 )
        {
            printf("Registering Client...\n");
            if(numClients >= MAX_NUM_CLIENTS)
            {
                printf("Max numbers of clients reached. Cannot create more!\n");
                printf("Client registration failed\n");
                continue;
            }

            //Create Client message Q
            memset(&msgSend, 0, sizeof(zMessage));
            snprintf(clientQName, MAX_MESSAGE_SIZE, "/client_%d",numClients);
            clientQ = mq_open(clientQName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, NULL);
            if (clientQ == -1)
            {
                perror("Error creating client Q");
                printf("Client registration failed\n");
                continue;
            }
            else
            {
                printf("Created Client Message Q\n");
                strcpy(msgSend.clientQName, clientQName);
            }
            
            //Send Client Q name to the Client
            if(mq_send(serverQ, (char*)&msgSend, sizeof(zMessage), 0) == -1)
            {
                perror("Cannot send client Message");
                printf("Cannot send client Q name to client\n");
                mq_close(clientQ);
                continue;
            }
            else
            {
                // Update client info
                clients[numClients].clientQ = clientQ;
                strcpy(clients[numClients].clientMsgQName,clientQName);
                for (i = 0; i< MAX_NUM_DATA_POINTS; i++)
                {
                    clients[numClients].dataPointSubscribed[i] = 0;
                }
                numClients++;

                printf("Client registration successfull. Total Clients: %d\n", numClients);
            }
        }

        // Check if this is an update request
        if( strcmp(msgRecv.operation, "update-request") == 0 )
        {
            printf("Got a data update request from Client. Updating database...\n");
            // Update data point in memory Map
            if (msgRecv.data.current != 0)
            {
                database.current = msgRecv.data.current;
            }

            if (msgRecv.data.voltage != 0)
            {
                database.voltage = msgRecv.data.voltage;
            }

            if(msgRecv.data.temperature != 0)
            {
                database.temperature = msgRecv.data.temperature;
            }

            // Send Update Ack to client
            memset(&msgSend, 0, sizeof(zMessage));
            strcpy(msgSend.operation, "update-ack");
            if (mq_send(serverQ, (char*)&msgSend, sizeof(zMessage), 0) == -1)
            {
                perror("Cannot send Update Ack");
            }
            else
            {
                printf("Successfully sent update-ack");
            }

            printDatabase();
            // Notify all clients 
        }

        // Check if this is a get request
        if( strcmp(msgRecv.operation, "get-request") == 0 )
        {
            //Send data point to client 
        }

        if( strcmp(msgRecv.operation, "register-notification") == 0 )
        {
            //Update client details

        }

        usleep(DHUB_CORE_THROTTLE);
    }

    return 0;
}
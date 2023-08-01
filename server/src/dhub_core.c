#include "dhub_core.h"

#define DHUB_CORE_THROTTLE 1000 * 10

zClient clients[MAX_NUM_CLIENTS] = {0};
static zData database = {0};

int datapointChanged[MAX_NUM_DATA_POINTS] = {0};

void disconnect_handler(int signum)
{
    for (int i=0; i<MAX_NUM_CLIENTS; i++)
    {
        if ( (clients[i].clientQ != 0) && (sizeof(clients[i].clientMsgQName) > 1))
        {
            mq_unlink(clients[i].clientMsgQName);
            printf("Closing Message Q: %s\n",clients[i].clientMsgQName);
        }
        if ( (clients[i].notificationQ != 0) && (sizeof(clients[i].notificationQName)) > 1)
        {
            mq_unlink(clients[i].notificationQName);
            printf("Closing notification Q: %s\n",clients[i].notificationQName);
        }
    }
    printf("Closing Message Q for Dhub Core\n");
    mq_unlink(SERVER_Q_NAME);

    printf("Exiting ...\n");
    exit(0);
}

void update_database(float *database_var, float *msg_var, int index)
{
    if (*database_var != *msg_var)
    {
        *database_var = *msg_var;
        datapointChanged[index-1] = 1;
    }
    else
    {
        datapointChanged[index-1] = 0;
    }
}

int main()
{
    int i,j;
    int numClients = 0;
    int clientId;
    mqd_t serverQ;
    mqd_t clientQ;
    char clientQName[MAX_MESSAGE_SIZE];
    zMessage msgSend;
    zMessage msgRecv;
    
    serverQ = create_queue(SERVER_Q_NAME);
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
        signal(SIGINT, disconnect_handler);
        clear_buffer(&msgRecv, sizeof(msgRecv));

        // Get a message from a client
        receive_message_from_Q(serverQ, &msgRecv, MSG_SIZE);
        printf("Received Message: %s\n",msgRecv.operation);

        //Check if this is a register client request
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
            snprintf(clientQName, MAX_MESSAGE_SIZE, "/client_%d",numClients);
            clientQ = create_queue(clientQName);
            printf("Created Client Message Q\n");

            // Update send message
            clear_buffer(&msgSend, sizeof(msgSend));
            strcpy(msgSend.clientQName, clientQName);
            msgSend.clientId = numClients;
            
            //Send Client Q name to the Client
            send_message_on_Q(serverQ, &msgSend, MSG_SIZE);
            
            // Update global client info
            clients[numClients].clientId = numClients;
            clients[numClients].clientQ = clientQ;
            strcpy(clients[numClients].clientMsgQName,clientQName);

            // Clear datapoint subsription info for client
            for (i = 0; i< MAX_NUM_DATA_POINTS; i++)
            {
                clients[numClients].dataPointSubscribed[i] = 0;
            }
            numClients++;

            printf("Client registration successfull. Total Clients: %d\n", numClients);
        }

        // Check if this is an update request
        if( strcmp(msgRecv.operation, "update-request") == 0 )
        {
            clientId = msgRecv.clientId;
            printf("Got a data update request from Client:%d. Updating database...\n",clientId);
            
            // Update data point in memory Map
            update_database(&database.var1, &msgRecv.data.var1, 1);
            update_database(&database.var2, &msgRecv.data.var2, 2);
            update_database(&database.var3, &msgRecv.data.var3, 3);
            update_database(&database.var4, &msgRecv.data.var4, 4);
            update_database(&database.var5, &msgRecv.data.var5, 5);
            update_database(&database.var6, &msgRecv.data.var6, 6);
            update_database(&database.var7, &msgRecv.data.var7, 7);
            update_database(&database.var8, &msgRecv.data.var8, 8);
            update_database(&database.var9, &msgRecv.data.var9, 9);
            update_database(&database.var10, &msgRecv.data.var10, 10);

            // Send Update Ack to client
            clear_buffer(&msgSend, sizeof(msgSend));
            strcpy(msgSend.operation, "update-ack");
            printf("Sending Update Ack to client\n");
            send_message_on_Q(clients[clientId].clientQ, &msgSend, MSG_SIZE);

            // Notify all clients
            clear_buffer(&msgSend, sizeof(msgSend));
            for (int i=0; i<numClients; i++)
            {
                if(clients[i].notificationQ != 0)
                {
                    // copy data point change info
                    for (int j = 0; j<MAX_NUM_DATA_POINTS; j++)
                    {
                        if (clients[i].dataPointSubscribed[j] == datapointChanged[j])
                        {
                            msgSend.notif.dataPointNotif[j] = datapointChanged[j];
                        }
                    }

                    send_message_on_Q(clients[i].notificationQ, &msgSend, MSG_SIZE);
                    printf("Sent update notification to client: %d\n", i);
                }
            } 
        }

        // Check if this is a get request
        if( strcmp(msgRecv.operation, "get-request") == 0 )
        {
            //Send data point to client
            clear_buffer(&msgSend, sizeof(msgSend));
            msgSend.data.var1 = database.var1;
            msgSend.data.var2 = database.var2;
            msgSend.data.var3 = database.var3;
            msgSend.data.var4 = database.var4;
            msgSend.data.var5 = database.var5;
            msgSend.data.var6 = database.var6;
            msgSend.data.var7 = database.var7;
            msgSend.data.var8 = database.var8;
            msgSend.data.var9 = database.var9;
            msgSend.data.var10 = database.var10;
            
            send_message_on_Q(clients[msgRecv.clientId].clientQ, &msgSend, MSG_SIZE);
        }

        // Check if this is a register-notification request
        if( strcmp(msgRecv.operation, "register-notification") == 0 )
        {
            //Create notification queue
            int client = msgRecv.clientId;
            char exepectednotificationQName[MAX_MSGQ_NAME_SIZE];
            snprintf(exepectednotificationQName, MAX_MSGQ_NAME_SIZE, "/notif_%d", client);

            printf("Expected Notification Q name: %s\n", exepectednotificationQName);
            printf("notifcation Q Name: %s\n", clients[client].notificationQName);
            printf("Notificatoin Q Length: %ld\n", strlen(clients[client].notificationQName));

            if (strlen(clients[client].notificationQName) != 8)
            {
                snprintf(clients[client].notificationQName, MAX_MSGQ_NAME_SIZE, "/notif_%d", client);
                clients[client].notificationQ = create_queue(clients[client].notificationQName);
                printf("Notification Q opened for client: %d\n",client);

                // Update Data point notifications for the client
                for (i=0; i<MAX_NUM_DATA_POINTS; i++)
                {
                    clients[client].dataPointSubscribed[i] = msgRecv.notif.dataPointSubscribed[i];
                }

                //Send notification name
                memset(&msgSend, 0, sizeof(zMessage));
                msgSend.clientId = client;
                strcpy(msgSend.clientQName, clients[client].clientMsgQName);
                strcpy(msgSend.notif.notificationQName, clients[client].notificationQName);
                send_message_on_Q(serverQ, &msgSend, MSG_SIZE);
                printf("Sent notif Q name to client\n");
            }
            else
            {
                for (i = 0; i<MAX_NUM_DATA_POINTS; i++)
                {
                    printf("Notification Scheme: %d\n", msgRecv.notif.dataPointSubscribed[i]);
                }
                printf("\n");
                for (i=0; i<MAX_NUM_DATA_POINTS; i++)
                {
                    clients[client].dataPointSubscribed[i] = msgRecv.notif.dataPointSubscribed[i];
                }
                printf("Updated notification scheme\n");
            }
        }

        usleep(DHUB_CORE_THROTTLE);
    }

    return 0;
}
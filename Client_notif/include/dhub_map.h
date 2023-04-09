#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define MAX_NUM_DATA_POINTS 10
#define MAX_MESSAGE_SIZE 256
#define MAX_NUM_CLIENTS 10
#define MAX_MSGQ_NAME_SIZE 128

#define SERVER_Q_NAME "/dhub_core"

typedef enum dataId {
    voltage = 1,
    current = 2,
    temperature = 3
}eDataId;

typedef struct dataPoint {
    float voltage;
    float current;
    float temperature;
}zData;

typedef struct client {
    int clientId;
    mqd_t clientQ;
    mqd_t notificationQ;
    char clientMsgQName[MAX_MSGQ_NAME_SIZE];
    char notificationQName[MAX_MSGQ_NAME_SIZE];
    int dataPointSubscribed[MAX_NUM_DATA_POINTS];
}zClient;

typedef struct notification {
    char notificationQName[MAX_MESSAGE_SIZE];
    char operation[MAX_MESSAGE_SIZE];
    int dataPointSubscribed[MAX_NUM_DATA_POINTS];
    int dataPointNotif[MAX_NUM_DATA_POINTS];
}zNotification;

typedef struct message {
    int clientId;
    char clientQName[MAX_MESSAGE_SIZE];
    char operation[MAX_MESSAGE_SIZE];
    zData data;
    zNotification notif;
}zMessage;

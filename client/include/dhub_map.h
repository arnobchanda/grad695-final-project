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
    mqd_t clientQ;
    char clientMsgQName[MAX_MSGQ_NAME_SIZE];
    int dataPointSubscribed[MAX_NUM_DATA_POINTS];
}zClient;

typedef struct Message {
    char clientQName[MAX_MESSAGE_SIZE];
    char operation[MAX_MESSAGE_SIZE];
    zData data;
}zMessage;


#include "dhub_map.h"
#define MSG_SIZE 8192

void disconnect_handler(int signum);
void clear_buffer(void *buffer, size_t bufferSize);
mqd_t create_queue(char* qName);
int receive_message_from_Q( mqd_t qfp, void* recvBuffer, size_t bufferSize );
int send_message_on_Q( mqd_t qfp, void* sendBuffer, size_t bufferSize );
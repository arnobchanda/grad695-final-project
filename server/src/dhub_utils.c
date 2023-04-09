#include "dhub_utils.h"

void clear_buffer(void *buffer, size_t bufferSize)
{
    memset(&buffer, 0, bufferSize);
}

mqd_t create_queue( char* qName )
{
    mqd_t qFileDescriptor;
    struct mq_attr qAttributes;
    
    qAttributes.mq_flags = 0;
    qAttributes.mq_maxmsg = 10;
    qAttributes.mq_msgsize = sizeof(zMessage);
    qAttributes.mq_curmsgs = 0;

    qFileDescriptor = mq_open(qName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, qAttributes);
    if (qFileDescriptor == -1)
    {
        printf("Cannot Create Message Q: %s\n", qName);
    }

    return qFileDescriptor;
}

int receive_message_from_Q( mqd_t qfp, void* recvBuffer, size_t bufferSize )
{
    int ret = 0;
    ret = mq_receive(qfp, (char*)recvBuffer, bufferSize, NULL);
    if (ret == -1)
    {
        perror("Error receving message");
    }

    return ret;
}

int send_message_on_Q( mqd_t qfp, void* sendBuffer, size_t bufferSize )
{
    int ret = 0;
    ret = mq_send(qfp, (char*)sendBuffer, bufferSize, 0);
    if (ret == -1)
    {
        perror("Cannot send message on Queue");
    }

    return ret;
}
#ifndef __IPC_DATA_H__
#define __IPC_DATA_H__
#include <sys/types.h>

#define  KEY_NUM   1238

typedef struct 
{
    unsigned short operationCode;
    unsigned char subCode;
    unsigned char targetModule;
    unsigned char state1:2;
    unsigned char state2:1;
    unsigned char state3:4;
    char contents1[256];
    char contents2[1024];
}request_payload_t;

typedef struct 
{
    pid_t clientPid;
    int requestId;
    //int priority;
    request_payload_t payload;
}request_t;

typedef struct 
{
    int requestId;
    int result;
}response_t;

typedef enum 
{
    STATUS_IDLE,
    STATUS_REQ_START,        
    STATUS_REQ_COMPLETE,     
    STATUS_PROCESSING,      
    STATUS_RESP_START,       
    STATUS_RESP_COMPLETE    
} MessageStatus_t;

typedef struct
{
    pthread_mutex_t mtx;
    MessageStatus_t messageStatus;
    pid_t server_pid;
    request_t request;
    response_t response;
}ipc_data_t;

#endif //__IPC_DATA_H__


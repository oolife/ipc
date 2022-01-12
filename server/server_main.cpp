#include <iostream>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "SharedMem.h"

using namespace std;

#ifdef USE_SIGNAL
static int SharedMemoryWritePID(pid_t pid);
void signalHandler( int signum ) {
    std::thread t([]()
    {
        std::cout << "[[New Request]]" << endl;
        request_t request;
        
        SharedMemoryReadRequest(request); 

        cout<<"Client PID:" << request.clientPid << endl;
        //cout<<"RequestID:" << request.requestId<<"(Priority:" << request.priority <<")"<< endl;
        cout<<"    OperationCode:" << request.payload.operationCode <<endl;
        cout<<"    subCode:" << (int) request.payload.subCode <<endl;
        cout<<"    state: (" << (int) request.payload.state1 << ", " << (int)request.payload.state2 << ", " << (int)request.payload.state3 << ")"<< endl;
        cout<<"    contents1: " << request.payload.contents1 << endl;
        cout<<"    contents2: " << request.payload.contents2 << endl << endl;

        response_t response;;
        response.requestId = request.requestId;
        response.result = 0;// success;

        SharedMemoryWriteResponse(response);
        kill(request.clientPid, SIGUSR1);
    }
    );
    t.detach();     
}

static int SharedMemoryWritePID(pid_t pid)
{
    ((ipc_data_t *)g_ShmAddr)->server_pid = pid;
    return 0;
}
#endif

static int readRequest(request_t& request)
{
    SharedMem *shmem = SharedMem::getInstance();

    while (shmem->SharedMemoryReadStatus() != STATUS_REQ_COMPLETE)
    {
        usleep(1000);
    }
        
    shmem->SharedMemorySetStatus(STATUS_PROCESSING);
    shmem->SharedMemoryReadRequest(request); 

    return 1;
}

static void printRequest(request_t& request)
{
    cout << "[Request]" << endl;
    cout<<"    OperationCode:" << request.payload.operationCode <<endl;
    cout<<"    subCode:" << (int) request.payload.subCode <<endl;
    cout<<"    targetModule:" << (int) request.payload.targetModule <<endl;
    cout<<"    state: (" << (int) request.payload.state1 << ", " << (int)request.payload.state2 << ", " << (int)request.payload.state3 << ")"<< endl;
    cout<<"    contents1: " << request.payload.contents1 << endl;
    cout<<"    contents2: " << request.payload.contents2 << endl << endl;
}

static int processByTargetModule(request_t& request)
{
    int moduleId = (int)request.payload.targetModule;

    if(moduleId%5==0)
        return 0;
    else
        return 1;
}

static response_t processRequest(request_t& request)
{
    response_t response;

    response.clientPid = request.clientPid;
    response.requestId = request.requestId;
    response.result = processByTargetModule(request);
    //cout << "result : " << response.result << endl; 
    return response;
}

static void sendResponse(response_t& response)
{
    SharedMem *shmem = SharedMem::getInstance();
    shmem->SharedMemorySetStatus(STATUS_RESP_START);
    shmem->SharedMemoryWriteResponse(response);
    shmem->SharedMemorySetStatus(STATUS_RESP_COMPLETE);
}

int main(int argc, char *argv[])
{   
    SharedMem *shmem = SharedMem::getInstance();

    //create shared memory
    //shared memory is created by the server only.
    shmem->AllocSharedMemory();
    while(1)
    {
        request_t request;
        if(readRequest(request))    //read request shared memory
        {
            printRequest(request);  //print payload
            response_t response = processRequest(request);//process payload
            sendResponse(response); //send response to client
        }  
        usleep(1000);
    }

    //release shared memory
    shmem->DeallocSharedMemory(true);
    return 0;
}

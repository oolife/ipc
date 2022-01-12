
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include "ipc_data.h"
#include "Request.h"
#include "SharedMem.h"

using namespace std;
 
static pid_t g_myPid;

#ifdef USE_SIGNAL
//#include <signal.h>
static pid_t server_pid;
static int SharedMemoryReadPID(pid_t* server_pid);
static int SharedMemoryReadPID(pid_t* server_pid)
{
    *server_pid = ((ipc_data_t *)g_ShmAddr)->server_pid;
    return 0;
}
#endif

int Request::s_requestId = 0;

int sendRequest(string str)
{
    unsigned short operationCode;
    unsigned char subCode;
    unsigned char targetModule;
    unsigned char state1, state2, state3;
    string contents1, contents2;

    try { 
        operationCode = (unsigned short) std::stoi(str);
        
        size_t nextPos = str.find(",")+1;
        str = str.substr(nextPos);
        subCode = (unsigned char)std::stoi(str);
        
        nextPos = str.find(",")+1;
        str = str.substr(nextPos);
        targetModule = (unsigned char)std::stoi(str);

        nextPos = str.find(",")+1;
        str = str.substr(nextPos);
        state1 = (unsigned char)std::stoi(str);
        
        nextPos = str.find(",")+1;
        str = str.substr(nextPos);
        state2 = (unsigned char)std::stoi(str);

        nextPos = str.find(",")+1;
        str = str.substr(nextPos);
        state3 = (unsigned char)std::stoi(str);

        nextPos = str.find(",")+1;
        str = str.substr(nextPos);

        nextPos = str.find("\"")+1;
        str = str.substr(nextPos);
        size_t lastPos = str.find("\"");
        contents1 = str.substr(0, lastPos);

        str = str.substr(lastPos+1);
        nextPos = str.find("\"")+1;
        str = str.substr(nextPos);
        lastPos = str.find("\"");
        contents2 = str.substr(0, lastPos);
        //cout << "operationCode: " << operationCode << endl;
        //cout << "subCode: " << subCode << endl;
        //cout << "targetModule: " << targetModule << endl;
        //cout << "state: {" << state1 << ", " << state2 << ", " << state3 << "}"<< endl;
        //cout << "contents1: " << contents1 << endl;
        //cout << "contents2: " << contents2 << endl;
    }
    catch (const std::invalid_argument& ia) {
	    cerr << "Input file error (Invalid argument: " << ia.what() << ")" << endl;
        return 0;
    }

    SharedMem *shmem = SharedMem::getInstance();
    while (shmem->SharedMemoryReadStatus() != STATUS_IDLE)
    {
            usleep(1000);
    }

    Request myRequest = Request(operationCode, targetModule)
                        .setSubCode(subCode)
                        .setState(state1,state2, state3)
                        .setContents(contents1, contents2);
    request_t request;
    myRequest.createIPCData(request);

    
    shmem->SharedMemorySetStatus(STATUS_REQ_START);
    shmem->SharedMemoryWriteRequest(request);
    shmem->SharedMemorySetStatus(STATUS_REQ_COMPLETE);

    while (shmem->SharedMemoryReadStatus() != STATUS_RESP_COMPLETE || shmem->SharedMemoryReadRespPid() != g_myPid)
    {
        usleep(1000);
    }

    response_t response;;
    shmem->SharedMemoryReadResponse(response); 
    cout << "[Response]  " <<"RequestID: " << response.requestId<<" (Result: " << response.result <<")"<< endl;

    shmem->SharedMemorySetStatus(STATUS_IDLE);
    return 1;
}


int main(int argc, char *argv[])
{    
    if(argc < 2)
    {
        cerr << "No input file" << endl;
        cout << "Usage: " << argv[0] << "  $(Input File)"<< endl;
    }
    g_myPid = getpid();

    SharedMem *shmem = SharedMem::getInstance();
    shmem->LInkSharedMemory();

    fstream inputFile;
    inputFile.open(argv[1],ios::in);    // open a file to perform write operation using file object

    if (inputFile.is_open()){           //checking whether the file is open
        string tp;
        while(getline(inputFile, tp)){ //read data from file object and put it into string.
            //cout << tp << "\n"; //print the data of the string
            sendRequest(tp);
            sleep(1);
        }
    }
    inputFile.close(); //close the file object.

    shmem->UnlinkSharedMemory();
    return 0;
}


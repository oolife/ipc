#include <iostream>
#include <sys/shm.h>
#include "SharedMem.h"
#include "ipc_data.h"

bool SharedMem::instanceFlag = false;
SharedMem* SharedMem::m_instance = nullptr;

// 싱글턴 인스턴스를 반환할 멤버함수
SharedMem* SharedMem::getInstance(){
	if(!m_instance){
		m_instance = new SharedMem();
		instanceFlag = true;
	}
	return m_instance;
}

int SharedMem::AllocSharedMemory()
{
    size_t size = sizeof(ipc_data_t);

    if((m_shmId = shmget((key_t)KEY_NUM, size, IPC_CREAT| IPC_EXCL | 0666)) == -1) {
        cout<<"KEY_NUM(" << (int) KEY_NUM << ") is used already." << endl;
        m_shmId = shmget((key_t)KEY_NUM, size, IPC_CREAT| 0666);
        
        if(m_shmId == -1)
        {
            perror("Shared memory create fail");
            return 1;
        }
        else
        {
            DeallocSharedMemory(false);
            m_shmId = shmget((key_t)KEY_NUM, size, IPC_CREAT| 0666);
            
            if(m_shmId == -1)
            {
                perror("Shared memory create fail");
                return 1;
            }
        }
    }
    
    if((m_shmAddr = shmat(m_shmId, (void *)0, 0)) == (void *)-1)
    {
        perror("Shmat failed");
    }

    ((ipc_data_t *)m_shmAddr)->messageStatus = STATUS_IDLE;

    return 0;
}

int SharedMem::SharedMemoryWriteResponse(response_t& response)
{
    ((ipc_data_t *)m_shmAddr)->response = response;
    return 0;
}

void SharedMem::SharedMemorySetStatus(MessageStatus_t status)
{
    ((ipc_data_t *)m_shmAddr)->messageStatus = status;
}

MessageStatus_t SharedMem::SharedMemoryReadStatus()
{
    return ((ipc_data_t *)m_shmAddr)->messageStatus;;
}

int SharedMem::SharedMemoryReadRequest(request_t& request)
{
    request = ((ipc_data_t *)m_shmAddr)->request;
    return 0;
}

int SharedMem::DeallocSharedMemory(bool bDetach)
{
    if(bDetach && shmdt(m_shmAddr) == -1)
    {
        perror("Shmdt failed");
    }

    if(shmctl(m_shmId, IPC_RMID, 0) == -1) 
    {
        perror("Shmctl failed");
        return 1;
    }
    return 0;
}

int SharedMem::LInkSharedMemory()
{
  
    if((m_shmId = shmget((key_t)KEY_NUM, 0, 0)) == -1)
    {
        perror("g_ShmId failed");
    }
    
    if((m_shmAddr = shmat(m_shmId, (void *)0, 0)) == (void *)-1) 
    {
        perror("Shmat failed");
       return 1;
    }

    //SharedMemoryReadPID(&server_pid);
    return 0;
}

int SharedMem::SharedMemoryWriteRequest(request_t& request)
{
    ((ipc_data_t *)m_shmAddr)->request = request;
     return 0;
}

int SharedMem::SharedMemoryReadResponse(response_t& response)
{
    response = ((ipc_data_t *)m_shmAddr)->response;
    return 0;
}

int SharedMem::UnlinkSharedMemory(void)
{
    if(shmdt(m_shmAddr) == -1)
    {
        perror("Shmdt failed");
    }
    
    return 0;
}

pid_t SharedMem::SharedMemoryReadRespPid()
{
    return ((ipc_data_t *)m_shmAddr)->request.clientPid;
}
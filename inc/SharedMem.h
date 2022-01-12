#ifndef __SHARED_MEM_H__
#define __SHARED_MEM_H__

#include "ipc_data.h"

using namespace std;

#define  KEY_NUM   1238
#define  MEM_SIZE  sizeof(ipc_data_t)

class SharedMem {

private:
	SharedMem(){};

	// 인스턴스가 생성되었는지 여부
	static bool instanceFlag;

	// 싱글턴 인스턴스
	static SharedMem* m_instance;

	int m_shmId;
    void *m_shmAddr;

public:

	// 싱글턴 인스턴스를 반환할 멤버함수
	static SharedMem* getInstance();

	// 소멸자, instanceFlag를 false로 변경
	virtual ~SharedMem(){
		instanceFlag = false;
	};

    int AllocSharedMemory();                            
    int SharedMemoryWriteResponse(response_t& );        
    void SharedMemorySetStatus(MessageStatus_t );
    MessageStatus_t SharedMemoryReadStatus(void);
    int SharedMemoryReadRequest(request_t& );
    int DeallocSharedMemory(bool);

    int LInkSharedMemory();
    int UnlinkSharedMemory(void);
    pid_t SharedMemoryReadRespPid();
    int SharedMemoryWriteRequest(request_t& );
    int SharedMemoryReadResponse(response_t& );
};

#endif /* __SHARED_MEM_H__ */
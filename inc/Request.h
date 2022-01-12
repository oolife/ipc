#include <string>
#include <sys/types.h>
#include <unistd.h>
#include "ipc_data.h"

using namespace std;

/* Use Builder Pattern*/
class Request
{
public:
    //operationCode and targetModule are mandatory variables
    Request(unsigned short operationCode, unsigned char targetModule)
    {
        m_operationCode = operationCode;
        m_targetModule = targetModule;
    }

    /* Set member variable subCode*/
    Request& setSubCode(unsigned char subCode) {  m_subCode = subCode;  return *this;}

    /* Set member variable state1, state2, state3*/
    Request& setState(unsigned char state1=0, unsigned char state2=0, unsigned char state3=0) 
    {  
        m_state1 = state1;  
        m_state2 = state2;  
        m_state3 = state3;  
        return *this;
    }

    /* Set member variable m_contents1, m_contents2*/
    Request& setContents(string contents1 = string(""), string contents2 = string("")) 
    {  
        m_contents1 = contents1;
        m_contents2 = contents2;
        return *this;
    }

    /* Create request_t from Request Class)*/
    int createIPCData(request_t& request)
    {
        request.clientPid = getpid();
        request.requestId = s_requestId++;

        request.payload.operationCode = m_operationCode;
        request.payload.subCode = m_subCode;
        request.payload.targetModule = m_targetModule;
        request.payload.state1 = m_state1;
        request.payload.state2 = m_state2;
        request.payload.state3 = m_state3;
        snprintf(request.payload.contents1, sizeof(request.payload.contents1), "%s", m_contents1.c_str());
        snprintf(request.payload.contents2, sizeof(request.payload.contents2), "%s", m_contents2.c_str());

        return 1;
    }
 
private:
    static int s_requestId;   
     
    unsigned short m_operationCode;
    unsigned char m_subCode;
    unsigned char m_targetModule;
    unsigned char m_state1:2;
    unsigned char m_state2:1;
    unsigned char m_state3:4;
    string m_contents1;
    string m_contents2;
};
#include "Channel.h"


CChannel::~CChannel()
{
    if(m_service != NULL)
    {
        delete m_service;
        m_service =  NULL;
    }
}

int CChannel::Read(size_t max,char* buf)
{
    #ifdef WIN32
    return  recv(m_fd,buf,max,0);
    #else
    return read(m_fd,buf,max);
    #endif // WINDOWS


}

int CChannel::Write(size_t max,char* buf)
{
    #ifdef WIN32
    return  send(m_fd,buf,max,0);
    #else
      return  write(m_fd,buf,max);
    #endif // WINDOWS


}

int CChannel::Getfd() const
{
    return m_fd;
}

void CChannel::SetService(const CServiceName& service)
{
    if (m_service == NULL)
        m_service = new CServiceName();
    memcpy(m_service,&service,sizeof(CServiceName));
}
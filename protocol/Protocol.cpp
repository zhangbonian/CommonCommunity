#include "Protocol.h"

CProtocol::CProtocol( CSelectReactor* reactor,CPackage* pUpperPackage,CPackage* pSendPackage,int ReserverLen ):CHandler(reactor)
{
	m_activeID=0;
	m_errHandler = NULL;
	m_uppderHandler = NULL;
	m_reserveLen = ReserverLen;
	m_sendPackage = pSendPackage;
	m_upperPackage =  pUpperPackage;
	m_lower = NULL;

}

CProtocol::~CProtocol()
{
	if(GetLower() != NULL)
	{
		((CProtocol*)GetLower())->RemoveUpper(GetActiveID());
	}
	if(m_upperMap.size() >0)
	{
		m_upperMap.clear();
	}
	if(m_upperPackage != NULL)
	{
		m_upperPackage->Release();
	}
	if(m_sendPackage != NULL)
		m_sendPackage->Release();

}

void CProtocol::AttachLower( CProtocol* pLower)
{
	if(pLower != NULL)
	{
		pLower->AddUpper(this);
	}
	m_lower = pLower;

}

void CProtocol::RegisterUpperHandler( CProtocolCallback* pUpperHandler )
{
	m_uppderHandler = pUpperHandler;
}

void CProtocol::RegisterErrHandler( CHandler* pHandler )
{
	m_errHandler = pHandler;
}

int CProtocol::send( CPackage* package )
{
	int ret =  package->MakePackage();
	if(ret < 0)
	{
		return ret;
	}
	if(m_lower != NULL)
	{
		return m_lower->Push(package,this);
	}
	else
	{
		exit(-1);
	}
	return 0;
}

int CProtocol::GetReserverLen() const
{
	return m_reserveLen;
}

unsigned int CProtocol::GetActiveID()
{
	return m_activeID;
}

int CProtocol::HandlePackage( CPackage* pPackage,CProtocol* protocol )
{
	return 0;
}

void CProtocol::AddUpper( CProtocol* pUpper )
{
	if(pUpper != NULL)
	{
		m_upperMap.insert(pair<unsigned int,CProtocol*>(pUpper->GetActiveID(),pUpper));
	}

}

CProtocol* CProtocol::GetUpper( unsigned int activeid )
{
	if(m_upperMap.find(activeid) != m_upperMap.end())
	{
		return m_upperMap[activeid];
	}
	return NULL;
}

CProtocol* CProtocol::RemoveUpper( unsigned int activeid )
{   map<unsigned int,CProtocol*>::iterator ret = m_upperMap.find(activeid);
	CProtocol* upper =NULL;
	if (ret != m_upperMap.end())
	{
		upper = (ret->second);
		m_upperMap.erase(ret);
	}
	return upper;
}

CProtocol* CProtocol::GetLower()
{
	return m_lower;
}

int CProtocol::Pop( CPackage* package )
{
	int activeID = package->GetActiveID();
	CProtocol* upper = GetUpper(activeID);
	//如果没有对应的上层协议处理，则利用默认的handler进行处理
	if(upper != NULL)
	{
		return upper->HandlePackage(package,this);
	}
	if(m_uppderHandler != NULL)
	{
		return m_uppderHandler->HandlePackage(package,this);
	}
	return 0;
}

int CProtocol::OnRecvErrPackage( CPackage* package )
{
	return 0;
}

void CProtocol::NotifyErr( int msg,unsigned int dwParam, void* pParam )
{
	return;
}

int CProtocol::Push( CPackage* package,CProtocol* protocol )
{
	m_sendPackage->AddBuf(package);
	int ret = m_sendPackage->MakePackage();
	if(ret == 0)
	{
		if(m_lower != NULL)
		{
			ret = m_lower->Push(m_sendPackage,this);
		}
	}
	m_sendPackage->BufRelease();
	return ret;
}

void CProtocol::SetActiveID( unsigned int activeid )
{
	m_activeID = activeid;
}


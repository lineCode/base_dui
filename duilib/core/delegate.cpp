#include "stdafx.h"

namespace dui {
	EventSource::~EventSource()
	{
		for( int i = 0; i < m_delegates.GetSize(); i++ ) {
			EventCallback* pObject = static_cast<EventCallback*>(m_delegates[i]);
			if( pObject) delete pObject;
		}
	}

	EventSource::operator bool()
	{
		return m_delegates.GetSize() > 0;
	}

	void EventSource::operator+= (const EventCallback& d)
	{ 
#if 0
		for( int i = 0; i < m_delegates.GetSize(); i++ ) {
			EventCallback* pObject = static_cast<EventCallback*>(m_delegates[i]);
			if( pObject && *pObject->Equals(d) ) return;
		}
#endif
		m_delegates.Add(new EventCallback(d));
	}

	void EventSource::operator+= (FnType pFn)
	{ 
		(*this) += std::bind(pFn, std::placeholders::_1);
	}

	void EventSource::operator-= (const EventCallback& d)
	{
#if 0
		for( int i = 0; i < m_delegates.GetSize(); i++ ) {
			EventCallback* pObject = static_cast<EventCallback*>(m_delegates[i]);
			if( pObject && pObject->Equals(d) ) {
				delete pObject;
				m_delegates.Remove(i);
				return;
			}
		}
#endif
	}
	void EventSource::operator-= (FnType pFn)
	{ 
		(*this) -= std::bind(pFn, std::placeholders::_1);
	}

	bool EventSource::operator() (Event* param)
	{
		bool bReturn = true;
		for( int i = 0; i < m_delegates.GetSize(); i++ ) {
			EventCallback* pObject = static_cast<EventCallback*>(m_delegates[i]);
			if( pObject && !(*pObject)(param) ) 
				bReturn = false;
		}
		return bReturn;
	}
} // namespace dui

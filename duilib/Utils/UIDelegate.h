#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once
#include "Core/UIDefine.h"

namespace dui {
	typedef std::function<bool(TEvent*)> EventCallback;

	class DUILIB_API CEventSource
	{
		typedef bool(*FnType)(void*);
	public:
		~CEventSource();
		operator bool();
		void operator+= (const EventCallback& d); // add const for gcc
		void operator+= (FnType pFn);
		void operator-= (const EventCallback& d);
		void operator-= (FnType pFn);
		bool operator() (TEvent* param);

	protected:
		PtrArray m_aDelegates;
	};

	typedef std::map<EVENTTYPE_UI, CEventSource> EventMap;
} // namespace dui

#endif // __UIDELEGATE_H__
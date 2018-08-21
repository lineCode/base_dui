#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once
#include "core/define.h"

namespace dui {
	typedef std::function<bool(Event*)> EventCallback;

	class DUILIB_API EventSource
	{
		typedef bool(*FnType)(void*);
	public:
		~EventSource();
		operator bool();
		void operator+= (const EventCallback& d); // add const for gcc
		void operator+= (FnType pFn);
		void operator-= (const EventCallback& d);
		void operator-= (FnType pFn);
		bool operator() (Event* param);

	protected:
		PtrArray m_aDelegates;
	};

	typedef std::map<EVENTTYPE_UI, EventSource> EventMap;
} // namespace dui

#endif // __UIDELEGATE_H__
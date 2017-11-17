#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once
#include "Core/UIDefine.h"

#define MODE_EVENTMAP	1

namespace dui {
#if 0
	typedef std::function<bool(TEvent*)> EventCallback;

	class DUILIB_API CEventSource
	{
		typedef bool(*FnType)(TEvent*);
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
#endif
#if MODE_EVENTMAP
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
#else
class DUILIB_API CDelegateBase	 
{
public:
    CDelegateBase(void* pObject, void* pFn);
    CDelegateBase(const CDelegateBase& rhs);
    virtual ~CDelegateBase();
    bool Equals(const CDelegateBase& rhs) const;
    bool operator() (void* param);
    virtual CDelegateBase* Copy() const = 0; // add const for gcc

protected:
    void* GetFn();
    void* GetObject();
    virtual bool Invoke(void* param) = 0;

private:
    void* m_pObject;
    void* m_pFn;
};

class CDelegateStatic: public CDelegateBase
{
    typedef bool (*Fn)(void*);
public:
    CDelegateStatic(Fn pFn) : CDelegateBase(NULL, pFn) { } 
    CDelegateStatic(const CDelegateStatic& rhs) : CDelegateBase(rhs) { } 
    virtual CDelegateBase* Copy() const { return new CDelegateStatic(*this); }

protected:
    virtual bool Invoke(void* param)
    {
        Fn pFn = (Fn)GetFn();
        return (*pFn)(param); 
    }
};

template <class O, class T>
class CDelegate : public CDelegateBase
{
    typedef bool (T::* Fn)(void*);
public:
    CDelegate(O* pObj, Fn pFn) : CDelegateBase(pObj, *(void**)&pFn) { }
    CDelegate(const CDelegate& rhs) : CDelegateBase(rhs) { } 
    virtual CDelegateBase* Copy() const { return new CDelegate(*this); }

protected:
    virtual bool Invoke(void* param)
    {
		O* pObject = (O*) GetObject();
		union
		{
			void* ptr;
			Fn fn;
		} func = { GetFn() };
		return (pObject->*func.fn)(param);
    }  

private:
	Fn m_pFn;
};

template <class O, class T>
CDelegate<O, T> MakeDelegate(O* pObject, bool (T::* pFn)(void*))
{
    return CDelegate<O, T>(pObject, pFn);
}

inline CDelegateStatic MakeDelegate(bool (*pFn)(void*))
{
    return CDelegateStatic(pFn); 
}

class DUILIB_API CEventSource
{
    typedef bool (*FnType)(void*);
public:
    ~CEventSource();
    operator bool();
    void operator+= (const CDelegateBase& d); // add const for gcc
    void operator+= (FnType pFn);
    void operator-= (const CDelegateBase& d);
    void operator-= (FnType pFn);
    bool operator() (void* param);

protected:
    PtrArray m_aDelegates;
};
#endif
} // namespace dui

#endif // __UIDELEGATE_H__
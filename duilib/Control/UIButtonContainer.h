#ifndef __UI_BUTTON_CONTAINER_H__
#define __UI_BUTTON_CONTAINER_H__

#pragma once

namespace dui {

	class DUILIB_API ButtonContainer : public Container
	{
	public:
		ButtonContainer();
		virtual ~ButtonContainer();

		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;

		virtual bool Activate();
		virtual void DoEvent(TEvent& event) override;
#if MODE_EVENTMAP
		void AttachClick(const EventCallback& callback)
		{
			OnEvent[UIEVENT_CLICK] += callback;
		}
#endif
	protected:
		UINT m_uButtonState;
	};

}

#endif
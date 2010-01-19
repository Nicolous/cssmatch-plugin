/* 
 * Copyright 2009, 2010 Nicolas Maingot
 * 
 * This file is part of CSSMatch.
 * 
 * CSSMatch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSSMatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#ifndef __EVENT_LISTENER_H__
#define __EVENT_LISTENER_H__

#include "../plugin/ServerPlugin.h"

#include "igameevents.h"

#include <map>
#include <list>

namespace cssmatch
{
	/** Event listener template, which can be used as a class member */
	template<class T>
	class EventListener : public IGameEventListener2
	{
	protected:
		/** Object which defines the callbacks */
		T * object;
	private:
		/** { event name => callback } <br>
		 * /!\ one event = one callback /!\ 
		 */
		std::map<std::string, void (T::*)(IGameEvent *)> callbacks;
	public:
		/** This class is used by some singletons. It could be too late to use <br>
		 * IGameEventListener2 at the end of the program. So please use <br>
		 * removeCallbacks or removeCallback to properly remove the listened events
		 */
		EventListener(T * callbackObject);
		/*~EventListener()
		{
			gameeventmanager2->RemoveListener(this);
		} 
		see above */

		// IGameEventListener2 method(s)
		void FireGameEvent(IGameEvent * event);

		/** Add a callback to an event
		 * @param eventName The event name to listen
		 * @param object The object which listens the event
		 * @param callback The object's function to call when the event occurs
		 * @param serverSide Is the event server side ?
		 */
		bool addCallback(	const std::string & eventName,
							void (T::* callback)(IGameEvent * event),
							bool serverSide = true);

		/** Remove an event callback <br>
		 * You have to invoke this method (or removeCallbacks) to unsubscribe to all the events listened
		 * @param eventName The event name
		 */
		bool removeCallback(const std::string & eventName);

		/** Remove all callbacks listened <br>
		 * You have to invoke this method (or removeCallback) to unsubscribe to all the events listened
		 */
		void removeCallbacks();
	};

	template<class T>
	EventListener<T>::EventListener(T * callbackObject) : object(callbackObject)
	{
	}

	template<class T>
	void EventListener<T>::FireGameEvent(IGameEvent * event)
	{
		std::string eventName = event->GetName();

		if (callbacks.find(eventName) != callbacks.end())
		{
			// Call all the callback corresponding to this event
			((object)->*(callbacks[eventName]))(event);
		}
	}

	template<class T>
	bool EventListener<T>::addCallback(	const std::string & eventName,
						void (T::* callback)(IGameEvent * event),
						bool serverSide = true)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		ValveInterfaces * interfaces = plugin->getInterfaces();

		bool success = false;
		const char * c_eventName = eventName.c_str();

		if (callbacks.find(eventName) != callbacks.end())
		{
			callbacks[eventName] = callback;
			success = true;
		}
		else
		{
			if (interfaces->gameeventmanager2->AddListener(this,c_eventName,serverSide))
			{
				callbacks[eventName] = callback;
				success = true;				
			}
		}

		return success;
	}

	template<class T>
	bool EventListener<T>::removeCallback(const std::string & eventName)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		ValveInterfaces * interfaces = plugin->getInterfaces();

		bool success = false;

		if (callbacks.find(eventName) != callbacks.end())
		{
			success = true;
			callbacks.erase(eventName);
		}

		if (callbacks.size() == 0)
			interfaces->gameeventmanager2->RemoveListener(this);

		return success;
	}

	template<class T>
	void EventListener<T>::removeCallbacks()
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		ValveInterfaces * interfaces = plugin->getInterfaces();

		interfaces->gameeventmanager2->RemoveListener(this);
		callbacks.clear();
	}
}

#endif // __EVENT_LISTENER_H__

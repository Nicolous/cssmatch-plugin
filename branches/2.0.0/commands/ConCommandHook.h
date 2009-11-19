/* 
 * Copyright 2008, 2009 Nicolas Maingot
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

#include "convar.h"

#include <string>

namespace cssmatch
{
	/** Hook a command at runtime 
	 * Note that this object calls ConCommand::Init(), <br>
	 * don't construct it before invoking ConCommandBaseMgr::OneTimeInit
	 */
	class ConCommandHook : public ConCommand
	{
	protected:
		/** Pointer to the hooked command */
		ConCommand * hooked;
	public:
		/** Similar to the ConCommand's constructor
		 * @param name The name must be dynamically allocated with new !
		 */
		ConCommandHook(const char * name);

		/**
		 * @see ConCommand
		 */
		void Init();

		/** 
		 * @see ConCommand
		 */
		void Dispatch();
	};
}

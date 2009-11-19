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
 * Portions of this code are also Copyright � 1996-2005 Valve Corporation, All rights reserved
 */

#ifndef __I18N_CON_COMMAND_H__
#define __I18N_CON_COMMAND_H__

#include "convar.h" // ConCommand, FnCommandCallback, FnCommandCompletionCallback

namespace cssmatch
{
	class I18nManager;

	/** As I18nConVar, this class allows the internationalization of the ConCommand's description */
	class I18nConCommand : public ConCommand
	{
	protected:
		/** Internationalization tool */
		I18nManager * i18n;
	public:
		// Classic ConCommand constructors plus the internalization tool
		I18nConCommand(	I18nManager * i18nManager,
						char const * pName,
						FnCommandCallback callback,
						char const * pHelpString = 0,
						int flags = 0,
						FnCommandCompletionCallback completionFunc = 0);

		// ConCommand methods override : dynamically allocates the memory ! 
		virtual char const * GetHelpText() const;
	};
}

#endif // __I18N_CON_COMMAND_H__

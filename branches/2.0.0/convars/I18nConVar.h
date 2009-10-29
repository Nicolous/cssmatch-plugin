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

#ifndef __I18N_CONVAR_H__
#define __I18N_CONVAR_H__

#include "convar.h"

#include <string>

namespace cssmatch
{
	class I18nManager;

	/** Abstraction of the Valve's ConVar to allow dynamic translations for ConVar's description
	 * (Glad that GetHelpText is virtual) <br>
	 * FCVAR_PLUGIN flag is automatically added to the ConVar's flags
	 */
	class I18nConVar : public ConVar
	{
	protected:
		/** Internationalization tool */
		I18nManager * i18n;
	public:
		// Classic ConVar constructors plus the internalization tool
		I18nConVar(I18nManager * i18nManager, char const * pName, char const * pDefaultValue, int flags = 0);
		I18nConVar(I18nManager * i18nManager, char const * pName, char const * pDefaultValue, int flags, char const * pHelpString);

		I18nConVar(	I18nManager * i18nManager, 
					char const * pName,
					char const * pDefaultValue,
					int flags,
					char const * pHelpString,
					bool bMin,
					float fMin,
					bool bMax,
					float fMax);

		I18nConVar(	I18nManager * i18nManager, 
					char const * pName,
					char const * pDefaultValue,
					int flags,
					char const * pHelpString,
					FnChangeCallback callback);

		I18nConVar(	I18nManager * i18nManager, 
					char const * pName,
					char const *pDefaultValue,
					int flags,
					char const * pHelpString,
					bool bMin,
					float fMin,
					bool bMax,
					float fMax,
					FnChangeCallback callback);

		// ConVar methods override
		virtual char const * GetHelpText() const;

		/** Functor to quickly find a ConVar instance by his name */
		friend class ConvarHavingName;
	};

	class ConvarHavingName
	{
	private:
		std::string name;
	public:
		ConvarHavingName(const std::string & conVarName) : name(conVarName) {}
		bool operator()(const ConVar * var)
		{
			return name == var->GetName();
		}
	};
}

#endif  // __I18N_CONVAR_H__

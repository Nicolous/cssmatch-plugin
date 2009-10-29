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

#ifndef __ENTITY_PROP_H__
#define __ENTITY_PROP_H__

#include "../common/common.h"

#include <string>
#include <fstream>

class SendTable;
struct edict_t;
class CBaseEntity;

#include "../exceptions/BaseException.h"

namespace cssmatch
{
	class EntityPropException : public BaseException
	{
	public:
		EntityPropException(const std::string & msg) : BaseException(msg)
		{
		}
	};

	/** Handle the (networked) entities game properties*/
	class EntityProp
	{
	private:
		/** The prop's class */
		std::string theClass;

		/** The prop's path */
		std::string path;

		/** The prop's offset */
		int offset;

		/** Compute the prop's offset
		 * @param table The table which contains the prop
		 * @param propPathLeft Path left until the prop
		 */
		void getOffset(SendTable * table,  std::istringstream & propPathLeft);

		/** Search the prop's offset through the server props */
		void initialize();
	public:
		/** Initialize the handler
		 * @param propClass The prop's classe
		 * @propPath The prop's path - Step separator is "." (e.g. step1.step2.prop)
		 */
		EntityProp(const std::string & propClass, const std::string & propPath);

		/** Retrieve a prop reference about a given entity
		 * @param entity The entity
		 * @throws EntityPropException If the prop was not found
		 */
		template<typename T>
		T & getProp(edict_t * entity) throw(EntityPropException)
		{
			T * prop = NULL;

			if (offset > 0)
			{
				CBaseEntity * baseEntity = getBaseEntity(entity);
				prop = reinterpret_cast<T *>(reinterpret_cast<char *>(baseEntity) + offset);
				if (prop == NULL)
					throw EntityPropException(std::string("CSSMatch was unable to retrieve prop ") + theClass + "." + path + " for an entity");
			}
			else
				throw EntityPropException(std::string("CSSMatch was unable to find the offset of the prop ") + theClass + "." + path);

			return *prop;
		}
	};
}

#endif // __ENTITY_PROP_H__

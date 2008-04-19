/* 
 * Copyright 2007, 2008 Nicolas Maingot
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

#include "../API/API.h"

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

/** Représente une configuration du match */
class Configuration
{
private:
	/** Nom du fichier contenant la configuration */
	std::string nomFichier;
public:
	/** Construit une configuration portant sur un fichier */
	Configuration(const std::string & nomFichier = "default.cfg");

	/** Retourne une string contenant le nom du fichier de configuration */
	const std::string & getNomFichier() const;

	/** Exécute le fichier de configuration <br>
	 * Ne donner que le nom de fichier + son extension ! <br>
	 * Jette une exeption de type std::out_of_range si le fichier n'a pas été trouvé
	 */
	void execute() const throw (std::out_of_range);
};

#endif // __CONFIGURATION_H__

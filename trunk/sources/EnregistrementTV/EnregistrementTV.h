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
#include "../ConVars/ConVars.h"

#ifndef __ENREGISTREMENTTV_H__
#define __ENREGISTREMENTTV_H__

/** Représente un enregistrement sourceTV */
class EnregistrementTV
{
private:
	/** Nom de l'enregistrement (arborescence comprise) */
	std::string nom;

public:
	/** Prépare un enregistrement avec un nom donné */
	EnregistrementTV(const std::string & fichierSansPathSansExtension);

	/** Lance un enregistrement sourceTV <br>
	 * Jette une exception de type "out_of_range" si il est impossible de lancer l'enregistrement
	 */
	void enregistre() const;

	/** Stop l'enregistrement en quelques secondes */
	void coupe() const;

	/** Retourne le nom de l'enregistrement */
	const std::string & getNom() const;
};

#endif // __ENREGISTREMENTTV_H__

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
 * Portions of this code are also Copyright � 1996-2005 Valve Corporation, All rights reserved
 */

#include <ctime>
#include <time.h>
#include <string>
#include <sstream>
//#include <algorithm>

#include "Incopiable/Incopiable.h"

#ifndef __OUTILS_H__
#define __OUTILS_H__

/** Outils sans rapport direct avec l'API */
namespace Outils
{
	/** Convertir en std::string tous types supportant l'op�rateur de flux <<
	 *
	 * @param aConvertir La donn�es � convertir
	 * @return La cha�ne de caract�res qui correspond
	 */
	template<typename T>
	std::string toString(const T & aConvertir)
	{
		std::ostringstream convertion;
		convertion << aConvertir;

		return convertion.str();
	}

	/** Recherche la date locale */
	tm * getDateLocale();

	/** Remplace les caract�res interdits dans les noms de fichiers ou dans la console par des tirets "-" <br>
	 * - remplace les caract�res interdits pour Windows <br>
	 * - remplace les caract�res d'�chappement de la console
	 *
	 * @param nom Le nom du fichier, sans son chemin complet (path)
	 * 
	 */
	void valideNomDeFichier(std::string & nom);
};

#endif // __OUTILS_H__

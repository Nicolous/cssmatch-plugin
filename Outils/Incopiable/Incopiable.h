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

#ifndef __INCOPIABLE_H__
#define __INCOPIABLE_H__

class BaseVide{};	// http://h-deb.clg.qc.ca/Sujets/Divers--cplusplus/Enchainement-parents.html
					// http://www.cantrip.org/emptyopt.html

/** Classe d�finissant un objet incopiable par d�faut */
template <class B = BaseVide>
class Incopiable : B
{
private:
	// Emp�che la copie, aucun corps ne sera donn� � ces op�rateurs
	Incopiable(const Incopiable &); // Un constructeur par d�faut devra �tre impl�ment� chez les enfants
	Incopiable & operator = (const Incopiable &);
protected:
	// Autorise l'instanciation des enfants
	Incopiable(){}
	~Incopiable(){}
};

#endif // __INCOPIABLE_H__

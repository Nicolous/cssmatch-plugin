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

#ifndef __LIGNE_H__
#define __LIGNE_H__

/** Représente une ligne dans un Menu 
 * La représentation fait penser à un std::map, mais c'est rendre le code moins lisible pour 1 clé par std::map...
 */
class Ligne
{
private:
	/** Texte associé à la ligne */
	std::string cle;
	/** Valeur associée à la ligne (ID ou code retour/suivant) <br>
	 * Le but de cette valeur est, par exemple, d'éviter qu'un joueur se nommant "Retour" ne perturbe le menu...
	 */
	int valeur;	

public:
	/** Constructeur implémenté en cas de stockage dans un vector susceptible de subir un "rezise" */
	Ligne();

	/** Construit une page éventuellement associée à une valeur */
	Ligne(const std::string & texte, int valeur = 0);

	/** Retourne le texte associé à la ligne */
	const std::string & getTexte() const;

	/** Retourne la valeur associée au texte de la ligne */
	int getValeur() const;
};

#endif // __LIGNE_H__

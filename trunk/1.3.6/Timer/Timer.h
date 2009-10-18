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

#include "../Api/Api.h"
#include "../I18n/I18n.h"
#include "../Messages/Messages.h"

#ifndef __TIMER_H__
#define __TIMER_H__

/** Objet permettant de retarder l'ex�cution d'une fonction, qui prend en param�tre une std::string */
class Timer
{
protected:
	/** Date d'ex�cution de la fonction (en secondes) */
	float dateExecution;

	/** Pointeur sur la fonction retard�e */
	void (*fonction)(const std::string & parametre, const std::map<std::string,std::string> & informations);

	/** Param�tre de la fonction � appeler */
	std::string parametre;

	/* Informations � passer � la fonction */
	std::map<std::string,std::string> informations;

public:
	/** Ce constructeur n'est l� que pour facilit� l'utilisation du timer dans d'autres [patron de ]classes */
	Timer();

	/** Retarde l'ex�cution d'une fonction par rapport � la date courante
	 *
	 * @param retard Retard (en secondes) de l'ex�cution de la fonction
	 * @param fonction Un pointeur sur la fonction � ex�cuter
	 * @param parametre Le param�tre de la fonction � ex�cuter
	 * @param informations (Optionnel) Informations � communiquer � la fonction
	 *
	 */
	Timer(float retard, void (*fonction)(const std::string & parametre, const std::map<std::string,std::string> & informations),
		const std::string & parametre, const std::map<std::string,std::string> & informations = I18n::SANS_PARAMETRE);

	/** Accesseur sur la date d'ex�cution de la fonction retard�e
	 *
	 * @return La date d'ex�cution
	 *
	 */
	float getDateExecution() const;

	/** Accesseur sur le param�tre de la fonction retard�e
	 *
	 * @return Le param�tre
	 *
	 */
	const std::string & getParametre() const;

	/** Accesseur sur les informations pass�es � la fonction 
	 *
	 * @return Les informations
	 */
	const std::map<std::string,std::string> & getInformations() const;

	/** Ex�cute la fonction retard�e <br>
	 * Note : l'�ventuelle destruction du timer est � la charge du code appelant
	 */
	void execute();

	/** Fonction contr�lant la validit� du timer
	 *
	 * @return true si le timer est valide
	 *
	 */
	bool estValide() const;

	/** Construit une repr�sentation du timer sous forme de cha�ne de caract�res
	 *
	 * @return La repr�sentation du timer
	 *
	 */
	const std::string toString() const;
};

#endif // __TIMER_H__

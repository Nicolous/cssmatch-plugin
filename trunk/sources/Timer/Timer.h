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

#include "../API/API.h"
#include "../Messages/Messages.h"

#ifndef __TIMER_H__
#define __TIMER_H__

/** Repr�sente l'ex�cution retard�e d'une fonction prenant en param�tre une std::string */
class Timer
{
protected:
	/** Date d'ex�cution de la fonction (en secondes) */
	float dateExecution;
	/** Pointeur sur la fonction retard�e */
	void (*fonction)(const std::string & parametre);
	/** Param�tre de la fonction � appeler (cha�ne vide si la fonction � appeler n'a pas de param�tre) */
	std::string parametre;

public:
	/** Ce constructeur n'est l� que pour facilit� l'utilisation du timer dans d'autres [patron de ]classes */
	Timer();

	/** Retarde l'ex�cution d'une fonction par rapport � la date courante
	 *
	 * @param retard Retard (en secondes) de l'ex�cution de la fonction
	 *
	 */
	Timer(float retard, void (*fonction)(const std::string & parametre), const std::string & parametre = "");
	
	Timer(const Timer & timer);

	Timer & operator = (const Timer & timer);

	/** Retourne la date d'ex�cution de la fonction retard�e */
	float getDateExecution() const;

	/** Retourne la date d'ex�cution de la fonction retard�e */
	const std::string & getParametre() const;

	/** Ex�cute la fonction retard�e <br>
	 * L'�ventuelle destruction du timer est � la charge du code appelant
	 */
	void execute();

	/** Fonction contr�lant la validit� du timer */
	bool estValide() const;
};

#endif // __TIMER_H__

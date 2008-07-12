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

#include "Timer.h"

using std::string;
using std::ostringstream;

Timer::Timer()
{
	dateExecution = -1.0f;
	fonction = NULL;
	parametre = "";
}

Timer::Timer(float retard, void (*fonction)(const std::string & parametre), const std::string & parametre)
{
	// On calcul la date d'ex�cution de la fonction
	dateExecution = Api::gpGlobals->curtime + retard;
	// On r�cup�re les donn�es relatives � la fonction � ex�cuter (adresse + param�tre)
	this->fonction = fonction;
	this->parametre = parametre;
}

Timer::Timer(const Timer & timer)
{
	dateExecution = timer.dateExecution;
	fonction = timer.fonction;
	parametre = timer.parametre;
}

Timer & Timer::operator = (const Timer & timer)
{
	dateExecution = timer.dateExecution;
	fonction = timer.fonction;
	parametre = timer.parametre;

	return *this;
}

float Timer::getDateExecution() const
{
	return dateExecution;
}

const std::string & Timer::getParametre() const
{
	return parametre;
}

void Timer::execute()
{
	// R�-initialisation (� faire avant l'ex�cution de la fonction)
	// dateExecution = -1.0f;

	// Si l'adresse de la fonction n'est pas initialis�e on sort
	if (fonction != NULL)
	{
		// Appel de la fonction
		fonction(parametre);
	}
	dateExecution = -1.0f;
	fonction = NULL;
	parametre = "";
}

bool Timer::estValide() const
{
	// Un timer valide est un timer qui n'a pas encore �t� ex�cut�
	return dateExecution>-1.0f;
}

const string Timer::toString() const
{
	ostringstream repr;

	repr << "Execution : " << dateExecution << "/" << Api::gpGlobals->curtime
		 << " (parametre=\"" << parametre <<"\")";

	return repr.str();
}

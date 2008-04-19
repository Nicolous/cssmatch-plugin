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

#include "Decompte.h"

using std::string;
using std::ostringstream;

void termineDecompte(const string & parametre)
{
	Decompte::getDecompte()->finDecompte();
}

void afficheDecompte(const string & parametre)
{
	// Calcul du temps en minutes et secondes
	Decompte * decompte = Decompte::getDecompte();
	int tempsRestant = decompte->getTempsRestant();
	int secondes = tempsRestant;
	int minutes = (int)(secondes/60);
	secondes -= minutes*60;

	// On affiche le nombre de minutes et de secondes restantes
	ostringstream msg;
	if (secondes >= 10)
	{
		if (minutes >= 10)
			msg << minutes << " : " << secondes;
		else
			msg << "0" <<minutes << " : " << secondes;
	}
	else
	{
		if (minutes >= 10)
			msg << minutes << " : " << "0" << secondes;
		else
			msg << "0" << minutes << " : " << "0" << secondes;
	}
	Messages::hintMsg(msg.str());

	// S'il reste du temps à décompter, on planifie le prochain timer
	if (decompte->decTempsRestant()>=0)
	{
		decompte->getMoteur()->addTimer(Timer(1.0f,afficheDecompte));
		return;
	}

	// Sinon on stoppe le décompte
	decompte->finDecompte();
}

Decompte * Decompte::decompte = NULL;

Decompte::Decompte()
{
	tempsRestant = 0;

	moteur = GestionTimers(2);

	fonction = NULL;
	parametre = "";
}

Decompte::~Decompte()
{
}

int Decompte::decTempsRestant()
{
	return --tempsRestant;
}

Decompte * Decompte::getDecompte()
{
	// Si aucun instance n'a encore été créée, on la créer
	if (decompte == NULL)
		decompte = new Decompte();

	// Retour de l'instance
	return decompte;
}

void Decompte::killDecompte()
{
	// Si une instance existe, on la supprime
	if (decompte != NULL)
		delete decompte;
}

void Decompte::lanceDecompte(int duree, void (*fonction)(const std::string &), const string & parametre)
{
	// On stoppe l'éventuel décompte déjà effectif
	finDecompte();

	// On initialise les données du décompte
	tempsRestant = duree;
	this->fonction = fonction;
	this->parametre = parametre;

	// Lancement du décompte
	afficheDecompte("");
}

void Decompte::finDecompte()
{
	// On vide les timers encore en attente (en cas de relance d'un nouveau décompte avant la fin du précédent)
	moteur.purge();

	// On s'assure que le temps restant passe bien à 0
	tempsRestant = 0;

	// On exécute la fonction s'il y en a une	
	if (fonction)
	{
		//moteur.addTimer(Timer(0.5f,fonction,parametre));
		fonction(parametre);
		// Au cas où finDecompte serait rappelée, on réinitialise l'adresse de la fonction
		fonction = NULL;
	}
}

int Decompte::getTempsRestant()
{
	return tempsRestant;
}

GestionTimers * Decompte::getMoteur()
{
	return &moteur;
}

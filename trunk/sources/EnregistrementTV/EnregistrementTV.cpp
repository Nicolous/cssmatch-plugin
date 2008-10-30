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

#include "EnregistrementTV.h"

using std::string;
using std::ostringstream;

#include "../CSSMatch/CSSMatch.h"

void timerStoppeRecords(const string & parametre)
{
	Api::serveurExecute("tv_stoprecord\n");
}

EnregistrementTV::EnregistrementTV()
{
	nom = "";
	enregistrant = false;
}

EnregistrementTV::EnregistrementTV(const string & fichierSansPathSansExtension) throw(CSSMatchTVException)
{
	// Si sourceTV n'est pas en mesure d'enregistrer (absente ou tv_enable 0)
	//	on jette une exception de type CSSMatchTVException
	if (! ConVars::tv_enable)
		throw CSSMatchTVException("Impossible de lancer l'enregistrement sourceTV car tv_enable est introuvable");
	if (! ConVars::tv_enable->GetBool())
		throw CSSMatchTVException("Impossible de lancer l'enregistrement sourceTV car tv_enable est sur 0");
	if (! Api::hltvPresente())
		throw CSSMatchTVException("Impossible de lancer l'enregistrement sourceTV car celle-ci n'est pas sur le serveur");

	// On s'alloue une copie modifiable de la variable
	string fichier(fichierSansPathSansExtension);

	// L'enregistrement n'est pas immédiatement lancé
	enregistrant = false;

	// On supprime les caractères génant pour Windows ou pour la console
	Api::valideNomDeFichier(fichier);

	// On construit le path relatif du fichier
	string path(ConVars::cssmatch_sourcetv_path.GetString());
	if (path != "")
		fichier = path + "/" + fichier + ".dem";

	const char * fichierC = fichier.c_str();

	nom = fichierC;

	// Tant que le nom du fichier formé existe déjà, nous lui ajoutons un indice
	int nbrFichier = 1;
	while (Api::filesystem->FileExists(nom.c_str(), "MOD"))
	{
		ostringstream renomme;
		renomme << "[" << nbrFichier << "]" << fichierC;
		nom = renomme.str();
		nbrFichier++;
	}
}

void EnregistrementTV::enregistre() throw (CSSMatchTVException)
{
	if (nom.empty())
		throw CSSMatchTVException("Impossible de lancer un engistrement SourceTV sans nom");

	string cmd("tv_record " + nom + "\n");

	// Nous lançons l'enregistrement en stoppant tout autre enregistrement en cours
	Api::serveurExecute("tv_stoprecord\n");
	Api::serveurExecute("tv_autorecord 0\n");
	Api::serveurExecute(cmd);

	enregistrant = true;
}

void EnregistrementTV::coupe()
{
	CSSMatch::getInstance()->getTimers()->addTimer(Timer(2.0f,timerStoppeRecords));
	enregistrant = false;
}

const string & EnregistrementTV::getNom() const
{
	return nom;
}

bool EnregistrementTV::getEnregistrant() const
{
	return enregistrant;
}

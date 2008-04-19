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
using std::out_of_range;
using std::ostringstream;

#ifndef __CSSMATCH_H__
#include "../CSSMatch/CSSMatch.h"
#endif

void timerStoppeRecords(const string & parametre)
{
	API::serveurExecute("tv_stoprecord\n");
}

EnregistrementTV::EnregistrementTV(const string & fichierSansPathSansExtension)
{
	// Si sourceTV n'est pas en mesure d'enregistrer (absente ou tv_enable 0) on jette une exception de type std::out_of_range
	if (!ConVars::tv_enable)
		throw out_of_range("Impossible de lancer l'enregistrement sourceTV car tv_enable est introuvable");
	if (!ConVars::tv_enable->GetBool())
		throw out_of_range("Impossible de lancer l'enregistrement sourceTV car tv_enable est sur 0");
	if (!API::hltvPresente())
		throw out_of_range("Impossible de lancer l'enregistrement sourceTV car celle-ci n'est pas sur le serveur");

	// On s'alloue une copie modifiable de la variable
	string fichier = fichierSansPathSansExtension;

	// On supprime les caractères génant pour Windows ou pour la console
	API::valideNomDeFichier(fichier);

	// On construit le path relatif du fichier
	string path = ConVars::cssmatch_sourcetv_path.GetString();
	if (path != "")
		fichier = path + "/" + fichier + ".dem";

	const char * fichierC = fichier.c_str();

	// Pour faciliter le passage par les méthodes de VALVE, nous repassons temporairement en char*...
	char buffer[512];
	V_snprintf(buffer,sizeof(buffer),"%s",fichierC);

	// Tant que le nom du fichier formé existe déjà, nous lui ajoutons un indice
	int nbrFichier = 1;
	while (API::filesystem->FileExists(buffer, "MOD"))
	{
		V_snprintf(buffer,sizeof(buffer),"[%i]%s",nbrFichier,fichierC);
		nbrFichier++;
	}

	// On récupère le nom final du fichier dans le champ nom
	nom = buffer;
}

void EnregistrementTV::enregistre() const
{
	// Nous préparons la commande
	string cmd = "tv_record " + nom + "\n";

	// Nous lançons l'enregistrement en stoppant tout autre enregistrement en cours
	API::serveurExecute("tv_stoprecord\n");
	API::serveurExecute("tv_autorecord 0\n");
	API::serveurExecute(cmd);
}

void EnregistrementTV::coupe() const
{
	// On stoppe l'enregistrement en cours, au bout de quelques secondes
	g_CSSMatchPlugin.getTimers()->addTimer(Timer(2.0f,timerStoppeRecords));
}

const string & EnregistrementTV::getNom() const
{
	return nom;
}

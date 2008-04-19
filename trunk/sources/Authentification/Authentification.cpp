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

#include "Authentification.h"

using std::string;
using std::vector;

bool Authentification::estAdmin(const string & steamID) const
{
	// On boucle sur chaque élément jusqu'à trouver le steamID
	vector<string>::const_iterator id = adminlist.begin();
	while(id != adminlist.end())
	{
		if (*id == steamID)
			return true;
		id++;
	}
	// Le steamID n'a pas été trouvé
	return false;
}

void Authentification::logAdminlist() const
{
	API::debug("Liste des steamID arbitre :");
	vector<string>::const_iterator id = adminlist.begin();
	while(id != adminlist.end())
	{
		API::debug("\"" + *id + "\"");
		id++;
	}
}

bool Authentification::addAdmin(const string & steamID)
{
	// Si le steamID est déjà authentifié on retourne faux
	if (estAdmin(steamID))
		return false;

	// Sinon on l'ajoute et on retourne vrai
	adminlist.push_back(steamID);
	return true;
}

bool Authentification::removeAdmin(const string & steamID)
{
	// Pour éviter une boucle nous n'appellerons pas estAdmin ici

	// On boucle sur chaque élément jusqu'à trouver le steamID
	vector<string>::iterator id = adminlist.begin();
	while(id != adminlist.end())
	{
		if (*id == steamID)
		{
			adminlist.erase(id);
			return true;
		}
		id++;
	}
	// Le steamID n'a pas été trouvé
	return false;
}

bool Authentification::parseAdminlist(const string & fichier)
{
	// On passe en char* pour travailler avec les méthodes de VALVE
	const char * nomFichier = fichier.c_str();

	// Si on ne trouve pas le fichier on sort
	if (!API::filesystem->FileExists(nomFichier,"MOD"))
		return false;

	// On ouvre le fichier
	FileHandle_t fh = API::filesystem->Open(nomFichier,"r","MOD");
	// Tant qu'on a pas atteint la fin du fichier
	while(!API::filesystem->EndOfFile(fh))
	{
		// Vous avez dit "écrit en C++" ?
		char texte[1028];
		string ligne = API::filesystem->ReadLine(texte,sizeof(texte),fh);
		// Si la ligne n'est pas un commentaire ou une ligne vide
		if (!API::estNulleOuCommentee(ligne))
			// On extrait correctement le steamID de la ligne et on l'ajoute à la liste
			//adminlist.push_back(API::extraitSteamID(ligne));
			adminlist.push_back(API::extraitChaineFormelle(ligne));
	}
	// Fermeture du fichier
	API::filesystem->Close(fh);

	// Le fichier a été trouvé
	return true;
}

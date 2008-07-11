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

#include "Team.h"

using std::string;
using std::ostringstream;

Team::Team()
{
	// Valeur bidon, toujours mieux qu'une cha�ne vide � l'�cran au moment de d�boguer ;)
	tag = "[TAG]";

	// Initialisation des scores � 0
	score = 0;
	scoreT = 0;
	scoreCT = 0;

	goStrats = false;

	nbrMembres = 0;
}

/*std::vector<Joueur> * Team::getLignup()
{
	return &lignup;
}*/

int Team::getScore() const
{
	return score;
}

int Team::incScore()
{
	return ++score;
}

void Team::setScore(int score)
{
	this->score = score;
}

int Team::getScoreT() const
{
	return scoreT;
}

int Team::incScoreT()
{
	return ++scoreT;
}

void Team::setScoreT(int scoreT)
{
	this->scoreT = scoreT;
}

int Team::getScoreCT() const
{
	return scoreCT;
}

int Team::incScoreCT()
{
	return ++scoreCT;
}

void Team::setScoreCT(int scoreCT)
{
	this->scoreCT = scoreCT;
}

const string & Team::getTag() const
{
	return tag;
}

void Team::setTag(const string & tag)
{
	this->tag = tag;
}

void Team::setGoStrats(bool go)
{
	goStrats = go;
}

bool Team::getGoStrats() const
{
	return goStrats;
}

int Team::getNbrMembres() const
{
	return nbrMembres;
}

int Team::incNbrMembres()
{
	return ++nbrMembres;
}

int Team::decNbrMembres()
{
	return --nbrMembres;
}

const string & Team::chercheTag(CodeTeam codeTeam)
{
	// Variable contenant le nombre de joueurs correspondant au codeTeam
	nbrMembres = Api::getPlayerCount(codeTeam);
	
	// Il n'y a personne dans le camp d�sign� par le codeTeam
	if (nbrMembres == 0)
		aucunTag();

	// Il y a 1 joueur dans le camp
	else if (nbrMembres == 1)
	{
		// On le recherche et son pseudo fera office de tag
		// (car aucun autre pseudo ne permettera de cerner quelle partie du pseudo constitue le tag)
		aucuneTeam(codeTeam);
	}
	else
	{
		// Nous sommes maintenant dans le cas o� il y a au moins 1 joueur dans le camp
		determineTag(codeTeam);

		// Si le tag trouv� fait moins de 2 caract�res, il s'agit probablement d'une erreur
		if (tag.size() < 3)
		{
			// Un d�termine donc un tag "neutre" contenant un num�ro �gal � codeTeam-1
			ostringstream oss;
			oss << "[team" << codeTeam-1 << "]";
			tag = oss.str();
		}
	}
	return tag;
}

void Team::aucunTag()
{
	// Aucun joueur n'est pr�sent dans le camp
	tag = "[Personne]";
}

void Team::aucuneTeam(CodeTeam codeTeam)
{
	// Le pseudo du seul joueur de le camp fera office de tag
	for (int i=1;i<=Api::maxplayers;i++)
	{
		edict_t * pEntity = Api::engine->PEntityOfEntIndex(i);
		try
		{
			IPlayerInfo * playerinfo = Api::getIPlayerInfoFromEntity(pEntity);
			if (playerinfo->GetTeamIndex() == codeTeam)
			{
				tag = playerinfo->GetName();
				i = Api::maxplayers + 1;
			}
		}
		catch(const CSSMatchApiException & e){}
	}
}

void Team::determineTag(CodeTeam codeTeam)
{
	// Variable buffer contenant l'entit� actuellement point�e
	edict_t * player = NULL;

	// Variables contenant les 2 joueurs trouv�s
	// La recherche d'un bout de pseudo commun d�bouchera sur le tag suppos� de la team
	edict_t * j1 = NULL;
	edict_t * j2 = NULL;

	// Variable pointant sur l'interface fournissant diverses infos sur les joueurs j1 et j2 (ex : leur pseudo 0_0)
	IPlayerInfo * infoJ1 = NULL;
	IPlayerInfo * infoJ2 = NULL;

	// On cherche � r�cup�rer 2 des joueurs de le camp, ainsi que le pointeur sur IPlayerInfo qui leur est associ�
	for (int i=1;i<=Api::maxplayers;i++)
	{
		// On r�cup�re l'entit� associ� � cet index i
		player = Api::engine->PEntityOfEntIndex(i);
		// Si l'entit� est invalide, on repasse player � NULL et on ne fait rien d'autre
		IPlayerInfo * playerinfo = NULL;
		try
		{
			playerinfo = Api::getIPlayerInfoFromEntity(player);
			if (playerinfo->GetTeamIndex() == codeTeam)
			{
				// On a trouv� notre j1 si ce n'�tait pas encore le cas
				if (j1 == NULL)
				{
					j1 = player;
					infoJ1 = playerinfo;
				}
				// Sinon on a trouv� notre j2
				else
				{
					j2 = player;
					infoJ2 = playerinfo;
					// Dans ce cas on a nos 2 joueurs, on peut stopper la boucle proprement
					i = Api::maxplayers + 1;
				}
			}
		}
		catch(const CSSMatchApiException & e)
		{
			player = NULL;
			playerinfo = NULL;
		}
	}
	
	// On r�cup�re le pseudo des 2 joueurs trouv�s
	const string pseudoJ1(infoJ1->GetName());
	const string pseudoJ2(infoJ2->GetName());

	// On r�cup�re la taille des 2 pseudos
	int tailleJ1 = pseudoJ1.size();
	int tailleJ2 = pseudoJ2.size();

	// D�s que l'on trouve un caract�re commun dans les pseudos
	for (int i=0;i<tailleJ1;i++)
	{
		// Pour cel� on compare chaque lettre du pseudo du joueur avec chaque lettre du pseudo du joueur 2
        int j = 0; // On souhaite que j est une dur�e de vie sup�rieure � celle de la boucle, donc on fera un while
		// Tant que les caract�res compar�s ne sont pas �gaux on incr�mente j

		while(j<tailleJ2 && pseudoJ1[i]!=pseudoJ2[j])
			j++;
		// Si on a pas atteint la fin du pseudo (pas d'exception jett�e par .at()), on est arriv� sur une �galit�
        if (j<tailleJ2 && confirmeTag(pseudoJ1,i,pseudoJ2,j))
            i = tailleJ1;
    }
}

bool Team::confirmeTag(const string & pseudoJ1, int i, const string & pseudoJ2,  int j)
{
	// On va concat�ner les caract�res communs
	tag = "";

	int tailleJ1 = pseudoJ1.size();
	int tailleJ2 = pseudoJ2.size();

	// Tant que l'on trouve un caract�re commun on concat�ne
	while(i<tailleJ1 && j<tailleJ2 && pseudoJ1[i]==pseudoJ2[j])
	{
		tag += pseudoJ1[i];
		i++;
		j++;
	}

	// Si le tag trouv� fait moins de 3 caract�res, il s'agit probablement d'une erreur
	return tag.size() >= 3;
}


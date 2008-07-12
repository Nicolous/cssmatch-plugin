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
#include "../CSSMatch/CSSMatch.h"
#include "../Messages/Messages.h"
#include "../Match/Match.h"

using std::ostringstream;
using std::string;

/*int testCompletion(char const *partial, char commands[ COMMAND_COMPLETION_MAXITEMS ][ COMMAND_COMPLETION_ITEM_LENGTH ] )
{
	string sug1(partial);
	sug1 += " ok";

	string sug2(partial);
	sug2 += " ok2";

	strcpy(commands[0], sug1.c_str());
	strcpy(commands[1], sug2.c_str());

	return 2;

}

ConCommand test = ConCommand("cssm_test",NULL,"une aide",NULL,testCompletion);*/

CON_COMMAND(cssm_help, "CSSMatch : Liste et décrit les commandes RCON du plugin dans la console")
{
	Msg("cssm_start [fichier de config optionnel] : lance un match\n");
	Msg("cssm_stop : stoppe un match\n");
	Msg("cssm_retag : lance la redétection des tags des teams\n");
	Msg("cssm_go : force le !go de toutes les teams pendant les strats\n");
	Msg("cssm_restartmanche : relance la manche en cours\n");
	Msg("cssm_restartround : relance la manche en cours\n");
	Msg("cssm_grant \"steamID\" : ajoute un arbitre le temps d'une map\n");
	Msg("cssm_revoke \"steamID\" : suspend les droits d'un arbitre le temps d'une map\n");
	Msg("cssm_teamt tag de la team terroriste : édite le tag de la team actuellement terroriste\n");
	Msg("cssm_teamct tag de la team anti-terroriste : édite le tag de la team actuellement anti-terroriste\n");
	Msg("cssm_swap ID : swap un joueur\n");
	Msg("cssm_spec ID : met en spectateur un joueur\n");
}

CON_COMMAND(cssm_start, "CSSMatch : Lance un match")
{
	// On r�cup�re l'instance du match en cours
	Match * match = CSSMatch::getInstance()->getMatch();

	// Cette commande ne fonctionne que s'il y a un match en cours
	if (match->getPhase() == PHASE_OFF)
	{
		// Si aucun fichier de configuration n'a �t� sp�cifi� en argument
		//		on cherche � ex�cuter le fichier default.cfg
		if (Api::engine->Cmd_Argc()==1)
		{
			// ... on tente de lancer le match avec le fichier default.cfg
			if (Api::filesystem->FileExists("cfg/cssmatch/configurations/default.cfg", "MOD"))
			{
				match->lanceMatch(new Configuration("default.cfg"));	
			}
			else
				Msg("CSSMatch : default.cfg absent, veuillez préciser un nom de fichier\n");
		}
		else
		{
			// Sinon nous supposons que le premier argument est un nom de fichier de configuration

			// On v�rifie que le retour du premier argument est valide
			const char * arg1 = Api::engine->Cmd_Argv(1);
			if (arg1 != NULL)
			{
				// On place le nom de fichier dans le r�pertoire attendu par le plugin
				string pathFichier("cfg/cssmatch/configurations/" + string(arg1));

				// On ne lance le match que si le fichier existe vraiment
				if (Api::filesystem->FileExists(pathFichier.c_str(), "MOD"))
					match->lanceMatch(new Configuration(arg1));
				else
					Msg("CSSMatch : Impossible de trouver le fichier %s",arg1);
					
			}
			else
				Msg("CSSMatch : Erreur dans le traitement de la commande\n");
		}
	}
	else
		Msg("CSSMatch : Un match est déjà en cours...\n");
}

CON_COMMAND(cssm_stop, "CSSMatch : Stoppe un match")
{
	// On r�cup�re l'instance Match du plugin
	Match * match = CSSMatch::getInstance()->getMatch();

	// Si il n'y a aucun match en cours on ne fait rien
	if (match->getPhase() != PHASE_OFF)
		match->finMatch();
	else
		Msg("CSSMatch : Il n'y a aucun match en cours...\n");
}

CON_COMMAND(cssm_retag, "CSSMatch : Lance la redetection des tags des teams")
{
	// On r�cup�re l'instance Match du plugin
	Match * match = CSSMatch::getInstance()->getMatch();

	// On lance la red�tection des tags
	match->detecteTags(true);
}

CON_COMMAND(cssm_go, "CSSMatch : Force le !go de toutes les teams pendant les strats")
{
	// On r�cup�re l'instance Match du plugin
	Match * match = CSSMatch::getInstance()->getMatch();

	// On force le !go des teams
	match->forceGo();
}

CON_COMMAND(cssm_restartmanche, "CSSMatch : Relance la manche en cours")
{
	// On r�cup�re l'instance Match du plugin
	Match * match = CSSMatch::getInstance()->getMatch();

	// On relance la manche
	match->restartManche();
}

CON_COMMAND(cssm_restartround, "CSSMatch : Relance le round en cours")
{
	// On r�cup�re l'instance Match du plugin
	Match * match = CSSMatch::getInstance()->getMatch();

	// On relance la manche
	match->restartRound();
}

CON_COMMAND(cssm_grant, "CSSMatch : Ajoute un arbitre le temps d'une map")
{
	// La diff�rence de traitement des guillemets dans la console Windows et la console Linux nous oblige � 
	// opter pour une capture de tous les arguments

	// Il nous faut au moins 2 arguments
	if (Api::engine->Cmd_Argc()>1)
	{
		// V�rification de la validit� du retour de la m�thode de VALVE
		const char * steamIDvalve = Api::engine->Cmd_Args();
		if (steamIDvalve != NULL)
		{
			string sSteamId(steamIDvalve);

			// Supression des espaces ajout�s par la console si le steamID n'�tait pas entre guillemets sous Windows
			size_t posCar;
			while((posCar = sSteamId.find(' ')) != string::npos)
			{
				sSteamId.replace(posCar,1,"",0,0);
			}

			// Supression des �ventuels guillemets pass�s en argument par la console sous Linux
			while((posCar = sSteamId.find('"')) != string::npos)
			{
				sSteamId.replace(posCar,1,"",0,0);
			}

			// Supression des tabulations �ventuelles
			while((posCar = sSteamId.find('\t')) != string::npos)
			{
				sSteamId.replace(posCar,1,"",0,0);
			}

			if (CSSMatch::getInstance()->getAdminList()->addAdmin(sSteamId))
				Msg("CSSMatch : La steamID %s est maintenant arbitre le temps d'une map\n",sSteamId.c_str());
			else
				Msg("CSSMatch : La steamID %s est arbitre\n",sSteamId.c_str());
		}
		else
			Warning("CSSMatch : Erreur de traitement de la commande\n");
	}
	else
		Msg("cssm_grant steamID\n");
}

CON_COMMAND(cssm_revoke, "CSSMatch : Supprime un arbitre le temps d'une map")
{
	// La diff�rence de traitement des guillemets dans la console Windows et la console Linux nous oblige � 
	// opter pour une capture de tous les arguments

	// Il nous faut au moins 2 arguments
	if (Api::engine->Cmd_Argc()>1)
	{
		// V�rification de la validit� du retour de la m�thode de VALVE
		const char * steamIDvalve = Api::engine->Cmd_Args();
		if (steamIDvalve != NULL)
		{
			string sSteamId(steamIDvalve);

			// Supression des espaces ajout�s par la console si le steamID n'�tait pas entre guillemets sous Windows
			size_t posCar;
			while((posCar = sSteamId.find(' ')) != string::npos)
			{
				sSteamId.replace(posCar,1,"",0,0);
			}

			// Supression des �ventuels guillemets pass�s en argument par la console sous Linux
			while((posCar = sSteamId.find('"')) != string::npos)
			{
				sSteamId.replace(posCar,1,"",0,0);
			}

			// Supression des tabulations �ventuelles
			while((posCar = sSteamId.find('\t')) != string::npos)
			{
				sSteamId.replace(posCar,1,"",0,0);
			}

			if (CSSMatch::getInstance()->getAdminList()->removeAdmin(sSteamId))
				Msg("CSSMatch : La steamID %s n'est maintenant plus arbitre\n",sSteamId.c_str());
			else
				Msg("CSSMatch : La steamID %s n'est pas arbitre\n",sSteamId.c_str());
		}
		else
			Warning("CSSMatch : Erreur de traitement de la commande\n");
	}
	else
		Msg("cssm_revoke steamID\n");
}

CON_COMMAND(cssm_teamt, "CSSMatch : Edite le tag de la team actuellement terroriste")
{
	// R�cup�ration des arguments
	const char * tag = Api::engine->Cmd_Args();
	if (tag != NULL)
	{
		// Cette commande n'aura un effet qu'en match
		Match * match = CSSMatch::getInstance()->getMatch();
		if (match->getPhase() != PHASE_OFF)
		{
			// Mise � jour du tag
			match->modifieTag(TEAM_T,string(tag));
			Msg("CSSMatch : Le nouveau tag de la team terroriste est %s\n",tag);
		}
		else
			Msg("CSSMatch : Il n'y a aucun match en cours...\n");
	}
	else
		Msg("CSSMatch : Erreur de traitement de la commande\n");
}

CON_COMMAND(cssm_teamct, "CSSMatch : Edite le tag de la team actuellement anti-terroriste")
{
	// R�cup�ration des arguments
	const char * tag = Api::engine->Cmd_Args();
	if (tag != NULL)
	{
		// Cette commande n'aura un effet qu'en match
		Match * match = CSSMatch::getInstance()->getMatch();
		if (match->getPhase() == PHASE_OFF)
		{
			// Mise � jour du tag
			match->modifieTag(TEAM_CT,string(tag));
			Msg("CSSMatch : Le nouveau tag de la team anti-terroriste est %s\n",tag);
		}
		else
			Msg("CSSMatch : Il n'y a aucun match en cours...\n");
	}
	else
		Msg("CSSMatch : Erreur de traitement de la commande\n");
}

CON_COMMAND(cssm_swap, "CSSMatch : Swap un joueur")
{
	// Il nous faut au moins 2 arguments
	if (Api::engine->Cmd_Argc()>1)
	{
		// R�cup�ration de l'ID du joueur
		const char * userid = Api::engine->Cmd_Argv(1);
		if (userid != NULL)
		{
			try
			{
				// Recherche du joueur
				edict_t * pEntity = Api::getPEntityFromID(atoi(userid));
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);

				if (player->GetTeamIndex() > TEAM_SPEC)
					Api::swap(player);
				else
					Msg("CSSMatch : Ce joueur est spectateur\n");
			}
			catch(const CSSMatchApiException & e)
			{
				Msg("CSSMatch : Joueur invalide\n");
			}
		}
		else
			Msg("CSSMatch : Erreur de traitement de la commande\n");
	}
	else
		Msg("cssm_swap ID\n");
}

CON_COMMAND(cssm_spec, "CSSMatch : Met en spectateur un joueur")
{
	// Il nous faut au moins 2 arguments
	if (Api::engine->Cmd_Argc()>1)
	{
		// R�cup�ration de l'ID du joueur
		const char * userid = Api::engine->Cmd_Argv(1);
		if (userid != NULL)
		{
			try
			{
				// Recherche du joueur
				edict_t * pEntity = Api::getPEntityFromID(atoi(userid));
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);

				if (player->GetTeamIndex() > TEAM_SPEC)
					Api::spec(player);
				else
					Msg("CSSMatch : Ce joueur est spectateur\n");
			}
			catch(const CSSMatchApiException & e)
			{
				Msg("CSSMatch : Joueur invalide\n");
			}
		}
		else
			Msg("CSSMatch : Erreur de traitement de la commande\n");
	}
	else
		Msg("cssm_spec ID\n");
}

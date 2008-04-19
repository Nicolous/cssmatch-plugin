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
#include "../CSSMatch/CSSMatch.h"
#include "../Messages/Messages.h"
#include "../Match/Match.h"

using std::ostringstream;
using std::string;
using std::out_of_range;

CON_COMMAND(cssm_help, "CSSMatch : Liste et dÃ©crit les commandes RCON du plugin dans la console")
{
	Msg("cssm_start [fichier de config optionnel] : lance un match\n");
	Msg("cssm_stop : stop un match\n");
	Msg("cssm_retag : lance la redÃ©tection des tags des teams\n");
	Msg("cssm_go : force le !go de toutes les teams pendant les strats\n");
	Msg("cssm_restartmanche : relance la manche en cours\n");
	Msg("cssm_restartround : relance la manche en cours\n");
	Msg("cssm_grant \"steamID\" : ajoute un arbitre le temps d'une map\n");
	Msg("cssm_revoke \"steamID\" : suspend les droits d'un arbitre le temps d'une map\n");
	Msg("cssm_teamt tag de la team terroriste : Ã©dite le tag de la team actuellement terroriste\n");
	Msg("cssm_teamct tag de la team anti-terroriste : Ã©dite le tag de la team actuellement anti-terroriste\n");
	Msg("cssm_swap ID : swap un joueur\n");
	Msg("cssm_spec ID : met en spectateur un joueur\n");
}

CON_COMMAND(cssm_start, "CSSMatch : Lance un match")
{
	// On récupère l'instance Match du plugin
	Match * match = g_CSSMatchPlugin.getMatch();

	// Si il n'y a déjà un match en cours on ne fait rien
	if (match->getPhase() != PHASE_OFF)
	{
		Msg("CSSMatch : Un match est dÃ©jÃ  en cours...\n");
		return;
	}

	// Si aucun fichier de configuration n'a été spécifié en argument on lance le match avec le fichier default.cfg (si possible)
	if (API::engine->Cmd_Argc()==1)
	{
		if (API::filesystem->FileExists("cfg/cssmatch/configurations/default.cfg", "MOD"))
		{
			match->setCutRound(true);
			match->lanceMatch(new Configuration("default.cfg"));	
		}
		else
			Msg("CSSMatch : default.cfg absent, veuillez prÃ©ciser un nom de fichier\n");
		return;
	}

	// Sinon nous supposons que le premier argument est un nom de fichier de configuration

	// On vérifie que le retour du premier argument est valide
	const char * arg1 = API::engine->Cmd_Argv(1);
	if (!arg1)
	{
		Msg("CSSMatch : Erreur dans le traitement de la commande\n");
		return;
	}

	// On place le nom de fichier dans le répertoire attendu par le plugin
	string pathFichier = "cfg/cssmatch/configurations/" + string(arg1);

	// On ne lance le match que si le fichier existe vraiment
	if (!API::filesystem->FileExists(pathFichier.c_str(), "MOD"))
	{
		Msg("CSSMatch : Impossible de trouver le fichier %s",arg1);
		return;
	}
	match->lanceMatch(new Configuration(arg1));
}

CON_COMMAND(cssm_stop, "CSSMatch : Stop un match")
{
	// On récupère l'instance Match du plugin
	Match * match = g_CSSMatchPlugin.getMatch();

	// Si il n'y a aucun match en cours on ne fait rien
	if (match->getPhase() == PHASE_OFF)
	{
		Msg("CSSMatch : Il n'y a aucun match en cours...\n");
		return;
	}

	// On stoppe le match
	match->finMatch();
}

CON_COMMAND(cssm_retag, "CSSMatch : Lance la redetection des tags des teams")
{
	// On récupère l'instance Match du plugin
	Match * match = g_CSSMatchPlugin.getMatch();

	// On lance la redétection des tags
	match->detecteTags(true);
}

CON_COMMAND(cssm_go, "CSSMatch : Force le !go de toutes les teams pendant les strats")
{
	// On récupère l'instance Match du plugin
	Match * match = g_CSSMatchPlugin.getMatch();

	// On force le !go des teams
	match->forceGo();
}

CON_COMMAND(cssm_restartmanche, "CSSMatch : Relance la manche en cours")
{
	// On récupère l'instance Match du plugin
	Match * match = g_CSSMatchPlugin.getMatch();

	// On relance la manche
	match->restartManche();
}

CON_COMMAND(cssm_restartround, "CSSMatch : Relance le round en cours")
{
	// On récupère l'instance Match du plugin
	Match * match = g_CSSMatchPlugin.getMatch();

	// On relance la manche
	match->restartRound();
}

CON_COMMAND(cssm_grant, "CSSMatch : Ajoute un arbitre le temps d'une map")
{
	// La différence de traitement des guilemets dans la console Windows et la console Linux nous oblige à 
	// opter pour une capture de tous les arguments

	// Il nous faut au moins 2 arguments
	if (API::engine->Cmd_Argc()<=1)
	{
		Msg("cssm_grant steamID\n");
		return;
	}

	// Vérification de la validité du retour de la méthode de VALVE
	const char * steamIDvalve = API::engine->Cmd_Args();
	if (!steamIDvalve)
	{
		Warning("CSSMatch : Erreur de traitement de la commande\n");
		return;
	}

	// Supression des espaces ajoutés par la console si le steamID n'était pas entre guillemets sous Windows
	char steamidSansEspaces[32];
	V_StrSubst(steamIDvalve," ","",steamidSansEspaces,sizeof(steamidSansEspaces));

	// Supression des éventuels guillemets passés en argument par la console sous Linux
	char steamidSansGuillements[32];
	V_StrSubst(steamidSansEspaces,"\"","",steamidSansGuillements,sizeof(steamidSansGuillements));

	// Supression des tabulations éventuelles
	char steamSansTabulations[32];
	V_StrSubst(steamidSansGuillements,"\t","",steamSansTabulations,sizeof(steamSansTabulations));

	if (g_CSSMatchPlugin.getAdminList()->addAdmin(steamSansTabulations))
		Msg("CSSMatch : La steamID %s est maintenant arbitre le temps d'une map\n",steamSansTabulations);
	else
		Msg("CSSMatch : La steamID %s est arbitre\n",steamSansTabulations);
}

CON_COMMAND(cssm_revoke, "CSSMatch : Supprime un arbitre le temps d'une map")
{
	// La différence de traitement des guilemets dans la console Windows et la console Linux nous oblige à 
	// opter pour une capture de tous les arguments

	// Il nous faut au moins 2 arguments
	if (API::engine->Cmd_Argc()<=1)
	{
		Msg("cssm_revoke steamID\n");
		return;
	}

	// Vérification de la validité du retour de la méthode de VALVE
	const char * steamIDvalve = API::engine->Cmd_Args();
	if (!steamIDvalve)
	{
		Warning("CSSMatch : Erreur de traitement de la commande\n");
		return;
	}

	// Supression des espaces ajoutés par la console si le steamID n'était pas entre guillemets sous Windows
	char steamidSansEspaces[32];
	V_StrSubst(steamIDvalve," ","",steamidSansEspaces,sizeof(steamidSansEspaces));

	// Supression des éventuels guillemets passés en argument par la console sous Linux
	char steamidSansGuillements[32];
	V_StrSubst(steamidSansEspaces,"\"","",steamidSansGuillements,sizeof(steamidSansGuillements));

	// Supression des tabulations éventuelles
	char steamSansTabulations[32];
	V_StrSubst(steamidSansGuillements,"\t","",steamSansTabulations,sizeof(steamSansTabulations));

	if (g_CSSMatchPlugin.getAdminList()->removeAdmin(steamSansTabulations))
		Msg("CSSMatch : La steamID %s n'est maintenant plus arbitre\n",steamSansTabulations);
	else
		Msg("CSSMatch : La steamID %s n'est pas arbitre\n",steamSansTabulations);
}

CON_COMMAND(cssm_teamt, "CSSMatch : Edite le tag de la team actuellement terroriste")
{
	// Récupération des arguments
	const char * tag = API::engine->Cmd_Args();
	if (!tag)
	{
		Msg("CSSMatch : Erreur de traitement de la commande\n");
		return;
	}
	
	// Cette commande n'aura un effet qu'en match
	Match * match = g_CSSMatchPlugin.getMatch();
	if (match->getPhase() == PHASE_OFF)
	{
		Msg("CSSMatch : Il n'y a aucun match en cours...\n");
		return;
	}

	// Mise à jour du tag
	match->getTeam(TEAM_T)->setTag(tag);
}

CON_COMMAND(cssm_teamct, "CSSMatch : Edite le tag de la team actuellement anti-terroriste")
{
	// Récupération des arguments
	const char * tag = API::engine->Cmd_Args();
	if (!tag)
	{
		Msg("CSSMatch : Erreur de traitement de la commande\n");
		return;
	}
	
	// Cette commande n'aura un effet qu'en match
	Match * match = g_CSSMatchPlugin.getMatch();
	if (match->getPhase() == PHASE_OFF)
	{
		Msg("CSSMatch : Il n'y a aucun match en cours...\n");
		return;
	}

	// Mise à jour du tag
	match->getTeam(TEAM_CT)->setTag(tag);
}

CON_COMMAND(cssm_swap, "CSSMatch : Swap un joueur")
{
	// Il nous faut au moins 2 arguments
	if (API::engine->Cmd_Argc()<=1)
	{
		Msg("cssm_swap ID\n");
		return;
	}

	// Récupération de l'ID du joueur
	const char * userid = API::engine->Cmd_Argv(1);
	if (!userid)
	{
		Msg("CSSMatch : Erreur de traitement de la commande\n");
		return;
	}

	// Récupération du pointeur sur l'entité
	edict_t * pEntity;
	try
	{
		pEntity = API::getEntityFromID(atoi(userid));
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		return;
	}

	IPlayerInfo * player;
	try
	{
		player = API::getIPlayerInfoFromEntity(pEntity);
	}
	catch(const out_of_range & e)
	{
		Msg("CSSMatch : Joueur invalide\n");
		return;
	}

	if (player->GetTeamIndex() > TEAM_SPEC)
		API::swap(player);
	else
		Msg("CSSMatch : Ce joueur est spectateur\n");
}

CON_COMMAND(cssm_spec, "CSSMatch : Met en spectateur un joueur")
{
	// Il nous faut au moins 2 arguments
	if (API::engine->Cmd_Argc()<=1)
	{
		Msg("cssm_spec ID\n");
		return;
	}

	// Récupération de l'ID du joueur
	const char * userid = API::engine->Cmd_Argv(1);
	if (!userid)
	{
		Msg("CSSMatch : Erreur de traitement de la commande\n");
		return;
	}

	// Récupération du pointeur sur l'entité
	edict_t * pEntity;
	try
	{
		pEntity = API::getEntityFromID(atoi(userid));
	}
	catch(const out_of_range & e)
	{
		API::debug(e.what());
		return;
	}

	IPlayerInfo * player;
	try
	{
		player = API::getIPlayerInfoFromEntity(pEntity);
	}
	catch(const out_of_range & e)
	{
		Msg("CSSMatch : Joueur invalide\n");
		return;
	}

	if (player->GetTeamIndex() > TEAM_SPEC)
		API::spec(player);
	else
		Msg("CSSMatch : Ce joueur est spectateur\n");
}

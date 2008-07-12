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

#include "SayHook.h"
#include "../../CSSMatch/CSSMatch.h"

using std::string;
using std::exception;
using std::istringstream;

SayHook::SayHook() : ConCommand("say", NULL, "[CSSMatch] : say hook command", FCVAR_GAMEDLL)
{
	say = NULL;
	succes = false;
} 

void SayHook::Init() 
{
	// Recherche de la commande "say" originale
	const ConCommandBase * commandes = NULL; 

	if (Api::cvars != NULL)
	{
		commandes = Api::cvars->GetCommands();
		while(commandes != NULL)
		{
			if (commandes->IsCommand() && commandes != this && string(commandes->GetName()) == "say")
			{ 
				say = (ConCommand*)(const_cast<ConCommandBase*>(commandes)); 
				commandes = NULL;
				succes = true;
			}
			else
				commandes = commandes->GetNext();
		}
		if (succes)
			ConCommand::Init();
		else
			Api::debug("Impossible de hooker la commande \"say\" : la commande est introuvale");
	}
	else
		Api::debug("Impossible de hooker la commande \"say\" : l'interface n'est pas prete");
} 

void SayHook::Dispatch() 
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int index = cssmatch->GetCommandIndex()+1; 

	try
	{
		// RÈcupÈration du message sans les guillemets qui l'entoure
		string ligneMessage;
		int argc = Api::engine->Cmd_Argc();
		for(int i=1;i<argc;i++)
		{
			ligneMessage += Api::engine->Cmd_Argv(i);
		}
		
		istringstream message(ligneMessage);
		string commande;
		
		message >> commande;

		// AccËs au menu d'arbitrage
		if (commande == "cssmatch")
		{
			cmdCssmatch(index,message);
		}
		// Edition du tag du camp actuellement terroriste
		else if (commande == "!teamt")
		{
			cmdTeamt(index,message);
		}
		// Edition du tag du camp actuellement anti-terroriste
		else if (commande == "!teamct")
		{
			cmdTeamct(index,message);
		}
		else
			say->Dispatch();
	}
	catch(const CSSMatchApiException & e)
	{
		Api::debug(e.what());
		say->Dispatch();
	}
}

void SayHook::cmdCssmatch(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();

	// Aucun argument n'est attendu pour cette commande
	string test;
	std::getline(in,test);
	if (test.empty())
	{
		edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
		const char * steamID = Api::engine->GetPlayerNetworkIDString(pEntity);

		if (steamID != NULL)
		{
			// Il faut que le joueur soit arbitre
			if (cssmatch->getAdminList()->estAdmin(steamID))
				Api::cexec(pEntity,"cssmatch\n");
			else
				Messages::sayTell(indexJoueur,"Tu n'es pas arbitre du match !");
		}
	}
}

void SayHook::cmdTeamt(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
	const char * steamID = Api::engine->GetPlayerNetworkIDString(pEntity);
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (steamID != NULL)
	{
		// Il faut que le joueur soit arbitre
		if (cssmatch->getAdminList()->estAdmin(steamID))
		{
			//int mancheCourante = match.getMancheCourante();
			if (match->getPhase() != PHASE_OFF)
			{
				// RÈcupÈration du pseudo aprËs la commande !teamt
				string nouvTag;
				std::getline(in,nouvTag);

				if (! nouvTag.empty())
				{
					// Retrait du premier espace sÈparant la commande de ses arguments
					string::iterator debutTag = nouvTag.begin();
					nouvTag.erase(debutTag,debutTag+1);

					// DÈtermination de le camp actuellement terroriste et enregistrement du tag
					match->modifieTag(TEAM_T,nouvTag);
					Messages::sayMsg("Le nouveau tag de la team terroriste est \003" + nouvTag);
					match->formateHostname();
				}
				else
					Messages::sayTell(indexJoueur,"!teamt Le Nouveau Tag");
			}
			else
				Messages::sayTell(indexJoueur,"Aucun match n'est en cours !");
		}
		else
			Messages::sayTell(indexJoueur,"Tu n'es pas arbitre du match !");
	}
}

void SayHook::cmdTeamct(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
	const char * steamID = Api::engine->GetPlayerNetworkIDString(pEntity);
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (steamID != NULL)
	{
		// Il faut que le joueur soit arbitre
		if (cssmatch->getAdminList()->estAdmin(steamID))
		{
			if (match->getPhase() != PHASE_OFF)
			{
				// RÈcupÈration du pseudo aprËs la commande !teamt
				string nouvTag;
				std::getline(in,nouvTag);

				if (! nouvTag.empty())
				{
					// Retrait du premier espace sÈparant la commande de ses arguments
					string::iterator debutTag = nouvTag.begin();
					nouvTag.erase(debutTag,debutTag+1);

					// DÈtermination de le camp actuellement terroriste et enregistrement du tag
					match->modifieTag(TEAM_CT,nouvTag);
					Messages::sayMsg("Le nouveau tag de la team anti-terroriste est \003" + nouvTag);
					match->formateHostname();
				}
				else
					Messages::sayTell(indexJoueur,"!teamct Le Nouveau Tag");
			}
			else
				Messages::sayTell(indexJoueur,"Il n'y a aucun match en cours !");
		}
		else
			Messages::sayTell(indexJoueur,"Tu n'es pas arbitre du match !");
	}
}

void SayHook::cmdTeam(int indexJoueur, std::istringstream & in, CodeTeam codeTeam) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
	const char * steamID = Api::engine->GetPlayerNetworkIDString(pEntity);
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (steamID != NULL)
	{
		// Il faut que le joueur soit arbitre
		if (cssmatch->getAdminList()->estAdmin(steamID))
		{
			//int mancheCourante = match.getMancheCourante();
			if (match->getPhase() != PHASE_OFF)
			{
				// RÈcupÈration du pseudo aprËs la commande !teamt
				string nouvTag;
				std::getline(in,nouvTag);

				if (! nouvTag.empty())
				{
					// Retrait du premier espace sÈparant la commande de ses arguments
					string::iterator debutTag = nouvTag.begin();
					nouvTag.erase(debutTag,debutTag+1);

					switch(codeTeam)
					{
					case TEAM_T:
						Messages::sayMsg("Le nouveau tag de la team terroriste est \003" + nouvTag);
						break;
					case TEAM_CT:
						//break;
					default:
						Messages::sayMsg("Le nouveau tag de la team anti-terroriste est \003" + nouvTag);
						codeTeam = TEAM_CT;
					}
						
					// Enregistrement du tag
					match->modifieTag(codeTeam,nouvTag);

					// Mise ‡ jour du nom du serveur si besoin
					match->formateHostname();
				}
				else
					Messages::sayTell(indexJoueur,"Veuillez sp√©cifier le nouveau tag de la team");
			}
			else
				Messages::sayTell(indexJoueur,"Aucun match n'est en cours !");
		}
		else
			Messages::sayTell(indexJoueur,"Tu n'es pas arbitre du match !");
	}
}

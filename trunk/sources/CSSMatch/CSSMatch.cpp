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

#include "CSSMatch.h"
#include "../Hooks/SayHook/SayHook.h"
#include "../Hooks/SayTeamHook/SayTeamHook.h"

#include "../GestionMenus/GestionMenus.h"

using std::string;
using std::ostringstream;
using std::exception;

CSSMatch * CSSMatch::instance = NULL;

CSSMatch * CSSMatch::getInstance()
{
	if (instance == NULL)
		instance = new CSSMatch();
	return instance;
}

CSSMatch::CSSMatch()
{
	Msg("CSSMatch : Instanciation du singleton...\n");
	m_iClientCommandIndex = 0;

	timers = GestionTimers(10);
}

CSSMatch::~CSSMatch()
{
	Msg("CSSMatch : Destruction du singleton..\n");
}

bool CSSMatch::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	bool succes = false; // we require all these interface to function

	Msg("CSSMatch : Chargement...\n");
	// get the interfaces we want to use
	if(Api::initialisation(interfaceFactory,gameServerFactory))
	{
		InitCVars(interfaceFactory); // register any cvars we have defined
		MathLib_Init(2.2f, 2.2f, 0.0f, 2);

		// Recherche des ConVars cr��es par VALVE qui vont nous servir
		ConVars::initialise();

		// Initialisation des menus par d�faut (cssmatch_advanced peut modifier les menus)
		GestionMenus::getMenus()->initialiseMenus();

		// Initialisation des props
		propCash = PlayerProp("CCSPlayer","m_iAccount");
		propLifeState = PlayerProp("CBasePlayer","m_lifeState");

		succes = true;
		Msg("CSSMatch : Chargement complet !\n");
	}

	return succes;
}

void CSSMatch::Unload()
{
	Msg("CSSMatch : Dechargement...\n");

	if (Api::gameeventmanager != NULL) // Si le d�chargement est provoqu� par l'impossibilit� d'initialiser l'interface GameEventManager
		Api::gameeventmanager->RemoveListener(&ecouteur); // make sure we are unloaded from the event system

	// Destruction de l'instance unique du d�compte
	Decompte::killDecompte();

	// Destruction de l'instance unique du gestionnaire de joueurs
	GestionJoueurs::killGestionJoueurs();

	// Destruction de l'instance unique du gestionnaire de menus
	GestionMenus::killMenus();

	// Destruction de l'instance unique du plugin
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}

	Msg("CSSMatch : Dechargement complet !\n");
}

void CSSMatch::Pause()
{
}

void CSSMatch::UnPause()
{
}

const char * CSSMatch::GetPluginDescription()
{
	return CSSMATCH_VERSION;
}

void CSSMatch::LevelInit(const char * pMapName)
{
	// R�cup�ration des �v�nements qui nous int�ressent
	ecouteur.initialise();

	// Effacement des timers en attente
	timers.purge();

	// Si le changement de map a �t� fait pendant un match on stoppe le match
	if (match.getPhase() != PHASE_OFF)
		match.finTempsMortFinMatch();


	// R�cup�ration de la liste des steamID arbitre
	if (! adminlist.parseAdminlist(FICHIER_ADMINLIST))
		Api::engine->LogPrint("CSSMatch : Attention, le fichier \"" FICHIER_ADMINLIST "\" est manquant !");
}

void CSSMatch::ServerActivate(edict_t * pEdictList, int edictCount, int clientMax)
{
	// Mise � jour de la variable maxplayers
	Api::maxplayers = clientMax;

	// Mise � jour de la taille de la liste des joueurs
	// Le resize n'est pas risqu�, les index concern�s n'auraient jamais �t� pris par un joueur
	GestionJoueurs::getPlayerList()->setTaillePlayerList(clientMax+1);
}

void CSSMatch::GameFrame(bool simulating)
{
	// Alimentation temporel du d�compte ;)
	Decompte::getDecompte()->getMoteur()->execute();

	// Ex�cution des timers en attente d'ex�cution
	timers.execute();
}

void CSSMatch::LevelShutdown() // !!!!this can get called multiple times per map change
{
}

void CSSMatch::ClientActive(edict_t * pEntity)
{
}

void CSSMatch::ClientDisconnect(edict_t * pEntity)
{
}

void CSSMatch::ClientPutInServer(edict_t * pEntity, const char * playername)
{
	// Validation de l'entit�
	if (Api::isValidePEntity(pEntity))
	{
		// Quel est l'index du joueur qui vient de se connecter ?
		int indexJoueur = Api::engine->IndexOfEdict(pEntity);

		// V�rification de la validit� de l'index obtenu
		if (Api::isValideIndex(indexJoueur))
		{
			// On s'assure que le joueur entrant sur le serveur n'utilisait pas un menu du plugin sur l'�ventuelle map pr�c�dente	
			GestionMenus::getMenus()->quitteMenu(indexJoueur);

			// Cet �v�nement ne nous int�resse pas en dehors d'un match
			if (match.getPhase() != PHASE_OFF)
			{
				Messages::sayMsg("\003" + string(playername) + "\001 rejoint la partie !",indexJoueur);
				Messages::tellAMX(indexJoueur,"Ce serveur héberge actuellement un match !\n\nSi tu n'en fais pas partie, quittes le serveur STP",0);
			}
		}
	}
}

void CSSMatch::SetCommandClient(int index)
{
	m_iClientCommandIndex = index;
}

void CSSMatch::ClientSettingsChanged(edict_t * pEdict)
{
}

PLUGIN_RESULT CSSMatch::ClientConnect(bool * bAllowConnect,
									  edict_t * pEntity,
									  const char * pszName,
									  const char * pszAddress,
									  char * reject,
									  int maxrejectlen)
{
	PLUGIN_RESULT decision = PLUGIN_CONTINUE;

	// On v�rifie que l'entit� est valide
	if (Api::isValidePEntity(pEntity)) 
	{
		// Quel est l'index du joueur qui vient de se connecter ?
		int indexJoueur = Api::engine->IndexOfEdict(pEntity);
		// On v�rifie la coh�rence de l'index trouv�
		if (Api::isValideIndex(indexJoueur))
		{
			// On met � jour l'instance Joueur situ� � cet index dans la liste de joueurs
			GestionJoueurs::getPlayerList()->connexion(indexJoueur);
		}
	}

	return decision;
}

PLUGIN_RESULT CSSMatch::ClientCommand(edict_t * pEntity)
{
	PLUGIN_RESULT decision = PLUGIN_CONTINUE;

	// R�cup�ration du nom de la commande ex�cut�e
	const char * pcmd = Api::engine->Cmd_Argv(0);

	// R�cup�ration de l'index de l'entit�
	int indexJoueur = Api::engine->IndexOfEdict(pEntity);

	// R�cup�ration du steamID du joueur en s'assurant qu'il est valide
	const char * steamIDvalve = Api::engine->GetPlayerNetworkIDString(pEntity);

	// R�cup�ration des arguments de la commande (peut �tre NULL)
	const char * args = Api::engine->Cmd_Args();

	// R�cup�ration de la phase de match en cours
	CodePhase phase = match.getPhase();
	try
	{
		// V�rification de la validit� des informations
		if (Api::isValidePEntity(pEntity) &&
			pcmd != NULL &&
			Api::isValideIndex(indexJoueur) &&
			steamIDvalve != NULL)
		{
			string commande(pcmd);
			string steamID(steamIDvalve);
			string arguments(args != NULL ? args : "");

			// Si nous sommes en match nous loguons toutes les commandes passant dans la console des joueurs
			if (phase != PHASE_OFF)
			{
				try
				{
					ostringstream log;
					log << "CSSMatch : " << Api::getPlayerName(pEntity) << " <id=" << Api::getUseridFromPEntity(pEntity) << "> <steamID=\"" << steamID << "\"> exécute la commande " << commande << " " + arguments << "\n";
					Api::engine->LogPrint(log.str().c_str());
				}
				catch(const CSSMatchApiException & e)
				{
					Api::reporteException(e,__FILE__,__LINE__);
				}
			}

			// *********************
			// Utilisation d'un menu
			// *********************
			if (commande == "menuselect")
			{
				// R�cup�ration et validation du premier param�tre 
				const char * parametre = Api::engine->Cmd_Argv(1);
				if (parametre != NULL)
				{
					// R�cup�ration du choix du joueur dans le menu
					int choix = atoi(parametre);

					// Ex�cution du code associ� � l'action dans le menu
					GestionMenus::getMenus()->actionMenu(indexJoueur,choix);
				}
			}

			// *****************************
			// Changement volontaire de camp
			// *****************************
			else if (commande == "jointeam")
			{
				// Si un match est en cours, on interdit les changements de camp
				CodePhase phaseLimite = PHASE_MANCHE;
				if (match.getMancheCourante() > 1)
					phaseLimite = PHASE_OFF;
				try
				{
					if (phase >= phaseLimite)
					{
						// Contr�le de la validit� de l'argument
						if (arguments.size() == 1) 
						{
							if (arguments[0] == '2' || arguments[0] == '3')
							{
								if (Api::getPEntityTeam(pEntity) > TEAM_SPEC)
								{
									Messages::sayTell(indexJoueur,"Seuls \004CSSMatch\001 et \004les arbitres\001 peuvent changer un joueur de camp");

									//Api::swap(Api::getIPlayerInfoFromEntity(pEntity));
									decision = PLUGIN_STOP;
								}
							}
							// Argument invalide (ex : lettre) : refus du swap
							else if (atoi(arguments.c_str()) == 0)
								decision = PLUGIN_STOP;
							// Le cas o� l'index d�passe 3 est pris en charge par le jeu
						}
						else // argument invalide : refus du swap
							decision = PLUGIN_STOP;
					}
				}
				catch(const CSSMatchApiException & e)
				{
					Api::reporteException(e,__FILE__,__LINE__);
				}
			}

			// ************
			// Menu arbitre
			// ************
			else if (commande == "cssmatch")
			{
				if (adminlist.estAdmin(steamID))
				{
					// Son d'ouverture du menu
					Api::cexec(pEntity,"playgamesound UI/buttonrollover.wav\n");

					GestionMenus::getMenus()->afficheMenu(phase,indexJoueur);
				}
				else
				{
					Messages::sayTell(indexJoueur,"Tu n'es pas arbitre !");
					string log("CSSMatch : La steamID " + string(steamIDvalve) + " n'est pas arbitre\n");
					Api::engine->LogPrint(log.c_str());
					adminlist.logAdminlist();
				}
				decision = PLUGIN_STOP;
			}

			// *****************
			// Rates des joueurs
			// *****************
			else if (commande == "cssm_rates")
			{
				Messages::getRatesTab(pEntity);
				
				decision = PLUGIN_STOP;
			}
		}
	}
	catch (const exception & e)
	{
		Messages::sayTell(indexJoueur,"Une erreur est survenue, veuillez en avertir l'auteur sur " CSSMATCH_SITE);
		Api::reporteException(e,__FILE__,__LINE__);
		//throw;
	}
	return decision;
}

PLUGIN_RESULT CSSMatch::NetworkIDValidated(const char * pszUserName, const char * pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

/*void CSSMatch::FireGameEvent(IGameEvent * event)
{
} Voir la classe EventListener */

int CSSMatch::GetCommandIndex()
{
	return m_iClientCommandIndex;
}

Authentification * CSSMatch::getAdminList()
{
	return &adminlist;
}

Match * CSSMatch::getMatch()
{
	return &match;
}

GestionTimers * CSSMatch::getTimers()
{
	return &timers;
}

void CSSMatch::goTeam(Team * team, edict_t * pEntity)
{
	team->setGoStrats(true);
	Messages::sayMsg("Les \003" + team->getTag() + "\001 sont prêts",Api::engine->IndexOfEdict(pEntity));
}

// 
// The plugin is a static singleton that is exported as an interface
//
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSSMatch, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, *CSSMatch::getInstance());

SayHook g_sayHook;
SayTeamHook g_sayTeamHook;

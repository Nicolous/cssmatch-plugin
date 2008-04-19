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

#include "CSSMatch.h"

// Protection contre l'inclusion mutuelle (pardon)
#ifndef __GESTIONMENUS_H__
#include "../GestionMenus/GestionMenus.h"
#endif

using std::string;
using std::ostringstream;
using std::out_of_range;

// 
// The plugin is a static singleton that is exported as an interface
//
CSSMatch g_CSSMatchPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSSMatch, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_CSSMatchPlugin );

// Fonctions permettant de retarder l'affichage d'un message via un timer
void timerSayMessage(const string & message)
{
	Messages::sayMsg(message);
}

void timersayAMX(const string & message)
{
	Messages::sayAMX(message,5);
}

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor
//---------------------------------------------------------------------------------
CSSMatch::CSSMatch()
{
	m_iClientCommandIndex = 0;

	timers = GestionTimers(10);
}

CSSMatch::~CSSMatch()
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool CSSMatch::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	Msg("CSSMatch : Chargement des interfaces...\n");
	// get the interfaces we want to use
	if(!API::initialisation(interfaceFactory,gameServerFactory))
	{
		Msg("CSSMatch : Impossible d'initialiser toutes les interfaces !\n");
		return false; // we require all these interface to function
	}

	InitCVars( interfaceFactory ); // register any cvars we have defined
	MathLib_Init( 2.2f, 2.2f, 0.0f, 2.0f );

	// Recherche des ConVars créées par VALVE qui vont nous servir
	ConVars::initialise();

	// Initialisation des menus par défaut (cssmatch_advanced peut modifier les menus)
	GestionMenus::getMenus()->initialiseMenus();

	Msg("CSSMatch : Chargement complet !\n");

	return true;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void CSSMatch::Unload( void )
{
	API::gameeventmanager->RemoveListener( this ); // make sure we are unloaded from the event system

	// Destruction de l'instance unique du décompte
	Decompte::killDecompte();
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------
void CSSMatch::Pause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void CSSMatch::UnPause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *CSSMatch::GetPluginDescription( void )
{
	return CSSMATCH_VERSION;
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CSSMatch::LevelInit( char const *pMapName )
{
	// Récupération des évènements qui nous intéressent
	if (!API::gameeventmanager->AddListener( this, "round_start", true ) ||
		!API::gameeventmanager->AddListener( this, "round_end", true ) ||
		!API::gameeventmanager->AddListener( this, "player_disconnect", true ) ||
		!API::gameeventmanager->AddListener( this, "player_say", true ) ||
		!API::gameeventmanager->AddListener( this, "player_hurt", true ) ||
		!API::gameeventmanager->AddListener( this, "player_death", true ) ||
		!API::gameeventmanager->AddListener( this, "item_pickup", true ) ||
		!API::gameeventmanager->AddListener( this, "bomb_beginplant", true )
		)
	{
		API::engine->LogPrint("CSSMatch : Un AddListener retourne faux !");
	}

	// Effacement des timers en attente
	timers.purge();

	// Si le changement de map a été fait pendant un match on stoppe le match
	if (match.getPhase()!=PHASE_OFF)
		match.finTempsMortFinMatch();

	// Récupération de la liste des steamID arbitre
	adminlist.parseAdminlist("cfg/cssmatch/adminlist.txt");
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void CSSMatch::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
	// Mise à jour de la variable maxplayers
	API::maxplayers = clientMax;

	// Mise à jour de la taille de la liste des joueurs
	// Le resize n'est pas risqué, les index concernés n'auraient jamais été pris par un joueur
	GestionJoueurs::getPlayerList()->setTaillePlayerList(clientMax+1);
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void CSSMatch::GameFrame( bool simulating )
{
	// Alimentation temporel du décompte ;)
	Decompte::getDecompte()->getMoteur()->execute();

	// Exécution des timers en attente d'exécution
	timers.execute();
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void CSSMatch::LevelShutdown( void ) // !!!!this can get called multiple times per map change
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void CSSMatch::ClientActive( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void CSSMatch::ClientDisconnect( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void CSSMatch::ClientPutInServer( edict_t *pEntity, char const *playername )
{
	// Validation de l'entité
	if (!API::isValidePEntity(pEntity))
		return;
	
	// Quel est l'index du joueur qui vient de se connecter ?
	int indexJoueur = API::engine->IndexOfEdict(pEntity);

	// Vérification de la validité de l'index obtenu
	if (!API::isValideIndex(indexJoueur))
		return;

	// On s'assure que le joueur entrant sur le serveur n'utilisait pas un menu du plugin sur l'éventuelle map précédente	
	GestionMenus::getMenus()->quitteMenu(indexJoueur);

	// Cet évènement ne nous intéresse pas en dehors d'un match
	if (match.getPhase()==PHASE_OFF)
		return;

	Messages::sayMsg("\003" + string(playername) + "\001 rejoint la partie !",indexJoueur);
	Messages::tellAMX(indexJoueur,"Ce serveur hÃ©berge actuellement un match !\n\nSi tu n'en fais pas parti, quittes le serveur STP",0);
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CSSMatch::SetCommandClient( int index )
{
	m_iClientCommandIndex = index;
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CSSMatch::ClientSettingsChanged( edict_t *pEdict )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT CSSMatch::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
	// On vérifie que l'entité est valide
	if (!API::isValidePEntity(pEntity)) 
		return PLUGIN_CONTINUE;

	// Quel est l'index du joueur qui vient de se connecter ?
	int indexJoueur = API::engine->IndexOfEdict(pEntity);
	// On vérifie la cohérence de l'index trouvé
	if (!API::isValideIndex(indexJoueur))
		return PLUGIN_CONTINUE;

	// On met à jour l'instance Joueur situé à cet index dans la liste de joueurs
	GestionJoueurs::getPlayerList()->connexion(indexJoueur);

	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT CSSMatch::ClientCommand( edict_t *pEntity )
{
	// On vérifie que l'entité est valide
	if (!API::isValidePEntity(pEntity)) 
		return PLUGIN_CONTINUE;

	// On récupère le nom de la commande exécutée
	const char *pcmd = API::engine->Cmd_Argv(0);
	// On vérifie la validité du pointeur retourné par Cmd_Argv
	if (!pcmd)
		return PLUGIN_CONTINUE;

	// On récupère l'index de l'entité
	int indexJoueur = API::engine->IndexOfEdict(pEntity);
	// On vérifie la cohérence de l'index trouvé
	if (!API::isValideIndex(indexJoueur))
		return PLUGIN_CONTINUE;

	// On recupère le steamID du joueur en s'assurant qu'il est valide
	const char * steamIDvalve = API::engine->GetPlayerNetworkIDString(pEntity);
	if (!steamIDvalve)
		return PLUGIN_CONTINUE;

	// On récupère les arguments de la commande (peut être NULL)
	const char * args = API::engine->Cmd_Args();

	// Nous préférerons travailler avec des std::string :-)
	string commande = pcmd;
	string steamID = steamIDvalve;
	string arguments = args ? args : "";

	// si nous sommes en match nous loguons toutes les commandes passant dans la console des joueurs
	if (match.getPhase() != PHASE_OFF)
	{
		try
		{
			ostringstream log;
			log << "CSSMatch : " << API::getPlayerName(pEntity) << " <id=" << API::getUseridFromPEntity(pEntity) << "> <steamID=\"" << steamID << "\"> exÃ©cute la commande " << commande << " " + arguments << "\n";
			API::engine->LogPrint(log.str().c_str());
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
		}
	}

	// ************
	// Menu arbitre
	// ************
	if (commande == "cssmatch")
	{
		if (adminlist.estAdmin(steamID))
			GestionMenus::getMenus()->afficheMenu(match.getPhase(),indexJoueur);
		else
		{
			Messages::sayTell(indexJoueur,"Tu n'es pas arbitre !");
			string log = "CSSMatch : La steamID " + string(steamIDvalve) + " n'est pas arbitre\n";
			API::engine->LogPrint(log.c_str());
			adminlist.logAdminlist();
		}
		return PLUGIN_STOP;
	}
	
	if (commande == "menuselect")
	{
		// Récupération et validation du premier paramètre 
		const char * parametre = API::engine->Cmd_Argv(1);
		if (!parametre)
			return PLUGIN_CONTINUE;

		// Récupération du choix du joueur dans le menu et du menu utilisé par le joueur
		int choix = atoi(parametre);
		GestionMenus::getMenus()->actionMenu(indexJoueur,choix);
	}

	if (commande == "cssm_dev")
	{
		if (adminlist.estAdmin(steamID))
		{
			if (match.getPhase() == PHASE_CUTROUND)
			{
				match.finCutRound(TEAM_T);
				return PLUGIN_STOP;
			}
			else
				return PLUGIN_CONTINUE;
		}
	}

	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT CSSMatch::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when an event is fired
//---------------------------------------------------------------------------------
void CSSMatch::FireGameEvent( IGameEvent * event )
{
	// Récupération du nom de l'évènement
	const char * name = event->GetName();

	// Vérification du pointeur retourné par GetName
	if (!name)
		return;

	// Vive les std::string :-)
	string nom = name;

	/* Les évènements sont classés dans l'ordre de leur fréquence de déclenchement */

	// ******************************
	//		Un joueur est blessé
	// ******************************
	if (nom == "player_hurt")
	{
		// En StratsTime nous n'autorisons pas la perte de vie
		if (match.getPhase() == PHASE_STRATS)
		{
			edict_t * pEntity;
			try
			{
				pEntity = API::getEntityFromID(event->GetInt("userid"));
			}
			catch(const out_of_range & e)
			{
				API::debug(e.what());
				return;
			}
			API::setHealth(pEntity,100);
		}
		return;
	}

	// **********************************
	//		Un joueur prend un objet
	// **********************************
	if (nom == "item_pickup")
	{
		CodePhase phaseMatch = match.getPhase();

		// En CutRound nous n'acceptons aucune autre arme que le couteau
		if (phaseMatch == PHASE_PASSAGE_CUTROUND || phaseMatch == PHASE_CUTROUND)
		{
			// On récupère le nom de l'arme
			const char * item = event->GetString("item");
			if (!item)
				return;

			// Si l'arme prise par le joueur n'est pas le couteau
			if (string(item) != "knife")
			{
				try
				{
					edict_t * pEntity = API::getEntityFromID(event->GetInt("userid"));
					API::sexec(pEntity,"use weapon_knife\n");
					// On détruit l'arme peut importe son slot
					API::detruitArme(pEntity,WEAPON_SLOT1);
					API::detruitArme(pEntity,WEAPON_SLOT2);
					API::detruitArme(pEntity,WEAPON_SLOT4);
				}
				catch(const out_of_range & e)
				{
					API::debug(e.what());
				}
			}
		}
		return;
	}

	// ****************************************************
	//		Un joueur a écrit un message dans le TCHAT
	// ****************************************************
	if (nom == "player_say")
	{
		// Récupération du message
		const char * messageValve = event->GetString("text");
		if (!messageValve)
			return;
		string message = messageValve;

		// Demande d'affichage des scores
		if (message == "!scores")
		{
			match.afficheScores();
			return;
		}

		// Edition du pseudo de le camp actuellement terroriste
		if (message.substr(0,7) == "!teamt ")
		{
			edict_t * pEntity;
			try
			{
				pEntity = API::getEntityFromID(event->GetInt("userid"));
			}
			catch(const out_of_range & e)
			{
				API::debug(e.what());
				return;
			}
			const char * steamID = API::engine->GetPlayerNetworkIDString(pEntity);
			if (!steamID)
				return;

			// Il faut que le joueur soit arbitre
			if (adminlist.estAdmin(steamID))
			{
				// Récupération du pseudo après la commande !teamt
				string nouvTag = message.substr(7,message.size());
				int mancheCourante = match.getMancheCourante();
				if (match.getPhase() == PHASE_OFF)
				{
					Messages::sayMsg("Aucun match n'est en cours !");
					return;
				}
				// Détermination de le camp actuellement terroriste et enregistrement du tag
				match.getTeam(TEAM_T)->setTag(nouvTag);
				Messages::sayMsg("Le nouveau tag de la team terroriste est " + nouvTag);
				match.formateHostname();
			}
			return;
		}

		// Edition du pseudo de le camp actuellement anti-terroriste
		if (message.substr(0,8) == "!teamct ")
		{
			edict_t * pEntity;
			try
			{
				pEntity = API::getEntityFromID(event->GetInt("userid"));
			}
			catch(const out_of_range & e)
			{
				API::debug(e.what());
				return;
			}
			const char * steamID = API::engine->GetPlayerNetworkIDString(pEntity);
			if (!steamID)
				return;

			// Il faut que le joueur soit arbitre
			if (adminlist.estAdmin(steamID))
			{
				// Récupération du pseudo après la commande !teamt
				string nouvTag = message.substr(8,message.size());
				int mancheCourante = match.getMancheCourante();
				if (match.getPhase() == PHASE_OFF)
				{
					Messages::sayMsg("Il n'y a aucun match en cours !");
					return;
				}
				// Détermination de le camp actuellement terroriste et enregistrement du tag
				match.getTeam(TEAM_CT)->setTag(nouvTag);
				Messages::sayMsg("Le nouveau tag de la team anti-terroriste est " + nouvTag);
				match.formateHostname();
			}
			return;
		}

		// Une team se déclare prête durant le StratsTime
		if (message == "!go" || message == "ready")
		{
			// Si nous ne sommes pas en StratsTime on ne fait rien
			if (match.getPhase() != PHASE_STRATS)
			{
				Messages::sayMsg("Cette fonctionnalitÃ© n'est disponible qu'en Strats Time");
				return;
			}

			// Il nous faut l'adresse de l'entité pour déterminer à quelle team appartient le joueur
			edict_t * pEntity;
			try
			{
				pEntity = API::getEntityFromID(event->GetInt("userid"));
			}
			catch(const out_of_range & e)
			{
				API::debug(e.what());
				return;
			}
			int codeTeam = API::getPEntityTeam(pEntity);

			// Mise à jour de la team correspondante
			int manche = match.getMancheCourante();
			Team * team1 = match.getTeam1();
			Team * team2 = match.getTeam2();
			bool goT = false;
			bool goCT = false;
			if (codeTeam == TEAM_T)
			{
				if (manche == 1)
				{
					if (team1->getGoStrats())
						Messages::sayMsg("Les \003" + team1->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",API::engine->IndexOfEdict(pEntity));
					else
					{
						team1->setGoStrats(true);
						Messages::sayMsg("Les \003" + team1->getTag() + "\001 sont prÃªts",API::engine->IndexOfEdict(pEntity));
						goT = true;
						goCT = team2->getGoStrats();
					}
				}
				else
				{
					if (team2->getGoStrats())
						Messages::sayMsg("Les \003" + team2->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",API::engine->IndexOfEdict(pEntity));
					else
					{
						team2->setGoStrats(true);
						Messages::sayMsg("Les \003" + team2->getTag() + "\001 sont prÃªts",API::engine->IndexOfEdict(pEntity));
						goT = true;
						goCT = team1->getGoStrats();
					}
				}
			}
			else if (codeTeam == TEAM_CT)
			{
				if (manche == 1)
				{
					if (team2->getGoStrats())
						Messages::sayMsg("Les \003" + team2->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",API::engine->IndexOfEdict(pEntity));
					else
					{
						team2->setGoStrats(true);
						Messages::sayMsg("Les \003" + team2->getTag() + "\001 sont prÃªts",API::engine->IndexOfEdict(pEntity));
						goCT = true;
						goT = team1->getGoStrats();
					}
				}
				else
				{
					if (team1->getGoStrats())
						Messages::sayMsg("Les \003" + team1->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",API::engine->IndexOfEdict(pEntity));
					else
					{
						team1->setGoStrats(true);
						Messages::sayMsg("Les \003" + team1->getTag() + "\001 sont prÃªts",API::engine->IndexOfEdict(pEntity));
						goCT = true;
						goT = team2->getGoStrats();
					}
				}
			}

			// Si toutes les teams ont dit "!go", on lance la suite du match
			if (goT && goCT)
			{
				Messages::sayMsg("Toutes les teams sont prÃªtes !");
				Decompte::getDecompte()->finDecompte();
				match.finStratsTime();
			}
			return;
		}
		return;
	}

	// *************************
	//		Un joueur meurt
	// *************************
	if (nom == "player_death")
	{
		// On met à jour les scores des joueurs concernés
		if (match.getPhase() == PHASE_MANCHE)
		{
			// La victime prend un death de plus
			int indexVictime = API::getIndexFromUserId(event->GetInt("userid"));
			if (API::isValideIndex(indexVictime))
				GestionJoueurs::getPlayerList()->getJoueur(indexVictime)->incDeaths();

			// L'attaquant prend un kill de plus (si ce n'est pas la victime elle-même)
			int indexAttaquant = API::getIndexFromUserId(event->GetInt("attacker"));
			if (API::isValideIndex(indexAttaquant))
			{
				if (indexVictime!=indexAttaquant && indexAttaquant!=0)
					GestionJoueurs::getPlayerList()->getJoueur(indexAttaquant)->incKills();
				else
					GestionJoueurs::getPlayerList()->getJoueur(indexAttaquant)->decKills();
			}
		}
		return;
	}

	// *********************************
	//		Un joueur se déconnecte
	// *********************************
	// On utilise pas la méthode ClientDisconnect pour pouvoir fournir le motif de la déconnexion
	if (nom == "player_disconnect")
	{
		// Cet évènement ne nous intéresse pas en dehors d'un match
		if (match.getPhase()==PHASE_OFF)
			return;

		// Nous avons besoin de connaître l'entité correspond au joueur pour trouver son index
		edict_t * pEntity;
		try
		{
			pEntity = API::getEntityFromID(event->GetInt("userid"));
		}
		catch(const out_of_range & e)
		{
			API::debug(e.what());
			return;
		}

		// Quel est l'index du joueur qui vient de se déconnecter ?
		int indexJoueur = API::engine->IndexOfEdict(pEntity);

		// Vérification de la validité de l'index obtenu
		if (!API::isValideIndex(indexJoueur))
			return;

		// Récupération du pseudo du joueur
		const char * pseudo = event->GetString("name");
		if (!pseudo)
			return;
		//Récupération du motif de la déconnexion du joueur
		const char * motif = event->GetString("reason");
		if (!motif)
			return;

		Messages::sayMsg("\003" + string(pseudo) + "\001 quitte le match ! (motif : " + motif + ")",indexJoueur);

		return;
	}

	// ***************************
	//		Un round commence
	// ***************************
	if (nom == "round_start")
	{
		// Exécution du code correspondant à la phase du match en cours
		switch(match.getPhase())
		{
		case PHASE_PASSAGE_CUTROUND:
			// Planification des 2 restarts restant
			switch(match.incRoundCourant())
			{
			case 1:
				API::serveurExecute("mp_restartgame 1\n");
				break;
			case 2:
				API::serveurExecute("mp_restartgame 2\n");
				break;
			default:
				match.setPhase(PHASE_CUTROUND);
			}
			break;
		case PHASE_PASSAGE_MANCHE:
			switch(match.incRoundCourant())
			{
			case 1:
				API::serveurExecute("mp_restartgame 1\n");
				break;
			case 2:
				API::serveurExecute("mp_restartgame 2\n");
				break;
			default:
				match.setPhase(PHASE_MANCHE);
				match.setRoundCourant(1);
			}
		}

		ostringstream message;
		switch(match.getPhase()) // la phase a peut-être changée !
		{
		case PHASE_CUTROUND:
			API::sexecGroupe(TEAM_ALL,"status\n");
			Messages::sayMsg("\004CutRound ! GO ! GO ! GO !");
			break;
		case PHASE_STRATS:
			API::sexecGroupe(TEAM_ALL,"status\n");
			Messages::sayMsg("\004Strats Time !");
			break;
		case PHASE_MANCHE:
			API::sexecGroupe(TEAM_ALL,"status\n");

			Team * team1 = match.getTeam1();
			Team * team2 = match.getTeam2();
			const string * tagTeam1 = &team1->getTag();
			const string * tagTeam2 = &team2->getTag();
			int scoreTeam1 = team1->getScore();
			int scoreTeam2 = team2->getScore();
			int roundCourant = match.getRoundCourant();

			// Envoie d'un message AMX
			message.str("");
			message << "Round " << roundCourant << "\n" << *tagTeam1 << " : " << scoreTeam1 \
				<<  "\n" << *tagTeam2 << " : " << scoreTeam2;
			string msg = message.str();
			// Nous retardons l'affichage AMX car sinon la fenêtre est à peine visible
			timers.addTimer(Timer(0.5f,timersayAMX,msg));

			// Envoie d'un message dans la console de tous les joueurs
			ostringstream echo;
			echo << "echo Round " << roundCourant << "\necho " << *tagTeam1 << " : " << scoreTeam1 \
				<<  "\necho " << *tagTeam2 << " : " << scoreTeam2 << "\n";
			API::cexecGroupe(TEAM_ALL,echo.str());

			// Sauvegarde des stats de tous les joueurs pour le round
			GestionJoueurs::getPlayerList()->sauvStatsRound();
		}

		return;
	}

	// *****************************
	//		Un round se termine
	// *****************************
	if (nom == "round_end")
	{
		// On ne peut pas effectuer cette allocation dans le switch
		CodeTeam gagnantRound = (CodeTeam)event->GetInt("winner");

		// Exécution du code correspondant à la phase du match en cours
		switch(match.getPhase())
		{
		case PHASE_CUTROUND:
			match.finCutRound(gagnantRound);
			break;
		case PHASE_MANCHE:
			try
			{
				match.getTeam((CodeTeam)event->GetInt("winner"))->incScore();
			}
			catch(const out_of_range & e) 
			{
				API::debug(e.what());
			}
			// Si le numéro du prochain round est plus grand que le nombre de round par manche, la manche est terminée
			if (match.incRoundCourant()>ConVars::cssmatch_rounds.GetInt())
				match.finManche();
		}

		return;
	}

	// ***********************************
	//		Un joueur amorce la bombe
	// ***********************************
	if (nom == "bomb_beginplant")
	{
		CodePhase phaseMatch = match.getPhase();

		// En CutRound il est possible que la pose du C4 soit interdite
		if (phaseMatch == PHASE_CUTROUND)
		{
			if (!ConVars::cssmatch_cutround_allows_c4.GetBool())
				return;
			try
			{
				edict_t * pEntity = API::getEntityFromID(event->GetInt("userid"));
				API::sexec(pEntity,"use weapon_knife");
				Messages::sayTell(API::engine->IndexOfEdict(pEntity),"La pose du C4 pendant le round au couteau est interdite !");
			}
			catch(const out_of_range & e)
			{
				API::debug(e.what());
				return;
			}
		}
		else if (phaseMatch == PHASE_STRATS)
		{
			try
			{
				edict_t * pEntity = API::getEntityFromID(event->GetInt("userid"));
				API::sexec(pEntity,"use weapon_knife");
				Messages::sayTell(API::engine->IndexOfEdict(pEntity),"La pose du C4 pendant le Strats Time est interdite !");
			}
			catch(const out_of_range & e)
			{
				API::debug(e.what());
				return;
			}
		}
		return;
	}
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

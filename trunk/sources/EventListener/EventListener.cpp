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

#include "EventListener.h"
#include "../CSSMatch/CSSMatch.h"
#include "../Hooks/SayHook/SayHook.h"

using std::string;
using std::ostringstream;
using std::istringstream;
using std::exception;

void EventListener::initialise()
{
	bool tousValides =	Api::gameeventmanager->AddListener(this, "round_start", true) &&
						Api::gameeventmanager->AddListener(this, "round_end", true)	&&
						Api::gameeventmanager->AddListener(this, "player_disconnect", true) &&
						Api::gameeventmanager->AddListener(this, "player_say", true) &&
						Api::gameeventmanager->AddListener(this, "player_changename", true) &&
						Api::gameeventmanager->AddListener(this, "player_team", true) &&
						//Api::gameeventmanager->AddListener(this, "player_hurt", true) &&
						Api::gameeventmanager->AddListener(this, "player_death", true) &&
						Api::gameeventmanager->AddListener(this, "item_pickup", true) &&
						Api::gameeventmanager->AddListener(this, "bomb_beginplant", true) &&
						Api::gameeventmanager->AddListener(this, "player_spawn", true);
	if (! tousValides)
		Api::debug("Un AddListener retourne faux !");
}

/*void EventListener::player_hurt(IGameEvent * event)
{
	// En StratsTime nous n'autorisons pas la perte de vie
	if (CSSMatch::getInstance()->getMatch()->getPhase() == PHASE_STRATS)
	{
		try
		{
			edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
			Api::setHealth(pEntity,100);
		}
		catch(const CSSMatchApiException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
	}
} */

void EventListener::item_pickup(IGameEvent * event)
{
	CodePhase phaseMatch = CSSMatch::getInstance()->getMatch()->getPhase();

	// En CutRound nous n'acceptons aucune autre arme que le couteau
	if (phaseMatch == PHASE_PASSAGE_CUTROUND || phaseMatch == PHASE_CUTROUND)
	{
		// On récupère le nom de l'arme
		const char * item = event->GetString("item");
		if (item != NULL)
		{
			// Si l'arme prise par le joueur n'est pas le couteau
			if (string(item) != "knife")
			{
				try
				{
					edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
					Api::sexec(pEntity,"use weapon_knife\n");
					// On détruit l'arme peu importe son slot
					Api::detruitArme(pEntity,WEAPON_SLOT1);
					Api::detruitArme(pEntity,WEAPON_SLOT2);
					Api::detruitArme(pEntity,WEAPON_SLOT4);
				}
				catch(const CSSMatchApiException & e)
				{
					Api::reporteException(e,__FILE__,__LINE__);
				}
			}
		}
	}
}

void EventListener::player_spawn(IGameEvent * event)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	CodePhase phaseMatch = match->getPhase();

	switch(phaseMatch)
	{
	case PHASE_PASSAGE_CUTROUND:
	case PHASE_CUTROUND:
		try
		{
			// Pendant le CutRound, les achats sont interdits
			edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
			CSSMatch::getInstance()->propCash.getPlayerProp<int>(pEntity) = 0;
		}
		catch(const CSSMatchApiException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
		catch(const CSSMatchPlayerPropException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
		break;
	case PHASE_STRATS:
		try
		{
			// Pendant les strats, on ne souhaite pas les joueurs puissent être blessés
			edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
			CSSMatch::getInstance()->propLifeState.getPlayerProp<int>(pEntity) = 0;
		}
		catch(const CSSMatchApiException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
		catch(const CSSMatchPlayerPropException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
		break;
	}
}

void EventListener::player_say(IGameEvent * event)
{
	Match * match = CSSMatch::getInstance()->getMatch();

	// Récupération du message
	const char * messageValve = event->GetString("text");
	int userid = event->GetInt("userid");
	if (messageValve != NULL)
	{
		istringstream message(messageValve);
		string nomCommande;

		message >> nomCommande;

		try
		{
			// Demande d'affichage des scores
			if (nomCommande == "!score" || nomCommande == "!scores")
				match->afficheScores();

			// Une team se déclare prête durant le StratsTime
			else if (nomCommande == "!go" || nomCommande == "ready")
			{
				// Si nous ne sommes pas en StratsTime on ne fait rien
				if (match->getPhase() == PHASE_STRATS)
				{
					// Il nous faut l'adresse de l'entité pour déterminer à quelle team appartient le joueur
					edict_t * pEntity = Api::getPEntityFromID(userid);
					int codeTeam = Api::getPEntityTeam(pEntity);

					// Mise à jour de la team correspondante
					int manche = match->getMancheCourante();
					Team * team1 = match->getTeam1();
					Team * team2 = match->getTeam2();
					bool goT = false;
					bool goCT = false;
					if (codeTeam == TEAM_T)
					{
						//if (manche == 1)
						if (manche%2 == 1)
						{
							if (team1->getGoStrats())
								Messages::sayMsg("Les \003" + team1->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",Api::engine->IndexOfEdict(pEntity));
							else
							{
								CSSMatch::getInstance()->goTeam(team1,pEntity);
								goT = true;
								goCT = team2->getGoStrats();
							}
						}
						else
						{
							if (team2->getGoStrats())
								Messages::sayMsg("Les \003" + team2->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",Api::engine->IndexOfEdict(pEntity));
							else
							{
								CSSMatch::getInstance()->goTeam(team2,pEntity);
								goT = true;
								goCT = team1->getGoStrats();
							}
						}
					}
					else if (codeTeam == TEAM_CT)
					{
						//if (manche == 1)
						if (manche%2 == 1)
						{
							if (team2->getGoStrats())
								Messages::sayMsg("Les \003" + team2->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",Api::engine->IndexOfEdict(pEntity));
							else
							{
								CSSMatch::getInstance()->goTeam(team2,pEntity);
								goCT = true;
								goT = team1->getGoStrats();
							}
						}
						else
						{
							if (team1->getGoStrats())
								Messages::sayMsg("Les \003" + team1->getTag() + "\001 sont dÃ©jÃ  dÃ©clarÃ©s prÃªts",Api::engine->IndexOfEdict(pEntity));
							else
							{
								CSSMatch::getInstance()->goTeam(team1,pEntity);
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
						//match->finStratsTime();
					}
				}
				else
					Messages::sayMsg("Cette fonctionnalitÃ© n'est disponible qu'en Strats Time");
			}

			// Commandes normalement hookées -> le hook n'a pu être effectués
			//	Nous reproduisons l'effet du hook, mais la commande ne sera pas cachée
			else if (nomCommande == "cssmatch")
			{
				SayHook::cmdCssmatch(Api::getIndexFromUserId(userid),message);
			}
			else if (nomCommande == "!teamt")
			{
				SayHook::cmdTeamt(Api::getIndexFromUserId(userid),message);
			}
			else if (nomCommande == "!teamct")
			{
				SayHook::cmdTeamct(Api::getIndexFromUserId(userid),message);
			}
		}
		catch(const CSSMatchApiException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
	}
}

void EventListener::player_death(IGameEvent * event)
{
	// On met à jour les scores des joueurs concernés
	if (CSSMatch::getInstance()->getMatch()->getPhase() == PHASE_MANCHE)
	{
		// La victime prend un death de plus
		int indexVictime = Api::getIndexFromUserId(event->GetInt("userid"));
		if (Api::isValideIndex(indexVictime))
			GestionJoueurs::getPlayerList()->getJoueur(indexVictime)->incDeaths();

		// L'attaquant prend un kill de plus (si ce n'est pas la victime elle-même)
		int indexAttaquant = Api::getIndexFromUserId(event->GetInt("attacker"));
		if (Api::isValideIndex(indexAttaquant))
		{
			if (indexVictime!=indexAttaquant && indexAttaquant!=0)
				GestionJoueurs::getPlayerList()->getJoueur(indexAttaquant)->incKills();
			else
				GestionJoueurs::getPlayerList()->getJoueur(indexAttaquant)->decKills();
		}
	}
}

void EventListener::player_disconnect(IGameEvent * event)
{
	if (CSSMatch::getInstance()->getMatch()->getPhase() != PHASE_OFF)
	{
		// Nous avons besoin de connaître l'entité correspond au joueur pour trouver son index
		try
		{
			edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
			// Quel est l'index du joueur qui vient de se déconnecter ?
			int indexJoueur = Api::engine->IndexOfEdict(pEntity);

			// Vérification de la validité de l'index obtenu
			if (Api::isValideIndex(indexJoueur))
			{
				// Récupération du pseudo du joueur
				const char * pseudo = event->GetString("name");
				//Récupération du motif de la déconnexion du joueur
				const char * motif = event->GetString("reason");
				if (pseudo!=NULL && motif!=NULL)
				{
					Messages::sayMsg("\003" + string(pseudo) + "\001 quitte le match ! (motif : " + motif + ")",indexJoueur);

					// On rappelle le mot de passe à tous les joueurs
					CSSMatch::getInstance()->getTimers()->addTimer(Timer(2.0f,Messages::timerSayMsg,"RAPPEL ! Le mot de passe du serveur est\001 \"" + string(ConVars::cssmatch_password.GetString()) + "\""));
				}
			}
		}
		catch(const CSSMatchApiException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
	}
}

void EventListener::round_start(IGameEvent * event)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	// Exécution du code correspondant à la phase du match en cours
	switch(match->getPhase())
	{
	case PHASE_PASSAGE_CUTROUND:
		// Planification des 2 restarts restant
		switch(match->incRoundCourant())
		{
		case 1:
			Api::serveurExecute("mp_restartgame 1\n");
			break;
		case 2:
			Api::serveurExecute("mp_restartgame 2\n");
			break;
		default:
			match->setPhase(PHASE_CUTROUND);
		}
		break;
	case PHASE_PASSAGE_MANCHE:
		switch(match->incRoundCourant())
		{
		case 1:
			Api::serveurExecute("mp_restartgame 1\n");
			break;
		case 2:
			Api::serveurExecute("mp_restartgame 2\n");
			break;
		default:
			match->setPhase(PHASE_MANCHE);
			match->setRoundCourant(1);
			match->detecteTags(false);
		}
	}

	ostringstream message;
	switch(match->getPhase()) // la phase a peut-être changé !
	{
	case PHASE_CUTROUND:
		Api::sexecGroupe(TEAM_ALL,"status\n");
		Messages::sayMsg("\004CutRound ! GO ! GO ! GO !");
		break;
	case PHASE_STRATS:
		Api::sexecGroupe(TEAM_ALL,"status\n");
		Messages::sayMsg("\004Strats Time !");
		break;
	case PHASE_MANCHE:
		Api::sexecGroupe(TEAM_ALL,"status\n");

		Team * team1 = match->getTeam1();
		Team * team2 = match->getTeam2();
		const string * tagTeam1 = &team1->getTag();
		const string * tagTeam2 = &team2->getTag();
		int scoreTeam1 = team1->getScore();
		int scoreTeam2 = team2->getScore();
		int roundCourant = match->getRoundCourant();
		int mancheCourante = match->getMancheCourante();

		// Envoie d'un popup
		message.str("");
		message << "\nRound " << roundCourant << " / " << ConVars::cssmatch_rounds.GetInt() \
			<< "\n" << *tagTeam1 << " : " << scoreTeam1 <<  "\n" << *tagTeam2 << " : " << scoreTeam2;
		string sMsg(message.str());
		// Retardement de l'affichage du popup car sinon la fenêtre est à peine visible
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(0.5f,Messages::timerSayAMX,sMsg));

		// Envoie d'un message dans la console de tous les joueurs
		Messages::clientPrintMsg(sMsg + "\n");

		// Sauvegarde des stats de tous les joueurs pour le round
		GestionJoueurs::getPlayerList()->sauvStatsRound();
	}
}

void EventListener::round_end(IGameEvent * event)
{
	Match * match = CSSMatch::getInstance()->getMatch();

	CodeTeam gagnantRound = (CodeTeam)event->GetInt("winner");

	// Exécution du code correspondant à la phase du match en cours
	switch(match->getPhase())
	{
	case PHASE_CUTROUND:
		match->finCutRound(gagnantRound);
		break;
	case PHASE_MANCHE:
		try
		{
			match->getTeam(gagnantRound)->incScore();

			// Si le numéro du prochain round est plus grand que le nombre de round par manche, la manche est terminée
			if (match->incRoundCourant()>ConVars::cssmatch_rounds.GetInt())
				match->finManche();
		}
		catch(const CSSMatchMatchException & e) 
		{
			// Cas où le round n'a été gagné par personne
		}
	}
}

void EventListener::player_team(IGameEvent * event)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	CodePhase phase = match->getPhase();
	GestionTimers * timers = cssmatch->getTimers();

	CodeTeam codeTeam = (CodeTeam)event->GetInt("team");
	CodeTeam codeOldTeam = (CodeTeam)event->GetInt("oldteam");

	switch(phase)
	{
	case PHASE_CUTROUND:
	case PHASE_STRATS:
	case PHASE_MANCHE:
		try
		{
			// Si la team que rejoint le joueur est terroriste ou anti-terroriste...
			if (codeTeam>TEAM_SPEC)
			{
				// Si la situation du match change, on redétecte les tags
				Team * team = match->getTeam(codeTeam);
				//Msg("join nbrMembres + 1 = %i\n",team->getNbrMembres()+1);
				if (team->incNbrMembres()-1 < 2)
				{
					Messages::sayMsg("Changement de situation, redÃ©tection des tags...");
					timers->addTimer(Timer(1.0,timerRetag,""));
				}
			}
			// ... ou si le joueur se déconnecte (ou va en spectateur)
			else
			{
				// On décrémente le nombre de joueurs appartenant à la team, 
				// pour que le tag soit redétecté si nécessaire lors des prochaines connexions

				Team * team = match->getTeam(codeOldTeam);
				
				//Msg("left nbrMembres = %i\n",team->getNbrMembres()-1);
				// On en profite pour redétecter les tags si la situation l'implique
				if (team->decNbrMembres() < 2)
				{
					Messages::sayMsg("Changement de situation, redÃ©tection des tags...");
					timers->addTimer(Timer(1.0,timerRetag,""));
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
	}
}

void EventListener::bomb_beginplant(IGameEvent * event)
{
	CodePhase phaseMatch = CSSMatch::getInstance()->getMatch()->getPhase();

	// En CutRound il est possible que la pose du C4 soit interdite
	if (phaseMatch == PHASE_CUTROUND)
	{
		if (! ConVars::cssmatch_cutround_allows_c4.GetBool())
		{
			try
			{
				edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
				Api::sexec(pEntity,"use weapon_knife");
				Messages::sayTell(Api::engine->IndexOfEdict(pEntity),"La pose du C4 pendant le round au couteau est interdite !");
			}
			catch(const CSSMatchApiException & e)
			{
				Api::reporteException(e,__FILE__,__LINE__);
			}
		}
	}
	else if (phaseMatch == PHASE_STRATS)
	{
		try
		{
			edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
			Api::sexec(pEntity,"use weapon_knife");
			Messages::sayTell(Api::engine->IndexOfEdict(pEntity),"La pose du C4 pendant le Strats Time est interdite !");
		}
		catch(const CSSMatchApiException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
	}
}

void EventListener::player_changename(IGameEvent * event)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	CodePhase phaseMatch = match->getPhase();

	/*
		Après le CutRound, s'il y en a eu un,
			si la situation du match est 1 vs N, (1 étant du côté du joueur qui change de pseudo)
				on veut mettre à jour le pseudo du joueur gagnant qui sera inscrit dans le rapport
	*/
	if (phaseMatch > PHASE_CUTROUND && match->getCutRound())
	{
		// Quelle est la team du joueur ?
		try
		{
			edict_t * pEntity = Api::getPEntityFromID(event->GetInt("userid"));
			int nbrMembresTeam = match->getTeam(Api::getPEntityTeam(pEntity))->getNbrMembres();

			// Si le joueur est tout seul et qu'il est le gagnant du CutRound, on met à jour le tag gagnant
			if (nbrMembresTeam == 1 && match->getTagGagnantCutRound() == event->GetString("oldname"))
			{
				match->setTagGagnantCutRound(event->GetString("newname"));
				Messages::sayMsg("Changement de tag, redÃ©tection...");
				CSSMatch::getInstance()->getTimers()->addTimer(Timer(1.0,timerRetag,""));
			}
		}
		catch(const CSSMatchApiException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
		catch(const CSSMatchMatchException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
		}
	}
}

void EventListener::FireGameEvent(IGameEvent * event)
{
	try
	{
		// Récupération du nom de l'évènement
		const char * name = event->GetName();

		// Vérification du pointeur retourné par GetName
		if (name != NULL)
		{
			string nom(name);

			/* Les évènements sont classés dans l'ordre de leur fréquence de déclenchement */
/*			if (nom == "player_hurt")
				player_hurt(event);

			else */if (nom == "item_pickup")
				item_pickup(event);

			else if (nom == "player_spawn")
				player_spawn(event);

			else if (nom == "player_say")
				player_say(event);

			else if (nom == "player_death")
				player_death(event);

			// On n'utilise pas la méthode ClientDisconnect pour pouvoir fournir le motif de la déconnexion
			else if (nom == "player_disconnect")
				player_disconnect(event);

			else if (nom == "round_start")
				round_start(event);

			else if (nom == "round_end")
				round_end(event);

			else if (nom == "player_team")
				player_team(event);

			else if (nom == "bomb_beginplant")
				bomb_beginplant(event);

			else if (nom == "player_changename")
				player_changename(event);
		}
	}
	catch (const exception & e)
	{
		Messages::sayColor("Une erreur est survenue, veuillez en avertir l'auteur sur " CSSMATCH_SITE);
		Api::reporteException(e,__FILE__,__LINE__);
		//throw;
	}
}

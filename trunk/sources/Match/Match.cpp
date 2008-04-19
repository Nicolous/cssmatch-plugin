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

#include "Match.h"

// Protection contre l'inclusion mutuelle (pardon)
#ifndef __CSSMATCH_H__
#include "../CSSMatch/CSSMatch.h"
#endif
// pour faciliter les fonctions interfaçant les timers avec le match

// Protection contre l'inclusion mutuelle (pardon)
#ifndef __GESTIONMENUS_H__
#include "../GestionMenus/GestionMenus.h"
#endif

// Protection contre l'inclusion mutuelle (pardon)
#ifndef __RAPPORT_H__
#include "../Rapport/Rapport.h"
#endif

using std::string;
using std::logic_error;
using std::out_of_range;
using std::ostringstream;

// Fonction interfaçant diverses fonctionnalitées avec les timers
void timerFinMatch(const string & parametre)
{
	Match * match = g_CSSMatchPlugin.getMatch();
	if (match->getPhase()==PHASE_FIN)
		match->finTempsMortFinMatch();
}

void timerLanceStrats(const string & parametre)
{
	Match * match = g_CSSMatchPlugin.getMatch();
	if (match->getPhase()==PHASE_PASSAGE_STRATS)
		match->lanceStratsTime();
}

void timerLanceManche(const string & parametre)
{
	Match * match = g_CSSMatchPlugin.getMatch();
	match->lanceManche();
}

void timerFinStrats(const string & parametre)
{
	Match * match = g_CSSMatchPlugin.getMatch();
	if (match->getPhase()==PHASE_STRATS)
		match->finStratsTime();
}

void timerChronoStrats(const string & parametre)
{
	Decompte::getDecompte()->lanceDecompte(ConVars::cssmatch_strats_time.GetInt()*60,timerFinStrats);
}

void timerSayMsg(const string & message)
{
	Messages::sayMsg(message);
}

void timerSayAMX(const string & message)
{
	Messages::sayAMX(message,5);
}

void timerCexecT(const string & commande)
{
	API::cexecGroupe(TEAM_T,commande);
}

void timerCexecCT(const string & commande)
{
	API::cexecGroupe(TEAM_CT,commande);
}

void timerSwapALL(const string & parametre)
{
	API::swapGroupe(TEAM_ALL);
}

void timerVerifAllTalk(const string & parametre)
{
	if (g_CSSMatchPlugin.getMatch()->getPhase()!=PHASE_OFF)
	{
		if (ConVars::sv_alltalk->GetBool())
			Messages::centerMsg("Le AllTalk est actif");
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(1.5f,timerVerifAllTalk));
	}
}

void timerVerifCheats(const string & parametre)
{
	if (g_CSSMatchPlugin.getMatch()->getPhase()!=PHASE_OFF)
	{
		if (ConVars::sv_cheats->GetBool())
			Messages::centerMsg("Sv_Cheats est actif");
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(1.5f,timerVerifCheats));
	}
}

Match::Match()
{
	phase = PHASE_OFF;

	tvManche1 = NULL;
	tvManche2 = NULL;

	mancheCourante = 1;
	roundCourant = 0;

	tagGagnantCutRound = "";

	team1 = Team();
	team2 = Team();

	cutround = true;
}

Match::~Match()
{
	if (tvManche1)
		delete tvManche1;
	if (tvManche2)
		delete tvManche2;	
}

CodePhase Match::getPhase() const
{
	return phase;
}

void Match::setPhase(CodePhase codePhase)
{
	phase = codePhase;
}

EnregistrementTV * Match::getTvManche1() const
{
	return tvManche1;
}

EnregistrementTV * Match::getTvManche2() const
{
	return tvManche2;
}

int Match::getMancheCourante() const
{
	return mancheCourante;
}

int Match::getRoundCourant() const
{
	return roundCourant;
}

void Match::setRoundCourant(int numeroRound)
{
	roundCourant = numeroRound;
}

int Match::incRoundCourant()
{
	return ++roundCourant;
}

const string & Match::getTagGagnantCutRound() const
{
	return tagGagnantCutRound;
}

Team * Match::getTeam(CodeTeam codeTeam)
{
	switch(codeTeam)
	{
	case TEAM_T:
		if (mancheCourante == 1)
			return &team1;
		else
			return &team2;
	case TEAM_CT:
		if (mancheCourante == 1)
			return &team2;
		else
			return &team1;
	default:
		throw out_of_range("getTeam() sur une team spectateur");
	}
}

Team * Match::getTeam1()
{
	return &team1;
}

Team * Match::getTeam2()
{
	return &team2;
}

bool Match::getCutRound() const
{
	return cutround;
}

void Match::setCutRound(bool cutround)
{
	this->cutround = cutround;
}

void Match::reset()
{
	phase = PHASE_OFF;

	if (tvManche1)
		delete tvManche1;
	tvManche1 = NULL;

	if (tvManche2)
		delete tvManche2;
	tvManche2 = NULL;

	mancheCourante = 1;
	roundCourant = 0;

	tagGagnantCutRound = "";
	team1 = Team();
	team2 = Team();
}

void Match::lanceMatch(Configuration * fichier, edict_t * pEntity)
{
	// On stoppe tout chrono ou timer en cours
	g_CSSMatchPlugin.getTimers()->purge(); // A purger en premier pour permettre l'exécution de la fonction associée à la fin du décompte !
	Decompte::getDecompte()->finDecompte();

	// Exécution de la configuration
	try
	{
		fichier->execute();
	}
	catch(const out_of_range & e)
	{
		Messages::sayMsg(e.what());
		return;
	}

	// On commence avec un match tout neuf
	reset();

	// Recherche du tag de chaque team et modification du nom du serveur en conséquence
	detecteTags(false);

	// Mise à jour du mot de passe
	if (ConVars::sv_password)
		ConVars::sv_password->SetValue(ConVars::cssmatch_password.GetString());
	else
		API::debug("Impossible de trouver la variable sv_password !");

	g_CSSMatchPlugin.getTimers()->addTimer(Timer(5.0f,timerSayAMX,"Mot de passe du serveur : \"" + string(ConVars::cssmatch_password.GetString()) + "\""));

	// On signale le lancement d'un match
	try
	{
		Messages::sayMsg("Un match est lancÃ© par \003" + API::getPlayerName(pEntity) + "\001...",API::engine->IndexOfEdict(pEntity));
	}
	catch(const out_of_range & e)
	{
		Messages::sayMsg("Un match est lancÃ©...");
		API::debug(e.what());
	}

	// On lance la vérification du alltalk est d'sv_cheats
	// On passe par des timers car la phase est encore PHASE_OFF ici
	if (ConVars::sv_alltalk)
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(1.0f,timerVerifAllTalk));
	else
		API::debug("Impossible de trouver la variable sv_alltalk !");
	if (ConVars::sv_cheats)
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(1.0f,timerVerifCheats));
	else
		API::debug("Impossible de trouver la variable sv_cheats !");

	// Si la match n'a pas été lancé via le menu on se fit à la valeur de cssmatch_cutround pour savoir si on doit lancer un CutRound
	if (!API::isValidePEntity(pEntity))
		cutround = ConVars::cssmatch_cutround.GetBool();

	// On lance la première phase du match
	if (ConVars::cssmatch_cutround.GetBool() && cutround)
		lanceCutRound();
	else
	{
		if (ConVars::cssmatch_strats_time.GetInt()>0)
		{
			API::serveurExecute("mp_restartgame 1\n");
			lanceStratsTime();
		}
		else
			lanceManche();
	}
}

void Match::finMatch(edict_t * pEntity)
{
	// On stopppe tout enregistrement en cours (le match a pu être stoppé dès la première manche)
	if (tvManche1)
		tvManche1->coupe();
	if (tvManche2)
		tvManche2->coupe();
	
	Messages::sayMsg("\004Fin du match !");

	const string * tagTeam1 = &team1.getTag();
	const string * tagTeam2 = &team2.getTag();
	int scoreTeam1 = team1.getScore();
	int scoreTeam2 = team2.getScore();

	// Envoie d'une annonce AMX
	ostringstream message;
	message << "Fin du match !\n\nScores :\n" << *tagTeam1 << " : " << scoreTeam1 << "\n" << \
		*tagTeam2 << " : " << scoreTeam2;
	Messages::sayAMX(message.str(),6);

	// Envoie de l'annonce dans la console de tous les joueurs
	ostringstream echo;
	echo << "echo Fin du match !\necho Scores :\necho " << *tagTeam1 << " : " << scoreTeam1 << "\n echo " << \
		*tagTeam2 << " : " << scoreTeam2;
	API::cexecGroupe(TEAM_ALL,echo.str());

	if (scoreTeam1 > scoreTeam2)
		Messages::sayMsg("Team gagnante :\003 " + *tagTeam1);
	else if (scoreTeam1 < scoreTeam2)
		Messages::sayMsg("Team gagnante :\003 " + *tagTeam2);
	else
		Messages::sayMsg("Team gagnante :\003 aucune");

	// Mise à jour des scores en fonction des camps
	team1.setScoreCT(team1.getScore()-team1.getScoreT());
	team2.setScoreT(team2.getScore()-team2.getScoreCT());

	Rapport::genereRapport(*this);

	phase = PHASE_FIN;

	// Annonce du temps mort pour la prise de screenshots
	int tempsMort = ConVars::cssmatch_end_manche.GetInt();
	ostringstream annonceTempsMort;
	annonceTempsMort << tempsMort << " secondes de temps mort, prenez vos screenshots !";
	g_CSSMatchPlugin.getTimers()->addTimer(Timer(2.0f,timerSayMsg,annonceTempsMort.str()));

	Decompte::getDecompte()->lanceDecompte(tempsMort + 2,timerFinMatch);
}

void Match::finTempsMortFinMatch()
{
	// On exécute la configuration par défaut
	string configParDefaut = ConVars::cssmatch_default_config.GetString();
	if (API::filesystem->FileExists(("cfg/" + configParDefaut).c_str(),"MOD"))
		API::configExecute(configParDefaut);
	else
	{
		Messages::sayColor("Impossible de trouver le fichier " + configParDefaut + " !");
		Messages::sayColor("Changez de map pour restaurer au maximum la configuration par dÃ©faut");
	}

	// On finalise la fin du match (un nouveau match peut être lancé)
	phase = PHASE_OFF;
}

void Match::lanceCutRound()
{
	roundCourant = 0;

	Messages::sayMsg("3 restarts puis CutRound !");
	phase = PHASE_PASSAGE_CUTROUND;

	// C'est le round_start qui déclenchera les 2 autres restarts afin d'éviter les conflits avec les restarts du jeu ou lancés par un admin
	API::serveurExecute("mp_restartgame 2\n");
}

void Match::finCutRound(CodeTeam codeTeamGagnante)
{
	// Nous n'acceptons la fin du CutRound que si une des 2 teams a gagnée
	if (codeTeamGagnante != TEAM_T && codeTeamGagnante != TEAM_CT)
		return;

	// On met le camp perdante en spectateur, et on affiche le menu de choix de team à le camp gagnante
	if (codeTeamGagnante == TEAM_T)
	{
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(1.0f,timerCexecT,"chooseteam\n"));
		API::specGroupe(TEAM_CT);

		// Mémorisation du tag de la team gagnante
		tagGagnantCutRound = team1.getTag();
	}
	else
	{
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(1.0f,timerCexecCT,"chooseteam\n"));
		API::specGroupe(TEAM_T);

		tagGagnantCutRound = team2.getTag();
	}

	// Affichage du tag de la team gagnante
	Messages::sayMsg("Le round au couteau a Ã©tÃ© remportÃ© par la team \003" + tagGagnantCutRound + "\001 !");

	// Mise en place d'un temps mort pour laisser le temps aux camps de choisir leur nouvelle team
	// et de la redétection des tags
	int tempsAttente = ConVars::cssmatch_end_cutround.GetInt();
	if (tempsAttente > 0)
	{
		ostringstream msg;
		msg << "Les " << tagGagnantCutRound << " ont \003" << tempsAttente << " secondes\001 pour choisir leur camp";
		Messages::sayMsg(msg.str());

		//Decompte::getDecompte()->lanceDecompte(tempsAttente);

		// Préparation de la prochaine phase de match
		if (ConVars::cssmatch_strats_time.GetInt() > 0)
		{
			phase = PHASE_PASSAGE_STRATS;
			Decompte::getDecompte()->lanceDecompte(tempsAttente,timerLanceStrats);
		}
		else
		{
			phase = PHASE_PASSAGE_MANCHE;
			Decompte::getDecompte()->lanceDecompte(tempsAttente,timerLanceManche);
		}
	}
	// S'il n'y en a pas on lance directement la suite
	else
	{
		if (ConVars::cssmatch_strats_time.GetInt() > 0)
			lanceStratsTime();
		else
			lanceManche();
	}
}

void Match::lanceStratsTime()
{
	// La situation a peut-être changée, redétection des tags et mise à jour du nom du serveur
	if (mancheCourante == 1)
		detecteTags(false);

	// On s'assure qu'un précédent !go n'a pas été mémorisé
	team1.setGoStrats(false);
	team2.setGoStrats(false);

	// Si cssmatch_strats_time est à zéro on passe directement au match
	int tempsStrats = ConVars::cssmatch_strats_time.GetInt();
	if (tempsStrats <= 0)
	{
		lanceManche();
		return;
	}
	// sinon on lance le StratsTime

	API::serveurExecute("mp_restartgame 2\n");
	phase = PHASE_STRATS;

	tempsStrats *= 60;
	g_CSSMatchPlugin.getTimers()->addTimer(Timer(3.0f,timerChronoStrats));
}

void Match::finStratsTime()
{
	Messages::sayMsg("Fin du Strats Time !");

	Messages::sayMsg("\004Lancement d'une manche...");

	lanceManche();
}

void Match::lanceManche()
{
	// Le nombre de round a été utilisé précédemment
	roundCourant = 0;

	// Gestion de l'enregistrement sourceTV
	if (ConVars::cssmatch_sourcetv.GetBool())
	{
		// Préparation du nom de l'enregistrement
		time_t date;
		time(&date);
		struct tm * locale = localtime(&date);
		char datebuf[96];
		strftime(datebuf,sizeof(datebuf),"%d-%m-%Y_%Hh%M",locale);

		// Constitution du nom de la TV
		ostringstream nomTV;
		nomTV << "cssmatch_" << datebuf << "_" << API::gpGlobals->mapname.ToCStr() << "_" << "manche_" << mancheCourante;
		// Lancement de l'enregistrement (si possible)
		try
		{
			if (mancheCourante == 1)
			{
				tvManche1 = new EnregistrementTV(nomTV.str());
				tvManche1->enregistre();
			}
			else
			{
				tvManche2 = new EnregistrementTV(nomTV.str());
				tvManche2->enregistre();
			}
		}
		catch(const out_of_range & e)
		{
			Messages::sayColor(e.what());
		}
	}

	Messages::sayMsg("3 restarts puis GO ! GO ! GO !");

	phase = PHASE_PASSAGE_MANCHE;

	// Lancement du match
	API::serveurExecute("mp_restartgame 2\n");
}

void Match::finManche()
{
	// Cas où il reste une manche à jouer
	if (mancheCourante == 1)
	{
		// On stoppe l'éventuel enregistrement en cours
		if (tvManche1)
			tvManche1->coupe();
		
		Messages::sayMsg("\004Fin de la premiÃ¨re manche !");
		const string * tagTeam1 = &team1.getTag();
		const string * tagTeam2 = &team2.getTag();
		int scoreTeam1 = team1.getScore();
		int scoreTeam2 = team2.getScore();

		// Envoie d'une annonce AMX
		ostringstream message;
		message << "Fin de la premiÃ¨re manche !\n\nScores :\n" << *tagTeam1 << " : " << scoreTeam1 << "\n" << \
			*tagTeam2 << " : " << team2.getScore();
		Messages::sayAMX(message.str(),6);

		// Envoie de l'annonce dans la console de tous les joueurs
		ostringstream echo;
		echo << "echo Fin de la premiÃ¨re manche !\necho Scores :\necho " << *tagTeam1 << " : " << team1.getScore() << "\n echo " << \
			*tagTeam2 << " : " << scoreTeam2;
		API::cexecGroupe(TEAM_ALL,echo.str());

		// On prépare la manche suivante
		mancheCourante = 2;
		phase = PHASE_PASSAGE_STRATS;

		// Le swap des joueurs est légèrement décalé, ce qui laisse le temps de prendre des screenshots
		float tempsTransition = ConVars::cssmatch_end_manche.GetFloat();
		Decompte::getDecompte()->lanceDecompte((int)tempsTransition,timerSwapALL);
		ostringstream annonceTempsMort;
		annonceTempsMort << (int)tempsTransition << " secondes de temps mort, prenez vos screenshots !";
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(2.0f,timerSayMsg,annonceTempsMort.str()));

		// On lance la prochaine phase en léger décalage avec le swap
		if (ConVars::cssmatch_strats_time.GetInt() > 0)
			g_CSSMatchPlugin.getTimers()->addTimer(Timer(tempsTransition + 2.0f,timerLanceStrats));
		else
			g_CSSMatchPlugin.getTimers()->addTimer(Timer(tempsTransition + 2.0f,timerLanceManche));

		// On rappel le mot de passe
		g_CSSMatchPlugin.getTimers()->addTimer(Timer(tempsTransition + 10.0f,timerSayAMX,"Mot de passe du serveur : \"" + string(ConVars::cssmatch_password.GetString()) + "\""));

		// On créer le point de restauration des scores des joueurs sur la manche qui vient d'être jouée
		GestionJoueurs::getPlayerList()->sauvStatsManche();

		// On créer le point de restauration des scores des teams sur la manche qui vient d'être jouée
		team1.setScoreT(team1.getScore());
		team2.setScoreCT(team2.getScore());

		return;
	}
	
	// Cas où le match est fini
	finMatch();
}

void Match::detecteTags(bool affiche)
{
	if (affiche)
	{
		string message = "\003" + team1.chercheTag(TEAM_T) + "\001 VS\003 " + team2.chercheTag(TEAM_CT);
		Messages::sayMsg(message);
	}
	else
	{
		team1.chercheTag(TEAM_T);
		team2.chercheTag(TEAM_CT);
	}

	formateHostname();
}

void Match::formateHostname()
{
	if (ConVars::hostname)
	{
		char buf[128];
		V_snprintf(buf,sizeof(buf),ConVars::cssmatch_hostname.GetString(),team1.getTag().c_str(),team2.getTag().c_str());
		ConVars::hostname->SetValue(buf);
	}
	else
		API::debug("Impossible de trouver la variable hostname !");
}

void Match::afficheScores()
{
	ostringstream msgTeam1;
	msgTeam1 << "\003" << team1.getTag() << " :\001 " << team1.getScore();
	ostringstream msgTeam2;
	msgTeam2 << "\003" << team2.getTag() << " :\001 " << team2.getScore();

	Messages::sayMsg("\004Scores :");
	Messages::sayMsg(msgTeam1.str());
	Messages::sayMsg(msgTeam2.str());
}

void Match::restartRound(edict_t * pEntity)
{
	// Restauration des scores du round précédent
	GestionJoueurs::getPlayerList()->restaureStatsRound();

	// Restart du round
	API::serveurExecute("mp_restartgame 1\n");

	try
	{
		Messages::sayMsg("Le round a Ã©tÃ© relancÃ© par \003" + API::getPlayerName(pEntity),API::engine->IndexOfEdict(pEntity));
	}
	catch(const out_of_range & e)
	{
		Messages::sayMsg("Le round a Ã©tÃ© relancÃ© par un arbitre");
		// API::debug(e.what()); // pEntity peut être nul
	}
}

void Match::restartManche(edict_t * pEntity)
{
	// Restauration des scores de la manche précédente
	GestionJoueurs::getPlayerList()->restaureStatsManche();

	// Restauration des scores des teams selon la manche en cours
	if (mancheCourante == 1)
	{
		// Aucune match n'a été jouée avant
		team1.setScore(0);
		team2.setScore(0);
	}
	else
	{
		// Restauration des scores de la manche 1
		team1.setScore(team1.getScoreT());
		team2.setScore(team2.getScoreCT());
	}

	// Restart du round
	API::serveurExecute("mp_restartgame 1\n");

	// Nous reprennons au premier round
	roundCourant = 1;

	try
	{
		Messages::sayMsg("Le manche a Ã©tÃ© relancÃ© par \003" + API::getPlayerName(pEntity),API::engine->IndexOfEdict(pEntity));
	}
	catch(const out_of_range & e)
	{
		Messages::sayMsg("Le manche a Ã©tÃ© relancÃ© par un arbitre");
		// API::debug(e.what()); // pEntity peut être nul
	}
}

void Match::forceGo(edict_t * pEntity)
{
	// Si nous n'en somme qu'au passage au strats on refuse le !go
	if (phase != PHASE_STRATS)
	{
		if (pEntity)
			Messages::sayTell(API::engine->IndexOfEdict(pEntity),"Impossible de forcer le !go, le Strats Time n'a pas encore commencÃ©");
		else
			Msg("CSSMatch : Le Strats Time n'a pas encore commencÃ©");
		return;
	}

	// On actualise proprement le !go des teams
	team1.setGoStrats(true);
	team2.setGoStrats(true);

	// Le !go de l'arbitre déclenchera la suite du match
	try
	{
		Messages::sayMsg("Toutes les teams ont Ã©tÃ© dÃ©clarÃ©es prÃªtes par \003" + API::getPlayerName(pEntity),API::engine->IndexOfEdict(pEntity));
	}
	catch(const out_of_range & e)
	{
		Messages::sayMsg("Toutes les teams ont Ã©tÃ© dÃ©clarÃ©es prÃªtes par un arbitre");
		API::debug(e.what());
	}

	// Arrêt du décompte et lancement de la suite du match
	Decompte::getDecompte()->finDecompte();
	//finStratsTime();
}

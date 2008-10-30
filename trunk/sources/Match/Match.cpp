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

#include "Match.h"

#include "../CSSMatch/CSSMatch.h" // pour faciliter les fonctions interfa�ant les timers avec le match

#include "../GestionMenus/GestionMenus.h"

//#include "../Rapport/RapportTxt/RapportTxt.h"
#include "../Rapport/RapportXml/RapportXml.h"

using std::string;
using std::ostringstream;
using std::vector;

/** Finalise la fin d'un match (fin du temps mort)
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerFinMatch(const string & parametre)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_FIN)
		match->finTempsMortFinMatch();
}

/** Lance les strats
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerLanceStrats(const string & parametre)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_PASSAGE_STRATS)
		match->lanceStratsTime();
}

/** Lance la manche suivante
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerLanceManche(const string & parametre)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_TRANSITION_MANCHE)
	match->lanceManche();
}

/** Met fin aux strats
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerFinStrats(const string & parametre)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_STRATS)
		match->finStratsTime();
}

/** Lance le d�compte des strats
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerChronoStrats(const string & parametre)
{
	Decompte::getDecompte()->lanceDecompte(ConVars::cssmatch_strats_time.GetInt()*60,timerFinStrats);
}

/** Surveille la valeur d'sv_alltalk durant le match
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerVerifAllTalk(const string & parametre)
{
	if (CSSMatch::getInstance()->getMatch()->getPhase() != PHASE_OFF)
	{
		if (ConVars::sv_alltalk->GetBool())
			Messages::centerMsg("Le AllTalk est actif");
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(1.5f,timerVerifAllTalk));
	}
}

/** Surveille la valeur d'sv_cheats durant le match
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerVerifCheats(const string & parametre)
{
	if (CSSMatch::getInstance()->getMatch()->getPhase() != PHASE_OFF)
	{
		if (ConVars::sv_cheats->GetBool())
			Messages::centerMsg("Sv_Cheats est actif");
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(1.5f,timerVerifCheats));
	}
}


void timerRetag(const string & parametre)
{
	if (CSSMatch::getInstance()->getMatch()->getPhase() != PHASE_OFF)
	{
		CSSMatch::getInstance()->getMatch()->detecteTags(true);
	}
}

Match::Match()
{
	phase = PHASE_OFF;

	//tvManche1 = NULL;
	//tvManche2 = NULL;

	mancheCourante = 1;
	roundCourant = 0;

	tagGagnantCutRound = "";

	team1 = Team();
	team2 = Team();

	cutround = true;
	strats = true;
}

Match::~Match()
{
	/*if (tvManche1 != NULL)
		delete tvManche1;

	if (tvManche2 != NULL)
		delete tvManche2;	*/
}

CodePhase Match::getPhase() const
{
	return phase;
}

void Match::setPhase(CodePhase codePhase)
{
	phase = codePhase;
}

/*EnregistrementTV * Match::getTvManche1() const
{
	return tvManche1;
}

EnregistrementTV * Match::getTvManche2() const
{
	return tvManche2;
}*/
const vector<EnregistrementTV> * Match::getEnregistrementsTV() const
{
	return &enregistrements;
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

void Match::setTagGagnantCutRound(const std::string & tag)
{
	tagGagnantCutRound = tag;
}

Team * Match::getTeam(CodeTeam codeTeam) throw(CSSMatchMatchException)
{
	Team * team = NULL;

	switch(codeTeam)
	{
	case TEAM_T:
		//if (mancheCourante == 1)
		if (mancheCourante%2 == 1)
		{
			team = &team1;
			//Warning("TEAM_T team1");
		}
		else
		{
			team = &team2;
			//Warning("TEAM_T team2");
		}
		break;
	case TEAM_CT:
		//if (mancheCourante == 1)
		if (mancheCourante%2 == 1)
		{
			team = &team2;
			//Warning("TEAM_CT team2");
		}
		else
		{
			team = &team1;
			//Warning("TEAM_CT team1");
		}
		break;
	default:
		throw CSSMatchMatchException("getTeam() sur une team spectateur !");
	}

	return team;
}

Team * Match::getTeam1()
{
	return &team1;
}

Team * Match::getTeam2()
{
	return &team2;
}

void Match::modifieTag(CodeTeam codeTeam, const std::string & nouvTag)
{
	// R�cup�ration d'un pointeur sur la team concern�e
	Team * cible = getTeam(codeTeam);
	// R�cup�ration du tag courant de la team
	const string * tagCourant = &cible->getTag();

	// Si la team est celle qui a gagn� le round au couteau, on met � jour le tag enregistr� comme tel
	if (*tagCourant == tagGagnantCutRound)
		tagGagnantCutRound = nouvTag;

	// On met � jour le tag
	cible->setTag(nouvTag);

	// On met � jour le nom du serveur
	formateHostname();
}

bool Match::getCutRound() const
{
	return cutround;
}

void Match::setCutRound(bool cutround)
{
	this->cutround = cutround;
}

bool Match::getStrats() const
{
	return strats;
}

void Match::setStrats(bool strats)
{
	this->strats = strats;
}

const string & Match::getDate() const
{
	return horaire;
}

void Match::reset()
{
	phase = PHASE_OFF;

/*	if (tvManche1 != NULL)
		delete tvManche1;
	tvManche1 = NULL;

	if (tvManche2 != NULL)
		delete tvManche2;
	tvManche2 = NULL;*/
	enregistrements.clear();

	mancheCourante = 1;
	roundCourant = 0;

	tagGagnantCutRound = "";
	team1 = Team();
	team2 = Team();
}

void Match::lanceMatch(Configuration * fichier, edict_t * pEntity)
{
	// On stoppe tout chrono ou timer en cours
	CSSMatch::getInstance()->getTimers()->purge(); // A purger en premier pour permettre l'ex�cution de la fonction associ�e � la fin du d�compte !
	Decompte::getDecompte()->finDecompte();

	// Ex�cution de la configuration
	try
	{
		fichier->execute();

		// On commence avec un match tout neuf
		reset();

		// R�cup�ration de la date du d�but de match
		time_t date = time(NULL);
		tm * dateDebut = gmtime(&date);
		char datebuf[96];
		strftime(datebuf,sizeof(datebuf),"%Hh%M",dateDebut);
		horaire = datebuf;

		// Recherche du tag de chaque team et modification du nom du serveur en cons�quence
		detecteTags(false);

		// Listage des plugins pr�sents sur le serveur
		Api::engine->LogPrint("CSSMatch : liste des plugins install�s et leur statut :\n");
		Api::serveurExecute("plugin_print\n");

		// Mise � jour du mot de passe
		if (ConVars::sv_password != NULL)
			ConVars::sv_password->SetValue(ConVars::cssmatch_password.GetString());
		else
			Api::debug("Impossible de trouver sv_password !");

		CSSMatch::getInstance()->getTimers()->addTimer(Timer(5.0f,Messages::timerSayAMX,"Mot de passe du serveur : \"" + string(ConVars::cssmatch_password.GetString()) + "\""));

		// On signale le lancement d'un match
		try
		{
			Messages::sayMsg("Un match est lancé par \003" + Api::getPlayerName(pEntity) + "\001...",Api::engine->IndexOfEdict(pEntity));
		}
		catch(const CSSMatchApiException & e)
		{
			Messages::sayMsg("Un match est lancé...");
			// Api::reporteException(e,__FILE__,__LINE__); // Match lanc� par RCON
		}

		// On lance la v�rification du alltalk est d'sv_cheats
		// On passe par des timers car la phase est encore PHASE_OFF ici
		if (ConVars::sv_alltalk != NULL)
			CSSMatch::getInstance()->getTimers()->addTimer(Timer(1.0f,timerVerifAllTalk));
		else
			Api::debug("Impossible de trouver sv_alltalk !");
		if (ConVars::sv_cheats != NULL)
			CSSMatch::getInstance()->getTimers()->addTimer(Timer(1.0f,timerVerifCheats));
		else
			Api::debug("Impossible de trouver sv_cheats !");

		// On lance la premi�re phase du match
		//	Si le match a �t� lanc� via le menu, le choix du round au couteau et des strats ont �t� d�termin�s via le menu
		if (Api::isValidePEntity(pEntity))
		{
			// On �crase la configuration concern�e si n�cessaire
			ConVars::cssmatch_cutround.SetValue((int)cutround);

			if (strats)
			{
				// Si aucun temps de strats n'a �t� d�fini on r�tablit la valeur par d�faut
				if (ConVars::cssmatch_strats_time.GetInt() == 0)
					ConVars::cssmatch_strats_time.SetValue(ConVars::cssmatch_strats_time.GetDefault());
				// Sinon on laisse le temps d�termin� par la configuration
			}
			else
				ConVars::cssmatch_strats_time.SetValue(0);
		}
		// Si la match n'a pas �t� lanc� via le menu on se fit � la valeur de cssmatch_cutround pour savoir si on doit lancer un CutRound
		else
			cutround = ConVars::cssmatch_cutround.GetBool();

		

		if (ConVars::cssmatch_cutround.GetBool())
			lanceCutRound();
		else if (ConVars::cssmatch_strats_time.GetInt() > 0)
		{
			Api::serveurExecute("mp_restartgame 1\n");
			lanceStratsTime();
		}
		else if (ConVars::cssmatch_manches.GetInt() > 0)
				lanceManche();
		else
		{
			Messages::sayColor("Erreur : le fichier de configuration n'impose aucune phase de match !"); 
			finMatch();
		}
	}
	catch(const CSSMatchConfigurationException & e)
	{
		Messages::sayMsg(e.what());
	}
}

void Match::finMatch(edict_t * pEntity)
{
	// On stopppe tout enregistrement en cours (le match a pu �tre stopp� d�s la premi�re manche)
	/*if (tvManche1 != NULL)
		tvManche1->coupe();
	if (tvManche2 != NULL)
		tvManche2->coupe();*/
	vector<EnregistrementTV>::iterator enregistrement = enregistrements.begin();
	vector<EnregistrementTV>::iterator finListe = enregistrements.end();
	while(enregistrement != finListe)
	{
		if (enregistrement->getEnregistrant())
			enregistrement->coupe();
		enregistrement++;
	}
	
	Messages::sayMsg("\004Fin du match !");

	const string * tagTeam1 = &team1.getTag();
	const string * tagTeam2 = &team2.getTag();
	int scoreTeam1 = team1.getScore();
	int scoreTeam2 = team2.getScore();

	// Envoie d'une annonce AMX
	ostringstream message;
	message << "Fin du match !\n\nScores :\n" << *tagTeam1 << " : " << scoreTeam1 << "\n" << \
		*tagTeam2 << " : " << scoreTeam2;
	string sMsg(message.str());
	Messages::sayAMX(sMsg,6);

	// Envoi de l'annonce dans la console de tous les joueurs
	Messages::clientPrintMsg(sMsg + "\n");

	if (scoreTeam1 > scoreTeam2)
		Messages::sayMsg("Team gagnante :\003 " + *tagTeam1);
	else if (scoreTeam1 < scoreTeam2)
		Messages::sayMsg("Team gagnante :\003 " + *tagTeam2);
	else
		Messages::sayMsg("Team gagnante :\003 aucune");

	// Mise � jour des scores en fonction des camps
	team1.setScoreCT(team1.getScore()-team1.getScoreT());
	team2.setScoreT(team2.getScore()-team2.getScoreCT());

	//RapportTxt(this).ecrit(); // ?
	RapportXml(this).ecrit();

	phase = PHASE_FIN;

	// Annonce du temps mort pour la prise de screenshots
	int tempsMort = ConVars::cssmatch_end_manche.GetInt();
	ostringstream annonceTempsMort;
	annonceTempsMort << tempsMort << " secondes de temps mort, prenez vos screenshots !";
	CSSMatch::getInstance()->getTimers()->addTimer(Timer(2.0f,Messages::timerSayMsg,annonceTempsMort.str()));

	Decompte::getDecompte()->lanceDecompte(tempsMort + 2,timerFinMatch);
}

void Match::finTempsMortFinMatch()
{
	// On ex�cute la configuration par d�faut
	string configParDefaut(ConVars::cssmatch_default_config.GetString());
	if (Api::filesystem->FileExists(("cfg/" + configParDefaut).c_str(),"MOD"))
		Api::configExecute(configParDefaut);
	else
	{
		Messages::sayColor("Impossible de trouver le fichier " + configParDefaut + " !");
		Messages::sayColor("Changez de map pour restaurer au maximum la configuration par défaut");
	}

	// On finalise la fin du match (un nouveau match peut �tre lanc�)
	phase = PHASE_OFF;
}

void Match::lanceCutRound()
{
	roundCourant = 0;

	Messages::sayMsg("3 restarts puis CutRound !");
	phase = PHASE_PASSAGE_CUTROUND;

	// C'est le round_start qui d�clenchera les 2 autres restarts afin d'�viter les conflits avec les restarts du jeu ou lanc�s par un admin
	Api::serveurExecute("mp_restartgame 2\n");
}

void Match::finCutRound(CodeTeam codeTeamGagnante)
{
	// Nous n'acceptons la fin du CutRound que si une des 2 teams a gagn�e
	if (codeTeamGagnante == TEAM_T || codeTeamGagnante == TEAM_CT)
	{
		phase = PHASE_TRANSITION_MANCHE;
			// => Pour �viter que lors de la mise en spectateur les tags soient red�tect�s

		// On met le camp perdante en spectateur, et on affiche le menu de choix de team � le camp gagnante
		if (codeTeamGagnante == TEAM_T)
		{
			CSSMatch::getInstance()->getTimers()->addTimer(Timer(1.0f,timerCexecT,"chooseteam\n"));
			Api::specGroupe(TEAM_CT);

			// M�morisation du tag de la team gagnante
			tagGagnantCutRound = team1.getTag();
		}
		else
		{
			CSSMatch::getInstance()->getTimers()->addTimer(Timer(1.0f,timerCexecCT,"chooseteam\n"));
			Api::specGroupe(TEAM_T);

			tagGagnantCutRound = team2.getTag();
		}

		// Affichage du tag de la team gagnante
		Messages::sayMsg("Le round au couteau a été remporté par la team \003" + tagGagnantCutRound + "\001 !");

		// Mise en place d'un temps mort pour laisser le temps aux camps de choisir leur nouvelle team
		// et de la red�tection des tags
		int tempsAttente = ConVars::cssmatch_end_cutround.GetInt();
		if (tempsAttente > 0)
		{
			ostringstream msg;
			msg << "Les " << tagGagnantCutRound << " ont \003" << tempsAttente << " secondes\001 pour choisir leur camp";
			Messages::sayMsg(msg.str());

			//Decompte::getDecompte()->lanceDecompte(tempsAttente);

			// Pr�paration de la prochaine phase de match (s'il y en a une)
			if (ConVars::cssmatch_strats_time.GetInt()>0 && strats)
			{
				phase = PHASE_PASSAGE_STRATS;
				Decompte::getDecompte()->lanceDecompte(tempsAttente,timerLanceStrats);
			}
			else
			{
				if (ConVars::cssmatch_manches.GetInt() > 0)
				{
					phase = PHASE_TRANSITION_MANCHE;
					Decompte::getDecompte()->lanceDecompte(tempsAttente,timerLanceManche);
				}
				else
					finMatch();
			}
		}
		else // S'il n'y en a pas on lance directement la suite (s'il y en a une)
		{
			if (ConVars::cssmatch_strats_time.GetInt()>0 && strats)
				lanceStratsTime();
			//else
			else if (ConVars::cssmatch_manches.GetInt() > 0)
				lanceManche();
			else
				finMatch();
		}
	}
}

void Match::lanceStratsTime()
{
	// La situation a peut-�tre chang�e suite au round au couteau, red�tection des tags et mise � jour du nom du serveur
	/*if (mancheCourante == 1) // peu importe cssmatch_manches
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(5.0,timerRetag,""));*/
	// // => devenu inutile depuis la d�tection des changements de situation ? // non, d�sactiv� lors des transitions

	// On s'assure qu'un pr�c�dent !go n'a pas �t� m�moris�
	team1.setGoStrats(false);
	team2.setGoStrats(false);

	// Si cssmatch_strats_time est � z�ro on passe directement au match (s'il y a lieu)
	int tempsStrats = ConVars::cssmatch_strats_time.GetInt();
	if (tempsStrats <= 0)
	{
		if (ConVars::cssmatch_manches.GetInt() > 0)
			lanceManche();
		else
			finMatch();
	}
	// sinon on lance le StratsTime
	else
	{
		Api::serveurExecute("mp_restartgame 2\n");
		phase = PHASE_STRATS;

		tempsStrats *= 60;
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(3.0f,timerChronoStrats));
	}
}

void Match::finStratsTime()
{
	Messages::sayMsg("Fin du Strats Time !");

	// S'il y a des manches � jouer on les lance
	if (ConVars::cssmatch_manches.GetInt() > 0)
		lanceManche();
	// Sinon c'est la fin du match
	else
		finMatch();
}

void Match::lanceManche()
{
	ostringstream say;
	say << "\004Lancement de la manche " << mancheCourante << " / " << ConVars::cssmatch_manches.GetInt();
	Messages::sayMsg(say.str());

	// Le nombre de round a �t� utilis� pr�c�demment
	roundCourant = 0;

	// Gestion de l'enregistrement sourceTV
	if (ConVars::cssmatch_sourcetv.GetBool())
	{
		// Pr�paration du nom de l'enregistrement
		time_t date;
		time(&date);
		struct tm * locale = localtime(&date);
		char datebuf[96];
		strftime(datebuf,sizeof(datebuf),"%Y-%m-%d_%Hh%M",locale);

		// Constitution du nom de la TV
		ostringstream nomTV;
		nomTV << datebuf << "_" << Api::gpGlobals->mapname.ToCStr() << "_" << "manche_" << mancheCourante;
		// Lancement de l'enregistrement (si possible)
		try
		{
			/*if (mancheCourante == 1)
			{
				tvManche1 = new EnregistrementTV(nomTV.str());
				tvManche1->enregistre();
			}
			else
			{
				tvManche2 = new EnregistrementTV(nomTV.str());
				tvManche2->enregistre();
			}*/
			EnregistrementTV enregistrement = EnregistrementTV(nomTV.str());
			enregistrement.enregistre();
			enregistrements.push_back(enregistrement);
		}
		catch(const CSSMatchTVException & e)
		{
			Messages::sayColor(e.what());
		}
	}

	Messages::sayMsg("3 restarts puis GO ! GO ! GO !");

	phase = PHASE_PASSAGE_MANCHE;

	// Lancement du match
	Api::serveurExecute("mp_restartgame 2\n");
}

void Match::finManche()
{
	// Listage des plugins pr�sents sur le serveur
	Api::engine->LogPrint("CSSMatch : liste des plugins install�s et leur statut :");
	Api::serveurExecute("plugin_print\n");

	// Cas o� il reste des manches � jouer
	//if (mancheCourante == 1)
	if (mancheCourante < ConVars::cssmatch_manches.GetInt())
	{
		// On stoppe l'�ventuel enregistrement en cours
		/*if (tvManche1 != NULL)
			tvManche1->coupe();*/
		int nbrEnregistrements = enregistrements.size();
		if (nbrEnregistrements>0)
		{
			EnregistrementTV enregistrement = enregistrements[nbrEnregistrements-1];
			if (enregistrement.getEnregistrant())
				enregistrement.coupe();
		}

		
		ostringstream say;
		say << "\004Fin de la manche " << mancheCourante << " !";
		Messages::sayMsg(say.str());
		const string * tagTeam1 = &team1.getTag();
		const string * tagTeam2 = &team2.getTag();
		int scoreTeam1 = team1.getScore();
		int scoreTeam2 = team2.getScore();

		// Envoie d'une annonce AMX
		ostringstream amx;
		amx << "Fin de la manche " << mancheCourante \
			<< " !\n\nScores :\n" \
			<< *tagTeam1 << " : " << scoreTeam1 << "\n" << \
			*tagTeam2 << " : " << team2.getScore();
		string sMsg(amx.str());
		Messages::sayAMX(sMsg,6);

		// Envoie de l'annonce dans la console de tous les joueurs
		Messages::clientPrintMsg(sMsg + "\n");


		// On lance la prochaine phase en l�ger d�calage avec le swap
		float tempsTransition = ConVars::cssmatch_end_manche.GetFloat();
		//		Ici il est devenue inutile de contr�ler la r�ponse aux questions en d�but de match, la config a �t� �cras�e
		if (ConVars::cssmatch_strats_time.GetInt() > 0)
		{
			phase = PHASE_PASSAGE_STRATS;
			CSSMatch::getInstance()->getTimers()->addTimer(Timer(tempsTransition + 2.0f,timerLanceStrats));
		}
		else
		{
			phase = PHASE_TRANSITION_MANCHE;
			CSSMatch::getInstance()->getTimers()->addTimer(Timer(tempsTransition + 2.0f,timerLanceManche));
		}

		// On pr�pare la manche suivante
		//mancheCourante = 2;
		mancheCourante++;
		

		// Le swap des joueurs est l�g�rement d�cal�, ce qui laisse le temps de prendre des screenshots
		Decompte::getDecompte()->lanceDecompte((int)tempsTransition,timerSwapALL);
		ostringstream annonceTempsMort;
		annonceTempsMort << (int)tempsTransition << " secondes de temps mort, prenez vos screenshots !";
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(2.0f,Messages::timerSayMsg,annonceTempsMort.str()));

		// On rappelle le mot de passe
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(tempsTransition + 5.0f,Messages::timerSayAMX,"Mot de passe du serveur : \"" + string(ConVars::cssmatch_password.GetString()) + "\""));

		// On cr�er le point de restauration des scores des joueurs sur la manche qui vient d'�tre jou�e
		GestionJoueurs::getPlayerList()->sauvStatsManche();

		// On cr�er le point de restauration des scores des teams sur la manche qui vient d'�tre jou�e
		team1.setScoreT(team1.getScore());
		team2.setScoreCT(team2.getScore());
	}
	else
	{	
		// Cas o� le match est fini
		finMatch();
	}
}

void Match::detecteTags(bool affiche)
{
	// Variable qui contiendra un pointeur sur la team dont le tag correspond � la team gagnante du round au couteau
	// (� mettre � jour)
	Team * teamCutRound = NULL;

	// Recherche de la team gagnante du round au couteau
	if (team1.getTag() == tagGagnantCutRound)
		teamCutRound = &team1;
	else if (team2.getTag() == tagGagnantCutRound)
		teamCutRound = &team2;

	// On d�termine quel est le camp courant de chacune des teams
	CodeTeam codeTeam1 = TEAM_T;
	CodeTeam codeTeam2 = TEAM_CT;
	if (mancheCourante % 2 == 0)
	{
		codeTeam1 = TEAM_CT;
		codeTeam2 = TEAM_T;
	}

	// Red�tection des tags
	if (affiche)
	{
		string message("\003" + team1.chercheTag(codeTeam1) + "\001 VS\003 " + team2.chercheTag(codeTeam2));
		Messages::sayMsg(message);
	}
	else
	{
			team1.chercheTag(codeTeam1);
			team2.chercheTag(codeTeam2);
	}

	// Si l'une des teams avait gagn� le round au couteau, on met � jour le tag enregistr�
	if (teamCutRound != NULL)
		tagGagnantCutRound = teamCutRound->getTag();

	formateHostname();
}

void Match::formateHostname()
{
	if (ConVars::hostname != NULL)
	{
		string nomServeur(ConVars::cssmatch_hostname.GetString());

		size_t posTag = nomServeur.find("%s");
		if (posTag != string::npos)
		{
			const string * tagTeam1 = &team1.getTag();
			nomServeur.replace(posTag,2,*tagTeam1,0,tagTeam1->size());
		}

		posTag = nomServeur.find("%s");
		if (posTag != string::npos)
		{
			const string * tagTeam2 = &team2.getTag();
			nomServeur.replace(posTag,2,*tagTeam2,0,tagTeam2->size());
		}

		ConVars::hostname->SetValue(nomServeur.c_str());
	}
	else
		Api::debug("Impossible de trouver hostname !");
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
	// Restauration des scores du round pr�c�dent
	GestionJoueurs::getPlayerList()->restaureStatsRound();

	// Si nous en �tions encore au premier round...
	if (roundCourant == 1)
	{
		// ... Nous faisons comme si nous commen�ions les manches
		//			pour obtenir 2 restarts de plus (voir round_start)
		phase = PHASE_PASSAGE_MANCHE;
		roundCourant = 0;
	}

	// Restart du round
	Api::serveurExecute("mp_restartgame 1\n");

	try
	{
		Messages::sayMsg("Le round a été relancé par \003" + Api::getPlayerName(pEntity),Api::engine->IndexOfEdict(pEntity));
	}
	catch(const CSSMatchApiException & e)
	{
		Messages::sayMsg("Le round a été relancé par un arbitre");
		// Api::reporteException(e,__FILE__,__LINE__); // restart fait pas RCON
	}
}

void Match::restartManche(edict_t * pEntity)
{
	// Restauration des scores de la manche pr�c�dente
	GestionJoueurs::getPlayerList()->restaureStatsManche();

	// Restauration des scores des teams selon la manche en cours
	if (mancheCourante == 1) // peu importe cssmatch_manches
	{
		// Aucune match n'a �t� jou�e avant
		team1.setScore(0);
		team2.setScore(0);
	}
	else
	{
		// Restauration des scores de la manche pr�c�dente
		team1.setScore(team1.getScoreT());
		team2.setScore(team2.getScoreCT());
	}

	// Nous faisons croire au plugin que nous commen�ons les manches
	//		pour obtenir 2 restarts de plus (voir round_start)
	phase = PHASE_PASSAGE_MANCHE;

	// Restart du round
	Api::serveurExecute("mp_restartgame 1\n");

	// Nous reprennons tout depuis le d�but
	roundCourant = 0;

	try
	{
		Messages::sayMsg("Le manche a été relancé par \003" + Api::getPlayerName(pEntity),Api::engine->IndexOfEdict(pEntity));
	}
	catch(const CSSMatchApiException & e)
	{
		Messages::sayMsg("Le manche a été relancé par un arbitre");
		// Api::reporteException(e,__FILE__,__LINE__); // restart fait par RCON
	}
}

void Match::forceGo(edict_t * pEntity)
{
	// Si nous n'en somme qu'au passage au strats on refuse le !go
	if (phase != PHASE_STRATS)
	{
		if (pEntity != NULL)
			Messages::sayTell(Api::engine->IndexOfEdict(pEntity),"Impossible de forcer le !go, le Strats Time n'a pas encore commencé");
		else
			Msg("CSSMatch : Le Strats Time n'a pas encore commencé");
		return;
	}

	// On actualise proprement le !go des teams
	team1.setGoStrats(true);
	team2.setGoStrats(true);

	// Le !go de l'arbitre d�clenchera la suite du match
	try
	{
		Messages::sayMsg("Toutes les teams ont été déclarées prêtes par \003" + Api::getPlayerName(pEntity),Api::engine->IndexOfEdict(pEntity));
	}
	catch(const CSSMatchApiException & e)
	{
		Messages::sayMsg("Toutes les teams ont été déclarées prêtes par un arbitre");
		// Api::reporteException(e,__FILE__,__LINE__); // d�claration par RCON
	}

	// Arr�t du d�compte et lancement de la suite du match
	Decompte::getDecompte()->finDecompte();
	//finStratsTime();
}

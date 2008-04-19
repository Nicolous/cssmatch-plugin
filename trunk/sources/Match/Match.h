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

#include "../Configuration/Configuration.h"
#include "../EnregistrementTV/EnregistrementTV.h"
#include "../ConVars/ConVars.h"
#include "../Team/Team.h"
#include "../Messages/Messages.h"
#include "../Decompte/Decompte.h"
#include "../GestionTimers/GestionTimers.h"
#include "../GestionJoueurs/GestionJoueurs.h"

#ifndef __MATCH_H__
#define __MATCH_H__

/** On associe � une constante chaque phase de match */
typedef enum CodePhase
{
	/** Aucun match en cours */
	PHASE_OFF,
	/** Attente du lancement du CutRound */
	PHASE_PASSAGE_CUTROUND,
	/** CutRound */
	PHASE_CUTROUND,
	/** Attente du lancement du StratsTime */
	PHASE_PASSAGE_STRATS,
	/** StratsTime */
	PHASE_STRATS,
	/** Attente du lancement d'une manche */
	PHASE_PASSAGE_MANCHE,
	/** Manche */
	PHASE_MANCHE,
	/** Fin de match */
	PHASE_FIN
};

/** Repr�sentant un match */
class Match
{
private:
	/** Phase courante du match */
	CodePhase phase;

	/** Enregistrement SourceTV de la manche 1 */
	EnregistrementTV * tvManche1;
	/** Enregistrement SourceTV de la manche 2 */
	EnregistrementTV * tvManche2;
	
	/** Num�ro de la manche courante */
	int mancheCourante;
	/** Num�ro du round courant */
	int roundCourant;

	/** Code de la team gagnante du CutRound */
	std::string tagGagnantCutRound;

	/** Team 1 */
	Team team1;
	/** Team 2 */
	Team team2;

	/** Doit-il y avoir un CutRound ? (r�sulte du choix dans le menu et de cssmatch_cutround) */
	bool cutround;

	/** Fonction initialisant un nouveau match */
	void reset();

public:
	Match();
	~Match();

	/** Retourne l'identificateur de la phase en cours */
	CodePhase getPhase() const;
	/** D�termine la phase en cours */
	void setPhase(CodePhase codePhase);

	/** Retourne un pointeur sur l'enregistrement TV de la manche 1 */
	EnregistrementTV * getTvManche1() const;
	/** Retourne un pointeur sur l'enregistrement TV de la manche 2 */
	EnregistrementTV * getTvManche2() const;
	
	/** Retourne le num�ro de la manche en cours */
	int getMancheCourante() const;

	/** Retourne le num�ro du round courant */
	int getRoundCourant() const;
	/** D�termine le num�ro du round courant */
	void setRoundCourant(int numeroRound);
	/** Incr�mente le num�ro du round courant (retourne le nouveau num�ro du round) */
	int incRoundCourant();

	/** Retourne le CodeTeam de la team gagnante du CutRound */
	const std::string & getTagGagnantCutRound() const;

	/** Retourne un pointeur sur la team actuellement dans le camp pass� en param�tre <br>
	 * Jette une exception de type std::out_of_range si le codeTeam ne correspond pas � TEAM_T ou TEAM_CT
	 */
	Team * getTeam(CodeTeam codeTeam);

	/** Retourne un pointeur sur le camp 1 (terroristes lors de la premi�re manche) */
	Team * getTeam1();

	/** Retourne un pointeur sur le camp 2 (anti-terroristes lors de la premi�re manche) */
	Team * getTeam2();

	/** Retourne vrai si un CutRound est planifi� */
	bool getCutRound() const;
	/** D�termine si un CutRound doit �tre planifi� */
	void setCutRound(bool cutround);

	/** Lance un match <br>
	 * Cette proc�dure lance automatiquement le CutRound ou le StratsTime selon la configuration
	 *
	 * @param pEntity Admin lanceur du match pourra �tre pass�e en param�tre
	 *
	 */
	void lanceMatch(Configuration * fichier, edict_t * pEntity = NULL);
	/** Met fin au match actuel <br>
	 * Les champs sont persistents tant que l'instance n'est pas d�truite <br>
	 * Cette proc�dure sera automatiquement appel�e si la seconde manche arrive � son terme
	 */
	void finMatch(edict_t * pEntity = NULL);

	/** Finalise le match (fin du temps mort en fin de match) */
	void finTempsMortFinMatch();

	/** Lance un CutRound */
	void lanceCutRound();
	/** Met fin au CutRound si le CodeTeam pass� en param�tre est celui de l'une des 2 camps */
	void finCutRound(CodeTeam codeTeamGagnante);

	/** Lance un StratsTime */
	void lanceStratsTime();
	/** Met fin � un StratsTime */
	/** Sera automatiquement appel� si le temps limite du StratsTime est atteint */
	void finStratsTime();

	/** Lance une manche (la premi�re ou la seconde selon celle qui aura d�j� �t� jou�e) */
	void lanceManche();
	/** Met fin � la manche en cours */
	void finManche();

	/** Lance la d�tection des tag des teams <br>
	 * Appel automatiquement formateHostname()
	 *
	 * @param affiche Si vrai, le r�sultat est affich� dans le TCHAT
	 *
	 */
	void detecteTags(bool affiche);

	/** Modifie le nom du serveur <br>
	 * Remplace automatiquement les %s par le tag des teams
	 */
	void formateHostname();

	/** Affiche les scores */
	void afficheScores();

	/** Relance le round en restaurant les scores des joueurs <br>
	 * L'entit� est celle qui a lanc� le restart
	 */
	void restartRound(edict_t * pEntity = NULL);

	/** Relance la manche en restaurant les scores des joueurs <br>
	 * L'entit� est celle qui a lanc� le restart
	 */
	void restartManche(edict_t * pEntity = NULL);

	/** Force le !go de toutes les teams en StratsTime */
	void forceGo(edict_t * pEntity = NULL);
};

#endif // __MATCH_H__

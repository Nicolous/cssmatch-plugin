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

/** Exception sp�cifique au d�roulement du match */
class CSSMatchMatchException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchMatchException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchMatchException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};

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
	PHASE_TRANSITION_MANCHE,
	/** Lancement d'une manche */
	PHASE_PASSAGE_MANCHE,
	/** Manche */
	PHASE_MANCHE,
	/* Placement des teams*/
	//PHASE_CHOOSETEAM,
	/** Fin de match */
	PHASE_FIN
};

/** Un match */
class Match
{
private:
	/** Phase courante du match */
	CodePhase phase;

	/* Enregistrement SourceTV de la manche 1 */
	//EnregistrementTV * tvManche1;
	/* Enregistrement SourceTV de la manche 2 */
	//EnregistrementTV * tvManche2;
	/** Liste des enregistrements SourceTV */
	std::vector<EnregistrementTV> enregistrements;
	
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
	/** "Team" spectatrice */
	// Team spec;


	/** Doit-il y avoir un CutRound ? (r�sulte du choix dans le menu) */
	bool cutround;

	/** Doit-il y avoir des strats ? (r�sulte du choix dans le menu) */
	bool strats;

	/** Heure de d�but du match */
	std::string horaire;

	/** Fonction initialisant un nouveau match */
	void reset();

public:
	Match();
	~Match();

	/** Accesseur sur le code de la phase de match actuelle
	 *
	 * @return Le code de la phase de match en cours
	 * @see L'�num�ration CodePhase
	 *
	 */
	CodePhase getPhase() const;

	/** D�termine le code phase en cours 
	 *
	 * @param codePhase Le nouveau code de la phase en cours
	 *
	 */
	void setPhase(CodePhase codePhase);


	/** Accesseur sur l'enregistrement TV de la manche 1
	 *
	 * @return Un pointeur sur l'enregistrement (peut �tre NULL !)
	 *
	 */
	//EnregistrementTV * getTvManche1() const;
	
	/* Accesseur sur l'enregistrement TV de la manche 2
	 *
	 * @return Un pointeur sur l'enregistrement (peut �tre NULL !)
	 *
	 */
	//EnregistrementTV * getTvManche2() const;
	/** Accesseur sur les enregistrements TV
	 *
	 * @return Un pointeur sur la liste des enregistrements
	 *
	 */
	const std::vector<EnregistrementTV> * getEnregistrementsTV() const;
	

	/** Accesseur sur le num�ro de la manche en cours
	 *
	 * @return Le num�ro de la manche
	 *
	 */
	int getMancheCourante() const;


	/** Accesseur sur le num�ro du round courant
	 *
	 * @return Le num�ro du round
	 *
	 */
	int getRoundCourant() const;

	/** D�termine le num�ro du round courant
	 *
	 * @param numeroRound Le nouveau num�ro du round
	 *
	 */
	void setRoundCourant(int numeroRound);

	/** Incr�mente le num�ro du round courant
	 *
	 * @return Le nouveau num�ro du round
	 *
	 */
	int incRoundCourant();


	/** Accesseur sur le tag de la team gagnante du CutRound 
	 *
	 * @return Le tag de la team gagnante (peut �tre �gal � "" si aucun round au couteau n'a eu lieu)
	 *
	 */
	const std::string & getTagGagnantCutRound() const;

	/** Accesseur sur le tag de la team gagnante du CutRound
	 *
	 * @param tag Le nouveau tag de la team gagnante
	 *
	 */
	void setTagGagnantCutRound(const std::string & tag);

	/** D�termine la team actuellement dans le camp pass� en param�tre <br/>
	 * FIXME : team/camp, que c'est confus...
	 * 
	 * @param codeTeam Le code du camp
	 * @return Un pointeur sur la team
	 * @throws CSSMatchMatchException si le CodeTeam ne correspond pas � TEAM_T ou TEAM_CT
	 * @see L'�num�ration CodeTeam
	 *
	 */
	Team * getTeam(CodeTeam codeTeam) throw(CSSMatchMatchException);

	/** Accesseur sur le camp 1 (terroristes lors de la premi�re manche)
	 *
	 * @return Un pointeur sur la team
	 *
	 */
	Team * getTeam1();

	/** Accesseur sur le camp 2 (anti-terroristes lors de la premi�re manche)
	 *
	 * @return Un pointeur sur la team
	 *
	 */
	Team * getTeam2();

	/** Modifie le tag d'une team (met � jour le tag de la team gagnante du round au couteau si n�cessaire)
	 *
	 * @param codeTeam Le code de la team portant ce tag
	 * @param nouvTag Le nouveau tag
	 *
	 */
	void modifieTag(CodeTeam codeTeam, const std::string & nouvTag);


	/** D�termine si un round au couteau est planifi�
	 *
	 * @return true si un round au couteau est planifi�
	 *
	 */
	bool getCutRound() const;

	/** D�termine si un CutRound doit �tre planifi�
	 *
	 * @param cutround true si un round au couteau doit �tre effectu�
	 *
	 */
	void setCutRound(bool cutround);

	/** D�termine si des strats sont planifi�s
	 *
	 * @return true si des strats sont planifi�s
	 *
	 */
	bool getStrats() const;

	/** D�termine si des strats sont planifi�s
	 *
	 * @param strats true si des strats sont planifi�s
	 *
	 */
	void setStrats(bool strats);

	/** Accesseur
	 *
	 * @return La date de d�but du match
	 */
	const std::string & getDate() const;

	/** Lance un match <br/>
	 * Note : cette proc�dure lance automatiquement le CutRound ou le StratsTime selon la configuration
	 *
	 * @param fichier Un pointeur sur le fichier de configuration � lancer
	 * @param pEntity Un pointeur sur l'entit� correspondant au joueur arbitre lanceur du match (optionnel)
	 *
	 */
	void lanceMatch(Configuration * fichier, edict_t * pEntity = NULL);

	/** Met fin au match actuel <br/>
	 * Note : les champs sont persistents tant que l'instance n'est pas d�truite <br/>
	 * Cette proc�dure est appel�e lorsque la seconde manche arrive � son terme
	 *
	 * @param pEntity Un pointeur sur l'entit� correspondant au joueur arbitre qui a stopp� le match (optionnel)
	 *
	 */
	void finMatch(edict_t * pEntity = NULL);

	/** Finalise le match (fin du temps mort en fin de match) */
	void finTempsMortFinMatch();

	/** Lance un CutRound */
	void lanceCutRound();

	/** Met fin au CutRound si le CodeTeam pass� en param�tre est celui de l'une des 2 camps
	 *
	 * @param codeTeamGagnante Le code de la team gagnant le round
	 *
	 */
	void finCutRound(CodeTeam codeTeamGagnante);

	/** Lance un StratsTime */
	void lanceStratsTime();

	/** Met fin � un StratsTime <br/> 
	 * Note : cette proc�dure est appel�e si le temps limite du StratsTime est atteint 
	 */
	void finStratsTime();

	/** Lance une manche (la premi�re ou la seconde selon le cas) */
	void lanceManche();

	/** Met fin � la manche en cours */
	void finManche();

	/** Lance la d�tection des tag des teams <br/>
	 * Note : appelle automatiquement formateHostname()
	 *
	 * @param affiche Si true, le r�sultat est affich� dans le TCHAT
	 *
	 */
	void detecteTags(bool affiche);

	/** Modifie le nom du serveur <br/>
	 * Note : remplace automatiquement les %s par le tag des teams
	 */
	void formateHostname();

	/** Affiche les scores */
	void afficheScores();

	/** Relance le round en restaurant les scores des joueurs
	 *
	 * @param pEntity Un pointeur sur l'entit� correspondant au joueur arbitre qui a relanc� le round (optionnel)
	 *
	 */
	void restartRound(edict_t * pEntity = NULL);

	/** Relance la manche en restaurant les scores des joueurs
	 *
	 * @param pEntity Un pointeur sur l'entit� correspondant au joueur arbitre qui a relanc� la manche (optionnel)
	 *
	 */
	void restartManche(edict_t * pEntity = NULL);

	/** Force le !go de toutes les teams en StratsTime 
	 *
	 * @param pEntity Un pointeur sur l'entit� correspondant au joueur arbitre qui a forc� les !go (optionnel)
	 *
	 */
	void forceGo(edict_t * pEntity = NULL);
};

/** Red�tecte et affiche les tags 
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerRetag(const std::string & parametre);

#endif // __MATCH_H__

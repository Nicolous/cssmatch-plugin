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
#include "../Team/Team.h"
#include "../ConVars/ConVars.h"
#include "../EnregistrementTV/EnregistrementTV.h"
#include "../Match/Match.h"
#include "../Joueur/Joueur.h"
#include "../GestionJoueurs/GestionJoueurs.h"

#ifndef __RAPPORT_H__
#define __RAPPORT_H__

/** Classe statique représentant un rapport de match sauvegardé dans le dossier cssmatch/rapports/ au format .txt */
class Rapport
{
private:
	/** Ecrit l'entête dans le fichier */
	static void ecritEntete(FileHandle_t & fh);

	/** Ecrit la situation du match */
	static void ecritSituationMatch(FileHandle_t & fh, Match & match);

	/** Ecrit les scores des teams et par teams */
	static void ecritScores(FileHandle_t & fh, Match & match);

	/** Ecrit la lignup de la team 1 */
	static void ecritLignupT(FileHandle_t & fh, Match & match, const std::vector<int> & lignupT, const std::string * tagT);
	/** Ecrit la lignup de la team 2 */
	static void ecritLignupCT(FileHandle_t & fh, Match & match, const std::vector<int> & lignupCT, const std::string * tagCT);
	/** Ecrit la lignup des spectateurs */
	static void ecritLignupSpec(FileHandle_t & fh, Match & match, const std::vector<int> & lignupSpec);
	/** Ecrit la liste des joueurs et leur score par team */
	static void ecritLignups(FileHandle_t & fh, Match & match);

	/** Ecrit le nom des enregistrements sourceTV */
	static void ecritEnregistrements(FileHandle_t & fh, Match & match);
public:
	/** Génère un rapport basé sur un match */
	static void genereRapport(Match & match);
};

#endif // __RAPPORT_H__
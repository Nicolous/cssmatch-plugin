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

#include "../Rapport.h"

#ifndef __RAPPORTTXT_H__
#define __RAPPORTTXT_H__

/** 
	Classe représentant un rapport de match sauvegardé dans le dossier cssmatch/rapports au format .txt 
*/
class RapportTxt : public Rapport
{
private:
	/** Surcharge de la méthode qui écrit l'entête du fichier */
	void ecritEntete(FileHandle_t & fh) const;

	/** Surcharge de la méthode qui écrit le corps du fichier */
	void ecritCorps(FileHandle_t & fh) const;

	/** Surcharge de la méthode qui écrit le pied du fichier */
	void ecritPied(FileHandle_t & fh) const;

	/** Ecrit la situation du match */
	void ecritSituationMatch(FileHandle_t & fh, const Team * team1, const Team * team2) const;

	/** Ecrit les scores des teams et par teams */
	void ecritScores(FileHandle_t & fh, const Team * team1, const Team * team2) const;

	/** Ecrit la lignup de la team 1 */
	void ecritLignup(FileHandle_t & fh, const std::string * tag, const std::vector<int> & lignup) const;
	void ecritLignups(FileHandle_t & fh, const Team * team1, const Team * team2) const;

	/** Ecrit le nom des enregistrements sourceTV */
	void ecritEnregistrements(FileHandle_t & fh) const;
public:
	/** Surcharge de la méthode qui écrit le fichier sur le disque */
	void ecrit();

	/** Surcharge du constructeur */
	RapportTxt(Match * match);
};

#endif // __RAPPORTTXT_H__

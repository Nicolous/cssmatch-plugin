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

#ifndef __MRECIPIENT_FILTER_H__
#define __MRECIPIENT_FILTER_H__

#include "irecipientfilter.h"
#include "bitvec.h"
#include "tier1/utlvector.h"

/** Une liste de destinataires pour les messages 
 *
 * @author http://www.hl2coding.com (l'auteur pr�cis m'est inconnu)
 *
 */
class MRecipientFilter : public IRecipientFilter
{
private:
	/* TODO � documenter */
	//bool m_bReliable;

	/* TODO � documenter */
	//bool m_bInitMessage;

	/** Liste des destinataires */
	CUtlVector<int> m_Recipients;

public:
	/** Pr�pare une liste de destinataires vide */
	MRecipientFilter();

	/** D�truit la liste de destinataires */
	~MRecipientFilter();

	/** TODO : � documenter  
	 *
	 */
	virtual bool IsReliable() const;

	/** TODO : � documenter
	 *
	 */
	virtual bool IsInitMessage() const;


	/** Permet de connaitre la taille de la liste 
	 *
	 * @return La taille de la liste
	 *
	 */
	virtual int GetRecipientCount() const;

	/** Ajoute tous les joueurs � la liste */
	void addAllPlayers();

	/** Ajoute un joueur � la liste 
	 *
	 * @param indexJoueur L'index du joueur
	 *
	 */
	void addRecipient(int indexJoueur);

	/** R�cup�re l'index du joueur situ� � une position de la liste de destinataire
	 *
	 * @param slot L'index dans la liste
	 */
	virtual int GetRecipientIndex(int slot) const;
};

#endif // __MRECIPIENT_FILTER_H__

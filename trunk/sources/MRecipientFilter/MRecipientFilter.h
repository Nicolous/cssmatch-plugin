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

#ifndef _MRECIPIENT_FILTER_H
#define _MRECIPIENT_FILTER_H
#include "irecipientfilter.h"
#include "bitvec.h"
#include "tier1/utlvector.h"

/** Permet de dresser des listes de destinataires pour les messages 
 *
 * @author http://www.hl2coding.com (auteur pr�cis inconnu)
 *
 */
class MRecipientFilter : public IRecipientFilter
{
public:
	MRecipientFilter(void);
	~MRecipientFilter(void);

	virtual bool IsReliable( void ) const;
	virtual bool IsInitMessage( void ) const;

	virtual int GetRecipientCount( void ) const;
	virtual int GetRecipientIndex( int slot ) const;
	void AddAllPlayers();
	void AddRecipient (int iPlayer );

private:
	bool m_bReliable;
	bool m_bInitMessage;
	CUtlVector< int > m_Recipients;
};

extern CGlobalVars *gpGlobals;

#endif

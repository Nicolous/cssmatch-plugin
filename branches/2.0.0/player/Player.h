/* 
 * Copyright 2008, 2009 Nicolas Maingot
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

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "../exceptions/BaseException.h"
#include "../entity/EntityProp.h"
#include "../messages/RecipientFilter.h"
#include "../common/common.h"

class IVEngineServer;
class CBasePlayer;
class CBaseCombatCharacter;
class CBaseCombatWeapon;

#include <string>

namespace cssmatch
{
	class RecipientFilter;

	class PlayerException : public BaseException
	{
	public:
		PlayerException(const std::string & message) : BaseException(message){};
	};

	/** Team id */
	typedef enum TeamCode
	{
		INVALID_TEAM = TEAM_INVALID,
		UN_TEAM = TEAM_UNASSIGNED,
		SPEC_TEAM = TEAM_SPECTATOR,
		T_TEAM,
		CT_TEAM
	};

	/** Weapon slot codes */
	typedef enum WeaponSlotCode
	{
		WEAPON_SLOT1 = 0,
		WEAPON_SLOT2,
		WEAPON_SLOT3,
		WEAPON_SLOT4,
		WEAPON_SLOT5
	};

	struct PlayerIdentity
	{
		/** Pointer to the player entity instance corresponding to this player */
		edict_t * pEntity;

		/** Index of this player */
		int index;

		/** Userid of this player */
		int userid;

		/** Steamid of this player */
		std::string steamid;

		PlayerIdentity() : pEntity(NULL),index(INVALID_ENTITY_INDEX),userid(INVALID_PLAYER_USERID),steamid(""){};
	};

	/** Base class for player informations */
	class Player
	{
	private:
		// Functors (friends to accelerates the lookups)
		friend struct PlayerHavingPEntity;
		friend struct PlayerHavingIndex;
		friend struct PlayerHavingUserid;
		friend struct PlayerHavingSteamid;
		friend struct PlayerHavingTeam;
		friend struct PlayerIsHltv;
		friend struct PlayerToRecipient;
	protected:
		/** Player's identity */
		PlayerIdentity identity;

		/** Cash handler */
		EntityProp cashHandler;

		/** Life state handler */
		EntityProp lifeStateHandler;

	public:
		/** Initialize the informations related to the player 
		 * @param index The player's index (must be valid !)
		 */
		Player(int index) throw(PlayerException);

		virtual ~Player();

		/** Get the identity informations about this player 
		 * @return A PlayerIdentity structure pointer
		 * @see struct PlayerIdentity
		 */
		PlayerIdentity * getIdentity();

		/** Get the current team of this player
		 * @return The team's id of the player, or INVALID_TEAM if something was invalid
		 * @see enum TeamCode
		 */
		TeamCode getMyTeam() const;

		/** Get an IPlayerInfo instance corresponding to this entity
		 * @return The IPlayerInfo instance corresponding to this entity, or NULL if something was invalid
		 */
		IPlayerInfo * getPlayerInfo() const;

		/** Get a base player instance corresponding to this entity
		 * @return The base player instance corresponding to this entity, or NULL if something was invalid
		 */
		CBasePlayer * getBasePlayer() const;

		/** Get a base combat character instance corresponding to this entity
		 * @return The base combat character instance corresponding to this entity, or NULL if something was invalid
		 */
		CBaseCombatCharacter * getBaseCombatCharacter() const;

		/** Get the CBaseCombatWeapon instance corresponding to a player entity's weapon slot 
		 * @param slot The slot code where is weapon can be found
		 * @return The CBaseCombatWeapon instance found, or NULL if something was invalid
		 */
		CBaseCombatWeapon * getWeaponFromWeaponSlot(WeaponSlotCode slot) const;

		/** Kick this player
		 * @param reason Reason for the kick
		 */
		void kick(const std::string & reason) const;

		/** Swap this player 
		 * @return <code>false</code> if the player is spectator
		 */
		bool swap();

		/** Put this player in the spectator team 
		 * @return <code>false</code> if the player is already spectator
		 */
		bool spec();

		/** Remove a weapon at a specified slot 
		 * @param slot The slot code where is weapon can be found
		 * @see enum WeaponSlotCode
		 */
		void removeWeapon(WeaponSlotCode slot);
			
		/** Set the cash account of this player 
		 * @param newCash New cash amount
		 */
		void setCash(unsigned int newCash);

		/** Set the life state of this player 
		 * @param newState New life state
		 */
		void setLifeState(unsigned int newState);
	};

	/** Functor to quickly find a Player instance by his edict_t instance */
	struct PlayerHavingPEntity
	{
		edict_t * pEntity;

		PlayerHavingPEntity(edict_t * pEnt) : pEntity(pEnt){} 

		bool operator ()(const Player * player)
		{
			return player->identity.pEntity == pEntity;
		}
	};

	/** Functor to quickly find a Player instance by his index */
	struct PlayerHavingIndex
	{
		int index;

		PlayerHavingIndex(int ind): index(ind){}

		bool operator ()(const Player * player)
		{
			return player->identity.index == index;
		}
	};

	/** Functor to quickly find a Player instance by his userid */
	struct PlayerHavingUserid
	{
		int userid;

		PlayerHavingUserid(int id) : userid(id){}

		bool operator ()(const Player * player)
		{
			return player->identity.userid == userid;
		}
	};

	/** Functor to quickly find a Player instance by his steamid */
	struct PlayerHavingSteamid
	{
		std::string steamid;

		PlayerHavingSteamid(const std::string & id) : steamid(id){}

		bool operator ()(const Player * player)
		{
			return player->identity.steamid == steamid;
		}
	};

	/** Functor to quickly find a Player instance by his team */
	struct PlayerHavingTeam
	{
		TeamCode team;

		PlayerHavingTeam(TeamCode t) : team(t){}

		bool operator ()(const Player * player)
		{
			return player->getMyTeam() == team;
		}
	};

	/** Functor to quickly find SourceTV */
	struct PlayerIsHltv
	{
		bool operator ()(const Player * player)
		{
			IPlayerInfo * pInfo = player->getPlayerInfo();

			return (pInfo != NULL) && pInfo->IsConnected() && pInfo->IsHLTV();
		}
	};

	/** Functor to add a player in a recipient (index list) */
	struct PlayerToRecipient
	{
		RecipientFilter * recipientFilter;

		PlayerToRecipient(RecipientFilter * filter) : recipientFilter(filter){}

		void operator ()(const Player * player)
		{
			IPlayerInfo * infos = player->getPlayerInfo();
			if ((infos != NULL) && (! infos->IsFakeClient())) // TODO: Test this under Linux
			{
				recipientFilter->addRecipient(player->identity.index);
			}
		}
	};

	/** Functor to quickly remove a player */
	struct PlayerToRemove
	{
		void operator ()(const Player * player)
		{
			delete player;
		}
	};
}

#endif // __PLAYER_H__

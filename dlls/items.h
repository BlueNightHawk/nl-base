/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#pragma once

class CItem : public CBaseEntity
{
public:
	int ObjectCaps() override { return CBaseEntity::ObjectCaps() | FCAP_PHYSICS; }
	void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType) override
	{
		PhysicsTakeDamage(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
	}

	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override { return true; }

	void Spawn() override;
	CBaseEntity* Respawn() override;
	void EXPORT ItemTouch(CBaseEntity* pOther);
	void EXPORT Materialize();
	virtual bool MyTouch(CBasePlayer* pPlayer) { return false; }
};

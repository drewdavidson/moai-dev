// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DFixture.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@name	getContactNormal
 @text	Returns the normal for the contact.
 
 @in	MOAIBox2DArbiter self
 @out	number normal.x
 @out	number normal.y
 */
int MOAIBox2DArbiter::_getContactNormal ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DArbiter, "U" )
	
	state.Push ( self->mContactNormal.x );
	state.Push ( self->mContactNormal.y );
	return 2;
}

//BEGIN DREW
//-----------------------------------------------------------------//
/**	@name	getContactPoints
 @text	Returns the first point of contact.
 
 @in	MOAIBox2DArbiter self
 @out	number contact.x
 @out	number contact.y
 */
int MOAIBox2DArbiter::_getContactPoints ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DArbiter, "U" )

	//USLog::Print("DREW test getContactPoint1 %f %f\n", self->mContactPoint.x, self->mContactPoint.y);
	
	state.Push ( self->mContactPoint1.x );
	state.Push ( self->mContactPoint1.y );
	if (self->mNumContactPoints == 2){
		state.Push ( self->mContactPoint2.x );
		state.Push ( self->mContactPoint2.y );
	} else {
		//To ensure a constant number of out params, we'll just push the first point twice
		state.Push ( self->mContactPoint1.x );
		state.Push ( self->mContactPoint1.y );
	}
	return 4;
}
//END DREW

//----------------------------------------------------------------//
/**	@name	getNormalImpulse
	@text	Returns total normal impulse for contact.
	
	@in		MOAIBox2DArbiter self
	@out	number impulse			Impulse in kg * units / s converted from kg * m / s
*/
int MOAIBox2DArbiter::_getNormalImpulse ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DArbiter, "U" )

	float impulse = self->mNormalImpulse;
	const float metersToUnits = 1 / self->GetUnitsToMeters();
	impulse = impulse * metersToUnits;

	state.Push ( impulse );
	return 1;
}

//----------------------------------------------------------------//
/**	@name	getTangentImpulse
	@text	Returns total tangent impulse for contact.
	
	@in		MOAIBox2DArbiter self
	@out	number impulse			Impulse in kg * units / s converted from kg * m / s
*/
int MOAIBox2DArbiter::_getTangentImpulse ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DArbiter, "U" )

	float impulse = self->mTangentImpulse;
	const float metersToUnits = 1 / self->GetUnitsToMeters();
	impulse = impulse * metersToUnits;

	state.Push ( impulse );
	return 1;
}

//----------------------------------------------------------------//
/**	@name	setContactEnabled
	@text	Enabled or disable the contact.
	
	@in		MOAIBox2DArbiter self
	@in		boolean enabled
	@out	nil
*/
int MOAIBox2DArbiter::_setContactEnabled ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DArbiter, "U" )
	
	bool enabled = state.GetValue < bool >( 2, false );
	self->mContact->SetEnabled ( enabled );
	
	return 0;
}

//================================================================//
// MOAIBox2DArbiter
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DArbiter::BeginContact ( b2Contact* contact ) {
	
	this->mContact = contact;
	this->mImpulse = 0;
	
	this->mContactNormal = b2Vec2();
	this->mNormalImpulse = 0.0f;
	this->mTangentImpulse = 0.0f;
	
	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();
	
	moaiFixtureA->HandleCollision ( BEGIN, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( BEGIN, moaiFixtureA, this );
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::EndContact ( b2Contact* contact ) {
	
	this->mContact = contact;
	this->mImpulse = 0;
	
	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();
	
	moaiFixtureA->HandleCollision ( END, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( END, moaiFixtureA, this );
}

//----------------------------------------------------------------//
float MOAIBox2DArbiter::GetUnitsToMeters ( ) const {
	if (this->mWorld) {
		return this->mWorld->GetUnitsToMeters();
	} else {
		return 1.0f;
	}
}

//----------------------------------------------------------------//
MOAIBox2DArbiter::MOAIBox2DArbiter ( ) :
	mContact ( 0 ),
	mImpulse ( 0 ),
	mWorld ( NULL ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAILuaObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DArbiter::MOAIBox2DArbiter ( const MOAIBox2DWorld& world ) :
	mContact ( 0 ),
	mImpulse ( 0 ),
	mWorld ( &world ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAILuaObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DArbiter::~MOAIBox2DArbiter () {
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::PostSolve ( b2Contact* contact, const b2ContactImpulse* impulse ) {

	this->mContact = contact;
	this->mImpulse = impulse;

	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();
		
	b2WorldManifold* worldManifold = new b2WorldManifold ();
	contact->GetWorldManifold ( worldManifold );
	this->mContactNormal = worldManifold->normal;
	
	b2Manifold* manifold = contact->GetManifold ();
	u32 totalPoints = manifold->pointCount;

	//BEGIN DREW
	this->mNumContactPoints = totalPoints;
	this->mContactPoint1 = worldManifold->points[0];
	if (totalPoints > 1){
		this->mContactPoint2 = worldManifold->points[1];
	} else {
		//Just leave junk in mContactPoint2, it's never getting used
	}
	//END DREW

	delete worldManifold;
	
	this->mNormalImpulse = 0.0f;
	this->mTangentImpulse = 0.0f;
	
	for ( u32 i = 0; i < totalPoints; ++i ) {
		this->mNormalImpulse += impulse->normalImpulses [ i ];
		this->mTangentImpulse += impulse->tangentImpulses [ i ];
	}
	
	moaiFixtureA->HandleCollision ( POST_SOLVE, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( POST_SOLVE, moaiFixtureA, this );
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::PreSolve ( b2Contact* contact, const b2Manifold* oldManifold ) {
	UNUSED ( oldManifold );
	
	this->mContact = contact;
	this->mImpulse = 0;
	
	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();


	b2WorldManifold* worldManifold = new b2WorldManifold ();
	contact->GetWorldManifold ( worldManifold );

	//BEGIN DREW
	b2Manifold * manifold = contact->GetManifold();

	this->mContactNormal = worldManifold->normal;

	int totalPoints = manifold->pointCount;
	this->mNumContactPoints = totalPoints;
	this->mContactPoint1 = worldManifold->points[0];
	if (totalPoints > 1){
		this->mContactPoint2 = worldManifold->points[1];
	} else {
		// uhh... I guess we'll just leave junk in the second point for now?
		// it will never reach user code
	}
	delete worldManifold;
	//END DREW
	
	moaiFixtureA->HandleCollision ( PRE_SOLVE, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( PRE_SOLVE, moaiFixtureA, this );
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::RegisterLuaClass ( MOAILuaState& state ) {

	state.SetField ( -1, "ALL", ( u32 )ALL );
	state.SetField ( -1, "BEGIN", ( u32 )BEGIN );
	state.SetField ( -1, "END", ( u32 )END );
	state.SetField ( -1, "PRE_SOLVE", ( u32 )PRE_SOLVE );
	state.SetField ( -1, "POST_SOLVE", ( u32 )POST_SOLVE );
	
	luaL_Reg regTable [] = {
		{ "new",							MOAILogMessages::_alertNewIsUnsupported },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::RegisterLuaFuncs ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "getContactNormal",			_getContactNormal },
		{ "getContactPoints",			_getContactPoints },
		{ "getNormalImpulse",			_getNormalImpulse },
		{ "getTangentImpulse",			_getTangentImpulse },
		{ "setContactEnabled",			_setContactEnabled },
		{ "new",						MOAILogMessages::_alertNewIsUnsupported },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


//Before anything, i'd like to give credits to Treeki, Tempus and megazig for their custom sprites code, as all my custom sprites are based on them.
//Why do I say that even if it's so obvious ? WELL BECAUSE A DUMBASS CALLED MEATBALL132 DARE SAY I'M STEALING HIS CODE AND I SHOULD GIVE CREDITS INTO EVERY SINGLE FUCKING LINE IM TYPING.
// -RedStoneMatt 11/10/2020


extern "C" void *StageScreen;

#define THROWWAIT 1 //Unused. -RedStoneMatt 11/10/2020
#define THROWSHOOT 2 //The thing is that i don't want to remove it from the public code repo because it's like part of the makings of kamek... -RedStoneMatt 11/10/2020
#define THROWSHOOTREVERSE 3

int imded;

const char* KMarcNameList [] = {
	"kameck",
	NULL	
};


class daBossKameck_c : public daBoss {
	int onCreate();						//Executed when the sprite spawns
	int onDelete();						//Executed when the sprite is being deleted
	int onExecute();					//Executed every frame after the onCreate is executed
	int onDraw();						//Executed every frame, handles the rendering

	mHeapAllocator_c allocator;			//The Heap Allocator
	m3d::mdl_c bodyModel;				//Kamek's model
	m3d::mdl_c broomModel;				//Kamek's broom's model
	m3d::mdl_c shieldModel;				//Kamek's shield's model
	m3d::anmChr_c animationChr;			//Kamek's "bone" animation
	m3d::anmChr_c animationBroomChr;	//Kamek's broom's "bone" animation
	m3d::anmChr_c animationShieldChr;	//Kamek's shield's "bone" animation
	m3d::anmClr_c animationClr;			//Kamek's "color" animation
	m3d::anmTexSrt_c animationTexSrt;	//Kamek's shield's "texture" animation
	nw4r::g3d::ResFile resFile;			//kameck.arc -> kameck.brres
	mMtx shieldMatrix;
	ActivePhysics::Info HitMeBaby;
	ActivePhysics::Info HitMyShieldBaby;

	VEC3 initialMainPos;				//Initial position at the top of the room
	VEC3 shieldScale;					//Kamek's shield scale

	int timer;							//Timer
	char dying;							//Is Kamek dying. 1 = Yup | 0 = Nope
	char damage;						//Total damages
	char isInvulnerable;				//True if the boss can't be hit
	int phase;							//Which phase is the current one
	bool isBroomInvisible;				//True if Kamek's broom is invisible
	bool stopAutoUpdate;				//True to stop the automatic resetting of the "bone" animations when they're done
	int spawningtimer;					//Another timer
	int attacktimer;					//Another one
	int fiveshoottimer;					//Yet a different timer
	bool isVisible;						//True if Kamek is visible
	int doOneTime;						//Variable to be able to do only once something in the Execute-typed functions
	int shootingtimer;					//Still a timer
	int appearingtimer;					//This one isn't the same timer
	int damagecounter;					//Total damage bone animations done
	int whichanim;						//Which animation is the current one
	int fiveshootdirection;				//Direction of the five shoot attack. 1 = right | -1 = left
	int damagetimer;					//YES THIS IS ANOTHER ONE
	int fiveshootvariation;				//How many magic bullets to shoot. 0 = 3 | 1 = 4
	int currentattack;					//What attack are we currently on. 0 = NormalShoot | 1 = DoFiveFlyingShots
	int freezingchargingtimer;			//I love timers
	int freezingcounter;				//Action counter for the FreezePlayers attack
	bool playerStunned[4];				//Which players are stunned
	bool aremybulletsspawned;			//True if the ShotFromCorner's bullets were shoot
	int shotfromcornerdirection;		//direction used in the ShotFromCorner state. 0 = left | 1 = right
	int isridewantanimation;			//True if the current "bone" animation is the ride_wand "bone" animation
	int howmuchdidishoot;				//How much bullets were shoot in the ShotFromCorner state.
	int original_4;						//Outro stuff original flags
	int original_8;						//Outro stuff original flags
	int original_m;						//Outro stuff original flags
	bool doAttackNextRefresh;			//Will the next attack be done at the next animation end
	bool isShieldVisible;				//True if Kamek's shield is visible
	int shieldTimer;					//Guess what ? A timer !
	int secondShieldTimer;				//I wonder what can it be... Maybe a timer ?
	bool goBackToDamage;				//Used to go back to the Damage state after a DoFiveFlyingShots instead of going to the Attack state
	int waitAnotherSecond;				//Jeez
	bool arePlayersOnGround;			//For the panfare in the Outro state
	int playerOnGround[4];
	dStageActor_c *trampolineWall1;		//Trampoline Wall used by Kamek's shield
	dStageActor_c *trampolineWall2;		//Trampoline Wall used by Kamek's shield
	dStageActor_c *trampolineWall3;		//Trampoline Wall used by Kamek's shield
	dStageActor_c *magicplateform;		//Kamek's Magic Platform
	VEC3 magicPos;						//Kamek's Magic Platform's position
	
	static daBossKameck_c *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate, bool isBroom);	//Binds a "bone" animation (CHR) to either Kamek's broom or Kamek himself
	void bindAnimClr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);					//Binds a "color" animation (CLR) to Kamek
	void bindAnimToShield();																				//Binds a "bone" animation (CHR) and a "texture" animation to Kamek's Shield
	void setupBodyModel();																					//Setup the required models
	void updateModelMatrices();																				//Update the models' positions, scales and rotations

	bool doDisappear(int timer);																			//Make kamek disappear using its disappear animation
	void doAppear(int timer);																				//Make kamek appear using its appear animation

	void stunPlayers();																						//Stun the players on the ground
	void unstunPlayers();																					//Unstun the stunned players

	void createShield(int timer);																			//Make Kamek's shield appear
	void removeShield(int timer);																			//Make Kamek's shield disappear

	void changeBroomVisibility(bool visibility);															//Make Kamek's broom appear or not

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);									//Executed when the player touches Kamek
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);										//Executed when yoshi touches Kamek

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);							//Executed when a starman player touches Kamek
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);					//Executed when a fireball touches Kamek
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);					//Executed when an iceball touches Kamek
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);							//Executed when Kamek is goundpounded by the player
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);						//Executed when Kamek is goundpounded by yoshi
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);						//Executed when a rolling object (such as a shell) touches Kamek
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);								//Executed when a hammer touches Kamek
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);							//Executed when a fireball spli by yoshi touches Kamek
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);							//Executed when penguin mario touches Kamek

	USING_STATES(daBossKameck_c);			//State Declaring for daBossKameck_c
	DECLARE_STATE(Appear);					//Appearing State
	DECLARE_STATE(Spawn);					//Spawing State
	DECLARE_STATE(Attack);					//Attack Choosing State
	DECLARE_STATE(NormalShoot);				//Magic Bullet Shooting State
	DECLARE_STATE(DoFiveFlyingShots);		//Multiple Magic Bullet Shoot by Flying Kamek State
	DECLARE_STATE(FreezePlayers);			//Player Freezing State
	DECLARE_STATE(ShootFromBottomCorners);	//Multiple Magic Bullet Shoot from the bottom corners by flying Kamek State
	DECLARE_STATE(ShootFromTopCorners);		//Multiple Magic Bullet Shoot from the top corners by flying Kamek State
	DECLARE_STATE(Damage);					//Damage State
	DECLARE_STATE(ChangePhase);				//Phase Changing State
	DECLARE_STATE(Outro);					//Outro State
};


daBossKameck_c *daBossKameck_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossKameck_c));
	return new(buffer) daBossKameck_c;
}


														//State Creating for daBossKameck_c
CREATE_STATE(daBossKameck_c, Appear);					//Appearing State
CREATE_STATE(daBossKameck_c, Spawn);					//Spawing State
CREATE_STATE(daBossKameck_c, Attack);					//Attack Choosing State
CREATE_STATE(daBossKameck_c, NormalShoot);				//Magic Bullet Shooting State
CREATE_STATE(daBossKameck_c, DoFiveFlyingShots);		//Multiple Magic Bullet Shoot by Flying Kamek State
CREATE_STATE(daBossKameck_c, FreezePlayers);			//Player Freezing State
CREATE_STATE(daBossKameck_c, ShootFromBottomCorners);	//Multiple Magic Bullet Shoot from the bottom corners by flying Kamek State
CREATE_STATE(daBossKameck_c, ShootFromTopCorners);	//Multiple Magic Bullet Shoot from the top corners by flying Kamek State
CREATE_STATE(daBossKameck_c, Damage);					//Damage State
CREATE_STATE(daBossKameck_c, ChangePhase);				//Phase Changing State
CREATE_STATE(daBossKameck_c, Outro);					//Outro State


void daBossKameck_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {						//Executed when the player touches Kamek
	if(!isShieldVisible && !isInvulnerable) {
		char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);			//Get how was Kamek hit
		if(hitType == 1 || hitType == 3) {																	//If it was hit by a regular jump or a spin jump
			this->playEnemyDownSound1();																	//Play enemy hurting sound
			this->damage++;																					//Increase the damage count by one
			if(this->damage < 6) {																			//If the damages are under 6
				if(this->damage == 3) {																		//If it's the 3rd damage
					this->phase++;																			//Switch to phase 2
				}
				this->isInvulnerable = true;
				nw4r::snd::SoundHandle damageHandle;
				PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, ((this->damage < 4) ? SE_VOC_KMC_DAMAGE_L1 : SE_VOC_KMC_DAMAGE_L2), 1);
				doStateChange(&StateID_Damage);																//Go to the Damage State
			}
			if(this->damage == 6) {																			//If it's the 6th damage
				nw4r::snd::SoundHandle damageHandle;
				PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, SE_VOC_KMC_DAMAGE_L3, 1);
				doStateChange(&StateID_Outro);																//Go to the Outro State
			}
		}
		else if(hitType == 0) {																				//If it wasn't hit by the top in any way
			this->_vf220(apOther->owner);																	//Damage the player
		}
	}
	else {
		bouncePlayer(apOther->owner, 4.0f);
	}
}
void daBossKameck_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 						//Executed when yoshi touches Kamek
	this->playerCollision(apThis, apOther);																	//Do the same as the player collision
}
bool daBossKameck_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}
bool daBossKameck_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {	//Executed when a fireball touches Kamek
	return true;
}
bool daBossKameck_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {				//Need to document other things.
	bouncePlayer(apOther->owner, 2.0f);
	this->playEnemyDownSound1();																			//Play enemy hurting sound
	this->damage++;																							//Increase the damage count by one
	if(this->damage < 6) {																					//If the damages are under 6
		if(this->damage == 3) {																				//If it's the 3rd damage
			this->phase++;																					//Switch to phase 2
		}
		this->isInvulnerable = true;
		nw4r::snd::SoundHandle damageHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, ((this->damage < 4) ? SE_VOC_KMC_DAMAGE_L1 : SE_VOC_KMC_DAMAGE_L2), 1);
		doStateChange(&StateID_Damage);																		//Go to the Damage State
	}
	if(this->damage == 6) {																					//If it's the 6th damage
		nw4r::snd::SoundHandle damageHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, SE_VOC_KMC_DAMAGE_L3, 1);
		doStateChange(&StateID_Outro);																		//Go to the Outro State
	}
	return true;
}
bool daBossKameck_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameck_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameck_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daBossKameck_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameck_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameck_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}




void daBossKameck_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate, bool isBroom) { //Binds a "bone" animation (CHR) to either Kamek's broom or Kamek himself
	if(!isBroom) {																//If not binding to the broom model
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);			//Get the "bone" animation by name
		this->animationChr.bind(&this->bodyModel, anmChr, unk);					//Bind it to Kamek's "bone" animation (this->animationChr)
		this->bodyModel.bindAnim(&this->animationChr, unk2);					//Bind Kamek's "bone" animation (this->animationChr) to Kamek's model (this->bodyModel)
		this->animationChr.setUpdateRate(rate);									//Set the update rate of Kamek's "bone" animation
	}
	else {																		//Else (if binding to the broom model)
		nw4r::g3d::ResAnmChr anmBroomChr = this->resFile.GetResAnmChr(name);	//Get the "bone" animation by name
		this->animationBroomChr.bind(&this->broomModel, anmBroomChr, unk);		//Bind it to Kamek's Broom's "bone" animation (this->animationBroomChr)
		this->broomModel.bindAnim(&this->animationBroomChr, unk2);				//Bind Kamek's Broom's "bone" animation (this->animationBroomChr) to the Boom's model (this->broomModel)
		this->animationBroomChr.setUpdateRate(rate);							//Set the update rate of Kamek's Broom's "bone" animation
	}
}

void daBossKameck_c::bindAnimClr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) { //Binds a "color" animation (CLR) to Kamek
	nw4r::g3d::ResAnmClr anmRes = this->resFile.GetResAnmClr(name);				//Get the "color" animation by name
	this->animationClr.bind(&this->bodyModel, anmRes, 0, 0);					//Bind it to Kamek's "color" animation (this->animationClr)
	this->bodyModel.bindAnim(&this->animationClr, 0.0);							//Bind Kamek's "color" animation (this->animationClr) to Kamek's model (this->bodyModel)
}

void daBossKameck_c::bindAnimToShield() { //Binds the "float" bone animation and texture animation to Kamek's shield
	nw4r::g3d::ResAnmChr anmShieldChr = this->resFile.GetResAnmChr("float");	//Get the "bone" animation by name ("float")
	this->animationShieldChr.bind(&this->shieldModel, anmShieldChr, 1);			//Bind it to Kamek's shield's "bone" animation (this->animationShieldChr)
	this->shieldModel.bindAnim(&this->animationShieldChr, 0);					//Bind Kamek's shield's "bone" animation (this->animationShieldChr) to the Shield's model (this->shieldModel)
	this->animationShieldChr.setUpdateRate(1);									//Set the update rate of Kamek's shield's "bone" animation to 1


	nw4r::g3d::ResAnmTexSrt anmResss = this->resFile.GetResAnmTexSrt("float");	//Get the "texture" animation by name ("float")
	this->animationTexSrt.bindEntry(&this->shieldModel, anmResss, 0, 0);		//Bind it to Kamek's shield's "texture" animation (this->animationTexSrt)
	this->shieldModel.bindAnim(&this->animationTexSrt, 0.0);					//Bind Kamek's shield's "texture" animation (this->animationTexSrt) to the Shield's model (this->shieldModel)
}

void daBossKameck_c::setupBodyModel() { //Setup the required models
	allocator.link(-1, GameHeaps[0], 0, 0x20);												//Link the GameHeap0 allocator

	this->resFile.data = getResource("kameck", "g3d/kameck.brres");							//Get the kameck.brres file from kameck.arc from the game files

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kameck");								//Get the MDL0 file "kameck" from kameck.brres
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);											//Setup that model in Kamek's model (bodyModel)
	SetupTextures_Boss(&bodyModel, 0);														//Setup its shader (the boss shader in this case)

	nw4r::g3d::ResMdl broommdl = this->resFile.GetResMdl("kameck_broom");					//Get the MDL0 file "kameck_broom" from kameck.brres
	broomModel.setup(broommdl, &allocator, 0x224, 1, 0);									//Setup that model in Kamek's Broom's model (broomModel)
	SetupTextures_Boss(&broomModel, 0);														//Setup its shader (the boss shader in this case)

	nw4r::g3d::ResMdl shieldmdl = this->resFile.GetResMdl("balloon");						//Get the MDL0 file "balloon" from kameck.brres
	shieldModel.setup(shieldmdl, &allocator, 0x224, 1, 0);									//Setup that model in Kamek's shield model (shieldModel)
	SetupTextures_Boss(&shieldModel, 0);													//Setup its shader (the boss shader in this case)

	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("ride_wait");					//Get the CHR0 animation file "ride_wait" from kameck.brres
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);						//Setup it into Kamek's "bone" animation (this->animationChr)

	bool rett;
	nw4r::g3d::ResAnmChr anmBroomChr = this->resFile.GetResAnmChr("ride_wait_broom");		//Get the CHR0 animation file "ride_wait_broom" from kameck.brres
	rett = this->animationBroomChr.setup(broommdl, anmBroomChr, &this->allocator, 0);		//Setup it into Kamek's Broom's "bone" animation (this->animationBroomChr)

	bool rettt;
	nw4r::g3d::ResAnmChr anmShieldChr = this->resFile.GetResAnmChr("float");				//Get the CHR0 animation file "float" from kameck.brres
	rettt = this->animationShieldChr.setup(shieldmdl, anmShieldChr, &this->allocator, 0);	//Setup it into Kamek's sheld's "bone" animation (this->animationShieldChr)
	
	nw4r::g3d::ResAnmClr anmRess = this->resFile.GetResAnmClr("wand");						//Get the CLR0 animation file "wand" from kameck.brres
	this->animationClr.setup(mdl, anmRess, &this->allocator, 0, 1);							//Setup it into Kamek's "color" animation (this->animationClr)
	this->animationClr.bind(&this->bodyModel, anmRess, 0, 0);								//Bind it into Kamek's "color" animation (this->animationClr)
	this->bodyModel.bindAnim(&this->animationClr, 0.0);										//Bind Kamek's "color" animation (this->animationClr) to Kamek's model (this->bodyModel)

	bool retttt;
	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("float");				//Get the SRT0 animation file "float" from kameck.brres
	retttt = this->animationTexSrt.setup(shieldmdl, anmSrt, &this->allocator, 0, 1);		//Setup it into Kamek's shield's "texture" animation (this->animationTexSrt)

	allocator.unlink();																		//Unlink the GameHeap0 allocator
}
int daBossKameck_c::onCreate() { //Executed when the sprite spawns
	setupBodyModel();														//Setup the required models


	this->scale = (Vec){0.0, 0.0, 0.0};										//Set Kamek's scale to 0
	this->removeShield(30);													//Remove kamek's shield


	HitMeBaby.xDistToCenter = 0.0;											//Set Kamek's Collision's X distance from center to 0
	HitMeBaby.yDistToCenter = 15.0;											//Set Kamek's Collision's Y distance from center to 15

	HitMeBaby.xDistToEdge = 10.0;											//Set Kamek's Collision's X scale to 10
	HitMeBaby.yDistToEdge = 15.0;											//Set Kamek's Collision's X scale to 15

	HitMeBaby.category1 = 0x3;												//Ask Treeki if you want to know what that means. it's been years since i try to figure it out and i still don't know.
	HitMeBaby.category2 = 0x0;												//^
	HitMeBaby.bitfield1 = 0x4F;												//^
	HitMeBaby.bitfield2 = 0x8028E;											//^
	HitMeBaby.unkShort1C = 0;												//^
	HitMeBaby.callback = &dEn_c::collisionCallback;							//Set the collision callback to the dEn_c's one


	this->aPhysics.initWithStruct(this, &HitMeBaby);						//Init the collision
	this->aPhysics.addToList();												//Add it to the collision list


	HitMyShieldBaby.xDistToCenter = 0.0;									//Set Kamek's shield's Collision's X distance from center to 0
	HitMyShieldBaby.yDistToCenter = 44.0;									//Set Kamek's shield's Collision's Y distance from center to 44

	HitMyShieldBaby.xDistToEdge = 22.5;										//Set Kamek's shield's Collision's X scale to 22.5
	HitMyShieldBaby.yDistToEdge = 3.0;										//Set Kamek's shield's Collision's X scale to 3

	HitMyShieldBaby.category1 = 0x3;										//Ask Treeki if you want to know what that means. it's been years since i try to figure it out and i still don't know.
	HitMyShieldBaby.category2 = 0x0;										//^
	HitMyShieldBaby.bitfield1 = 0x4F;										//^
	HitMyShieldBaby.bitfield2 = 0x8028E;									//^
	HitMyShieldBaby.unkShort1C = 0;											//^
	HitMyShieldBaby.callback = &dEn_c::collisionCallback;					//Set the collision callback to the dEn_c's one

	
	// this->pos.y = this->pos.y + 6;
	this->rot.x = 0; // X is vertical axis									//Set Kamek's X rotation to 0
	this->rot.z = 0; // Z is ... an axis >.>								//Set Kamek's Z rotation to 0

	
	this->speed.x = 0;														//Set Kamek's X speed to 0
	imded = 0;																//Set Kamek's global death flag to 0
	this->damage = 0;														//Set Kamek's damages to 0
	this->isInvulnerable = 0;												//Set Kamek's invulnerable flag to 0 (false)
	this->disableEatIn();													//Make Kamek not edible by yoshi so cheaters are disappointed
	this->phase = 1;														//Set Kamek's phase to 1
	this->isVisible = true;													//Set Kamek's visibility flag to 1
	this->doAttackNextRefresh = false;										//Set kamek's doAttackNextRefresh flag to false
	this->goBackToDamage = false;											//Set kamek's goBackToDamage flag to false

	bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);			//Bind the "ride_wait" bone animation to Kamek
	bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);	//Bind the "ride_wait_broom" bone animation to Kamek's Broom
	bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 2.0);						//Bind the "wand" color animation to Kamek
	bindAnimToShield();														//Binds the "float" bone animation and texture animation to Kamek's shield
		
	doStateChange(&StateID_Spawn);											//Go in the Spawn state

	this->onExecute();														//Starts the onExecute loop
	return true;
}

int daBossKameck_c::onDelete() { //Executed when the sprite is being deleted
	return true;
}

int daBossKameck_c::onExecute() { //Executed every frame after the onCreate is executed
	acState.execute();															//Execute the current state
	updateModelMatrices();														//Update the models' positions, scales and rotations
	bodyModel._vf1C();															//Update the "bone" animation bound to Kamek's model (bodyModel)
	broomModel._vf1C();															//Update the "bone" animation bound to Kamek's Broom's model (broomModel)
	shieldModel._vf1C();														//Update the "bone" animation bound to Kamek's shield's model (shieldModel)

	if(this->animationChr.isAnimationDone() && !this->stopAutoUpdate) {			//If the current Kamek's "bone" animation is done playing and the stopAutoUpdate flag is disabled
		this->animationChr.setCurrentFrame(0.0);								//Set the current frame of Kamek's "bone" animation to 0
	}

	if(this->animationBroomChr.isAnimationDone() && !this->stopAutoUpdate) {	//If the current Kamek's Broom's "bone" animation is done playing and the stopAutoUpdate flag is disabled
		this->animationBroomChr.setCurrentFrame(0.0);							//Set the current frame of Kamek's Broom's "bone" animation to 0
	}

	if(this->animationShieldChr.isAnimationDone()) {							//If the current Kamek's shield's "bone" animation is done playing
		this->animationBroomChr.setCurrentFrame(0.0);							//Set the current frame of Kamek's shield's "bone" animation to 0
	}

	if(this->isShieldVisible) {													//If Kamek's shield is visible
		this->shieldTimer++;													//Increment the shield timer
		if(this->shieldTimer > 120) {											//If the shield timer is >120
			this->removeShield(this->secondShieldTimer);						//Remove the shield progressively using the second shield timer
			this->secondShieldTimer++;											//Increment the second shield timer
			if(this->secondShieldTimer > 30) {									//If the second shield timer is >30
				this->shieldTimer = 0;											//Set the shield timer to 0
				this->secondShieldTimer = 0;									//Set the second shield timer to 0
				this->isShieldVisible = false;									//Set the isShieldVisible flag to false
			}
		}
	}
	
	return true;
}


int daBossKameck_c::onDraw() { //Executed every frame, handles the rendering
	bodyModel.scheduleForDrawing();			//Shedule Kamek's model (bodyModel) drawing
	if(this->isBroomInvisible) {			//If Kamek's Broom is visible
		broomModel.scheduleForDrawing();	//Shedule Kamek's Broom's model (broomModel) drawing
	}
	shieldModel.scheduleForDrawing();		//Shedule Kamek's shield's model (shieldModel) drawing
	this->animationClr.process();			//Process Kamek's "color" animation
	this->animationTexSrt.process();		//Process Kamek's shield's "texture" animation
	return true;
}


void daBossKameck_c::updateModelMatrices() { //Update the models' positions, scales and rotations
	// This won't work with wrap because I'm lazy.				//This comment was wrote by Treeki, I leave it there because it looks weird otherwise :c
	matrix.translation(pos.x, pos.y, pos.z);					//Update the main drawing matrix's position with this sprite's position
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);			//Update the main drawing matrix's rotation with this sprite's rotation

	bodyModel.setDrawMatrix(matrix);							//Set Kamek's model (bodyModel) draw matrix to the main drawing matrix
	bodyModel.setScale(&scale);									//Set Kamek's model's (bodyModel) scale to this sprite's scale
	bodyModel.calcWorld(false);									//Ask Treeki if you want to know what that means. it's been years since i try to figure it out and i still don't know.

	if(this->isBroomInvisible) {								//If Kamek's Broom is visible
		broomModel.setDrawMatrix(matrix);						//Set Kamek's Broom's model (broomModel) draw matrix to the main drawing matrix
		broomModel.setScale(&scale);							//Set Kamek's Broom's model's (broomModel) scale to this sprite's scale
		broomModel.calcWorld(false);							//Ask Treeki if you want to know what that means. it's been years since i try to figure it out and i still don't know.
	}

	shieldMatrix.translation(pos.x, pos.y + 20, pos.z + 200);	//Update the shield drawing matrix's position with this sprite's position but with some edits on it (+20 on Y position, +200 on Z position)
	shieldMatrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);		//Update the shield drawing matrix's rotation with this sprite's rotation
	shieldModel.setDrawMatrix(shieldMatrix);					//Set Kamek's shield's model (shieldModel) draw matrix to the shield drawing matrix
	shieldModel.setScale(&shieldScale);							//Set Kamek's shield's model's (shieldModel) scale to the shield's scale
	shieldModel.calcWorld(false);								//Ask Treeki if you want to know what that means. it's been years since i try to figure it out and i still don't know.
}

void daBossKameck_c::changeBroomVisibility(bool visibility) { //Make Kamek's broom appear or not
	bindAnimChr_and_setUpdateRate(((visibility) ? "ride_wait_broom" : "disappear_broom"), 1, 0.0, 1.0, true);	//Bind to Kamek's Broom either the "ride_wait_broom" CHR0 animation or the "disappear_broom" CHR0 animation
	this->isBroomInvisible = visibility;																		//Set the isBroomInvisible flag
}

void daBossKameck_c::createShield(int timer) { //Make Kamek's shield appear
	if(timer < 30) {																								//If the timer is <30
		this->shieldScale.x += 0.04;																				//Add 0.04 to Kamek's shield's scale on the X axis
		this->shieldScale.y += 0.04;																				//Add 0.04 to Kamek's shield's scale on the Y axis
		this->shieldScale.z += 0.04;																				//Add 0.04 to Kamek's shield's scale on the Z axis
	}
	if(timer == 30) {																								//If the timer is 30
		this->shieldScale = (Vec){1.3, 1.3, 1.3};																	//Set Kamek's shield's scale to 1.3 on all axes
		VEC3 basePos = {this->initialMainPos.x - 8, this->initialMainPos.y + 44, this->initialMainPos.z + 200};		//Set the basePos VEC3 to this->initialMainPos but with some edits (-8 on X position, +44 on Y position, +200 on Z position)
		VEC3 spawningPos1 = {basePos.x, basePos.y, basePos.z};														//Set the first spawning position VEC3 to the basePos VEC3
		VEC3 spawningPos2 = {basePos.x - 16, basePos.y, basePos.z};													//Set the second spawning position VEC3 to the basePos VEC3 but with some edits (-16 on X position)
		VEC3 spawningPos3 = {basePos.x + 16, basePos.y, basePos.z};													//Set the third spawning position VEC3 to the basePos VEC3 but with some edits (+16 on X position)
		S16Vec nullRot = {0,0,0};																					//Set the nullRot S16Vec to 0 on all axes
		trampolineWall1 = createChild(EN_TRPLN_WALL, this, (0 | (2 << 0)), &spawningPos1, &nullRot, 0);				//Create a Trampoline Wall with 2 on the nybble 12 of its settings, the first spawning position as its position, nullRot as its rotation and 0 as its layer
		trampolineWall2 = createChild(EN_TRPLN_WALL, this, (0 | (2 << 0)), &spawningPos2, &nullRot, 0);				//Create a Trampoline Wall with 2 on the nybble 12 of its settings, the second spawning position as its position, nullRot as its rotation and 0 as its layer
		trampolineWall3 = createChild(EN_TRPLN_WALL, this, (0 | (2 << 0)), &spawningPos3, &nullRot, 0);				//Create a Trampoline Wall with 2 on the nybble 12 of its settings, the third spawning position as its position, nullRot as its rotation and 0 as its layer
		trampolineWall1->scale = (Vec){0.0, 0.0, 0.0};																//Set the first Trampoline Wall's scale to 0 on all axes
		trampolineWall2->scale = (Vec){0.0, 0.0, 0.0};																//Set the second Trampoline Wall's scale to 0 on all axes
		trampolineWall3->scale = (Vec){0.0, 0.0, 0.0};																//Set the third Trampoline Wall's scale to 0 on all axes
		this->aPhysics.removeFromList();																			//Remove Kamek's collision from the collision list
		this->aPhysics.initWithStruct(this, &HitMyShieldBaby);														//Initiate it with the shield collision instead
		this->aPhysics.addToList();																					//Readd Kamek's collision to the collision list
	}
}

void daBossKameck_c::removeShield(int timer) { //Make Kamek's shield disappear
	if(timer == 1) {											//If the timer is 1
		if(this->isShieldVisible) {								//If Kamek's shield is visible
			this->aPhysics.removeFromList();					//Remove Kamek's collision from the collision list
			this->aPhysics.initWithStruct(this, &HitMeBaby);	//Initiate it with the main collision instead
			this->aPhysics.addToList();							//Readd Kamek's collision to the collision list
			trampolineWall1->Delete(1);							//Delete the first Trampoline Wall
			trampolineWall2->Delete(1);							//Delete the second Trampoline Wall
			trampolineWall3->Delete(1);							//Delete the third Trampoline Wall
		}
	}
	if(timer < 30) {											//If the timer is <30
		this->shieldScale.x -= 0.04;							//Subtract 0.04 to Kamek's shield's scale on the X axis
		this->shieldScale.y -= 0.04;							//Subtract 0.04 to Kamek's shield's scale on the Y axis
		this->shieldScale.z -= 0.04;							//Subtract 0.04 to Kamek's shield's scale on the Z axis
	}
	if(timer == 30) {											//If the timer is 30
		this->shieldScale = (Vec){0.0, 0.0, 0.0};				//Set Kamek's shield's scale to 0 on all axes
		this->isShieldVisible = false;							//Set the isShieldVisible flag to false
		this->isInvulnerable = false;							//Set the isInvulnerable flag to false
	}
}

bool daBossKameck_c::doDisappear(int timer) { //Make kamek disappear using its disappear animation
	if(this->isVisible) {																				//If Kamek is visible
		if(timer == 0) {																				//If the timer is 0
			nw4r::snd::SoundHandle disappearHandle;														//Create a SoundHandle for Kamek's Disappearing SFX
			PlaySoundWithFunctionB4(SoundRelatedClass, &disappearHandle, SE_BOSS_KAMECK_DISAPP, 1);		//Play Kamek's Disappearing SFX using the previously created SoundHandle
			changeBroomVisibility(false);																//Make Kamek's Broom invisible
			bindAnimChr_and_setUpdateRate("disappear", 1, 0.0, 1.0, false);								//Bind the "disappear" bone animation to Kamek
		}
		if(timer > 0) {																					//If the timer is >0
			stopAutoUpdate = true;																		//Stop the automatic CHR0 animation update
			if(this->animationChr.isAnimationDone()) {													//When the current Kamek's "bone" animation is done (in this case, the "disappear" bone animation)
				stopAutoUpdate = false;																	//Re-enable the automatic CHR0 animation update
				isVisible = false;																		//Set Kamek's visibility flag (isVisible) to false
				if(this->magicplateform != 0) {															//If there is a Magic Platform
					this->magicplateform->Delete(1);													//Delete it
				}
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		return true;
	}
}

void daBossKameck_c::doAppear(int timer) { //Make kamek appear using its appear animation
	if(!this->isVisible) {																					//If Kamek isn't visible
		if(timer == 0) {																					//If the timer is 0
			this->rot.y = 0;																				//Set Kamek's Y Rotation to 0
			this->pos = this->initialMainPos;																//Set Kamek's position to this->initialMainPos
			nw4r::snd::SoundHandle appearHandle;															//Create a SoundHandle for Kamek's Appearing SFX
			PlaySoundWithFunctionB4(SoundRelatedClass, &appearHandle, SE_BOSS_KAMECK_APP, 1);				//Play Kamek's Appearing SFX using the previously created SoundHandle
			changeBroomVisibility(false);																	//Make Kamek's Broom invisible
			S16Vec nullRot = {0,0,0};																		//Set the nullRot S16Vec to 0 on all axes
			magicplateform = createChild(EN_SLIP_PENGUIN, this, 16777217, &this->magicPos, &nullRot, 0);	//Create a Magic Platform with some settings that are too long to detail, this->magicPos as its position, nullRot as its rotation and 0 as its layer
			bindAnimChr_and_setUpdateRate("appear", 1, 0.0, 1.0, false);									//Bind the "appear" bone animation to Kamek
		}
		if(timer > 0) {																						//If the timer is >0
			stopAutoUpdate = true;																			//Stop the automatic CHR0 animation update
			if(this->animationChr.isAnimationDone()) {														//When the current Kamek's "bone" animation is done (in this case, the "appear" bone animation)
				stopAutoUpdate = false;																		//Re-enable the automatic CHR0 animation update
				isVisible = true;																			//Set Kamek's visibility flag (isVisible) to true
				bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 1.0, false);							//Bind the "throw_st_wait" bone animation to Kamek
				changeBroomVisibility(false);																//Make Kamek's Broom invisible
				doStateChange(&StateID_Attack);																//Go to the Attack State
				return;
			}
			else {
				return;
			}
		}
	}
	else {
		return;
	}
}


/***********************************************/
/**********I won't document the states,*********/
/******that would be waaaaay too long to do*****/
/**so i'll go documenting other files instead.**/
/***********************************************/


/*************/
/*Spawn State*/
/*************/

void daBossKameck_c::beginState_Spawn() {

}

void daBossKameck_c::executeState_Spawn() { 
	this->spawningtimer++;
	if(this->spawningtimer > 700) {
		doStateChange(&StateID_Appear);
	}
}

void daBossKameck_c::endState_Spawn() { 
	
}

/**************/
/*Appear State*/
/**************/

void daBossKameck_c::beginState_Appear() {
	this->pos.x -= 119;
	this->pos.y += 151; 
	this->initialMainPos = this->pos;

	this->magicPos = {this->initialMainPos.x - 17, this->initialMainPos.y + 1, this->initialMainPos.z};

	this->scale = (Vec){1.0, 1.0, 1.0};
	changeBroomVisibility(false);
	bindAnimChr_and_setUpdateRate("appear", 1, 0.0, 1.0, false);
	S16Vec nullRot = {0,0,0};
	magicplateform = createChild(EN_SLIP_PENGUIN, this, 16777217, &this->magicPos, &nullRot, 0);
	nw4r::snd::SoundHandle appearHandle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &appearHandle, SE_BOSS_KAMECK_APP, 1);
	this->stopAutoUpdate = true;
}

void daBossKameck_c::executeState_Appear() { 
	if(this->animationChr.isAnimationDone()) {
		bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 1.0, false);
		doStateChange(&StateID_Attack);
	}
}

void daBossKameck_c::endState_Appear() { 

}

/**************/
/*Attack State*/
/**************/
//THIS PART IS FROM THE GIANT GOOMBA AAAAAAAAAAAAAAAAAAAAAAAAAAAAA. -RedStoneMatt 11/10/2020
extern "C" void stunPlayer(void *, int);
extern "C" void unstunPlayer(void *);

void daBossKameck_c::stunPlayers() { 
	for (int i = 0; i < 4; i++) {
		playerStunned[i] = false;

		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player) {
			if (player->collMgr.isOnTopOfTile() && player->currentZoneID == currentZoneID) {
				S16Vec nullRot = {0,0,0};
				VEC3 vecOne = {1.0f, 1.0f, 1.0f};
				SpawnEffect("Wm_ob_powdown", 0, &player->pos, &nullRot, &vecOne);
				stunPlayer(player, 1);
				playerStunned[i] = true;
			}
		}
	}
}

void daBossKameck_c::unstunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && playerStunned[i]) {
			unstunPlayer(player);
		}
	}
}
//END OF THE GIANT GOOMBA PART AAAAAAAAAAAAAAAAAAAAAAAAAAAAA. -RedStoneMatt 11/10/2020

void daBossKameck_c::beginState_Attack() { 
	this->stopAutoUpdate = false;
	this->attacktimer = 0;
	this->rot.y = 0;
	this->shotfromcornerdirection = 0;
	this->pos = this->initialMainPos;
	this->doAttackNextRefresh = false;
}

void daBossKameck_c::executeState_Attack() { 
	this->attacktimer++;
	int waitingtime[5] = {150, 180, 110, 180, 180};
	if(this->attacktimer == waitingtime[this->currentattack]) {
		this->doAttackNextRefresh = true;
		this->stopAutoUpdate = true;
	}
	if(this->doAttackNextRefresh && this->animationChr.isAnimationDone()) {
		this->stopAutoUpdate = false;
		this->attacktimer = 0;
		this->doAttackNextRefresh = false;
		unstunPlayers();
		if(this->phase == 1) {
			int whichAttack = GenerateRandomNumber(100);
			if(whichAttack > 10) {
				this->currentattack = 0;
				doStateChange(&StateID_NormalShoot);
			}
			else {
				this->currentattack = 1;
				doStateChange(&StateID_DoFiveFlyingShots);
			}
		}
		if(this->phase == 2) {
			int whichAttack = GenerateRandomNumber(100);
			if(whichAttack < 75) {
				this->currentattack = 2;
				doStateChange(&StateID_FreezePlayers);
			}
			if(whichAttack > 74 && whichAttack < 90) {
				this->currentattack = 3;
				doStateChange(&StateID_ShootFromBottomCorners);
			}
			if(whichAttack > 89 && whichAttack < 100) {
				this->currentattack = 4;
				doStateChange(&StateID_ShootFromTopCorners);
			}
		}
	}
}

void daBossKameck_c::endState_Attack() { 
	
}

/*******************/
/*NormalShoot State*/
/*******************/

void daBossKameck_c::beginState_NormalShoot() { 
	bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 2.0, false);
	this->stopAutoUpdate = true;
	this->whichanim = 1;
	this->shootingtimer = 0;
}

void daBossKameck_c::executeState_NormalShoot() { 
	// OSReport("current animation: %d, current frame: %d\n", this->whichanim, this->animationChr.getCurrentFrame());
	if(this->animationChr.isAnimationDone() && this->whichanim == 1) {
		bindAnimChr_and_setUpdateRate("throw_ed", 1, 0.0, 2.0, false);
		this->whichanim = 2;
	}
	if(this->whichanim == 2) {
		this->shootingtimer++;
	}
	if(this->shootingtimer == 15) {
		S16Vec nullRot = {0,0,0};
		VEC3 spawningPos = {this->pos.x - 4, this->pos.y + 16, this->pos.z + 100};
		dStageActor_c *bullet = createChild(KAMECK_MAGIC, this, 0, &spawningPos, &nullRot, 0);

		u8 facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

		float totalSpeed = 1.0f;

		dAc_Py_c *mario = GetSpecificPlayerActor(0);
		float ratio = (this->pos.y - mario->pos.y) / (this->pos.x - mario->pos.x);
		bullet->speed.x = sqrtf(totalSpeed * totalSpeed / (1 + ratio * ratio));
		if(facing == 1) {
			bullet->speed.x = bullet->speed.x * -1;
		}
		bullet->speed.y = ratio * bullet->speed.x;

	}
	if(this->shootingtimer == 30) {
		this->stopAutoUpdate = false;
		bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 2.0, false);
		this->whichanim = 1;
		doStateChange(&StateID_Attack);
	}
}

void daBossKameck_c::endState_NormalShoot() { 
	
}

/*************************/
/*DoFiveFlyingShots State*/
/*************************/

void daBossKameck_c::beginState_DoFiveFlyingShots() { 
	this->doOneTime = 0;
	this->appearingtimer = 0;
	this->fiveshoottimer = 0;
	this->shootingtimer = 0;
	this->fiveshootdirection = ((GenerateRandomNumber(2) == 0) ? -1 : 1);
	this->fiveshootvariation = GenerateRandomNumber(2);
}

void daBossKameck_c::executeState_DoFiveFlyingShots() { 
	bool ret = doDisappear(this->fiveshoottimer);
	if((ret && (this->waitAnotherSecond > 60)) || !ret) {
		this->fiveshoottimer++;
	}
	if(ret) {
		if(this->waitAnotherSecond < 61) {
			this->waitAnotherSecond++;
		}
		else {
			if(this->doOneTime == 0) {
				this->fiveshoottimer = 0;
				this->pos.x -= 264 * this->fiveshootdirection;
				changeBroomVisibility(true);
				this->rot.y = ((this->fiveshootdirection == 1) ? 0x2800 : 0xD800);
				bindAnimChr_and_setUpdateRate("demo_castleB", 1, 0.0, 2.0, false);
				bindAnimChr_and_setUpdateRate("demo_castleB_broom", 1, 0.0, 2.0, true);
				this->doOneTime = 1;
			}
			if(fiveshoottimer < 71) {
				this->pos.x += 8 * this->fiveshootdirection;
				this->shootingtimer++;
				if(this->shootingtimer > ((this->fiveshootvariation == 0) ? 16 : 12)) {
					S16Vec nullRot = {0,0,0};
					dStageActor_c *bullet = createChild(KAMECK_MAGIC, this, 0, &this->pos, &nullRot, 0);
					bullet->speed.x = 0;
					bullet->speed.y = -2;
					this->shootingtimer = 0;
				}
			}
			if(fiveshoottimer < 36) {
				this->pos.y -= 0.4;
			}
			if(fiveshoottimer > 35 && fiveshoottimer < 71) {
				this->pos.y += 0.4;
			}
			if(fiveshoottimer > 70) {
				if(this->goBackToDamage) {
					// OSReport("nyeh3\n");
					doStateChange(&StateID_Damage);
					return;
				}
				// OSReport("nyeh4\n");
				doAppear(this->appearingtimer);
				this->appearingtimer++;
			}
		}
	}
}

void daBossKameck_c::endState_DoFiveFlyingShots() { 
	
}

/******************************/
/*ShootFromBottomCorners State*/
/******************************/
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

void daBossKameck_c::beginState_ShootFromBottomCorners() { 
	this->doOneTime = 0;
	this->appearingtimer = 0;
	this->fiveshoottimer = 0;
	this->shootingtimer = 0;
	this->aremybulletsspawned = false;
	this->stopAutoUpdate = false;
	this->isridewantanimation = 0;
}

void daBossKameck_c::executeState_ShootFromBottomCorners() { 
	bool ret = doDisappear(this->fiveshoottimer);
	this->fiveshoottimer++;
	if(ret) {
		if(this->doOneTime == 0) {
			this->fiveshoottimer = 0;
			this->pos.x = this->initialMainPos.x - ((this->shotfromcornerdirection == 0) ? 296 : -296);
			this->pos.y = this->initialMainPos.y - 174;
			changeBroomVisibility(true);
			this->rot.y = ((this->shotfromcornerdirection == 0) ? 0x2800 : 0xD800);
			bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
			bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);
			this->doOneTime = 1;
		}
		if(fiveshoottimer > 18 && this->isridewantanimation == 0) {
			bindAnimChr_and_setUpdateRate("ride_wand", 1, 0.0, 1.0, false);
			this->isridewantanimation = 1;
		}
		if(fiveshoottimer > 62 && this->isridewantanimation == 1) {
			bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
			this->isridewantanimation = 2;
		}
		if(fiveshoottimer < 41) {
			this->pos.x += ((this->shotfromcornerdirection == 0) ? 2.4 : -2.4);
			this->pos.y -= 0.25;
		}
		if(fiveshoottimer > 41 && this->aremybulletsspawned == false) {
			S16Vec nullRot = {0,0,0};
			dStageActor_c *bulletRight = createChild(KAMECK_MAGIC, this, 0, &this->pos, &nullRot, 0);
			bulletRight->speed.x = ((this->shotfromcornerdirection == 0) ? 2 : -2);
			bulletRight->speed.y = 0.4;
			dStageActor_c *bulletUp = createChild(KAMECK_MAGIC, this, 0, &this->pos, &nullRot, 0);
			bulletUp->speed.x = ((this->shotfromcornerdirection == 0) ? 0.4 : -0.4);
			bulletUp->speed.y = 2;
			dStageActor_c *bulletDiagonal = createChild(KAMECK_MAGIC, this, 0, &this->pos, &nullRot, 0);
			bulletDiagonal->speed.x = ((this->shotfromcornerdirection == 0) ? 2 : -2);
			bulletDiagonal->speed.y = 2;
			this->aremybulletsspawned = true;
		}
		if(fiveshoottimer > 49 && fiveshoottimer < 71) {
			this->rot.y -= ((this->shotfromcornerdirection == 0) ? 0x3B3 : -0x3B3);
		}
		if(fiveshoottimer > 70 && fiveshoottimer < 111) {
			this->pos.x -= ((this->shotfromcornerdirection == 0) ? 2.4 : -2.4);
			this->pos.y += 0.25;
		}
		if(fiveshoottimer > 110) {
			if(this->shotfromcornerdirection == 1) {
				changeBroomVisibility(false);
				doAppear(this->appearingtimer);
				this->appearingtimer++;
				// OSReport("this->appearingtimer = %d\n", this->appearingtimer);
			}
		}
		if(fiveshoottimer > 230) {
			if(this->shotfromcornerdirection == 0) {
				this->shotfromcornerdirection = 1;
				doStateChange(&StateID_ShootFromBottomCorners);
				return;
			}
		}
	}
}

void daBossKameck_c::endState_ShootFromBottomCorners() { 
	
}

/******************************/
/*ShootFromTopCorners State*/
/******************************/
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

void daBossKameck_c::beginState_ShootFromTopCorners() { 
	this->doOneTime = 0;
	this->appearingtimer = 0;
	this->fiveshoottimer = 0;
	this->shootingtimer = 0;
	this->aremybulletsspawned = false;
	this->howmuchdidishoot = 0;
	this->isridewantanimation = 0;
}

void daBossKameck_c::executeState_ShootFromTopCorners() { 
	bool ret = doDisappear(this->fiveshoottimer);
	this->fiveshoottimer++;
	if(ret) {
		if(this->doOneTime == 0) {
			this->fiveshoottimer = 0;
			this->pos.x = this->initialMainPos.x - ((this->shotfromcornerdirection == 0) ? 296 : -296);
			// this->pos.y = this->initialMainPos.y - 174;
			changeBroomVisibility(true);
			this->rot.y = ((this->shotfromcornerdirection == 0) ? 0x2800 : 0xD800);
			bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
			bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);
			this->doOneTime = 1;
		}
		if(fiveshoottimer > 18 && this->isridewantanimation == 0) {
			bindAnimChr_and_setUpdateRate("ride_wand", 1, 0.0, 1.0, false);
			this->isridewantanimation = 1;
		}
		if(fiveshoottimer > 62 && this->isridewantanimation == 1) {
			bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
			this->isridewantanimation = 2;
		}
		if(fiveshoottimer < 41) {
			this->pos.x += ((this->shotfromcornerdirection == 0) ? 2.4 : -2.4);
			this->pos.y -= 0.25;
		}
		if(fiveshoottimer > 41 && this->aremybulletsspawned == false) {
			S16Vec nullRot = {0,0,0};
			if(howmuchdidishoot == 0 && this->fiveshoottimer > 41) {
				dStageActor_c *bulletSide = createChild(KAMECK_MAGIC, this, 0, &this->pos, &nullRot, 0);
				bulletSide->speed.x = ((this->shotfromcornerdirection == 0) ? 2 : -2);
				bulletSide->speed.y = -0.4;
				howmuchdidishoot++;
			}
			if(howmuchdidishoot == 1 && this->fiveshoottimer > 45) {
				dStageActor_c *bulletDiagonal = createChild(KAMECK_MAGIC, this, 0, &this->pos, &nullRot, 0);
				bulletDiagonal->speed.x = ((this->shotfromcornerdirection == 0) ? 2 : -2);
				bulletDiagonal->speed.y = -2;
				howmuchdidishoot++;
			}
			if(howmuchdidishoot == 2 && this->fiveshoottimer > 49) {
				dStageActor_c *bulletDown = createChild(KAMECK_MAGIC, this, 0, &this->pos, &nullRot, 0);
				bulletDown->speed.x = ((this->shotfromcornerdirection == 0) ? 0.4 : -0.4);
				bulletDown->speed.y = -2;
				howmuchdidishoot++;
			}
		}
		if(fiveshoottimer > 49 && fiveshoottimer < 71) {
			this->rot.y -= ((this->shotfromcornerdirection == 0) ? 0x3B3 : -0x3B3);
		}
		if(fiveshoottimer > 70 && fiveshoottimer < 111) {
			this->pos.x -= ((this->shotfromcornerdirection == 0) ? 2.4 : -2.4);
			this->pos.y += 0.25;
		}
		if(fiveshoottimer > 110) {
			if(this->shotfromcornerdirection == 1) {
				changeBroomVisibility(false);
				doAppear(this->appearingtimer);
				this->appearingtimer++;
				// OSReport("this->appearingtimer = %d\n", this->appearingtimer);
			}
		}
		if(fiveshoottimer > 230) {
			if(this->shotfromcornerdirection == 0) {
				this->shotfromcornerdirection = 1;
				doStateChange(&StateID_ShootFromTopCorners);
				return;
			}
		}
	}
}

void daBossKameck_c::endState_ShootFromTopCorners() { 
	
}

/*********************/
/*FreezePlayers State*/
/*********************/

void daBossKameck_c::beginState_FreezePlayers() { 
	this->stopAutoUpdate = true;
	this->freezingcounter = 0;
	this->freezingchargingtimer = 0;
	bindAnimChr_and_setUpdateRate("stickup", 1, 0.0, 2.0, false);
	bindAnimClr_and_setUpdateRate("stickup", 1, 0.0, 2.0);
}

void daBossKameck_c::executeState_FreezePlayers() { 
	this->freezingchargingtimer++;
	if(this->freezingchargingtimer > 30 && this->freezingcounter == 0) {
		bindAnimChr_and_setUpdateRate("stickup_wait", 1, 0.0, 1.0, false);
		this->stopAutoUpdate = false;
		this->freezingcounter = 1;
	}
	if(this->freezingchargingtimer > 59 && this->freezingcounter == 1) {
		bindAnimClr_and_setUpdateRate("stickup_wait", 1, 0.0, 1.0);
		this->freezingcounter = 2;
	}
	if(this->freezingchargingtimer > 90 && this->freezingcounter == 2) {
		bindAnimChr_and_setUpdateRate("throw_ed", 1, 0.0, 1.0, false);
		this->freezingcounter = 3;
	}
	if(this->freezingchargingtimer > 105 && this->freezingcounter == 3) {
		ShakeScreen(StageScreen, 0, 1, 0, 0);
		stunPlayers();
		nw4r::snd::SoundHandle powHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &powHandle, SE_OBJ_POW_BLOCK_QUAKE, 1);
		S16Vec nullRot = {0,0,0};
		VEC3 vecOne = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_ob_powdown_ind", 0, &this->pos, &nullRot, &vecOne);
		this->freezingcounter = 4;
	}
	if(this->freezingchargingtimer > 120 && this->freezingcounter == 4) {
		bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 2.0);
		bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 2.0, false);
		this->stopAutoUpdate = false;
		this->freezingcounter = 5;
	}
	if(this->freezingchargingtimer > 190 && this->freezingcounter == 5) {
		doStateChange(&StateID_NormalShoot);
	}
}

void daBossKameck_c::endState_FreezePlayers() { 
	
}

/**************/
/*Damage State*/
/**************/

void daBossKameck_c::beginState_Damage() { 
	if(!this->goBackToDamage) {
		bindAnimClr_and_setUpdateRate("damage_s", 1, 0.0, 2.0);
		bindAnimChr_and_setUpdateRate("damage_s", 1, 0.0, 2.0, false);
	}
	this->damagecounter = 0;
	this->damagetimer = 0;
	this->stopAutoUpdate = true;
	removeMyActivePhysics();
	changeBroomVisibility(false);
}

void daBossKameck_c::executeState_Damage() { 
	if(this->goBackToDamage) {
		this->damagetimer = 121;
		this->damagecounter = 5;
		this->pos = this->initialMainPos;
		this->goBackToDamage = false;
		isVisible = false;
		changeBroomVisibility(false);
		// if(this->magicplateform != 0) {
		// 	this->magicplateform->Delete(1);
		// }
		// OSReport("nyeh1\n");
		bindAnimChr_and_setUpdateRate("disappear_kamek", 1, 0.0, 2.0, false);
		return;
	}
	bool ret = false;
	if(this->animationChr.isAnimationDone() && this->damagecounter < 3) {
		damagecounter++;
		this->animationChr.setCurrentFrame(0.0);
	}
	if(this->animationClr.getCurrentFrame() == 8.00 && this->damagecounter == 3) {
		// this->animationClr.setFrameForEntry(0.0, 0);
		bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 1.0);
		bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 1.0, false);
		// this->stopAutoUpdate = false;
		this->damagecounter++;
		this->damagetimer = 0;
	}
	if(this->damagecounter == 4) {
		ret = doDisappear(this->damagetimer);
		this->damagetimer++;
	}
	if(ret == true && this->damagecounter == 4) {
		bindAnimChr_and_setUpdateRate("disappear_kamek", 1, 0.0, 2.0, false);
		this->damagecounter++;
		this->damagetimer = 0;
	}
	if(this->damagecounter == 5) {
		this->damagetimer++;
		if(this->damagetimer == 120) {
			this->goBackToDamage = true;
			doStateChange(&StateID_DoFiveFlyingShots);
		}
		// OSReport("nyeh2\n");
		if(this->damagetimer > 240) {
			this->damagecounter = 6;
			this->damagetimer = 0;
		}
	}
	if(this->damagecounter == 6) {
		addMyActivePhysics();
		this->createShield(this->damagetimer);
		doAppear(this->damagetimer);
		this->damagetimer++;
	}
}

void daBossKameck_c::endState_Damage() { 
	if(!this->goBackToDamage) {
		this->isShieldVisible = true;
	}
	this->currentattack = 0;
}

/*******************/
/*ChangePhase State*/
/*******************/

void daBossKameck_c::beginState_ChangePhase() { 
	bindAnimClr_and_setUpdateRate("damage_s", 1, 0.0, 2.0);
	bindAnimChr_and_setUpdateRate("damage_s", 1, 0.0, 2.0, false);
	this->damagecounter = 0;
	this->stopAutoUpdate = true;
}

void daBossKameck_c::executeState_ChangePhase() { 
	if(this->animationChr.isAnimationDone()) {
		damagecounter++;
		this->animationChr.setCurrentFrame(0.0);
	}
	if(this->animationClr.getCurrentFrame() == 8.00 && this->damagecounter > 3) {
		// this->animationClr.setFrameForEntry(0.0, 0);
		bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 2.0);
		bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 2.0, false);
		this->stopAutoUpdate = false;
		doStateChange(&StateID_Attack);
	}
}

void daBossKameck_c::endState_ChangePhase() { 
	
}

/*************/
/*Outro State*/
/*************/

void daBossKameck_c::beginState_Outro() { 
	this->stopAutoUpdate = true;
	changeBroomVisibility(false);
	removeMyActivePhysics();
	StopBGMMusic();

	this->original_4 = WLClass::instance->_4;
	this->original_8 = WLClass::instance->_8;
	this->original_m = dStage32C_c::instance->freezeMarioBossFlag;

	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;

	nw4r::snd::SoundHandle damageHandle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, SE_BOSS_CMN_DAMAGE_LAST, 1);
	nw4r::snd::SoundHandle downHandle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &downHandle, SE_BOSS_KAMECK_DOWN, 1);
	bindAnimChr_and_setUpdateRate("demo_escapeA1", 1, 0.0, 2.0, false);

	imded = 1;
	for (int i = 0; i < 4; i++) {
		this->playerOnGround[i] = 0;
	}
	
	//from bossCaptainBowser.cpp
	dActor_c *iter = 0;
	while (iter = (dActor_c*)dActor_c::searchByBaseType(2, iter)) {
		dStageActor_c *sa = (dStageActor_c*)iter;

		if (sa->name == EN_DOSUN ||			//Thwomp
				sa->name == EN_NOKONOKO ||	//Koopa Troopa
				sa->name == EN_CHOROPU ||	//Monty Mole
				sa->name == EN_KURIBO ||	//Goomba
				sa->name == EN_BOMHEI ||	//Bob-Omb
				sa->name == EN_ITEM) {		//Item
			sa->killedByLevelClear();
			sa->Delete(1);
		}
	}
}

void daBossKameck_c::executeState_Outro() {
	if(!this->arePlayersOnGround) {
		for (int i = 0; i < 4; i++) {
			if(this->playerOnGround[i] == 0) {
				dStageActor_c *player = GetSpecificPlayerActor(i);
				if (player) {
					if ((player->collMgr.isOnTopOfTile() && player->currentZoneID == currentZoneID) || (Player_Lives[i] == 0)) {
						this->playerOnGround[i] = 1; //If current player is on top of a tile, add him to the playerOnGround
						// OSReport("Player %d on top of a tile\n", i);
					}
				}
				else {
					this->playerOnGround[i] = 1; //If that player don't even exist, ignore him by adding him to the playerOnGround
					// OSReport("Player %d don't exist\n", i);
				}
			}
		}
		if(this->playerOnGround[0] == 1 && this->playerOnGround[1] == 1 && this->playerOnGround[2] == 1 && this->playerOnGround[3] == 1) { //If all the players are on ground
			this->arePlayersOnGround = true;
			this->timer = 100;
			// OSReport("All the players are on top of a tile\n");
		}
	}
	else {
		this->timer++;
	}
	if(this->timer == 120) {
		nw4r::snd::SoundHandle clearHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &clearHandle, STRM_BGM_SHIRO_BOSS_CLEAR, 1);
		BossGoalForAllPlayers();
		UpdateGameMgr();
	}
	if(this->timer == 300) {
		bindAnimChr_and_setUpdateRate("demo_escapeA2", 1, 0.0, 2.0, false);
	}
	if(this->timer == 360) {
		changeBroomVisibility(true);
		this->rot.y = 0x2800;
		nw4r::snd::SoundHandle mutateHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &mutateHandle, SE_BOSS_KAMECK_BLOCK_MUTATE, 1);
		nw4r::snd::SoundHandle flyHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &flyHandle, SE_BOSS_KAMECK_FLY_START, 1);
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 3.0f, 1.0f};
		SpawnEffect("Wm_mr_2dlandsmoke", 0, &this->pos, &nullRot, &oneVec);
		bindAnimChr_and_setUpdateRate("fly", 1, 0.0, 2.0, false);
		bindAnimChr_and_setUpdateRate("fly_broom", 1, 0.0, 2.0, true);
		this->stopAutoUpdate = false;
	}
	if(this->timer > 360) {
		this->pos.x += 5;
		this->pos.y += 1;
	}
	if(this->timer == 400) {
		S16Vec nullRot = {0,0,0};
		VEC3 spawningPos = {this->initialMainPos.x + 95, this->initialMainPos.y - 191, this->initialMainPos.z};
		dStageActor_c *door = createChild(EN_DOOR, this, 0, &spawningPos, &nullRot, 0);
		VEC3 spawningPos2 = {this->initialMainPos.x - 225, this->initialMainPos.y - 191, this->initialMainPos.z};
		dStageActor_c *magicplateformTwo = createChild(EN_SLIP_PENGUIN, this, 16777218, &spawningPos2, &nullRot, 0);
		VEC3 spawningPos3 = {this->initialMainPos.x + 104, this->initialMainPos.y - 176, this->initialMainPos.z};
		Vec oneVec = {0.5f, 6.0f, 1.0f};
		SpawnEffect("Wm_mr_2dlandsmoke", 0, &spawningPos3, &nullRot, &oneVec);
		WLClass::instance->_4 = this->original_4;
		WLClass::instance->_8 = this->original_8;
		dStage32C_c::instance->freezeMarioBossFlag = this->original_m;
		MakeMarioExitDemoMode();
		UpdateGameMgr();
	}

}

void daBossKameck_c::endState_Outro() { 

}



/************************************/
/*Related to the original Kamek Boss*/
/************************************/


class daBossKameckDemo_c : public dEn_c {	//The original Kamek Boss' class
	int onExecute_orig();
	int newOnExecute();
};

int timm;									//Frame counter, named it "timm" instead of "time" to avoid confusion with potential other ints named "time"

int daBossKameckDemo_c::newOnExecute() {
	int orig_val = this->onExecute_orig();	//Do the original onExecute
	timm++;									//Increase the frame counter by one
	if(timm > 700) {						//After 700 frames passed
		this->pos.y -= 500;					//Place the original Kamek Boss under the level (deleting it breaks the magic plateforms)
	}
	if(imded == 1) {
		this->Delete(1);
	}
	return orig_val;						//Returns the original onExecute
}
#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include <playerAnim.h>
#include "boss.h"

bool isTriggered;


const char* KMGarcNameList [] = {
	"kameck",
	"bubble",
	NULL	
};


class daBossKameckGiant_c : public daBoss {
	int onCreate();						//Executed when the sprite spawns
	int onDelete();						//Executed when the sprite is being deleted
	int onExecute();					//Executed every frame after the onCreate is executed
	int onDraw();						//Executed every frame, handles the rendering

	mHeapAllocator_c allocator;			//The Heap Allocator
	m3d::mdl_c bodyModel;				//Kamek's model
	m3d::mdl_c broomModel;				//Kamek's broom's model
	m3d::anmChr_c animationChr;			//Kamek's "bone" animations
	m3d::anmChr_c animationBroomChr;	//Kamek's broom's "bone" animations
	m3d::anmClr_c animationClr;			//Kamek's "color" animations
	nw4r::g3d::ResFile resFile;			//kameck.arc -> kameck.brres

	nw4r::snd::SoundHandle handle;		//SoundHandle for SE_BOSS_KAMECK_MAGIC_MAKE
	nw4r::snd::SoundHandle owoHandle;	//SoundHandle for STRM_BGM_SELECT_W6
	mEf::es2 bolt1;						//First Lightning Bolt effect
	mEf::es2 bolt2;						//Second Lightning Bolt effect
	mEf::es2 bolt3;						//Third Lightning Bolt effect
	mEf::es2 bolt4;						//Fourth Lightning Bolt effect
	mEf::es2 bolt5;						//Fifth Lightning Bolt effect

	VEC3 initialMainPos;				//Initial position at the top of the room

	int timer;							//Timer
	char dying;							//Is Kamek dying. 1 = Yup | 0 = Nope
	char damage;						//Total damages
	char isInvulnerable;				//True if the boss can't be hit
	int phase;							//Which phase is the current one
	bool isBroomInvisible;				//True if Kamek's broom is invisible
	bool stopAutoUpdate;				//True to stop the automatic resetting of the "bone" animations when they're done
	int appearingtimer;					//A wild timer appears !
	int walkingtimer;					//Is it a bird ? Is it a Plane ? No, it's a timer !
	bool canIwalkNow;					//Flag to fly and not walk actually
	int attacktimer;					//https://i.imgur.com/s7qdmCb.png
	int shootingtimer;					//UwU
	bool doAttackNextRefresh;			//Flag to attack at the next refresh ? Pretty obvious yeah
	int dyingtimer;						//;w;
	int dyingRelatedCheck;				//Just read the name duh
	VEC3 boltpos;						//Position of the Lightning Bolts





	dAcPy_c *players[4];
	
	static daBossKameckGiant_c *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate, bool isBroom);	//Binds a "bone" animation (CHR) to either Kamek's broom or Kamek himself
	void bindAnimClr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);					//Binds a "color" animation (CLR) to Kamek
	void setupBodyModel();																					//Setup the required models
	void updateModelMatrices();																				//Update the models' positions, scales and rotations

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

	USING_STATES(daBossKameckGiant_c);	//State Declaring for daBossKameckGiant_c
	DECLARE_STATE(Appear);				//Appearing State
	DECLARE_STATE(Attack);				//Attack Choosing State
	DECLARE_STATE(FireShoot);
	DECLARE_STATE(Outro);				//Outro State
};


daBossKameckGiant_c *daBossKameckGiant_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossKameckGiant_c));
	return new(buffer) daBossKameckGiant_c;
}


												//State Creating for daBossKameckGiant_c
CREATE_STATE(daBossKameckGiant_c, Appear);		//Appearing State
CREATE_STATE(daBossKameckGiant_c, Attack);		//Attack Choosing State
CREATE_STATE(daBossKameckGiant_c, FireShoot);
CREATE_STATE(daBossKameckGiant_c, Outro);		//Outro State


void daBossKameckGiant_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {						//Executed when the player touches Kamek
	this->_vf220(apOther->owner);																				//Damage the player
}
void daBossKameckGiant_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 						//Executed when yoshi touches Kamek
	this->playerCollision(apThis, apOther);																		//Do the same as the player collision
}
bool daBossKameckGiant_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}
bool daBossKameckGiant_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {	//Executed when a fireball touches Kamek
	return true;
}
bool daBossKameckGiant_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool daBossKameckGiant_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool daBossKameckGiant_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameckGiant_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daBossKameckGiant_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameckGiant_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameckGiant_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}


/***************************************************/
/**Most of this file is working basically the same**/
/**way as the main kamek file, so i won't document**/
/*****************the code further.*****************/
/***************************************************/


void daBossKameckGiant_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate, bool isBroom) {
	if(!isBroom) {
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
		this->animationChr.bind(&this->bodyModel, anmChr, unk);
		this->bodyModel.bindAnim(&this->animationChr, unk2);
		this->animationChr.setUpdateRate(rate);
	}
	else {
		nw4r::g3d::ResAnmChr anmBroomChr = this->resFile.GetResAnmChr(name);
		this->animationBroomChr.bind(&this->broomModel, anmBroomChr, unk);
		this->broomModel.bindAnim(&this->animationBroomChr, unk2);
		this->animationBroomChr.setUpdateRate(rate);
	}
}

void daBossKameckGiant_c::bindAnimClr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmClr anmRes = this->resFile.GetResAnmClr(name);
	this->animationClr.bind(&this->bodyModel, anmRes, 0, 0);
	this->bodyModel.bindAnim(&this->animationClr, 0.0);
}

bool isEventTriggered(int eventID) {
	u64 eventFlag = ((u64)1 << (eventID - 1));
	if(dFlagMgr_c::instance->flags & eventFlag) {
		return true;
	}
	return false;
}

void triggerEvent(int eventID) {
	dFlagMgr_c::instance->flags |= ((u64)1 << (eventID - 1));
}

void daBossKameckGiant_c::setupBodyModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("kameck", "g3d/kameck.brres");

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kameck");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&bodyModel, 0);

	nw4r::g3d::ResMdl broommdl = this->resFile.GetResMdl("kameck_broom");
	broomModel.setup(broommdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&broomModel, 0);

	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("ride_wait");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	bool rett;
	nw4r::g3d::ResAnmChr anmBroomChr = this->resFile.GetResAnmChr("ride_wait_broom");
	rett = this->animationBroomChr.setup(broommdl, anmBroomChr, &this->allocator, 0);
	
	nw4r::g3d::ResAnmClr anmRess = this->resFile.GetResAnmClr("wand");
	this->animationClr.setup(mdl, anmRess, &this->allocator, 0, 1);
	this->animationClr.bind(&this->bodyModel, anmRess, 0, 0);
	this->bodyModel.bindAnim(&this->animationClr, 0.0);

	allocator.unlink();
}
int daBossKameckGiant_c::onCreate() {
	setupBodyModel();
	OSReport("settings = %d\n", this->settings); //This is a debug thing, tbh i don't even remember why did i need kamek's settings, but not commenting it here because it isn't commented in the 1.0 build, my bad. -RedStoneMatt 10/11/2020

	this->scale = (Vec){1.0, 1.0, 1.0};


	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 40.0;
	HitMeBaby.yDistToCenter = 140.0;

	HitMeBaby.xDistToEdge = 52.5;
	HitMeBaby.yDistToEdge = 150.5;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	
	this->rot.x = 0;
	this->rot.y = 0x2800;
	this->rot.z = 0;
	// this->pos.z = -200;

	
	this->speed.x = 0;
	this->damage = 0;
	this->isInvulnerable = 0;
	this->dying = 0;
	this->disableEatIn();
	this->phase = 1;
	this->doAttackNextRefresh = false;

	bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
	bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);
	bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 2.0);
		
	doStateChange(&StateID_Appear);

	this->onExecute();
	return true;
}

int daBossKameckGiant_c::onDelete() {
	isTriggered = false;
	return true;
}

int daBossKameckGiant_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
	broomModel._vf1C();


	if(this->animationChr.isAnimationDone() && !this->stopAutoUpdate) {
		this->animationChr.setCurrentFrame(0.0);
	}

	if(this->animationBroomChr.isAnimationDone() && !this->stopAutoUpdate) {
		this->animationBroomChr.setCurrentFrame(0.0);
	}

	if(this->canIwalkNow && !isTriggered) {
		if(this->walkingtimer < 141) {
			this->walkingtimer++;
		}
		else {
			this->pos.x += 0.88;
		}
	}

	bool flagOn = isEventTriggered(17);
	if(flagOn && !isTriggered) {
		OSReport("OWO TRIGGGGGEEEEERRRRRRRRED\n"); //PLEASE don't judge me. -RedStoneMatt 10/11/2020
		doStateChange(&StateID_Outro);
		isTriggered = true;
	}
	if(!flagOn && isTriggered) {
		OSReport("OWO UNTRIGGGGGEEEEERRRRRRRRED\n"); //You saw nothing. -RedStoneMatt 10/11/2020
		isTriggered = false;
	}
	
	return true;
}


int daBossKameckGiant_c::onDraw() {
	bodyModel.scheduleForDrawing();
	broomModel.scheduleForDrawing();
	this->animationClr.process();
	return true;
}


void daBossKameckGiant_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	broomModel.setDrawMatrix(matrix);
	broomModel.setScale(&scale);
	broomModel.calcWorld(false);
}

void daBossKameckGiant_c::changeBroomVisibility(bool visibility) {
	bindAnimChr_and_setUpdateRate(((visibility) ? "ride_wait_broom" : "disappear_broom"), 1, 0.0, 1.0, true);
	this->isBroomInvisible = visibility;
}


/**************/
/*Appear State*/
/**************/

void daBossKameckGiant_c::beginState_Appear() {
	this->scale = (Vec){1.0, 1.0, 1.0};
	for (int i = 0; i < 4; i++) { //Okay, writing it because otherwise i'm sure people will call me a code stealer: THIS FOR LOOP IS FROM bossCaptainBowser.cpp AND I USED IT BECAUSE THAT'S JUST THE FUCKING WAY IT WORKS. -RedStoneMatt 11/10/2020
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
			// dAcPy_c *player = dAcPy_c::findByID(i);
			player->direction = 1;
		}
	}
	this->stopAutoUpdate = false;
	changeBroomVisibility(true);
	MakeMarioEnterDemoMode();
	bindAnimChr_and_setUpdateRate("demo_castleC2", 1, 0.0, 1.0, false);
	bindAnimChr_and_setUpdateRate("demo_castleC2_broom", 1, 0.0, 1.0, true);
}

void daBossKameckGiant_c::executeState_Appear() { 
	// StopBGMMusic();
	OSReport("oh hello %d\n", this->appearingtimer);
	this->appearingtimer++;
	if(this->appearingtimer == 30) {
		// PlaySound(this, SE_BOSS_KAMECK_FLY_START);
		nw4r::snd::SoundHandle otherHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &otherHandle, SE_BOSS_KAMECK_FLY_START, 1);
	}
	if(this->appearingtimer == 90) {
		bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
		bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);
	}
	if(this->appearingtimer == 210) {
		changeBroomVisibility(false);
		bindAnimChr_and_setUpdateRate("throw_st_wait_giant", 1, 0.0, 1.0, false);
		// PlaySound(this, SE_BOSS_KAMECK_BLOCK_MUTATE);
		nw4r::snd::SoundHandle anotherHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &anotherHandle, SE_BOSS_KAMECK_BLOCK_MUTATE, 1);
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 3.0f, 1.0f};
		SpawnEffect("Wm_mr_2dlandsmoke", 0, &this->pos, &nullRot, &oneVec);
		// this->stopAutoUpdate = false;
	}
	if(this->appearingtimer > 210 && this->appearingtimer < 229) {
		this->pos.y -= 4;
	}
	if(this->appearingtimer == 229) {
		bindAnimClr_and_setUpdateRate("stickup_giant", 1, 0.0, 1.0);
		PlaySoundWithFunctionB4(SoundRelatedClass, &this->handle, SE_BOSS_KAMECK_MAGIC_MAKE, 1);
	}
	if(this->appearingtimer == 288) {
		bindAnimClr_and_setUpdateRate("stickup_giant_wait", 1, 0.0, 1.0);
	}
	if(this->appearingtimer > 288 && this->appearingtimer < 500) {
		this->scale.x += 0.03;
		this->scale.y += 0.03;
		this->scale.z += 0.03;
		// this->pos.y -= 0.3;
	}
	if(this->appearingtimer == 500) {
		this->handle.Stop(1);
	}
	if(this->appearingtimer == 600) {
		bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 1.0);
		bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
		bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);
		PlaySound(this, SE_BOSS_KAMECK_BLOCK_MUTATE);
		this->pos.y -= 64;
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {3.0f, 15.0f, 1.0f};
		VEC3 newPos = {this->pos.x, this->pos.y, this->pos.z + 300};
		SpawnEffect("Wm_mr_2dlandsmoke", 0, &newPos, &nullRot, &oneVec);
		StartBGMMusic();
		triggerEvent(3);
		MakeMarioExitDemoMode();
		this->canIwalkNow = true;
		doStateChange(&StateID_Attack);
	}
}

void daBossKameckGiant_c::endState_Appear() { 

}

/**************/
/*Attack State*/
/**************/

void daBossKameckGiant_c::beginState_Attack() { 
	this->attacktimer = 0;
	this->doAttackNextRefresh = false;
	this->stopAutoUpdate = false;
}

void daBossKameckGiant_c::executeState_Attack() {
	this->attacktimer++;
	if(this->attacktimer > 200) {
		this->doAttackNextRefresh = true;
		this->stopAutoUpdate = true;
	}
	if(this->doAttackNextRefresh && this->animationChr.isAnimationDone()) {
		this->stopAutoUpdate = false;
		this->attacktimer = 0;
		this->doAttackNextRefresh = false;
		doStateChange(&StateID_FireShoot);
	}
}

void daBossKameckGiant_c::endState_Attack() { 
	
}

/*******************/
/*FireShoot State*/
/*******************/

void daBossKameckGiant_c::beginState_FireShoot() { 
	bindAnimChr_and_setUpdateRate("ride_wand", 1, 0.0, 1.0, false);
	bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);
	// this->stopAutoUpdate = true;
	this->shootingtimer = 0;
}

void daBossKameckGiant_c::executeState_FireShoot() {
	this->shootingtimer++;
	if(this->shootingtimer == 24) {
		S16Vec nullRot = {0,0,0};
		VEC3 spawningPos = {this->pos.x + 170, this->pos.y + 140, 2000.0f};
		dStageActor_c *bullet = createChild(KAMECK_MAGIC, this, 1, &spawningPos, &nullRot, 0);
		bullet->scale = (Vec){0.0, 0.0, 0.0};

		u8 facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

		float totalSpeed = 2.0f;
		// bullet->speed.x = sqrtf(totalSpeed * totalSpeed / (1 + ratio * ratio));
		// if(facing == 1) {
		// 	bullet->speed.x = bullet->speed.x * -1;
		// }
		// bullet->speed.y = ratio * bullet->speed.x;


		dAc_Py_c *mario = GetSpecificPlayerActor(0);
		float ratio = (spawningPos.y - mario->pos.y) / (spawningPos.x - mario->pos.x); //This line was made by Grop.
		float thatx = sqrtf(totalSpeed * totalSpeed / (1 + ratio * ratio)); //This line was made by Grop.
		if(facing == 1) {
			thatx = thatx * -1; //Could literally have used *= actually. -RedStoneMatt 11/10/2020
		}
		float thaty = ratio * bullet->speed.x;

		if(thaty > 0.75) {
			thatx = 1.8;
			thaty = 0.75;
		}
		if(thaty < -0.75) {
			thatx = 1.8;
			thaty = -0.75;
		}
		bullet->speed.x = thatx;
		bullet->speed.y = thaty;

		OSReport("----\nShoot Fireball with speed:\n");
		OSReport("X = %f\n", thatx);
		OSReport("Y = %f\n", thaty);
		OSReport("----\n");
	}
	if(this->shootingtimer == 46) {
		bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
		doStateChange(&StateID_Attack);
	}
}

void daBossKameckGiant_c::endState_FireShoot() { 
	
}

/*************/
/*Outro State*/
/*************/

void daBossKameckGiant_c::beginState_Outro() { 
	this->stopAutoUpdate = false;
	this->dyingtimer = 0;
	this->dyingRelatedCheck = 0;
	StopBGMMusic();
	UpdateGameMgr();
	dFlagMgr_c::instance->set(31, 0, false, false, false);
	*((u32*)(((char*)dBgGm_c::instance) + 0x900F0)) = 999;
	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	// WLClass::instance->activateStandardAnim(5);
	BossGoalForAllPlayers();
	UpdateGameMgr();
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
			player->direction = 1;
		}
	}
}

extern void *_8042A788;
extern void playFanfare(void *, int type);
void daBossKameckGiant_c::executeState_Outro() {
	if(this->dyingtimer < 90) {
		this->dyingtimer++;
		this->pos.x += 0.88;
	}
	if(this->dyingtimer == 90) {
		bindAnimChr_and_setUpdateRate("demo_castleC3", 1, 0.0, 1.0, false);
		bindAnimChr_and_setUpdateRate("demo_castleC3_broom", 1, 0.0, 1.0, true);
		nw4r::snd::SoundHandle hiImAnotherHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &hiImAnotherHandle, SE_VOC_KMC_SURPRISE, 1);
		this->stopAutoUpdate = true;
		this->dyingtimer++;
	}
	if(this->dyingtimer > 90) {
		if(this->animationChr.isAnimationDone() && this->dyingRelatedCheck == 0) {
			bindAnimChr_and_setUpdateRate("damage_s", 1, 0.0, 1.0, false);
			bindAnimClr_and_setUpdateRate("damage_s", 1, 0.0, 1.0);
			changeBroomVisibility(false);
			this->stopAutoUpdate = false;
			nw4r::snd::SoundHandle yetAnotherHandle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &yetAnotherHandle, SE_BOSS_JR_ELEC_APP, 1);
			nw4r::snd::SoundHandle stillAnotherHandle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &stillAnotherHandle, SE_BOSS_JR_DAMAGE_ELEC, 1);
			nw4r::snd::SoundHandle newAnotherHandle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &newAnotherHandle, SE_OBJ_KAZAN_ERUPTION, 1);
			this->dyingtimer++;
			this->dyingRelatedCheck = 1;
			this->boltpos = {this->pos.x, this->pos.y + 160, 5750.0f};
		}
		if(this->dyingtimer > 211 && this->dyingtimer < 318) {
			this->scale.x -= 0.06;
			this->scale.y -= 0.06;
			this->scale.z -= 0.06;
			this->pos.y += 1.1;
		}
		if(this->dyingRelatedCheck == 1 && this->dyingtimer < 271) {
			S16Vec nullRot = {0,0,0};
			Vec efPos4 = {this->boltpos.x + 32, this->boltpos.y, 5750.0f};
			Vec efPos5 = {this->boltpos.x - 32, this->boltpos.y, 5750.0f};
			Vec otherEfScale = {1.0f, 3.0f, 1.0f};
			bolt4.spawn("Wm_jr_electricline", 0, &efPos4, &nullRot, &otherEfScale);
			bolt5.spawn("Wm_jr_electricline", 0, &efPos5, &nullRot, &otherEfScale);
		}
		if(this->dyingRelatedCheck == 1 && this->dyingtimer < 294) {
			S16Vec nullRot = {0,0,0};
			Vec efPos2 = {this->boltpos.x + 16, this->boltpos.y, 5750.0f};
			Vec efPos3 = {this->boltpos.x - 16, this->boltpos.y, 5750.0f};
			Vec otherEfScale = {1.0f, 3.0f, 1.0f};
			bolt2.spawn("Wm_jr_electricline", 0, &efPos2, &nullRot, &otherEfScale);
			bolt3.spawn("Wm_jr_electricline", 0, &efPos3, &nullRot, &otherEfScale);
		}
		if(this->dyingRelatedCheck == 1 && this->dyingtimer < 318) {
			S16Vec nullRot = {0,0,0};
			Vec efPos1 = {this->boltpos.x, this->boltpos.y, 5750.0f};
			Vec otherEfScale = {1.0f, 3.0f, 1.0f};
			bolt1.spawn("Wm_jr_electricline", 0, &efPos1, &nullRot, &otherEfScale);
		}
		if(this->dyingtimer == 212) {
			nw4r::snd::SoundHandle iLoveSoundHandles;
			PlaySoundWithFunctionB4(SoundRelatedClass, &iLoveSoundHandles, SE_PLY_CHANGE_SMALL, 1);
		}
		if(this->dyingtimer > 318 && this->animationClr.getCurrentFrame() == 8.00 && this->dyingRelatedCheck == 1) {
			bindAnimChr_and_setUpdateRate("look_down", 1, 0.0, 1.0, false);
			bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 2.0);
			this->stopAutoUpdate = true;
			this->dyingRelatedCheck = 2;
		}
		if(this->dyingtimer == 558) {
			PlaySoundWithFunctionB4(SoundRelatedClass, &this->owoHandle, STRM_BGM_SELECT_W6, 1);
		}
		if(this->dyingtimer == 798) {
			this->owoHandle.Stop(1);
			this->owoHandle.DetachSound();
		}
		if(this->dyingtimer > 558) {
			this->pos.y -= 2;
		}
		if(this->dyingtimer == 590) {
			S16Vec nullRot = {0,0,0};
			VEC3 goompos = {this->pos.x, this->pos.y, this->pos.z - 200};
			dStageActor_c *goomba = create(EN_KURIBO, 0, &goompos, &nullRot, 0);
		}
		if(this->dyingtimer == 600) {
			nw4r::snd::SoundHandle goombaHandle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &goombaHandle, SE_PLY_SPLASH_LAVA, 1);
		}
		OSReport("this->dyingtimer = %d\n", this->dyingtimer);
	}
	if(this->dyingtimer > 91) {
		this->dyingtimer++;
	}
	if(this->dyingtimer == 770) {
		playFanfare(_8042A788, 4);
	}
	if(this->dyingtimer > 770 && this->dyingtimer < 1190) {
		for (int i = 0; i < 4; i++) {
			if (dAcPy_c *player = dAcPy_c::findByID(i)) {
				player->rot.y = 0;
			}
		}
	}
	if(this->dyingtimer == 1190) {
		FinalBossGoalForAllPlayers();
		UpdateGameMgr();
	}
	if(this->dyingtimer == 1550) {
		RESTART_CRSIN_LevelStartStruct.purpose = 0;
		RESTART_CRSIN_LevelStartStruct.world1 = 7;
		RESTART_CRSIN_LevelStartStruct.world2 = 7;
		RESTART_CRSIN_LevelStartStruct.level1 = 40;
		RESTART_CRSIN_LevelStartStruct.level2 = 40;
		RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
		RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
		RESTART_CRSIN_LevelStartStruct.unk4 = 0; // load replay
		DontShowPreGame = true;
		ExitStage(RESTART_CRSIN, 0, BEAT_LEVEL, MARIO_WIPE);

		for (int i = 0; i < 4; i++) {
			if (Player_Lives[i] == 0) {
				Player_Lives[i] = 5;
			}
		}
	}
}

void daBossKameckGiant_c::endState_Outro() { 

}

class EN_BUBBLE : public dEn_c {
public:
	int onExecute_orig();
	int newOnExecute();
};

int EN_BUBBLE::newOnExecute() {
	int orig_val = this->onExecute_orig();	//Do the original onExecute
	if(isTriggered) {
		this->Delete(1);
	}
	return orig_val;						//Returns the original onExecute
}
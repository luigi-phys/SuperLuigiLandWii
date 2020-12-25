#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

extern "C" void *StageScreen;
// extern bool enableHardMode;

#define THROWWAIT 1
#define THROWSHOOT 2
#define THROWSHOOTREVERSE 3

int imded;

const char* KMarcNameList [] = {
	"kameck",
	NULL
};


class daBossKameck_c : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c broomModel;
	m3d::mdl_c shieldModel;
	m3d::anmChr_c animationChr;
	m3d::anmChr_c animationBroomChr;
	m3d::anmChr_c animationShieldChr;
	m3d::anmClr_c animationClr;
	m3d::anmTexSrt_c animationTexSrt;
	nw4r::g3d::ResFile resFile;
	mMtx shieldMatrix;
	ActivePhysics::Info HitMeBaby;
	ActivePhysics::Info HitMyShieldBaby;

	VEC3 initialMainPos;
	VEC3 shieldScale;

	int timer;
	char dying;
	char damage;
	char isInvulnerable;
	int phase;
	bool isBroomInvisible;
	bool stopAutoUpdate;
	int spawningtimer;
	int attacktimer;
	int fiveshoottimer;
	bool isVisible;
	int doOneTime;
	int shootingtimer;
	int appearingtimer;
	int damagecounter;
	int whichanim;
	int fiveshootdirection;
	int damagetimer;
	int fiveshootvariation;
	int currentattack;
	int freezingchargingtimer;
	int freezingcounter;
	bool playerStunned[4];
	bool aremybulletsspawned;
	int shotfromcornerdirection;
	int isridewantanimation;
	int howmuchdidishoot;
	int original_4;
	int original_8;
	int original_m;
	bool doAttackNextRefresh;
	bool isShieldVisible;
	int shieldTimer;
	int secondShieldTimer;
	bool goBackToDamage;
	int waitAnotherSecond;
	bool arePlayersOnGround;
	int playerOnGround[4];
	dStageActor_c *trampolineWall1;
	dStageActor_c *trampolineWall2;
	dStageActor_c *trampolineWall3;
	dStageActor_c *magicplateform;
	VEC3 magicPos;

	static daBossKameck_c *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate, bool isBroom);
	void bindAnimClr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimToShield();
	void setupBodyModel();
	void updateModelMatrices();

	bool doDisappear(int timer);
	void doAppear(int timer);

	void stunPlayers();
	void unstunPlayers();

	void createShield(int timer);
	void removeShield(int timer);

	void changeBroomVisibility(bool visibility);

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daBossKameck_c);
	DECLARE_STATE(Appear);
	DECLARE_STATE(Spawn);
	DECLARE_STATE(Attack);
	DECLARE_STATE(NormalShoot);
	DECLARE_STATE(DoFiveFlyingShots);
	DECLARE_STATE(FreezePlayers);
	DECLARE_STATE(ShootFromBottomCorners);
	DECLARE_STATE(ShootFromTopCorners);
	DECLARE_STATE(Damage);
	DECLARE_STATE(ChangePhase);
	DECLARE_STATE(Outro);
};


daBossKameck_c *daBossKameck_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossKameck_c));
	return new(buffer) daBossKameck_c;
}



CREATE_STATE(daBossKameck_c, Appear);
CREATE_STATE(daBossKameck_c, Spawn);
CREATE_STATE(daBossKameck_c, Attack);
CREATE_STATE(daBossKameck_c, NormalShoot);
CREATE_STATE(daBossKameck_c, DoFiveFlyingShots);
CREATE_STATE(daBossKameck_c, FreezePlayers);
CREATE_STATE(daBossKameck_c, ShootFromBottomCorners);
CREATE_STATE(daBossKameck_c, ShootFromTopCorners);
CREATE_STATE(daBossKameck_c, Damage);
CREATE_STATE(daBossKameck_c, ChangePhase);
CREATE_STATE(daBossKameck_c, Outro);


void daBossKameck_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if(!isShieldVisible && !isInvulnerable) {
		char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);
		if(hitType == 1 || hitType == 3) {
			this->playEnemyDownSound1();
			this->damage++;
			if(this->damage < 6) {
				if(this->damage == 3) {
					this->phase++;
				}
				this->isInvulnerable = true;
				nw4r::snd::SoundHandle damageHandle;
				PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, ((this->damage < 4) ? SE_VOC_KMC_DAMAGE_L1 : SE_VOC_KMC_DAMAGE_L2), 1);
				doStateChange(&StateID_Damage);
			}
			if(this->damage == 6) {
				nw4r::snd::SoundHandle damageHandle;
				PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, SE_VOC_KMC_DAMAGE_L3, 1);
				doStateChange(&StateID_Outro);
			}
		}
		else if(hitType == 0) {
			this->_vf220(apOther->owner);
		}
	}
	else {
		bouncePlayer(apOther->owner, 4.0f);
	}
}
void daBossKameck_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->playerCollision(apThis, apOther);
}
bool daBossKameck_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}
bool daBossKameck_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossKameck_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	bouncePlayer(apOther->owner, 2.0f);
	this->playEnemyDownSound1();
	this->damage++;
	if(this->damage < 6) {
		if(this->damage == 3) {
			this->phase++;
		}
		this->isInvulnerable = true;
		nw4r::snd::SoundHandle damageHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, ((this->damage < 4) ? SE_VOC_KMC_DAMAGE_L1 : SE_VOC_KMC_DAMAGE_L2), 1);
		doStateChange(&StateID_Damage);
	}
	if(this->damage == 6) {
		nw4r::snd::SoundHandle damageHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &damageHandle, SE_VOC_KMC_DAMAGE_L3, 1);
		doStateChange(&StateID_Outro);
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

void daBossKameck_c::bindAnimClr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) { //Binds a "color" animation (CLR) to Kamek
	nw4r::g3d::ResAnmClr anmRes = this->resFile.GetResAnmClr(name);
	this->animationClr.bind(&this->bodyModel, anmRes, 0, 0);
	this->bodyModel.bindAnim(&this->animationClr, 0.0);
}

void daBossKameck_c::bindAnimToShield() { //Binds the "float" bone animation and texture animation to Kamek's shield
	nw4r::g3d::ResAnmChr anmShieldChr = this->resFile.GetResAnmChr("float");
	this->animationShieldChr.bind(&this->shieldModel, anmShieldChr, 1);
	this->shieldModel.bindAnim(&this->animationShieldChr, 0);
	this->animationShieldChr.setUpdateRate(1);


	nw4r::g3d::ResAnmTexSrt anmResss = this->resFile.GetResAnmTexSrt("float");
	this->animationTexSrt.bindEntry(&this->shieldModel, anmResss, 0, 0);
	this->shieldModel.bindAnim(&this->animationTexSrt, 0.0);
}

void daBossKameck_c::setupBodyModel() { //Setup the required models
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("kameck", "g3d/kameck.brres");

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kameck");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&bodyModel, 0);

	nw4r::g3d::ResMdl broommdl = this->resFile.GetResMdl("kameck_broom");
	broomModel.setup(broommdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&broomModel, 0);

	nw4r::g3d::ResMdl shieldmdl = this->resFile.GetResMdl("balloon");
	shieldModel.setup(shieldmdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&shieldModel, 0);

	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("ride_wait");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	bool rett;
	nw4r::g3d::ResAnmChr anmBroomChr = this->resFile.GetResAnmChr("ride_wait_broom");
	rett = this->animationBroomChr.setup(broommdl, anmBroomChr, &this->allocator, 0);

	bool rettt;
	nw4r::g3d::ResAnmChr anmShieldChr = this->resFile.GetResAnmChr("float");
	rettt = this->animationShieldChr.setup(shieldmdl, anmShieldChr, &this->allocator, 0);

	nw4r::g3d::ResAnmClr anmRess = this->resFile.GetResAnmClr("wand");
	this->animationClr.setup(mdl, anmRess, &this->allocator, 0, 1);
	this->animationClr.bind(&this->bodyModel, anmRess, 0, 0);
	this->bodyModel.bindAnim(&this->animationClr, 0.0);

	bool retttt;
	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("float");
	retttt = this->animationTexSrt.setup(shieldmdl, anmSrt, &this->allocator, 0, 1);

	allocator.unlink();
}
int daBossKameck_c::onCreate() { //Executed when the sprite spawns
	setupBodyModel();


	this->scale = (Vec){0.0, 0.0, 0.0};
	this->removeShield(30);


	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 15.0;

	HitMeBaby.xDistToEdge = 10.0;
	HitMeBaby.yDistToEdge = 15.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	HitMyShieldBaby.xDistToCenter = 0.0;
	HitMyShieldBaby.yDistToCenter = 44.0;

	HitMyShieldBaby.xDistToEdge = 22.5;
	HitMyShieldBaby.yDistToEdge = 3.0;

	HitMyShieldBaby.category1 = 0x3;
	HitMyShieldBaby.category2 = 0x0;
	HitMyShieldBaby.bitfield1 = 0x4F;
	HitMyShieldBaby.bitfield2 = 0x8028E;
	HitMyShieldBaby.unkShort1C = 0;
	HitMyShieldBaby.callback = &dEn_c::collisionCallback;

	this->rot.x = 0; // X is vertical axis
	this->rot.z = 0; // Z is ... an axis >.>

	this->speed.x = 0;
	imded = 0;
	this->damage = 0;
	this->isInvulnerable = 0;
	this->disableEatIn();
	this->phase = 1;
	this->isVisible = true;
	this->doAttackNextRefresh = false;
	this->goBackToDamage = false;

	bindAnimChr_and_setUpdateRate("ride_wait", 1, 0.0, 1.0, false);
	bindAnimChr_and_setUpdateRate("ride_wait_broom", 1, 0.0, 1.0, true);
	bindAnimClr_and_setUpdateRate("wand", 1, 0.0, 2.0);
	bindAnimToShield();

	doStateChange(&StateID_Spawn);

	this->onExecute();
	return true;
}

int daBossKameck_c::onDelete() { //Executed when the sprite is being deleted
	return true;
}

int daBossKameck_c::onExecute() { //Executed every frame after the onCreate is executed
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
	broomModel._vf1C();
	shieldModel._vf1C();

	if(this->animationChr.isAnimationDone() && !this->stopAutoUpdate) {
		this->animationChr.setCurrentFrame(0.0);
	}

	if(this->animationBroomChr.isAnimationDone() && !this->stopAutoUpdate) {
		this->animationBroomChr.setCurrentFrame(0.0);
	}

	if(this->animationShieldChr.isAnimationDone()) {
		this->animationBroomChr.setCurrentFrame(0.0);
	}

	if(this->isShieldVisible) {
		this->shieldTimer++;
		if(this->shieldTimer > 120) {
			this->removeShield(this->secondShieldTimer);
			this->secondShieldTimer++;
			if(this->secondShieldTimer > 30) {
				this->shieldTimer = 0;
				this->secondShieldTimer = 0;
				this->isShieldVisible = false;
			}
		}
	}

	return true;
}


int daBossKameck_c::onDraw() { //Executed every frame, handles the rendering
	bodyModel.scheduleForDrawing();
	if(this->isBroomInvisible) {
		broomModel.scheduleForDrawing();
	}
	shieldModel.scheduleForDrawing();
	this->animationClr.process();
	this->animationTexSrt.process();
	return true;
}


void daBossKameck_c::updateModelMatrices() { //Update the models' positions, scales and rotations
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	if(this->isBroomInvisible) {
		broomModel.setDrawMatrix(matrix);
		broomModel.setScale(&scale);
		broomModel.calcWorld(false);
	}

	shieldMatrix.translation(pos.x, pos.y + 20, pos.z + 200);
	shieldMatrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);
	shieldModel.setDrawMatrix(shieldMatrix);
	shieldModel.setScale(&shieldScale);
	shieldModel.calcWorld(false);
}

void daBossKameck_c::changeBroomVisibility(bool visibility) { //Make Kamek's broom appear or not
	bindAnimChr_and_setUpdateRate(((visibility) ? "ride_wait_broom" : "disappear_broom"), 1, 0.0, 1.0, true);
	this->isBroomInvisible = visibility;
}

void daBossKameck_c::createShield(int timer) { //Make Kamek's shield appear
	if(timer < 30) {
		this->shieldScale.x += 0.04;
		this->shieldScale.y += 0.04;
		this->shieldScale.z += 0.04;
	}
	if(timer == 30) {
		this->shieldScale = (Vec){1.3, 1.3, 1.3};
		VEC3 basePos = {this->initialMainPos.x - 8, this->initialMainPos.y + 44, this->initialMainPos.z + 200};
		VEC3 spawningPos1 = {basePos.x, basePos.y, basePos.z};
		VEC3 spawningPos2 = {basePos.x - 16, basePos.y, basePos.z};
		VEC3 spawningPos3 = {basePos.x + 16, basePos.y, basePos.z};
		S16Vec nullRot = {0,0,0};
		trampolineWall1 = createChild(EN_TRPLN_WALL, this, (0 | (2 << 0)), &spawningPos1, &nullRot, 0);
		trampolineWall2 = createChild(EN_TRPLN_WALL, this, (0 | (2 << 0)), &spawningPos2, &nullRot, 0);
		trampolineWall3 = createChild(EN_TRPLN_WALL, this, (0 | (2 << 0)), &spawningPos3, &nullRot, 0);
		trampolineWall1->scale = (Vec){0.0, 0.0, 0.0};
		trampolineWall2->scale = (Vec){0.0, 0.0, 0.0};
		trampolineWall3->scale = (Vec){0.0, 0.0, 0.0};
		this->aPhysics.removeFromList();
		this->aPhysics.initWithStruct(this, &HitMyShieldBaby);
		this->aPhysics.addToList();
	}
}

void daBossKameck_c::removeShield(int timer) { //Make Kamek's shield disappear
	if(timer == 1) {
		if(this->isShieldVisible) {
			this->aPhysics.removeFromList();
			this->aPhysics.initWithStruct(this, &HitMeBaby);
			this->aPhysics.addToList();
			trampolineWall1->Delete(1);
			trampolineWall2->Delete(1);
			trampolineWall3->Delete(1);
		}
	}
	if(timer < 30) {
		this->shieldScale.x -= 0.04;
		this->shieldScale.y -= 0.04;
		this->shieldScale.z -= 0.04;
	}
	if(timer == 30) {
		this->shieldScale = (Vec){0.0, 0.0, 0.0};
		this->isShieldVisible = false;
		this->isInvulnerable = false;
	}
}

bool daBossKameck_c::doDisappear(int timer) { //Make kamek disappear using its disappear animation
	if(this->isVisible) {
		if(timer == 0) {
			nw4r::snd::SoundHandle disappearHandle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &disappearHandle, SE_BOSS_KAMECK_DISAPP, 1);
			changeBroomVisibility(false);
			bindAnimChr_and_setUpdateRate("disappear", 1, 0.0, 1.0, false);
		}
		if(timer > 0) {
			stopAutoUpdate = true;
			if(this->animationChr.isAnimationDone()) {
				stopAutoUpdate = false;
				isVisible = false;
				if(this->magicplateform != 0) {
					this->magicplateform->Delete(1);
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
	if(!this->isVisible) {
		if(timer == 0) {
			this->rot.y = 0;
			this->pos = this->initialMainPos;
			nw4r::snd::SoundHandle appearHandle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &appearHandle, SE_BOSS_KAMECK_APP, 1);
			changeBroomVisibility(false);
			S16Vec nullRot = {0,0,0};
			magicplateform = createChild(EN_SLIP_PENGUIN, this, 16777217, &this->magicPos, &nullRot, 0);
			bindAnimChr_and_setUpdateRate("appear", 1, 0.0, 1.0, false);
		}
		if(timer > 0) {
			stopAutoUpdate = true;
			if(this->animationChr.isAnimationDone()) {
				stopAutoUpdate = false;
				isVisible = true;
				bindAnimChr_and_setUpdateRate("throw_st_wait", 1, 0.0, 1.0, false);
				changeBroomVisibility(false);
				doStateChange(&StateID_Attack);
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
	// if(this->attacktimer == waitingtime[this->currentattack] / ((int)enableHardMode + 1)) { //Waiting time is divided by 2 when Hard Mode is enabled (disabled)
	if(this->attacktimer == waitingtime[this->currentattack]) { //Waiting time is divided by 2 when Hard Mode is enabled
		this->doAttackNextRefresh = true;
		this->stopAutoUpdate = true;
	}
	if(this->doAttackNextRefresh && this->animationChr.isAnimationDone()) {
		this->stopAutoUpdate = false;
		this->attacktimer = 0;
		this->doAttackNextRefresh = false;
		unstunPlayers();
		if(this->phase == 1) {
			//Here are the odds:
			//Normal Mode -> NormalShoot:       90% chance
			//               DoFiveFlyingShots: 10% chance
			//Hard Mode   -> NormalShoot:       50% chance (disabled)
			//               DoFiveFlyingShots: 50% chance (disabled)
			int whichAttack = GenerateRandomNumber(100);
			// if(whichAttack > ((enableHardMode) ? 50 : 10)) {
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
			//Here are the odds:
			//Normal Mode -> FreezePlayers:          75% chance
			//               ShootFromBottomCorners: 15% chance
			//               ShootFromTopCorners:    10% chance
			//Hard Mode   -> FreezePlayers:          50% chance (disabled)
			//               ShootFromBottomCorners: 15% chance (disabled)
			//               ShootFromTopCorners:    15% chance (disabled)
			//               DoFiveFlyingShots:      20% chance (disabled)
			int whichAttack = GenerateRandomNumber(100);
			// if(whichAttack < ((enableHardMode) ? 50 : 75)) {
			if(whichAttack < 75) {
				this->currentattack = 2;
				doStateChange(&StateID_FreezePlayers);
			}
			// if(whichAttack > ((enableHardMode) ? 49 : 74) && whichAttack < ((enableHardMode) ? 65 : 90)) {
			if(whichAttack > 74 && whichAttack < 90) {
				this->currentattack = 3;
				doStateChange(&StateID_ShootFromBottomCorners);
			}
			// if(whichAttack > ((enableHardMode) ? 64 : 89) && whichAttack < ((enableHardMode) ? 80 : 100)) {
			if(whichAttack > 89 && whichAttack < 100) {
				this->currentattack = 4;
				doStateChange(&StateID_ShootFromTopCorners);
			}
			// if(whichAttack > 79 && enableHardMode) {
			// 	this->currentattack = 1;
			// 	doStateChange(&StateID_DoFiveFlyingShots);
			// }
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
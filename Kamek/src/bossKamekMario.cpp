#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include <playerAnim.h>
#include "boss.h"
#include "effects.h"

bool isMarioEventTriggered(int eventID) {
	u64 eventFlag = ((u64)1 << (eventID - 1));
	if(dFlagMgr_c::instance->flags & eventFlag) {
		return true;
	}
	return false;
}

class daKameckMario_c : public daEnBlockMain_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c animationChr;
	m3d::mdl_c bodyModel;
	m3d::mdl_c cageModel;
	mMtx cageMatrix;
	
	const char *brresName;
	Physics::Info physicsInfo;


	u32 timer;
	u32 gtimer;
	u32 movingtimer;
	bool doMove;
	u32 filename;
	bool isDieing;
	
	int spawnersettings;
	int type;

	VEC3 initialMainPos;
	VEC3 cagePos;
	S16Vec cageRot;

	static daKameckMario_c *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupBodyModel();
	void updateModelMatrices();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daKameckMario_c);
	DECLARE_STATE(Attack);
	DECLARE_STATE(Outro);
};

daKameckMario_c *daKameckMario_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daKameckMario_c));
	return new(buffer) daKameckMario_c;
}



CREATE_STATE(daKameckMario_c, Attack);
CREATE_STATE(daKameckMario_c, Outro);


extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);


void daKameckMario_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	u16 name = ((dEn_c*)apOther->owner)->name;

	if (name == BIGHANA_MGR) {// If the sprite we collided with is the magic bullet
		((dEn_c*)apOther->owner)->Delete(1);
		doStateChange(&StateID_Outro);
	}
	return;
}
void daKameckMario_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
}
void daKameckMario_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
}
bool daKameckMario_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKameckMario_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKameckMario_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKameckMario_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKameckMario_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

bool daKameckMario_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daKameckMario_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKameckMario_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}



void daKameckMario_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}




void daKameckMario_c::setupBodyModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("Mario", "g3d/cutscene.brres");

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("MB");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Player(&bodyModel, 0);

	nw4r::g3d::ResMdl cagemdl = this->resFile.GetResMdl("cage");
	cageModel.setup(cagemdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&cageModel, 0);
	
	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();
}


int daKameckMario_c::onCreate() {

	/***********/
	/*Setupping*/
	/***********/

	
	setupBodyModel();


	this->scale = (Vec){1.0, 1.0, 1.0};
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->cageRot.x = 0; // X is vertical axis
	this->cageRot.y = 0; // Y is horizontal axis
	this->cageRot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	this->speed.x = 0.5;
	this->speed.y = 3;

	this->pos.z = 0;
	this->doMove = true;

	/*********************/
	/*Start the behaviour*/
	/*********************/

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);

	doStateChange(&StateID_Attack);

	this->onExecute();
	return true;
}

int daKameckMario_c::onDelete() {
	return true;
}

int daKameckMario_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	if(this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}


int daKameckMario_c::onDraw() {
	bodyModel.scheduleForDrawing();
	if(!isMarioEventTriggered(1)) {
		cageModel.scheduleForDrawing();
	}
	bodyModel._vf1C();
	return true;
}


void daKameckMario_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);
	cageMatrix.translation(cagePos.x, cagePos.y, cagePos.z);
	cageMatrix.applyRotationYXZ(&cageRot.x, &cageRot.y, &cageRot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
	cageModel.setDrawMatrix(cageMatrix);
	cageModel.setScale(&scale);
	cageModel.calcWorld(false);
}


// Attack State

void daKameckMario_c::beginState_Attack() {
	this->initialMainPos = this->pos;
	this->cagePos = {this->pos.x, this->pos.y - 30, this->pos.z};
	this->pos.y -= 26;
	this->timer = 0;
}
void daKameckMario_c::executeState_Attack() {
	this->gtimer++;
	if(!isMarioEventTriggered(1) && this->gtimer == 250) {
		nw4r::snd::SoundHandle luigiiiiHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &luigiiiiHandle, STRM_BGM_BONUS, 1);
		bindAnimChr_and_setUpdateRate("dm_surprise", 1, 0.0, 1.0);
	}
	if(!isMarioEventTriggered(1) && this->gtimer == 350) {
		bindAnimChr_and_setUpdateRate("dm_surp_wait", 1, 0.0, 1.0);
	}
	if(isMarioEventTriggered(1) && this->timer == 0) {
		bindAnimChr_and_setUpdateRate("2jumped", 1, 0.0, 0.0);
		this->timer = 1;
	}
	if(isMarioEventTriggered(1) && this->pos.y > (this->initialMainPos.y - 96) && this->timer == 1) {
		this->pos.y -= 3;
	}
	if(isMarioEventTriggered(1) && this->pos.y <= (this->initialMainPos.y - 96) && this->timer == 1) {
		bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
		this->pos.y = this->initialMainPos.y - 96;
		this->timer = 2;
	}
	if(this->timer > 1) {
		this->timer ++;
	}
	if(this->timer == 240) {
		bindAnimChr_and_setUpdateRate("dm_glad", 1, 0.0, 1.0);
		nw4r::snd::SoundHandle yayHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &yayHandle, STRM_BGM_BONUS_FAST, 1);
	}
	if(this->timer == 300) {
		nw4r::snd::SoundHandle yayHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &yayHandle, SE_VOC_MA_SAVE_PRINCESS, 1);
		for (int i = 0; i < 4; i++) {
			if (dAcPy_c *player = dAcPy_c::findByID(i)) {
				player->setAnimePlayWithAnimID(dm_glad);
			}
		}
	}
}
void daKameckMario_c::endState_Attack() {

}

// Outro State

void daKameckMario_c::beginState_Outro() {

}
void daKameckMario_c::executeState_Outro() {

}
void daKameckMario_c::endState_Outro() { 

}






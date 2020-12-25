#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include "msgbox.h"

//Hey dear player, i have a request:
//Could you NOT talk about this file on the New Luigi Team server please ?
//I'd like players to discover this boss by themselves in the game, so please do not tell anyone you found this, okay ?
//If you disrespect that, then you'll make me very mad >:(
//Anyway, have fun ^w^

class daBossMatt_c : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	m3d::anmTexSrt_c animationTexSrt;
	nw4r::g3d::ResFile resFile;
	ActivePhysics::Info HitMeBaby;

	mEf::es2 effect;

	int timer;
	char damage;
	char isInvulnerable;
	int phase;
	bool stopAutoUpdate;
	bool isntDoneYet;
	
	static daBossMatt_c *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupBodyModel();
	void updateModelMatrices();

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

	USING_STATES(daBossMatt_c);
	DECLARE_STATE(Appear);
	DECLARE_STATE(Intro);
	DECLARE_STATE(Outro);
};


daBossMatt_c *daBossMatt_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossMatt_c));
	return new(buffer) daBossMatt_c;
}


		
CREATE_STATE(daBossMatt_c, Appear);
CREATE_STATE(daBossMatt_c, Intro);
CREATE_STATE(daBossMatt_c, Outro);

extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);


void daBossMatt_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

}
void daBossMatt_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 

}
bool daBossMatt_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}
bool daBossMatt_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {	
	return true;
}
bool daBossMatt_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMatt_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMatt_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMatt_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daBossMatt_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMatt_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMatt_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}




void daBossMatt_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) { 
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);			
	this->animationChr.bind(&this->bodyModel, anmChr, unk);					
	this->bodyModel.bindAnim(&this->animationChr, unk2);					
	this->animationChr.setUpdateRate(rate);
}

void daBossMatt_c::setupBodyModel() { 
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("Matt", "g3d/Matt.brres");	

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("Matt");		
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);					
	SetupTextures_Player(&bodyModel, 0);

	// bool ret;
	// nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	// ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();
}
int daBossMatt_c::onCreate() { 
	setupBodyModel();		


	this->scale = (Vec){1.0, 1.0, 1.0};


	HitMeBaby.xDistToCenter = 0.0;					
	HitMeBaby.yDistToCenter = 0.0;					

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
	
	this->rot.x = 0; 
	this->rot.z = 0; 

	
	this->speed.x = 0;
	this->damage = 0;
	this->isInvulnerable = 0;
	this->disableEatIn();
	this->visible = true;
	this->isntDoneYet = true;

	// bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
		
	doStateChange(&StateID_Appear);					

	this->onExecute();		
	return true;
}

int daBossMatt_c::onDelete() { 
	return true;
}

int daBossMatt_c::onExecute() { 
	acState.execute();			
	updateModelMatrices();		
	// bodyModel._vf1C();

	// if(this->animationChr.isAnimationDone() && !this->stopAutoUpdate) {
	// 	this->animationChr.setCurrentFrame(0.0);
	// }
	
	return true;
}


int daBossMatt_c::onDraw() { 
	bodyModel.scheduleForDrawing();				
	return true;
}


void daBossMatt_c::updateModelMatrices() { 
	matrix.translation(pos.x, pos.y, pos.z);					
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);			

	bodyModel.setDrawMatrix(matrix);	
	bodyModel.setScale(&scale);			
	bodyModel.calcWorld(false);	
}

/**************/
/*Appear State*/
/**************/

void daBossMatt_c::beginState_Appear() {
	this->rot.y = 0xD800;
}

void daBossMatt_c::executeState_Appear() {
	if (!dMsgBoxManager_c::instance->visible) {
		this->timer++;
	}
	if(this->timer == 1) {

	}
	if(this->timer == 120) {
		dMsgBoxManager_c::instance->doNotPause = true;
		dMsgBoxManager_c::instance->showMessage(297, true, -1);
	}
	if(this->timer == 130) {
		dMsgBoxManager_c::instance->showMessage(298, true, -1);
	}
	if(this->timer == 140) {
		dMsgBoxManager_c::instance->showMessage(299, true, -1);
	}
	if(this->timer == 150) {
		dMsgBoxManager_c::instance->showMessage(300, true, -1);
	}
	if(this->timer == 160) {
		dMsgBoxManager_c::instance->showMessage(301, true, -1);
	}
	if(this->timer == 170) {
		dMsgBoxManager_c::instance->showMessage(302, true, -1);
	}
	if(this->timer == 180) {
		PlaySound(this, SE_EMY_DOWN);
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);
	}
	if(this->timer == 190) {
		this->Delete(1);
	}
}

void daBossMatt_c::endState_Appear() { 

}

/*************/
/*Intro State*/
/*************/

void daBossMatt_c::beginState_Intro() { 
}

void daBossMatt_c::executeState_Intro() {
}

void daBossMatt_c::endState_Intro() { 

}

/*************/
/*Outro State*/
/*************/

void daBossMatt_c::beginState_Outro() { 
}

void daBossMatt_c::executeState_Outro() {
}

void daBossMatt_c::endState_Outro() { 

}
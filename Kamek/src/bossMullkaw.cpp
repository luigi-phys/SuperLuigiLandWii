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

const char* MKarcNameList [] = {
	"gesso",
	"Matt",
	NULL
};


class daBossMullkaw_c : public daBoss {
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


	int timer;
	char damage;
	char isInvulnerable;
	int phase;
	bool stopAutoUpdate;
	bool isntDoneYet;

	dStageActor_c *magicplatform;
	
	static daBossMullkaw_c *build();

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

	USING_STATES(daBossMullkaw_c);
	DECLARE_STATE(Appear);
	DECLARE_STATE(Intro);
	DECLARE_STATE(Outro);
};


daBossMullkaw_c *daBossMullkaw_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossMullkaw_c));
	return new(buffer) daBossMullkaw_c;
}


		
CREATE_STATE(daBossMullkaw_c, Appear);
CREATE_STATE(daBossMullkaw_c, Intro);
CREATE_STATE(daBossMullkaw_c, Outro);

extern bool isEventTriggered(int eventID);
extern void triggerEvent(int eventID);


void daBossMullkaw_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

}
void daBossMullkaw_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 

}
bool daBossMullkaw_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}
bool daBossMullkaw_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {	
	return true;
}
bool daBossMullkaw_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMullkaw_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMullkaw_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMullkaw_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daBossMullkaw_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMullkaw_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossMullkaw_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}




void daBossMullkaw_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) { 
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);			
	this->animationChr.bind(&this->bodyModel, anmChr, unk);					
	this->bodyModel.bindAnim(&this->animationChr, unk2);					
	this->animationChr.setUpdateRate(rate);
}

void daBossMullkaw_c::setupBodyModel() { 
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("gesso", "g3d/gesso.brres");	

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("gesso");		
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);					
	SetupTextures_Enemy(&bodyModel, 0);

	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");					
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);	

	allocator.unlink();
}
int daBossMullkaw_c::onCreate() { 
	setupBodyModel();		


	this->scale = (Vec){1.3, 1.3, 1.3};


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

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
		
	doStateChange(&StateID_Appear);					

	this->onExecute();		
	return true;
}

int daBossMullkaw_c::onDelete() { 
	return true;
}

int daBossMullkaw_c::onExecute() { 
	acState.execute();			
	updateModelMatrices();		
	bodyModel._vf1C();

	if(this->animationChr.isAnimationDone() && !this->stopAutoUpdate) {			
		this->animationChr.setCurrentFrame(0.0);		
	}
	
	return true;
}


int daBossMullkaw_c::onDraw() { 
	bodyModel.scheduleForDrawing();				
	return true;
}


void daBossMullkaw_c::updateModelMatrices() { 
	matrix.translation(pos.x, pos.y, pos.z);					
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);			

	bodyModel.setDrawMatrix(matrix);	
	bodyModel.setScale(&scale);			
	bodyModel.calcWorld(false);	
}

/**************/
/*Appear State*/
/**************/

void daBossMullkaw_c::beginState_Appear() {
	this->rot.y = 0xE800;
	this->pos.x += 8;
	this->pos.y -= 10;
	this->timer = 0;
	this->scale = {0.0, 0.0, 0.0};
	S16Vec nullRot = {0,0,0};
	VEC3 spawningPos = {this->pos.x + 80, this->pos.y + 64, this->pos.z};
	this->magicplatform = createChild(EN_SLIP_PENGUIN, this, 3, &spawningPos, &nullRot, 0);
}

void daBossMullkaw_c::executeState_Appear() {
	if(isEventTriggered(13)) {
		if (!dMsgBoxManager_c::instance->visible) {
			this->timer++;
		}
		if(this->timer == 1) {
			MakeMarioEnterDemoMode();
		}
		if(this->timer == 119) {
			triggerEvent(9);
		}
		if(this->timer == 120) {
			triggerEvent(10);
		}
		if(this->timer == 240) {
			triggerEvent(11);
		}
		if(this->timer == 241) {
			this->scale = {1.3, 1.3, 1.3};
			triggerEvent(12);
		}
		if(this->timer > 299 && this->timer < 601) {
			this->rot.y += 50;
		}
		if(this->timer == 450) {
			for (int i = 0; i < 4; i++) {
				if (dAcPy_c *player = dAcPy_c::findByID(i)) {
					player->direction = 1;
				}
			}
		}
		if(this->timer == 640) {
			dMsgBoxManager_c::instance->doNotPause = true;
			dMsgBoxManager_c::instance->showMessage(295, true, -1);
		}
		if(this->timer == 650) {
			triggerEvent(15);
			this->rot.y = 0xE800;
			this->pos.x += 880;
		}
		if(this->timer == 651) {
			triggerEvent(16);
		}
		if(this->timer == 670) {
			this->magicplatform->Delete(1);
			triggerEvent(14);
			MakeMarioExitDemoMode();
			doStateChange(&StateID_Intro);
		}
	}
}

void daBossMullkaw_c::endState_Appear() { 

}

/*************/
/*Intro State*/
/*************/

void daBossMullkaw_c::beginState_Intro() { 
	this->timer = 0;
	S16Vec nullRot = {0,0,0};
	VEC3 spawningPos = {this->pos.x - 96, this->pos.y + 124, this->pos.z};
	this->magicplatform = createChild(EN_SLIP_PENGUIN, this, 3, &spawningPos, &nullRot, 0);
}

void daBossMullkaw_c::executeState_Intro() {
	if(isEventTriggered(17)) {
		if (!dMsgBoxManager_c::instance->visible) {
			this->timer++;
		}
		if(this->timer == 1) {
			MakeMarioEnterDemoMode();
			for (int i = 0; i < 4; i++) {
				if (dAcPy_c *player = dAcPy_c::findByID(i)) {
					player->direction = 0;
				}
			}
		}
		if(this->timer == 60) {
			dMsgBoxManager_c::instance->showMessage(296, true, -1);
		}
		if(this->timer == 70) {
			CreateActor(EN_LIFT_ROTATION_FULL, 0, {this->pos.x - 64, this->pos.y - 96, this->pos.z}, 0, 0);
		}
		if(this->timer > 70) {
			OSReport("timer = %d\n", this->timer);
		}
	}
}

void daBossMullkaw_c::endState_Intro() { 

}

/*************/
/*Outro State*/
/*************/

void daBossMullkaw_c::beginState_Outro() { 
}

void daBossMullkaw_c::executeState_Outro() {
}

void daBossMullkaw_c::endState_Outro() { 

}
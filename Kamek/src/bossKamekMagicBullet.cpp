#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


const char* KMBarcNameList [] = {
	"boss_ef_attack",
	NULL	
};

class daKameckMagicBullet_c; // forward declaration

class daKameckMagic_c : public dEn_c {
public:
	virtual void Wait_Begin();
	virtual void Wait_Execute();
	virtual void Wait_End();
	void Move_Begin();
	void Move_Execute();
	void Move_End();

	USING_STATES(daKameckMagic_c);
	REF_NINTENDO_STATE(Wait);
	REF_NINTENDO_STATE(Move);

	int onCreate_orig();
	int getsettings();
	void newMoveBegin();
	void newMoveExecute();
	daKameckMagicBullet_c *otherBullet;
	// int isFire;
};

class daKameckMagicBullet_c : public daKameckMagic_c {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	
	// daKameckMagic_c *magicKamek;

	const char *brresName;

	u32 timer;
	u32 filename;
	bool isDieing;
	
	int spawnersettings;
	int type;
	mEf::es2 middleLava;
	mEf::es2 lava;
	bool timeToDie;
	int firetimer;
	ActivePhysics::Info LavaCollision;
	float lavaColX;
	float lavaColY;

	static daKameckMagicBullet_c *build();

	void setupBodyModel();
	void updateModelMatrices();

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

	USING_STATES(daKameckMagicBullet_c);
	DECLARE_STATE(Attack);
	DECLARE_STATE(Outro);
};

daKameckMagicBullet_c *daKameckMagicBullet_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daKameckMagicBullet_c));
	return new(buffer) daKameckMagicBullet_c;
}



CREATE_STATE(daKameckMagicBullet_c, Attack);
CREATE_STATE(daKameckMagicBullet_c, Outro);


extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);


void daKameckMagicBullet_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->_vf220(apOther->owner);
	this->Delete(1);
}
void daKameckMagicBullet_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
}
bool daKameckMagicBullet_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daKameckMagicBullet_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	this->Delete(1);
	return true;
}
bool daKameckMagicBullet_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	this->Delete(1);
	return true;
}
bool daKameckMagicBullet_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daKameckMagicBullet_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

bool daKameckMagicBullet_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daKameckMagicBullet_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daKameckMagicBullet_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	this->Delete(1);
	return true;
}




void daKameckMagicBullet_c::setupBodyModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("boss_ef_attack", "g3d/boss_ef_attack.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl(((this->settings == 1) ? "kameck_attack_fire" : "kameck_attack"));
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	allocator.unlink();
}


int daKameckMagicBullet_c::onCreate() {

	this->type = this->settings >> 28 & 0xF;
	
	setupBodyModel();


	this->scale = (Vec){1.0, 1.0, 1.0};
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	this->timeToDie = false;

	ActivePhysics::Info HitMeBaby;
	
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;

	HitMeBaby.xDistToEdge = 6.0;
	HitMeBaby.yDistToEdge = 6.0; 

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	S16Vec nullRot = {0,0,0};

	doStateChange(&StateID_Attack);

	this->onExecute();
	return true;
}

int daKameckMagicBullet_c::onDelete() {
	this->timeToDie = true;
	return true;
}

int daKameckMagicBullet_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	return true;
}


int daKameckMagicBullet_c::onDraw() {
	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();
	return true;
}


void daKameckMagicBullet_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


// Attack State

void daKameckMagicBullet_c::beginState_Attack() {
	if(this->settings == 1) {
		this->scale = (Vec){4.0, 4.0, 4.0};
		
		LavaCollision.xDistToCenter = -2.0;
		LavaCollision.yDistToCenter = -1.0;

		this->lavaColX = 24.0f;
		this->lavaColY = 24.0f;

		LavaCollision.category1 = 0x3;
		LavaCollision.category2 = 0x0;
		LavaCollision.bitfield1 = 0x4F;
		LavaCollision.bitfield2 = 0x8028E;
		LavaCollision.unkShort1C = 0;
		LavaCollision.callback = &dEn_c::collisionCallback;

	}
}

void daKameckMagicBullet_c::executeState_Attack() {
	if(this->settings == 1) {
		S16Vec nullRot = {0,0,0};
		this->firetimer++;
		if(this->firetimer < 240) {
			this->scale.x -= 0.0125;
			this->scale.y -= 0.0125;
			this->scale.z -= 0.0125;
			this->lavaColX -= 0.075;
			this->lavaColY -= 0.075;
		}
		else {
			nw4r::snd::SoundHandle bubbleHandle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &bubbleHandle, SE_BOSS_KAMECK_BLOCK_MUTATE, 1);
			dStageActor_c *ImAbubbleNow = create(EN_BUBBLE, 2, &this->pos, &nullRot, 0);
			this->timeToDie = true;
		}
		Vec otherEfScale = {1.0f, 1.0f, 1.0f};
		middleLava.spawn("Wm_en_keronpafire", 0, &this->pos, &nullRot, &this->scale);
		Vec lavaspawn = {this->pos.x, this->pos.y, this->pos.z - 100};
		lava.spawn("Wm_en_firebrnsign", 0, &lavaspawn, &nullRot, &this->scale);
		LavaCollision.xDistToEdge = this->lavaColX;
		LavaCollision.yDistToEdge = this->lavaColY;
		this->aPhysics.initWithStruct(this, &LavaCollision);
		this->aPhysics.addToList();
	}
}
void daKameckMagicBullet_c::endState_Attack() {

}

// Outro State

void daKameckMagicBullet_c::beginState_Outro() {
}
void daKameckMagicBullet_c::executeState_Outro() {
		this->Delete(1);
}
void daKameckMagicBullet_c::endState_Outro() { 

}

extern int getNybbleValue(u32 settings, int fromNybble, int toNybble, bool doOSReport);


int daKameckMagic_c::getsettings() {
	int orig_val = this->onCreate_orig();
	OSReport("--------\nSpawning daKameckMagic_c:\n");
	OSReport("this->settings: -> %d\n", this->settings);
	OSReport("this->settings: nybble 5  -> %d\n", getNybbleValue(this->settings, 5, 5, true));
	OSReport("this->settings: nybble 6  -> %d\n", getNybbleValue(this->settings, 6, 6, true));
	OSReport("this->settings: nybble 7  -> %d\n", getNybbleValue(this->settings, 7, 7, true));
	OSReport("this->settings: nybble 8  -> %d\n", getNybbleValue(this->settings, 8, 8, true));
	OSReport("this->settings: nybble 9  -> %d\n", getNybbleValue(this->settings, 9, 9, true));
	OSReport("this->settings: nybble 10 -> %d\n", getNybbleValue(this->settings, 10, 10, true));
	OSReport("this->settings: nybble 11 -> %d\n", getNybbleValue(this->settings, 11, 11, true));
	OSReport("this->settings: nybble 12 -> %d\n", getNybbleValue(this->settings, 12, 12, true));
	OSReport("this->pos.x: %d\n", this->pos.x);
	OSReport("this->pos.y: %d\n", this->pos.y);
	OSReport("this->pos.z: %d\n", this->pos.z);
	OSReport("this->speed.x: %d\n", this->speed.x);
	OSReport("this->speed.y: %d\n", this->speed.y);
	OSReport("this->speed.z: %d\n", this->speed.z);
	OSReport("this->scale.x: %d\n", this->scale.x);
	OSReport("this->scale.y: %d\n", this->scale.y);
	OSReport("this->scale.z: %d\n", this->scale.z);
	OSReport("this->direction: %d\n", this->direction);
	OSReport("--------\n");
	doStateChange(&daKameckMagic_c::StateID_Move);
	OSReport("current state: %s\n", this->acState.getCurrentState()->getName());
	addMyActivePhysics();
	return orig_val;
}



void daKameckMagic_c::newMoveBegin() {
	this->Move_Begin();
	S16Vec nullRot = {0,0,0};
	int bulletsettings = ((this->settings == 1) ? 1 : 0);
	this->otherBullet = (daKameckMagicBullet_c *)createChild(BIGHANA_MGR, this, bulletsettings, &this->pos, &nullRot, 0);
}


void daKameckMagic_c::newMoveExecute() {
	this->Move_Execute();
	this->otherBullet->pos = this->pos;
	if(this->otherBullet->timeToDie) {
		OSReport("it's time to die\n");
		this->Delete(1);
	}
}


#include "koopatlas/core.h"
#include "koopatlas/camera.h"
#include "koopatlas/player.h"
#include "music.h"

extern "C" void LoadMapScene();
extern u8 MaybeFinishingLevel[2];

dScKoopatlas_c *dScKoopatlas_c::instance = 0;

CREATE_STATE_E(dScKoopatlas_c, Limbo);
CREATE_STATE(dScKoopatlas_c, ContinueWait);
CREATE_STATE_E(dScKoopatlas_c, Normal);
CREATE_STATE(dScKoopatlas_c, CompletionMsg);
CREATE_STATE_E(dScKoopatlas_c, CompletionMsgHideWait);
CREATE_STATE(dScKoopatlas_c, TipMsg);
CREATE_STATE_E(dScKoopatlas_c, TipMsgHideWait);
CREATE_STATE_E(dScKoopatlas_c, CSMenu);
CREATE_STATE_E(dScKoopatlas_c, TitleConfirmOpenWait);
CREATE_STATE_E(dScKoopatlas_c, TitleConfirmSelect);
CREATE_STATE_E(dScKoopatlas_c, TitleConfirmHitWait);
CREATE_STATE_E(dScKoopatlas_c, PlayerChangeWait);
CREATE_STATE_E(dScKoopatlas_c, EasyPairingWait);
CREATE_STATE_E(dScKoopatlas_c, PowerupsWait);
CREATE_STATE_E(dScKoopatlas_c, ShopWait);
CREATE_STATE_E(dScKoopatlas_c, CoinsWait);
CREATE_STATE_E(dScKoopatlas_c, SaveOpen);
CREATE_STATE_E(dScKoopatlas_c, SaveSelect);
CREATE_STATE_E(dScKoopatlas_c, SaveWindowClose);
CREATE_STATE_E(dScKoopatlas_c, SaveDo);
CREATE_STATE_E(dScKoopatlas_c, SaveEndWindow);
CREATE_STATE_E(dScKoopatlas_c, SaveEndCloseWait);
#ifdef QUICK_SAVE
CREATE_STATE_E(dScKoopatlas_c, QuickSaveOpen);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveSelect);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveWindowClose);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveDo);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveEndWindow);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveEndCloseWait);
#endif
CREATE_STATE_E(dScKoopatlas_c, SaveError);

dScKoopatlas_c *dScKoopatlas_c::build() {
	// return new dScKoopatlas_c;
	MapReport("Creating WorldMap\n");

	void *buffer = AllocFromGameHeap1(sizeof(dScKoopatlas_c));
	dScKoopatlas_c *c = new(buffer) dScKoopatlas_c;

	MapReport("Created WorldMap @ %p\n", c);

	instance = c;
	return c;
}

bool WMInit_StartLoading(void*);
bool WMInit_LoadStaticFiles(void*);
bool StockWMInit_LoadEffects(void*);
bool WMInit_LoadSIAnims(void*);
bool WMInit_EndLoading(void*);
bool WMInit_LoadResources1(void*);
bool WMInit_LoadResources2(void*);
bool WMInit_SetupWait(void*);
bool WMInit_SetupExtra(void*);
bool WMInit_SetupWipe(void*);

ChainedFunc initFunctions[] = {
	WMInit_StartLoading,
	WMInit_LoadStaticFiles,
	StockWMInit_LoadEffects,
	WMInit_LoadSIAnims,
	WMInit_EndLoading,
	WMInit_LoadResources1,
	WMInit_LoadResources2,
	WMInit_SetupWait,
	WMInit_SetupExtra,
	WMInit_SetupWipe
};

dScKoopatlas_c::dScKoopatlas_c() : state(this) {
	initChain.setup(initFunctions, 9);

	setInitChain(initChain);
}



#define SELC_SETUP_DONE(sc) (*((bool*)(((u32)(sc))+0xD38)))

#define EASYP_SETUP_DONE(ep) (*((bool*)(((u32)(ep))+0x278)))
#define EASYP_ACTIVE(ep) (*((bool*)(((u32)(ep))+0x279)))

#define CSMENU_SETUP_DONE(csm) (*((bool*)(((u32)(csm))+0x270)))
#define CSMENU_ACTIVE(csm) (*((bool*)(((u32)(csm))+0x271)))
#define CSMENU_CHOICE_OK(csm) (*((bool*)(((u32)(csm))+0x272)))
#define CSMENU_UNK(csm) (*((bool*)(((u32)(csm))+0x273)))
#define CSMENU_CURRENT(csm) (*((int*)(((u32)(csm))+0x268)))

#define NPCHG_SETUP_DONE(npc) (*((bool*)(((u32)(npc))+0x67C)))
#define NPCHG_ACTIVE(npc) (*((bool*)(((u32)(npc))+0x67E)))
#define NPCHG_HIDE_FOR_EASYP(npc) (*((bool*)(((u32)(npc))+0x67F)))
#define NPCHG_READY(npc) (*((bool*)(((u32)(npc))+0x680)))
#define NPCHG_CCSB(npc,idx) (((void**)(((u32)(npc))+0x74))[(idx)])
#define NPCHG_CCSC(npc,idx) (((void**)(((u32)(npc))+0x84))[(idx)])
#define NPCHG_CCSA(npc,idx) (((void**)(((u32)(npc))+0x94))[(idx)])
#define NPCHG_CCI(npc,idx) (((void**)(((u32)(npc))+0xA4))[(idx)])
#define NPCHG_2DPLAYER(npc,idx) (((void**)(((u32)(npc))+0x64C))[(idx)])

#define CCSB_ACTIVE(ccsb) (*((bool*)(((u32)(ccsb))+0x29C)))

#define CCSC_ACTIVE(ccsc) (*((bool*)(((u32)(ccsc))+0x2A1)))

#define PLAYER2D_SHOW_EASY_PAIRING(p2d) (*((bool*)(((u32)(p2d))+0x264)))

#define CONT_LIVES(cont,idx) (((int*)(((u32)(cont))+0x2B8))[(idx)])
#define CONT_SETUP_DONE(cont) (*((bool*)(((u32)(cont))+0x2D4)))
#define CONT_UNK1(cont) (*((bool*)(((u32)(cont))+0x2D5)))
#define CONT_UNK2(cont) (*((bool*)(((u32)(cont))+0x2D6)))
#define CONT_DONE(cont) (*((bool*)(((u32)(cont))+0x2D7)))
#define CONT_UNK3(cont) (*((bool*)(((u32)(cont))+0x2E0)))

bool WMInit_StartLoading(void *ptr) {
	SpammyReport("WMInit_StartLoading called\n");

	DVD_Start();
	return true;
}

bool WMInit_LoadStaticFiles(void *ptr) {
	return dKPMusic::loadInfo();
}

bool WMInit_LoadSIAnims(void *ptr) {
	SpammyReport("WMInit_LoadSIAnims called\n");

	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_kinoko", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_fireflower", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_iceflower", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_penguin", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_propeller", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_star", 0);
	DVD_LoadFile(GetDVDClass(), "Maps", "SI_hammer", 0);

	DVD_LoadFile(GetDVDClass(), "Object", "cobCourse", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "I_kinoko_bundle", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "lakitu", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "star_coin", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "StarRing", 0);
	return true;
}

bool WMInit_EndLoading(void *ptr) {
	SpammyReport("WMInit_EndLoading called\n");

	if (DVD_StillLoading(GetDVDClass2())) {
		SpammyReport("WMInit_EndLoading returning false\n");
		return false;
	}

	DVD_End();
	SpammyReport("WMInit_EndLoading returning true\n");
	return true;
}

bool WMInit_LoadResources1(void *ptr) {
	SpammyReport("WMInit_LoadResources1 returning true\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	return wm->mapListLoader.load("/Maps/List.txt");
}

bool WMInit_LoadResources2(void *ptr) {
	SpammyReport("WMInit_LoadResources2 returning true\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	if (wm->mapPath == 0) {
		wm->mapPath = wm->getMapNameForIndex(wm->currentMapID);
		if (wm->mapPath == 0)
			wm->mapPath = wm->getMapNameForIndex(0);
		if (!strcmp(wm->mapPath, "/Maps/WSEL.kpbin"))
			wm->warpZoneHacks = true;
		else
			wm->warpZoneHacks = false;
		OSReport("Load map: %s\n", wm->mapPath);
	}

	if (wm->mapData.load(wm->mapPath)) {
		return true;
	} else
		return false;
}

bool WMInit_SetupWait(void *ptr) {
	SpammyReport("WMInit_SetupWait called\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	bool success = true;

	success &= CSMENU_SETUP_DONE(wm->csMenu);
	success &= SELC_SETUP_DONE(wm->selectCursor);
	success &= NPCHG_SETUP_DONE(wm->numPeopleChange);
	success &= wm->yesNoWindow->layoutLoaded;
	success &= CONT_SETUP_DONE(wm->continueObj);
	success &= wm->stockItem->layoutLoaded;
	success &= wm->stockItemShadow->layoutLoaded;
	success &= EASYP_SETUP_DONE(wm->easyPairing);

	return success;
}

bool WMInit_SetupExtra(void *ptr) {
	SpammyReport("WMInit_SetupExtra called\n");

	// ok, now we can set up other required shit
	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	// check if we need to handle Continue
	if (CheckIfContinueShouldBeActivated()) {
		SpammyReport("continue is activated\n");
		wm->state.setState(&wm->StateID_ContinueWait);
	} else {
		SpammyReport("continue is not activated\n");
		wm->state.setState(&wm->StateID_Normal);
	}

	// first up: player models for Stocked Items
	for (int i = 0; i < 4; i++) {
		void *obj = CreateChildObject(WM_2D_PLAYER, wm, i, 0, 0);
		wm->stockItem->player2d[i] = obj;
		NPCHG_2DPLAYER(wm->numPeopleChange,i) = obj;
	}

	// next: items for the Powerup screen
	for (int i = 0; i < 8; i++) {
		void *obj = CreateChildObject(WM_ITEM, wm, i, 0, 0);
		wm->stockItem->newItemPtr[i] = obj;
	}

	// need Player before we can set up paths
	SpammyReport("creating player\n");
	wm->player = (daWMPlayer_c*)CreateParentedObject(WM_PLAYER, wm, 0, 2);
	wm->player->modelHandler->mdlClass->setPowerup(Player_Powerup[0]);
	wm->player->bindPats();
	wm->player->modelHandler->mdlClass->startAnimation(0, 1.2f, 10.0f, 0.0f);

	// since we've got all the resources, set up the path data too
	SpammyReport("preparing path manager\n");
	wm->pathManager.setup();

	// and put the player into position
	dKPNode_s *cNode = wm->pathManager.currentNode;
	wm->player->pos = (Vec){float(cNode->x), float(-cNode->y), wm->player->pos.z};

	// is last param correct? must check :/
	SpammyReport("creating MAP\n");
	wm->map = (dWMMap_c*)CreateParentedObject(WM_MAP, wm, 0, 0);
	SpammyReport("creating HUD\n");
	wm->hud = (dWMHud_c*)CreateParentedObject(WM_HUD, wm, 0, 0);
	// note: world_camera is not created here
	// because we require it earlier
	// it is created in dScKoopatlas_c::onCreate

	SpammyReport("creating SHOP\n");
	wm->shop = (dWMShop_c*)CreateParentedObject(WM_SHOP, wm, 0, 2);

	SpammyReport("creating Star Coin Menu\n");
	wm->coins = (dWMStarCoin_c*)CreateParentedObject(WM_STARCOIN, wm, 0, 0);


	SpammyReport("SetupExtra done\n");

	return true;
}

bool WMInit_SetupWipe(void *ptr) {
	SpammyReport("WMInit_SetupWipe called\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	if (wm->hasUninitialisedProcesses()) {
		SpammyReport("WMInit_SetupWipe returning false\n");

#ifdef WM_SPAMMY_DEBUGGING
		fBase_c *p = wm->findNextUninitialisedProcess();
		SpammyReport("Not done yet: %p [%d]\n", p, p->name);
#endif
		return false;
	}

	SpammyReport("WMInit_SetupWipe returning true\n");

	return true;
}





/*void dScKoopatlas_c::startLevel(LevelInfo::Entry *entry) {
  for (int i = 0; i < 4; i++) {
  bool isThere = QueryPlayerAvailability(i);
  int id = Player_ID[i];
  Player_Active[i] = isThere ? 1 : 0;
  if (!isThere) Player_Flags[i] = 0;
  }

  StartLevelInfo sl;
  sl.unk1 = 0;
  sl.unk2 = 0xFF;
  sl.unk3 = 0;
  sl.unk4 = 0;
  sl.purpose = 0;

  sl.world1 = entry->world;
  sl.world2 = entry->world;
  sl.level1 = entry->level;
  sl.level2 = entry->level;

// hopefully this will fix the Star Coin issues
SetSomeConditionShit(entry->world, entry->level, 2);

ActivateWipe(WIPE_MARIO);

DoStartLevel(GetGameMgr(), &sl);
}*/

void dScKoopatlas_c::startMusic() {
	dKPMusic::play(GetSaveFile()->GetBlock(-1)->currentMapMusic);
}


int dScKoopatlas_c::onCreate() {
	OSReport("KP scene settings: %08x\n", settings);

	SpammyReport("onCreate() called\n");

	SpammyReport("LoadMapScene()\n");
	LoadMapScene();

	SpammyReport("GameSetup__LoadScene(0)\n");
	GameSetup__LoadScene(0); // lol, stolen from GAME_SETUP

	SpammyReport("Setting Active Players\n");
	for (int i = 0; i < 4; i++) {
		bool isThere = QueryPlayerAvailability(i);
		int id = Player_ID[i];
		Player_Active[i] = isThere ? 1 : 0;
		if (!isThere) Player_Flags[i] = 0;
	}

	SpammyReport("select cursor\n");
	this->selectCursor = CreateParentedObject(SELECT_CURSOR, this, 0, 0);

	SpammyReport("cs menu\n");
	this->csMenu = CreateParentedObject(COURSE_SELECT_MENU, this, 0, 0);

	SpammyReport("yes no window\n");
	this->yesNoWindow = (dYesNoWindow_c*)CreateParentedObject(YES_NO_WINDOW, this, 0, 0);

	SpammyReport("number of people change\n");
	this->numPeopleChange = CreateParentedObject(NUMBER_OF_PEOPLE_CHANGE, this, 0, 0);

	for (int i = 0; i < 4; i++) {
		SpammyReport("ccsb %d\n", i);
		void *ccsb = CreateParentedObject(CHARACTER_CHANGE_SELECT_BASE, this, i, 0);
		SpammyReport("ccsc %d\n", i);
		void *ccsc = CreateParentedObject(CHARACTER_CHANGE_SELECT_CONTENTS, this, i, 0);
		SpammyReport("ccsa %d\n", i);
		void *ccsa = CreateParentedObject(CHARACTER_CHANGE_SELECT_ARROW, this, i, 0);
		SpammyReport("ccsi %d\n", i);
		void *cci = CreateParentedObject(CHARACTER_CHANGE_INDICATOR, this, i, 0);

		NPCHG_CCSB(this->numPeopleChange, i) = ccsb;
		NPCHG_CCSC(this->numPeopleChange, i) = ccsc;
		NPCHG_CCSA(this->numPeopleChange, i) = ccsa;
		NPCHG_CCI(this->numPeopleChange, i) = cci;
	}

	SpammyReport("continue\n");
	this->continueObj = CreateParentedObject(CONTINUE, this, 0, 0);

	SpammyReport("stock item\n");
	this->stockItem = (dStockItem_c*)CreateParentedObject(STOCK_ITEM, this, 0, 0);
	SpammyReport("stock item shadow\n");
	this->stockItemShadow = (dStockItemShadow_c*)CreateParentedObject(STOCK_ITEM_SHADOW, this, 0, 0);
	stockItem->shadow = stockItemShadow;

	SpammyReport("easy pairing\n");
	this->easyPairing = CreateParentedObject(EASY_PAIRING, this, 0, 0);

	SpammyReport("world camera\n");
	CreateParentedObject(WORLD_CAMERA, this, 0, 0);

	SpammyReport("setting NewerMapDrawFunc\n");
	*CurrentDrawFunc = NewerMapDrawFunc;

	SpammyReport("onCreate() completed\n");
	
	// Prepare this first
	isFirstPlayMessageDone = true;
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	currentMapID = save->current_world;
	isFirstPlay = (currentMapID == 0) && (settings & 0x80000000);
	if(isFirstPlay) {
		isFirstPlayMessageDone = false;
	}

	// Are we coming from Kamek cutscene? If so, then do.. some stuff!
	isAfterKamekCutscene = (settings & 0x40000000);
	if (isAfterKamekCutscene) {
		currentMapID = 6; // KoopaPlanet
		save->current_world = 6;
	}

	if (MaybeFinishingLevel[0] == 7 && MaybeFinishingLevel[1] == 24 && save->CheckLevelCondition(7, 24, COND_NORMAL)) {
		currentMapID = 7; // KoopaPlanetUnd
		save->current_world = 7;
		isAfter8Castle = true;
	}

	isEndingScene = (settings & 0x20000000);
	if (isEndingScene) {
		currentMapID = 0;
		save->current_world = 0;
		save->current_path_node = 0;
	}

	somethingAboutSound(_8042A788);

	return true;
}

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);

int dScKoopatlas_c::onDelete() {
	if (!keepMusicPlaying)
		dKPMusic::stop();

	FreeScene(0);
	FreeScene(1);

	CleanUpEffectThings();
	FreeEffects(1);
	FreeBreff(1);
	FreeBreft(1);

	DVD_FreeFile(GetDVDClass2(), "SI_kinoko");
	DVD_FreeFile(GetDVDClass2(), "SI_fireflower");
	DVD_FreeFile(GetDVDClass2(), "SI_iceflower");
	DVD_FreeFile(GetDVDClass2(), "SI_penguin");
	DVD_FreeFile(GetDVDClass2(), "SI_propeller");
	DVD_FreeFile(GetDVDClass2(), "SI_star");
	DVD_FreeFile(GetDVDClass2(), "SI_hammer");

	DVD_FreeFile(GetDVDClass2(), "cobCourse");
	DVD_FreeFile(GetDVDClass2(), "I_kinoko_bundle");
	DVD_FreeFile(GetDVDClass2(), "lakitu");
	DVD_FreeFile(GetDVDClass2(), "star_coin");
	DVD_FreeFile(GetDVDClass2(), "StarRing");

	mapListLoader.unload();

	CleanUpEffectThings();

	return true;
}

bool dScKoopatlas_c::canDoStuff() {
	if (QueryGlobal5758(0xFFFFFFFF)) return false;
	if (CheckIfWeCantDoStuff()) return false;
	if (state.getCurrentState() == &StateID_Limbo) return false;
	return true;
}

bool dScKoopatlas_c::mapIsRunning() {
	if (QueryGlobal5758(0xFFFFFFFF)) return false;
	if (CheckIfWeCantDoStuff()) return false;
	if (state.getCurrentState() != &StateID_Normal) return false;
	return true;
}

extern bool enableDebugMode;

int dScKoopatlas_c::onExecute() {
	dKPMusic::execute();
	if (!canDoStuff()) return true;

	// OSReport("Executing state: %s\n", state.getCurrentState()->getName());
	state.execute();

	int input = Remocon_GetButtons(GetActiveRemocon());
	if(enableDebugMode && (input & WPAD_MINUS)) {
		pathManager.completionMessagePending = true;
		pathManager.completionMessageType = CMP_MSG_EVERYTHING;
	}

	return true;
}


void dScKoopatlas_c::executeState_Limbo() { }


void dScKoopatlas_c::beginState_ContinueWait() {
	GameMgrP->_AFC = 1;
	CONT_UNK1(this->continueObj) = true;
	CONT_UNK2(this->continueObj) = true;
	CONT_UNK3(this->continueObj) = false;
}
void dScKoopatlas_c::executeState_ContinueWait() {
	// Waiting for the Continue anim to finish
	if (CONT_DONE(this->continueObj)) {
		CONT_UNK1(this->continueObj) = 0;
		CONT_UNK2(this->continueObj) = 0;
		CONT_UNK3(this->continueObj) = 0;

		static const int things[] = {0,1,3,2,4};
		for (int i = 0; i < 4; i++) {
			int idx = SearchForIndexOfPlayerID(things[i]);
			Player_Lives[Player_ID[idx]] = CONT_LIVES(this->continueObj, i);
		}

		state.setState(&StateID_Normal);
	}
}
void dScKoopatlas_c::endState_ContinueWait() {
	GameMgrP->_AFC = 0;
}


void dScKoopatlas_c::executeState_Normal() {
	// ghb
	if (pathManager.completionMessagePending) {
		OSReport("Going to set CompletionMsg\n");
		state.setState(&StateID_CompletionMsg);
		return;
	}
	if (!isFirstPlayMessageDone) {
		OSReport("Going to show the tip message\n");
		state.setState(&StateID_TipMsg);
		return;
	}

	if (pathManager.doingThings())
		return;

	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	// Nothing related to the menu is going on
	if (nowPressed & WPAD_ONE) {
		stockItem->show = true;
		state.setState(&StateID_PowerupsWait);
		hud->hideAll();
	} else if (nowPressed & WPAD_PLUS) {
		CSMENU_ACTIVE(this->csMenu) = true;
		state.setState(&StateID_CSMenu);
		hud->hideAll();
#ifdef NEWER_DEBUG
	// } else if (nowPressed & WPAD_MINUS) {
	// 	pathManager.unlockAllPaths(2);
	// } else if (nowPressed & WPAD_A) {
	// 	pathManager.unlockAllPaths(0);
	// 	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	// 	for (int w = 0; w < 6; w++)
	// 		for (int l = 0; l < 6; l++)
	// 			save->SetLevelCondition(w, l, COND_COIN_ALL);
#endif
	} 
}

void dScKoopatlas_c::executeState_CSMenu() {
	// The course select menu is currently being shown

	// First off, check to see if it's been hidden
	if (!CSMENU_ACTIVE(this->csMenu)) {
		// That means something happened
		if (CSMENU_CHOICE_OK(this->csMenu)) {
			// Player pressed a button
			SaveBlock *save = GetSaveFile()->GetBlock(-1);

			switch (CSMENU_CURRENT(this->csMenu)) {
				case 0:
					// Star Coins
					coins->show();
					state.setState(&StateID_CoinsWait);
					break;

				case 1:
					// Add/Drop Players
					MapReport("Add/Drop Players was pressed\n");
					state.setState(&StateID_PlayerChangeWait);
					NPCHG_ACTIVE(this->numPeopleChange) = true;
					WpadShit(10);

					break;

				case 2:
					// Save or Quick Save
					MapReport("Save or Quick Save was pressed\n");
#ifdef QUICK_SAVE
					if (GetSaveFile()->GetBlock(-1)->bitfield & 2) {
#endif
						state.setState(&StateID_SaveOpen);
						yesNoWindow->type = 1;
						yesNoWindow->visible = true;

#ifdef QUICK_SAVE
					} else {
						state.setState(&StateID_QuickSaveOpen);
						yesNoWindow->type = 15;
						yesNoWindow->visible = 1;

					}
#endif
					break;

				case 3:
					// Title Screen
					MapReport("Title Screen was pressed\n");
					state.setState(&StateID_TitleConfirmOpenWait);
					yesNoWindow->visible = true;
					yesNoWindow->type = 10;
					break;
			}

		} else {
			// Ok, change back to STATE_Normal
			hud->unhideAll();
			state.setState(&StateID_Normal);
		}
	}
}

void dScKoopatlas_c::executeState_TitleConfirmOpenWait() {
	// Waiting for the "Go to Title Screen" YesNoWindow to finish opening

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_TitleConfirmSelect);
	}

}

/**********************************************************************/
// STATE_TitleConfirmSelect : Let the user choose an option on the
// 	"Go to Title Screen" YesNoWindow.
void dScKoopatlas_c::executeState_TitleConfirmSelect() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (nowPressed & WPAD_LEFT) {
		// Select "OK!"
		yesNoWindow->current = 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Select "Cancel"
		yesNoWindow->current = 0;

	} else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
		// Pick the current option
		yesNoWindow->close = true;
		if (yesNoWindow->current != 1)
			yesNoWindow->keepOpen = true;
		state.setState(&StateID_TitleConfirmHitWait);

	} else {
		// Cancel using B or 1
		if (CheckIfMenuShouldBeCancelledForSpecifiedWiimote(0)) {
			yesNoWindow->cancelled = true;
			yesNoWindow->current = true;
			state.setState(&StateID_TitleConfirmHitWait);
		}
	}

}

/**********************************************************************/
// STATE_TitleConfirmHitWait : Process the user's chosen option on
// 	the "Go to Title Screen" YesNoWindow. Also, wait for the
//  animation to be complete.
void dScKoopatlas_c::executeState_TitleConfirmHitWait() {

	if (!yesNoWindow->animationActive) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_Limbo);
			StartTitleScreenStage(false, 0);
		}
	}

}

/**********************************************************************/
// STATE_PlayerChangeWait : Wait for the user to do something on the
// 	Add/Drop Players screen.
void dScKoopatlas_c::executeState_PlayerChangeWait() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (NPCHG_READY(this->numPeopleChange)) {
		if (nowPressed & WPAD_PLUS) {
			// activate easy pairing. FUN !!
			NPCHG_HIDE_FOR_EASYP(this->numPeopleChange) = 1;

			for (int i = 0; i < 4; i++) {
				void *obj = NPCHG_2DPLAYER(this->numPeopleChange, i);
				void *ccsb = NPCHG_CCSB(this->numPeopleChange, i);
				void *ccsc = NPCHG_CCSC(this->numPeopleChange, i);

				PLAYER2D_SHOW_EASY_PAIRING(obj) = 1;
				CCSB_ACTIVE(ccsb) = 1;
				CCSC_ACTIVE(ccsc) = 1;
			}

			EASYP_ACTIVE(this->easyPairing) = 1;
			state.setState(&StateID_EasyPairingWait);
		}
	} else {
		if (!NPCHG_ACTIVE(this->numPeopleChange)) {

			for (int i = 0; i < 4; i++) {
				bool isThere = QueryPlayerAvailability(i);
				int id = Player_ID[i];
				Player_Active[i] = isThere ? 1 : 0;
				if (!isThere) Player_Flags[i] = 0;
			}

			state.setState(&StateID_Normal);
			hud->unhideAll();
		}
	}

}

/**********************************************************************/
// STATE_EasyPairingWait : Wait for the user to exit Easy Pairing.
void dScKoopatlas_c::executeState_EasyPairingWait() {

	if (!EASYP_ACTIVE(this->easyPairing)) {
		NPCHG_HIDE_FOR_EASYP(this->numPeopleChange) = 0;

		for (int i = 0; i < 4; i++) {
			void *obj = NPCHG_2DPLAYER(this->numPeopleChange, i);
			void *ccsb = NPCHG_CCSB(this->numPeopleChange, i);
			void *ccsc = NPCHG_CCSC(this->numPeopleChange, i);

			PLAYER2D_SHOW_EASY_PAIRING(obj) = 0;
			CCSB_ACTIVE(ccsb) = 0;
			CCSC_ACTIVE(ccsc) = 0;
		}

		state.setState(&StateID_PlayerChangeWait);
		WpadShit(10);
	}

}

/**********************************************************************/
// STATE_PowerupsWait : Wait for the user to exit the Powerups screen.
void dScKoopatlas_c::executeState_PowerupsWait() {

	if (!stockItem->show) {
		player->modelHandler->mdlClass->setPowerup(Player_Powerup[0]);
		player->bindPats();

		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}


/**********************************************************************/
// STATE_ShopWait : Wait for the user to exit the Shop screen.
void dScKoopatlas_c::executeState_ShopWait() {

	if (!shop->visible) {
		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}


/**********************************************************************/
// STATE_StarCoin : Wait for the user to exit the Star Coin screen.
void dScKoopatlas_c::executeState_CoinsWait() {

	if (!coins->visible) {
		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}

/**********************************************************************/
// STATE_SaveOpen : Waiting for the "Save?" YesNoWindow to open
void dScKoopatlas_c::executeState_SaveOpen() {

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_SaveSelect);
	}

}

/**********************************************************************/
// STATE_SaveSelect : Let the user choose an option on the
// 	"Save?" YesNoWindow.
void dScKoopatlas_c::executeState_SaveSelect() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (nowPressed & WPAD_LEFT) {
		// Select "OK!"
		yesNoWindow->current = 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Select "Cancel"
		yesNoWindow->current = 0;

	} else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
		// Pick the current option
		yesNoWindow->close = true;

		if (yesNoWindow->current != 1)
			yesNoWindow->hasBG = true;
		state.setState(&StateID_SaveWindowClose);

	} else {
		// Cancel using B or 1
		if (CheckIfMenuShouldBeCancelledForSpecifiedWiimote(0)) {
			yesNoWindow->cancelled = true;
			yesNoWindow->current = 1;
			state.setState(&StateID_SaveWindowClose);
		}
	}

}

/**********************************************************************/
// STATE_SaveWindowClose : Process the user's chosen option on the
// 	"Save?" YesNoWindow. Also, wait for the animation to be complete.
void dScKoopatlas_c::executeState_SaveWindowClose() {

	if (!yesNoWindow->visible) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_SaveDo);
			SaveGame(0, false);
		}
	}

}

/**********************************************************************/
// STATE_SaveDo : Save the game.
void dScKoopatlas_c::executeState_SaveDo() {

	if (!GetSaveFile()->CheckIfWriting()) {
		if (GetSaveHandler()->CurrentError == 0) {
			yesNoWindow->type = 2;
			yesNoWindow->visible = true;
			state.setState(&StateID_SaveEndWindow);
		} else {
			state.setState(&StateID_SaveError);
		}
	}

}

/**********************************************************************/
// STATE_SaveEndWindow : Handle the Save End window.
void dScKoopatlas_c::executeState_SaveEndWindow() {

	if (!yesNoWindow->animationActive) {
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
			yesNoWindow->close = true;
			state.setState(&StateID_SaveEndCloseWait);
		}
	}

}

/**********************************************************************/
// STATE_SaveEndCloseWait : Wait for the Save End window to close.
void dScKoopatlas_c::executeState_SaveEndCloseWait() {

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}

#ifdef QUICK_SAVE
/**********************************************************************/
// STATE_QuickSaveOpen : Waiting for the "Save?" YesNoWindow to open
void dScKoopatlas_c::executeState_QuickSaveOpen() {

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_QuickSaveSelect);
	}

}

/**********************************************************************/
// STATE_QuickSaveSelect : Let the user choose an option on the
// 	"Save?" YesNoWindow.
void dScKoopatlas_c::executeState_QuickSaveSelect() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (nowPressed & WPAD_LEFT) {
		// Select "OK!"
		yesNoWindow->current = 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Select "Cancel"
		yesNoWindow->current = 0;

	} else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
		// Pick the current option
		yesNoWindow->close = true;

		if (yesNoWindow->current != 1)
			yesNoWindow->hasBG = true;
		state.setState(&StateID_QuickSaveWindowClose);

	} else {
		// Cancel using B or 1
		if (CheckIfMenuShouldBeCancelledForSpecifiedWiimote(0)) {
			yesNoWindow->cancelled = true;
			yesNoWindow->current = 1;
			state.setState(&StateID_QuickSaveWindowClose);
		}
	}

}

/**********************************************************************/
// STATE_QuickSaveWindowClose : Process the user's chosen option on
// 	the "Save?" YesNoWindow. Also, wait for the animation to be complete
void dScKoopatlas_c::executeState_QuickSaveWindowClose() {

	if (!yesNoWindow->visible) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_QuickSaveDo);
			SaveGame(0, true);
		}
	}

}

/**********************************************************************/
// STATE_QuickSaveDo : Save the game.
void dScKoopatlas_c::executeState_QuickSaveDo() {

	if (!GetSaveFile()->CheckIfWriting()) {
		if (GetSaveHandler()->CurrentError == 0) {
			yesNoWindow->type = 16;
			yesNoWindow->visible = true;
			state.setState(&StateID_QuickSaveEndWindow);
		} else {
			state.setState(&StateID_SaveError);
		}
	}

}

// STATE_QuickSaveEndWindow : Handle the Save End window.
void dScKoopatlas_c::executeState_QuickSaveEndWindow() {

	if (!yesNoWindow->animationActive) {
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
			yesNoWindow->close = true;
			yesNoWindow->keepOpen = true;
			state.setState(&StateID_QuickSaveEndCloseWait);
		}
	}

}

void dScKoopatlas_c::executeState_QuickSaveEndCloseWait() {
	// Wait for Save End window to close

	if (!yesNoWindow->animationActive) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_Limbo);
			StartTitleScreenStage(false, 0);
		}
	}

}
#endif


void dScKoopatlas_c::executeState_SaveError() { }

extern int previewID;
extern bool enableHardMode;
extern bool wasHardModeReallyEnabled;
void dScKoopatlas_c::startLevel(dLevelInfo_c::entry_s *entry) {

	StartLevelInfo sl;
	sl.replayTypeMaybe = 0;
	sl.entrance = 0xFF;
	sl.areaMaybe = 0;
	sl.unk4 = 0;
	sl.purpose = 0;

	sl.world1 = entry->worldSlot;
	sl.world2 = entry->worldSlot;
	sl.level1 = entry->levelSlot;
	sl.level2 = entry->levelSlot;

	// Player_ID[0] = 1;

	ActivateWipe(WIPE_MARIO);

	if(GameMgrP->eight.checkpointEntranceID != 255 && GameMgrP->eight.checkpointWorldID == entry->worldSlot && GameMgrP->eight.checkpointLevelID == entry->levelSlot) {
		previewID = 1;
	}
	else {
		previewID = 0;
	}

	if(entry->levelSlot == 06) {//so filename == XX-07
		if(!wasHardModeReallyEnabled) {
			enableHardMode = true;
		}
	}
	else {
		if(!wasHardModeReallyEnabled) {
			enableHardMode = false;
		}
	}

	DoStartLevel(GetGameMgr(), &sl);
}


u32 dScKoopatlas_c::iterateMapList(u32(*callback)(u32,const char *,int,int), u32 userData, int *ptrIndex) {
	u8 *ptr = (u8*)mapListLoader.buffer;
	u8 *strStart = ptr;
	u8 *end = ptr + mapListLoader.size;
	int index = 0;

	while (true) {
		u8 chr = *ptr;
		if (chr == 13) {
			*ptr = 0;
			++ptr;
			continue;
		}

		if (chr == 10 || chr == 0 || ptr >= end) {
			if (strStart == ptr) {
				// Blank string, ignore
				++strStart;
				++ptr;
				continue;
			}

			// Change the linefeed to a NUL so we can use the line as a C string later
			if (ptr < end)
				*ptr = 0;

			u32 ret = callback(userData, (const char*)strStart, ptr - strStart, index);
			if (ptrIndex)
				*ptrIndex = index;
			if (ret > 0)
				return ret;

			strStart = ++ptr;
			++index;

			if (ptr >= end)
				break;

		} else {
			++ptr;
		}
	}

	return 0;
}

static u32 _cb_getIndex(u32 userData, const char *str, int size, int index) {
	if (index == userData)
		return (u32)str;
	else
		return 0;
}

static u32 _cb_searchName(u32 userData, const char *str, int size, int index) {
	if (strncmp(str, (const char*)userData, size) == 0)
		return (u32)(index+1);
	else
		return 0;
}

const char *dScKoopatlas_c::getMapNameForIndex(int index) {
	return (const char *)iterateMapList(&_cb_getIndex, (u32)index, 0);
}

int dScKoopatlas_c::getIndexForMapName(const char *name) {
	return ((int)iterateMapList(&_cb_searchName, (u32)name, 0)) - 1;
}


void dScKoopatlas_c::showSaveWindow() {
	hud->hideAll();
	state.setState(&StateID_SaveOpen);
	yesNoWindow->type = 1;
	yesNoWindow->visible = true;
}

static const wchar_t *completionMsgsJP[] = {
	L"NANIIIIIIIIIII?!",
	L"\nですべての\x0B\x014F\xBEEFスターコ\nインをかくとくしました！",
	L"\nですべての\x0B\x013B\xBEEFゴール\nをコンプリートしました！",
	L"\nのすべてのコース\nをコンプリートしました！",
	L"こんにちは、RedStoneMattです。",
	L"Super Luigi Land Wiiですべての\n\x0B\x014F\xBEEFスターコインを\nかくとくしました！",
	L"Super Luigi Land Wiiですべての\n\x0B\x013B\xBEEFゴールをコンプリートしました！",
	L"Super Luigi Land Wiiを\nすべてコンプリートしました！！\n\nここまであそんでくれてありがとう！\nスターコイン画面で「\x0B\x0122\xBEEF」と「\x0B\x0123\xBEEF」と「\x0B\x0125\xBEEF」\nをどうじにおしてみてください。\n何かおこるかも？"
};

static const wchar_t *completionMsgsGE[] = {
	L"UwU",
	L"Du hast alle \x0B\x014F\xBEEF Sternenmünzen in\n",
	L"Du hast jeden \x0B\x013B\xBEEF Ausgang\nin",
	L"Du hast alles in\n",
	L"Du hast genügend \x0B\x014F\xBEEF Sternenmünzen\ngesammelt, um die Spezialwelt zu betreten!\nDies wird sowieso nicht benutzt. ¯\\_(ツ)_/¯",
	L"Du hast alle \x0B\x014F\xBEEF Sternenmünzen\nim Spiel gefunden!",
	L"Du hast jeden \x0B\x013B\xBEEF Ausgang\nim Spiel gefunden!",
	L"Du hast alles in\nSuper Luigi Land Wii gefunden!\nWir haben aber noch eine neue\nHerausforderung für dich!\nDrücke \x0B\x0122\xBEEF, \x0B\x0123\xBEEF und \x0B\x0125\xBEEF, im\nSternenmünzenmenü."
};

static const wchar_t *completionMsgsEN[] = {
	L"The most erudite of Buttocks (OwO treeki hides messages here too !)",
	L"You've collected all of\nthe \x0B\x014F\xBEEF Star Coins in\n",
	L"You have gotten every \x0B\x013B\xBEEF exit\nin",
	L"You have gotten everything\nin",
	L"You have collected all the\nnecessary \x0B\x014F\xBEEF coins to enter\nthe Special World!",
	L"You have collected all the \x0B\x014F\xBEEF Star\nCoins in the game!",
	L"You've found every \x0B\x013B\xBEEF exit in the\ngame!",
	L"You've completed everything in\nSuper Luigi Land Wii!\n\nWe present you a new quest.\nTry pressing \x0B\x0122\xBEEF, \x0B\x0123\xBEEF and \x0B\x0125\xBEEF\non the Star Coin menu."
};

static const wchar_t *completionMsgsFR[] = {
	L"Treeki faut penser a arrêter les messages cachés",
	L"Vous avez collecté toutes les\n\x0B\x014F\xBEEF Pièces Étoiles dans\n",
	L"Vous avez trouvé toutes les\n\x0B\x013B\xBEEF sorties dans \n",
	L"Vous avez tout obtenu\ndans",
	L"Vous avez collecté toutes les\n\x0B\x014F\xBEEF Pièces Étoiles nécéssaires\npour entrer dans le Monde Spécial!",
	L"Vous avez obtenu toutes les\n\x0B\x014F\xBEEF Pièces Étoiles du jeu!",
	L"Vous avez trouvé toutes les\n\x0B\x013B\xBEEF sorties du jeu !",
	L"Vous avez tout complété dans\nSuper Luigi Land Wii!\n\nNous avons donc une\nnouvelle quête pour vous:\nAppuyez sur \x0B\x0122\xBEEF, \x0B\x0123\xBEEF et \x0B\x0125\xBEEF\ndans le menu des Pièces Étoiles."
};

static const wchar_t *completionMsgsSP[] = {
	L"Descargar ya super luigi land wii para android hack 2020",
	L"¡Has conseguido todas las\n\x0B\x014F\xBEEF Monedas Estrella en\n",
	L"¡Has encontrado todas las\n\x0B\x013B\xBEEF salidas en\n",
	L"¡Has conseguido todo en\n",
	L"¡Has conseguido todas las\n\x0B\x014F\xBEEF Monedas Estrella\nnecesarias para ir al\nMundo Especial!",
	L"¡Has conseguido todas las\n\x0B\x014F\xBEEF Monedas Estrella en\nel juego!",
	L"¡Has encontrado todas las\n\x0B\x013B\xBEEF salidas en\nel juego!",
	L"¡Has completado todo en\nSuper Luigi Land Wii!\n\nTe presentamos un nuevo reto.\nApreta \x0B\x0122\xBEEF, \x0B\x0123\xBEEF y \x0B\x0125\xBEEF en el\nmenú de las Monedas Estrella."
};

static const wchar_t *completionMsgsIT[] = {
	L"Pizza",
	L"Hai raccolto tutte le\n\x0B\x014F\xBEEF Monete Stella in\n",
	L"Hai trovato tutte le\n\x0B\x013B\xBEEF uscite in\n",
	L"Hai trovato tutto in\n",
	L"Hai raccolto tutte le \x0B\x014F\xBEEF\nMonete Stella necessarie per\naccedere al Mondo Speciale!",
	L"Hai raccolto tutte le \x0B\x014F\xBEEF\nMonete Stella nel gioco!",
	L"Hai trovato tutte le\n\x0B\x013B\xBEEF uscite nel gioco!",
	L"Hai completato tutto in\nSuper Luigi Land Wii!\n\nTi presentiamo una nuova sfida.\nProva a premere \x0B\x0122\xBEEF, \x0B\x0123\xBEEF e \x0B\x0125\xBEEF\nsul menu Monete Stella."
};

static const wchar_t *completionMsgsDU[] = {
	L"The most erudite of Buttocks",
	L"Je hebt alle \x0B\x014F\xBEEF Sterrenmunten\nvezameld in\n",
	L"Je hebt iedere \x0B\x013B\xBEEF uitgang in\n",
	L"Je hebt alles gevonden\nin",
	L"Je hebt alle benodigde\n\x0B\x014F\xBEEF Sterrenmunten verzameld\nvoor de Speciale Wereld!",
	L"Je hebt alle \x0B\x014F\xBEEF Sterrenmunten\nin het spel verzameld!",
	L"Je hebt ieder \x0B\x013B\xBEEF uitgang\nin het spel gevonden!",
	L"Je hebt alles gevonden in\nSuper Luigi Land Wii!\n\nWe hebben een nieuwe uitdaging\nvoor je. Klik op \x0B\x0122\xBEEF, \x0B\x0123\xBEEF en \x0B\x0125\xBEEF in het\nSterrenmunten Menu."
 };

extern "C" int GetGameLanguage(int nyeh); //nyeh is always 4 for some reasons

void dScKoopatlas_c::beginState_CompletionMsg() {
	if (pathManager.completionMessageType == 0)
		pathManager.completionMessageType = 1;
	OSReport("CompletionMsg beginning with type %d\n", pathManager.completionMessageType);
	static const int ynTypes[8] = {
		/*NULL*/ -1,
		/*COINS*/ 14,
		/*EXITS*/ 7,
		/*WORLD*/ 8,
		/*COINS EXC W9*/ 9,
		/*GLOBAL COINS*/ 11,
		/*GLOBAL EXITS*/ 27,
		/*EVERYTHING*/ 21
	};
	yesNoWindow->type = ynTypes[pathManager.completionMessageType];
	yesNoWindow->visible = true;
	mustFixYesNoText = 10; // hacky shit
}

void dScKoopatlas_c::endState_CompletionMsg() {
	// ghb
	pathManager.completionMessagePending = false;
	pathManager.completionMessageType = 0;
	//pathManager.completionMessagePending = true;
	//pathManager.completionMessageType ++;
}

void dScKoopatlas_c::executeState_CompletionMsg() {
	// hacky shit
	if (mustFixYesNoText > 0) {
		mustFixYesNoText--;

		int type = pathManager.completionMessageType;

		const wchar_t *baseText;

		if(GetGameLanguage(4) == 0) { // Japanese
			baseText = completionMsgsJP[type];
		}
		if(GetGameLanguage(4) == 1) { // English
			baseText = completionMsgsEN[type];
		}
		if(GetGameLanguage(4) == 2) { // German
			baseText = completionMsgsGE[type];
		}
		if(GetGameLanguage(4) == 3) { // French
			baseText = completionMsgsFR[type];
		}
		if(GetGameLanguage(4) == 4) { // Spanish
			baseText = completionMsgsSP[type];
		}
		if(GetGameLanguage(4) == 5) { // Italian
			baseText = completionMsgsIT[type];
		}
		if(GetGameLanguage(4) == 6) { // Dutch
			baseText = completionMsgsDU[type];
		}


		// Used when we assemble a dynamic message
		wchar_t text[512];

		if (type >= CMP_MSG_COINS && type <= CMP_MSG_WORLD) {
			// title
			int w = pathManager.completionMessageWorldNum;
			int l = ((w == 5) || (w == 7)) ? 101 : 100;
			dLevelInfo_c::entry_s *titleEntry = dLevelInfo_c::s_info.searchByDisplayNum(w, l);
			const char *title = dLevelInfo_c::s_info.getNameForLevel(titleEntry);

			// assemble the string
			if(GetGameLanguage(4) > 0) {
				wcscpy(text, baseText);
				int pos = wcslen(text);

				text[pos++] = ' ';

				while (*title) {
					char chr = *(title++);
					if (chr != '-')
						text[pos++] = (unsigned char)chr;
				}

				if(GetGameLanguage(4) == 2) { // German
					text[pos++] = '\n';
					text[pos++] = 'g';
					text[pos++] = 'e';
					text[pos++] = 'f';
					text[pos++] = 'u';
					text[pos++] = 'n';
					text[pos++] = 'd';
					text[pos++] = 'e';
					text[pos++] = 'n';
				}
				if(GetGameLanguage(4) == 6 && type == CMP_MSG_EXITS) { // Dutch + Exit message
					text[pos++] = '\n';
					text[pos++] = 'g';
					text[pos++] = 'e';
					text[pos++] = 'v';
					text[pos++] = 'o';
					text[pos++] = 'n';
					text[pos++] = 'd';
					text[pos++] = 'e';
					text[pos++] = 'n';
				}
				text[pos++] = '!';
				text[pos++] = 0;
				baseText = text;
			}
			else {
				GetJapaneseWorldName(text, pathManager.completionMessageWorldNum-1);

				int pos = wcslen(text);

				for(int i = 0; baseText[i] != 0; i++) {
					text[pos++] = baseText[i];
				}

				text[pos++] = 0;
				baseText = text;
			}
		}

		yesNoWindow->T_question_00->SetString(baseText);
		yesNoWindow->T_questionS_00->SetString(baseText);
	}

	if (!yesNoWindow->animationActive) {
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
			yesNoWindow->close = true;
			state.setState(&StateID_CompletionMsgHideWait);
		}
	}
}

void dScKoopatlas_c::executeState_CompletionMsgHideWait() {
	if (!yesNoWindow->visible)
		state.setState(&StateID_Normal);
}

static const wchar_t *tipMsgJP = L"ーおしらせー\nミスをすると、じどうてきに\nコースのはじめからさいかいします。\nワールドマップにいどう\nするには、ミスしたあと\n\x0B\x0128\xBEEF または\x0B\x0122\xBEEF をながおししてください。";

static const wchar_t *tipMsgEN = L"Tip:\nAfter losing a life, you'll\nautomatically go back to the\nlevel you were playing.\nTo go to the world map instead,\nhold the \x0B\x0128\xBEEF or \x0B\x0122\xBEEF button right\nafter getting hit.";

static const wchar_t *tipMsgGE = L"Hinweis:\nWenn du stirbst, wirst\ndu automatisch in das gerade\ngespielte Level zurückgebracht.\nUm zur Karte zurückzukehren,\nhalte den \x0B\x0128\xBEEF Knopf gedrückt,\nnachdem du getroffen wurdest."; //" oder \x0B\x0122\xBEEF" was removed cuz too long

static const wchar_t *tipMsgFR = L"Astuce:\nAprès avoir perdu une vie, vous\nrecommencerez automatiquement\nle niveau dans lequel vous étiez.\nPour retourner à la carte du\nmonde à la place, maintenez \x0B\x0128\xBEEF\nou \x0B\x0122\xBEEF après vous être fait touché.";

static const wchar_t *tipMsgSP = L"Ayuda:\nDespués de perder una vida,\nvolverás automáticamente\nal nivel que estabas jugando.\nPara volver al mapa en vez,\nmantén apretado \x0B\x0128\xBEEF o \x0B\x0122\xBEEF\ndespués de ser golpeado.";

static const wchar_t *tipMsgIT = L"Suggerimento:\nQuando perdi una vita,\ntornerai automaticamente al\nlivello che stavi giocando.\nPer tornare alla mappa, tieni\npremuto il pulsante \x0B\x0128\xBEEF o \x0B\x0122\xBEEF\nappena vieni colpito.";

static const wchar_t *tipMsgDU = L"Tip:\nNa het verliezen van een leven\nga je automatisch terug het level\nin wat je aan het spelen was.\nOm in plaats daarvan naar de\nwereld map te gaan moet je\n\x0B\x0128\xBEEF of \x0B\x0122\xBEEF knop indrukken";

void dScKoopatlas_c::beginState_TipMsg() {
	yesNoWindow->type = 14;
	yesNoWindow->visible = true;
	mustFixYesNoText = 10; // hacky shit
}

void dScKoopatlas_c::endState_TipMsg() {
	isFirstPlayMessageDone = true;
}

void dScKoopatlas_c::executeState_TipMsg() {
	if (mustFixYesNoText > 0) {
		mustFixYesNoText--;

		const wchar_t *baseText;

		if(GetGameLanguage(4) == 0) { // Japanese
			baseText = tipMsgJP;
		}
		if(GetGameLanguage(4) == 1) { // English
			baseText = tipMsgEN;
		}
		if(GetGameLanguage(4) == 2) { // German
			baseText = tipMsgGE;
		}
		if(GetGameLanguage(4) == 3) { // French
			baseText = tipMsgFR;
		}
		if(GetGameLanguage(4) == 4) { // Spanish
			baseText = tipMsgSP;
		}
		if(GetGameLanguage(4) == 5) { // Italian
			baseText = tipMsgIT;
		}
		if(GetGameLanguage(4) == 6) { // Dutch
			baseText = tipMsgDU;
		}

		yesNoWindow->T_question_00->SetString(baseText);
		yesNoWindow->T_questionS_00->SetString(baseText);
	}

	if (!yesNoWindow->animationActive) {
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
			yesNoWindow->close = true;
			state.setState(&StateID_TipMsgHideWait);
		}
	}
}

void dScKoopatlas_c::executeState_TipMsgHideWait() {
	if (!yesNoWindow->visible)
		state.setState(&StateID_Normal);
}


void NewerMapDrawFunc() {
	Reset3DState();
	SetCurrentCameraID(0);
	DoSomethingCameraRelatedWithEGGScreen(0, &dWorldCamera_c::instance->screen);
	LinkScene(0);
	SceneCalcWorld(0);
	SceneCameraStuff(0);
	ChangeAlphaUpdate(false);
	DrawOpa();
	DrawXlu();
	UnlinkScene(0);
	// Something
	SetupLYTDrawing();
	DrawAllLayoutsBeforeX(0x81);
	RenderEffects(0, 3);
	RenderEffects(0, 2);
	GXDrawDone();
	RemoveAllFromScnRoot();
	Reset3DState();
	SetCurrentCameraID(1);
	DoSpecialDrawing1();
	LinkScene(1);
	SceneCalcWorld(1);
	SceneCameraStuff(1);
	CalcMaterial();
	DrawOpa();
	DrawXlu();
	UnlinkScene(1);
	SetCurrentCameraID(0);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 0xB+i);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 7+i);
	GXDrawDone();
	// Leaving out some stuff here
	DrawAllLayoutsAfterX(0x80);
	ClearLayoutDrawList();
	SetCurrentCameraID(0);
}


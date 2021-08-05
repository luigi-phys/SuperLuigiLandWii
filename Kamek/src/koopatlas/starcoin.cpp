#include "koopatlas/starcoin.h"
#include <game.h>

dWMStarCoin_c *dWMStarCoin_c::instance = 0;

dWMStarCoin_c *dWMStarCoin_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMStarCoin_c));
	dWMStarCoin_c *c = new(buffer) dWMStarCoin_c;

	instance = c;
	return c;
}

dWMStarCoin_c::dWMStarCoin_c() : state(this) {
	layoutLoaded = false;
	visible = false;
	state.setState(&StateID_Hidden);
}

CREATE_STATE(dWMStarCoin_c, Hidden);
CREATE_STATE(dWMStarCoin_c, ShowWait);
CREATE_STATE(dWMStarCoin_c, ShowSectionWait);
CREATE_STATE(dWMStarCoin_c, Wait);
CREATE_STATE(dWMStarCoin_c, HideSectionWait);
CREATE_STATE(dWMStarCoin_c, HideWait);

extern "C" int GetGameLanguage(int nyeh); //nyeh is always 4 for some reasons

int dWMStarCoin_c::onCreate() {

	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("StarCoins.arc", false);
		if (!gotFile)
			return false;

		bool output = layout.build(((GetGameLanguage(4) == 0 || GetGameLanguage(4) == 2) ? "StarCoinsJP.brlyt" : "StarCoins.brlyt"));

		layout.layout.rootPane->trans.x = -112.0f;
		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.735f;
		} else {
			layout.clippingEnabled = true;
			layout.clipX = 0;
			layout.clipY = 52;
			layout.clipWidth = 640;
			layout.clipHeight = 352;
			layout.layout.rootPane->scale.x = 0.731f;
			layout.layout.rootPane->scale.y = 0.7711f;
		}

		static const char *brlanNames[] = {
			"StarCoins_Show.brlan",
			"StarCoins_ShowSection.brlan",
			"StarCoins_HideSection.brlan",
			"StarCoins_ShowArrow.brlan",
			"StarCoins_HideArrow.brlan",
		};
		static const char *groupNames[] = {
			"base", "section", "section", "leftArrow", "leftArrow", "rightArrow", "rightArrow"
		};

		layout.loadAnimations(brlanNames, 5);
		layout.loadGroups(groupNames, (int[7]){0, 1, 2, 3, 4, 3, 4}, 7);
		layout.disableAllAnimations();

		layout.drawOrder = 1;

		for (int col = 0; col < COLUMN_COUNT; col++) {
			for (int shine = 0; shine < SHINE_COUNT; shine++) {
				char name[8];
				sprintf(name, "Shine%d%d", col, shine);
				Shine[col][shine] = layout.findPictureByName(name);
			}

			for (int row = 0; row < ROW_COUNT; row++) {
				char lname[12];
				sprintf(lname, "LevelName%d%d", col, row);
				LevelName[col][row] = layout.findTextBoxByName(lname);

				char coname[16], cname[8];
				for (int i = 0; i < 3; i++) {
					sprintf(coname, "CoinOutline%d%d%d", col, row, i);
					CoinOutline[col][row][i] = layout.findPictureByName(coname);

					sprintf(cname, "Coin%d%d%d", col, row, i);
					Coin[col][row][i] = layout.findPictureByName(cname);
				}
			}
		}

		static const char *tbNames[] = {
			"LeftTitle", "RightTitle", "TotalCoinCount",
			"EarnedCoinCount", "EarnedCoinMax", "BtnBackText", "CollectedTitle",
			"BtnWorldSelText", "TotalCoinsTitle",
		};
		layout.getTextBoxes(tbNames, &LeftTitle, 9);

		static const char *picNames[] = {
			"DPadLeft", "DPadRight",
		};
		layout.getPictures(picNames, &DPadLeft, 2);

		DPadLeft->SetVisible(false);
		DPadRight->SetVisible(false);

		layoutLoaded = true;
	}

	return true;
}


int dWMStarCoin_c::onDelete() {
	return layout.free();
}


void dWMStarCoin_c::show() {
	if (state.getCurrentState() == &StateID_Hidden)
		state.setState(&StateID_ShowWait);
}


int dWMStarCoin_c::onExecute() {
	state.execute();

	if (visible) {
		layout.execAnimations();
		layout.update();
	}

	return true;
}

int dWMStarCoin_c::onDraw() {
	if (visible)
		layout.scheduleForDrawing();

	return true;
}


void dWMStarCoin_c::showLeftArrow() {
	if (!isLeftArrowVisible) {
		isLeftArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_LEFT_ARROW);
		DPadLeft->SetVisible(true);
	}
}

void dWMStarCoin_c::showRightArrow() {
	if (!isRightArrowVisible) {
		isRightArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_RIGHT_ARROW);
		DPadRight->SetVisible(true);
	}
}

void dWMStarCoin_c::hideLeftArrow() {
	if (isLeftArrowVisible) {
		isLeftArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_LEFT_ARROW);
		DPadLeft->SetVisible(false);
	}
}

void dWMStarCoin_c::hideRightArrow() {
	if (isRightArrowVisible) {
		isRightArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_RIGHT_ARROW);
		DPadRight->SetVisible(false);
	}
}

void dWMStarCoin_c::setLeftArrowVisible(bool value) {
	if (value)
		showLeftArrow();
	else
		hideLeftArrow();
}

void dWMStarCoin_c::setRightArrowVisible(bool value) {
	if (value)
		showRightArrow();
	else
		hideRightArrow();
}


bool dWMStarCoin_c::canScrollLeft() const {
	return (currentSectionIndex > 0);
}
bool dWMStarCoin_c::canScrollRight() const {
	return (currentSectionIndex < (availableSectionCount - 1));
}

void dWMStarCoin_c::loadInfo() {
	WriteBMGToTextBox(BtnBackText, GetBMG(), 3, 1, 0);
	
	// Language-related things
	if(GetGameLanguage(4) == 0) { // Japanese
		CollectedTitle->SetString(L"まい");
		BtnWorldSelText->SetString(L"ワールド選択    ");
		TotalCoinsTitle->SetString(L"スターコイン:");
	}
	if(GetGameLanguage(4) == 1) { // English
		CollectedTitle->SetString(L"collected");
		BtnWorldSelText->SetString(L"Select World     ");
		TotalCoinsTitle->SetString(L"Star Coins:");
	}
	if(GetGameLanguage(4) == 2) { // German
		CollectedTitle->SetString(L"gesammelt");
		BtnWorldSelText->SetString(L"Wähle eine Welt     ");
		TotalCoinsTitle->SetString(L"Sternenmünzen:");
	}
	if(GetGameLanguage(4) == 3) { // French
		CollectedTitle->SetString(L"collectées");
		BtnWorldSelText->SetString(L"Choisir Monde     ");
		TotalCoinsTitle->SetString(L"Pièces Étoiles:");
	}
	if(GetGameLanguage(4) == 4) { // Spanish
		CollectedTitle->SetString(L"conseguidas");
		BtnWorldSelText->SetString(L"Elegir Mundo     ");
		TotalCoinsTitle->SetString(L"Monedas Estrella:");
	}
	if(GetGameLanguage(4) == 5) { // Italian
		CollectedTitle->SetString(L"raccolte");
		BtnWorldSelText->SetString(L"Scegli un Mondo     ");
		TotalCoinsTitle->SetString(L"Monete Stella:");
	}
	if(GetGameLanguage(4) == 6) { // Dutch
		CollectedTitle->SetString(L"verzameld");
		BtnWorldSelText->SetString(L"Selecteer Wereld     ");
		TotalCoinsTitle->SetString(L"Sterrenmunten:");
	}

	int coins = getStarCoinCount();

	WriteNumberToTextBox(&coins, TotalCoinCount, false);

	currentSection = -1;
	currentSectionIndex = -1;
	availableSectionCount = 0;

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int wantedSection = save->newerWorldID;

	// figure out which sections should be available
	for (int i = 0; i < dLevelInfo_c::s_info.sectionCount(); i++) {
		dLevelInfo_c::section_s *section = dLevelInfo_c::s_info.getSectionByIndex(i);

		bool haveLevels = false;
		for (int j = 0; j < section->levelCount; j++) {
			dLevelInfo_c::entry_s *l = &section->levels[j];
			if (l->flags & 2) {
				if (save->GetLevelCondition(l->worldSlot, l->levelSlot) & COND_UNLOCKED) {
					haveLevels = true;
					break;
				}
			}
		}

		if (haveLevels) {
			if (i == wantedSection) {
				currentSection = wantedSection;
				currentSectionIndex = availableSectionCount;
			}
			sectionIndices[availableSectionCount++] = i;
		}
	}

	// if we didn't find the wanted one, use the first one available
	if (currentSectionIndex == -1) {
		currentSectionIndex = 0;
		currentSection = sectionIndices[0];
	}
}

extern "C" int GetGameLanguage(int nyeh); //nyeh is always 4 for some reasons

void dWMStarCoin_c::loadSectionInfo() {
	dLevelInfo_c::entry_s *visibleLevels[COLUMN_COUNT][ROW_COUNT];

	// reset everything... everything
	for (int i = 0; i < COLUMN_COUNT; i++) {
		for (int j = 0; j < SHINE_COUNT; j++)
			Shine[i][j]->SetVisible(false);

		for (int j = 0; j < ROW_COUNT; j++) {
			visibleLevels[i][j] = 0;

			LevelName[i][j]->SetVisible(false);

			for (int k = 0; k < 3; k++) {
				CoinOutline[i][j][k]->SetVisible(false);
				Coin[i][j][k]->SetVisible(false);
			}
		}
	}

	// get everything we'll need
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	dLevelInfo_c *linfo = &dLevelInfo_c::s_info;

	dLevelInfo_c::entry_s *names[COLUMN_COUNT];
	for (int i = 0; i < COLUMN_COUNT; i++)
		names[i] = linfo->searchByDisplayNum(currentSection, 100+i);

	bool useSubworlds = (COLUMN_COUNT > 1) && names[1];

	int currentPosition[COLUMN_COUNT];
	int currentColumn = 0; // only incremented in single-subworld mode

	for (int i = 0; i < COLUMN_COUNT; i++)
		currentPosition[i] = 0;

	dLevelInfo_c::section_s *section = linfo->getSectionByIndex(currentSection);

	int earnedCoins = 0, earnableCoins = 0;
	// earnedCoins is calculated later

	for (int i = 0; i < section->levelCount; i++) {
		dLevelInfo_c::entry_s *level = &section->levels[i];

		// only pay attention to real levels
		if (!(level->flags & 2))
			continue;

		earnableCoins += 3;

		// is this level unlocked?
		u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

		if (!(conds & COND_UNLOCKED))
			continue;

		// well, let's give it a slot
		if (useSubworlds) {
			currentColumn = (level->flags & 0x400) ? 1 : 0;
		} else {
			if (currentPosition[currentColumn] >= ROW_COUNT)
				currentColumn++;
		}

		visibleLevels[currentColumn][currentPosition[currentColumn]++] = level;
	}

	// if the first column is empty, then move the second one over
	if (currentPosition[0] == 0 && useSubworlds) {
		for (int i = 0; i < currentPosition[1]; i++) {
			visibleLevels[0][i] = visibleLevels[1][i];
			visibleLevels[1][i] = 0;
		}

		names[0] = names[1];
		names[1] = 0;
	}

	// if the second column is empty, remove its name
	if (currentPosition[1] == 0 && useSubworlds)
		names[1] = 0;

	WriteWorldNameToTextBox(LeftTitle, currentSection);
	if (names[1])
		WriteAsciiToTextBox(RightTitle, linfo->getNameForLevel(names[1]));
	RightTitle->SetVisible(names[1] != 0);

	// load all level info
	for (int col = 0; col < COLUMN_COUNT; col++) {
		for (int row = 0; row < ROW_COUNT; row++) {
			dLevelInfo_c::entry_s *level = visibleLevels[col][row];
			if (!level)
				continue;

			u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

			if (!(row & 1)) {
				int shineID = row / 2;
				if (shineID < SHINE_COUNT)
					Shine[col][shineID]->SetVisible(true);
			}

			for (int coin = 0; coin < 3; coin++) {
				CoinOutline[col][row][coin]->SetVisible(true);

				if (conds & (COND_COIN1 << coin)) {
					Coin[col][row][coin]->SetVisible(true);
					earnedCoins++;
				}
			}

			LevelName[col][row]->SetVisible(true);
			WriteLevelNameToTextBox(LevelName[col][row], level->worldSlot, level->levelSlot);
		}
	}

	// set up coin things
	WriteNumberToTextBox(&earnedCoins, EarnedCoinCount, false);
	WriteNumberToTextBox(&earnableCoins, EarnedCoinMax, false);
}


void dWMStarCoin_c::beginState_Hidden() { }
void dWMStarCoin_c::executeState_Hidden() { }
void dWMStarCoin_c::endState_Hidden() { }

static const int secretCode[] = {
	WPAD_UP,WPAD_UP,WPAD_DOWN,WPAD_DOWN,
	WPAD_LEFT,WPAD_RIGHT,WPAD_LEFT,WPAD_RIGHT,
	WPAD_ONE,WPAD_TWO,0
};
static const int secretCodeButtons = WPAD_UP|WPAD_DOWN|WPAD_LEFT|WPAD_RIGHT|WPAD_ONE|WPAD_TWO;
static int secretCodeIndex = 0; //"B, A, 1, 2, Up, Down"

static const int travelerCode[] = { //What ? You really thought i'd make it readable so easily ? Go check MessagesSP.bin, at message 26, and do it LEGITIMATLY !!!
	1024,2048,512,256,
	2,1,0
};
static const int travelerCodeButtons = 1024|2048|512|256|2|1;
static int travelerCodeIndex = 0;

static int minusCount = 0;
extern bool enableHardMode;
extern bool wasHardModeReallyEnabled;
bool enableDebugMode;
extern u8 isReplayEnabled;

void dWMStarCoin_c::beginState_ShowWait() {
	visible = true;
	loadInfo();
	layout.enableNonLoopAnim(SHOW_ALL);
	layout.resetAnim(SHOW_SECTION);
	layout.resetAnim(SHOW_LEFT_ARROW);
	layout.resetAnim(SHOW_RIGHT_ARROW);

	secretCodeIndex = 0;
	minusCount = 0;
}
void dWMStarCoin_c::executeState_ShowWait() {
	if (!layout.isAnimOn(SHOW_ALL))
		state.setState(&StateID_ShowSectionWait);
}
void dWMStarCoin_c::endState_ShowWait() { }

void dWMStarCoin_c::beginState_ShowSectionWait() {
	loadSectionInfo();
	layout.enableNonLoopAnim(SHOW_SECTION);

	if (canScrollLeft())
		showLeftArrow();
	if (canScrollRight())
		showRightArrow();
}
void dWMStarCoin_c::executeState_ShowSectionWait() {
	if (!layout.isAnimOn(SHOW_SECTION))
		state.setState(&StateID_Wait);
}
void dWMStarCoin_c::endState_ShowSectionWait() { }

void dWMStarCoin_c::showSecretMessage(const wchar_t *title, const wchar_t **body, int lineCount, const wchar_t **body2, int lineCount2) {
	LeftTitle->SetVisible(true);
	LeftTitle->SetString(title);
	RightTitle->SetVisible(false);

	for (int c = 0; c < COLUMN_COUNT; c++) {
		for (int i = 0; i < SHINE_COUNT; i++)
			Shine[c][i]->SetVisible(false);
		for (int r = 0; r < ROW_COUNT; r++) {
			LevelName[c][r]->SetVisible(false);
			for (int i = 0; i < 3; i++) {
				CoinOutline[c][r][i]->SetVisible(false);
				Coin[c][r][i]->SetVisible(false);
			}
		}
	}

	for (int i = 0; i < lineCount; i++) {
		LevelName[0][i]->SetVisible(true);
		LevelName[0][i]->SetString(body[i]);
	}

	if (body2) {
		for (int i = 0; i < lineCount2; i++) {
			LevelName[1][i]->SetVisible(true);
			LevelName[1][i]->SetString(body2[i]);
		}
	}
}




void dWMStarCoin_c::beginState_Wait() { }
void dWMStarCoin_c::executeState_Wait() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if ((GetActiveRemocon()->heldButtons == 0xc10) && (nowPressed & 0xc10)) { // A, B, and Plus

		const int lineCountOn = 8, lineCountOff = 2;
		static const wchar_t *hardModeJP[8] = {
			L"ハードモードがオンになりました！",
			L" ",
			L"ハードモードでは、",
			L"一回のダメージでミスになります。",
			L" ",
			L"これほどむずかしいものはそうありません！",
			L"ぜひ うでだめししてみましょう！",
			L" ", //to lazy to change the countfor the unused line --'
		};
		static const wchar_t *hardModeEN[8] = {
			L"You've activated Hard Mode!",
			L" ",
			L"In Hard Mode, Luigi goes down",
			L"in only one hit!",
			L" ",
			L"It doesn't get much harder",
			L"than this, so let's see",
			L"what you're made of!",
		};
		static const wchar_t *hardModeGE[8] = {
			L"Du hast den Expertenmodus aktiviert!",
			L" ",
			L"Im Expertenmodus verlierst du direkt",
			L"nach einem Treffer ein Leben!",
			L" ",
			L"Jetzt wird's wirklich nicht",
			L"mehr härter, also zeig uns,",
			L"was du drauf hast!",
		};
		static const wchar_t *hardModeFR[8] = {
			L"Vous avez activé le Mode Difficile!",
			L" ",
			L"Dans ce mode, Luigi",
			L"meurt en un coup !",
			L" ",
			L"C'est aussi simple que ça,",
			L"donc voyons voir de quel",
			L"bois vous vous chauffez !",
		};
		static const wchar_t *hardModeSP[8] = {
			L"¡Has activado el Modo Difícil!",
			L" ",
			L"En este modo, Luigi siempre",
			L"muere en un solo golpe.",
			L" ",
			L"No hay retos mucho más difíciles",
			L"que esto, así que ¡veamos de",
			L"lo que eres capaz!",
		};
		static const wchar_t *hardModeIT[8] = {
			L"Hai attivato la Modalità Esperto!",
			L" ",
			L"In questa modalità, Luigi",
			L"muore in un colpo solo!",
			L" ",
			L"Non c'è sfida più difficile,",
			L"quindi vediamo di che",
			L"pasta sei fatto!",
		};
		static const wchar_t *hardModeDU[8] = {
			L"Je hebt Spelmodus",
			L"Hardcore geactiveerd!",
			L" ",
			L"In Hardcore modus gaat Luigi ",
			L"in een keer dood.",
			L" ",
			L"Het wordt niet moeilijker dan",
			L"dit dus laat zien wat je waard bent!",
		};
		static const wchar_t *hardModeOffJP[2] = {
			L"ハードモードがオフになりました。",
			L" ",
		};
		static const wchar_t *hardModeOffEN[2] = {
			L"Hard Mode has been",
			L"turned off.",
		};
		static const wchar_t *hardModeOffGE[2] = {
			L"Der Expertenmodus",
			L"wurde deaktiviert.",
		};
		static const wchar_t *hardModeOffFR[2] = {
			L"Le Mode Difficile",
			L"a été désactivé.",
		};
		static const wchar_t *hardModeOffSP[2] = {
			L"El Modo Difícil ha",
			L"sido desactivado.",
		};
		static const wchar_t *hardModeOffIT[2] = {
			L"La Modalità Esperto",
			L"è stata disattivata.",
		};
		static const wchar_t *hardModeOffDU[2] = {
			L"Spelmodus Hardcore",
			L"is nu uitgeschakeld.",
		};

		if (!enableHardMode) {
			enableHardMode = true;
			wasHardModeReallyEnabled = true;
			OSReport("Hard Mode enabled!\n");
			MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_CS_COURSE_IN_HARD, 1);
			// Language-related things
			if(GetGameLanguage(4) == 0) { // Japanese
				showSecretMessage(L"ハードモードへようこそ！", hardModeJP, lineCountOn);
			}
			if(GetGameLanguage(4) == 1) { // English
				showSecretMessage(L"Hard Mode", hardModeEN, lineCountOn);
			}
			if(GetGameLanguage(4) == 2) { // German
				showSecretMessage(L"Expertenmodus", hardModeGE, lineCountOn);
			}
			if(GetGameLanguage(4) == 3) { // French
				showSecretMessage(L"Mode Difficile", hardModeFR, lineCountOn);
			}
			if(GetGameLanguage(4) == 4) { // Spanish
				showSecretMessage(L"Modo Difícil", hardModeSP, lineCountOn);
			}
			if(GetGameLanguage(4) == 5) { // Italian
				showSecretMessage(L"Modalità Esperto", hardModeIT, lineCountOn);
			}
			if(GetGameLanguage(4) == 6) { // Dutch
				showSecretMessage(L"Spelmodus Hardcore", hardModeDU, lineCountOn);
			}
		} else {
			enableHardMode = false;
			wasHardModeReallyEnabled = false;
			OSReport("Hard Mode disabled!\n");
			// Language-related things
			if(GetGameLanguage(4) == 0) { // Japanese
				showSecretMessage(L"またおいで！", hardModeOffJP, lineCountOff);
			}
			if(GetGameLanguage(4) == 1) { // English
				showSecretMessage(L"Classic Luigi", hardModeOffEN, lineCountOff);
			}
			if(GetGameLanguage(4) == 2) { // German
				showSecretMessage(L"Klassisches Spiel", hardModeOffGE, lineCountOff);
			}
			if(GetGameLanguage(4) == 3) { // French
				showSecretMessage(L"Luigi Classique", hardModeOffFR, lineCountOff);
			}
			if(GetGameLanguage(4) == 4) { // Spanish
				showSecretMessage(L"Luigi Clásico", hardModeOffSP, lineCountOff);
			}
			if(GetGameLanguage(4) == 5) { // Italian
				showSecretMessage(L"UT", hardModeOffIT, lineCountOff);
			}
			if(GetGameLanguage(4) == 6) { // Dutch
				showSecretMessage(L"Standaard Luigi", hardModeOffDU, lineCountOff);
			}
		}
		return;
	}

	if (nowPressed & travelerCodeButtons) {
		int nextKey = travelerCode[travelerCodeIndex];
		if (nowPressed & nextKey) {
			travelerCodeIndex++;
			if (travelerCode[travelerCodeIndex] == 0) {
				travelerCodeIndex = 0;
				MapSoundPlayer(SoundRelatedClass, 0x24D, 1); 
				const int travelerCountOn = 9;
				static const wchar_t *travelerEntry3[travelerCountOn] = { //Okay listen: yes this is a secret but you're NOT allowed to see it through the code ! Open MessagesSP.bin and check the message 26, this is where it starts
					L"\x49\x20\x65\x73\x63\x61\x70\x65\x64\x2e",
					L"\x20",
					L"\x54\x68\x65\x20\x74\x68\x69\x6e\x67\x20\x69\x73\x2e\x2e\x2e\x20\x77\x65\x27\x72\x65\x20\x69\x6e\x20\x73\x70\x61\x63\x65\x2e\x2e\x2e",
					L"\x49\x20\x6e\x65\x65\x64\x20\x74\x6f\x20\x66\x69\x6e\x64\x20\x61\x20\x77\x61\x79\x20\x74\x6f\x20\x67\x6f\x20\x62\x61\x63\x6b",
					L"\x6f\x6e\x20\x65\x61\x72\x74\x68\x20\x74\x6f\x20\x74\x65\x6c\x6c\x20\x50\x65\x61\x63\x68\x2e",
					L"\x20",
					L"\x49\x20\x68\x6f\x70\x65\x20\x69\x20\x77\x69\x6c\x6c\x20\x62\x65\x20\x61\x62\x6c\x65\x20\x74\x6f\x2e\x2e\x2e",
					L"\x20",
					L"\x2e\x2e\x2e\x61\x66\x74\x65\x72\x20\x61\x6c\x6c\x2c\x20\x69\x27\x6d\x20\x6a\x75\x73\x74\x20\x61\x20\x62\x6f\x62\x2d\x6f\x6d\x62\x2e",
				};
				showSecretMessage(L"\x54\x61\x6c\x65\x73\x20\x6f\x66\x20\x61\x20\x53\x65\x63\x72\x65\x74\x20\x54\x72\x61\x76\x65\x6c\x65\x72\x2c\x20\x45\x6e\x74\x72\x79\x20\x33", travelerEntry3, travelerCountOn);
			}
			return;
		} else {
			travelerCodeIndex = 0;
		}
	}

	if (nowPressed & secretCodeButtons) {
		int nextKey = secretCode[secretCodeIndex];
		if (nowPressed & nextKey) {
			secretCodeIndex++;
			if (secretCode[secretCodeIndex] == 0) {
				secretCodeIndex = 0;
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_THANK_YOU, 1);
				const int lineCountOn = 8, lineCountOff = 2;
				static const wchar_t *replayRecordJP[lineCountOn] = {
					L"じっけん的なリプレイろく画きのう",
					L"がオンになりました。",
					L"ただし、画めんと接続かのうでない",
					L"ばあいは動作しません。",
					L"オフにすることをおすすめします。",
					L" ", //too lazy to remove those, again
					L" ",
					L" ",
				};
				static const wchar_t *replayRecordEN[lineCountOn] = {
					L"The experimental Replay",
					L"Recording feature has",
					L"been enabled. Tho it doesn't",
					L"work if you don't reenable",
					L"the replay recorder main hook",
					L"in the code so you're probably",
					L"loosing your time right now.",
					L"You should maybe turn it off.",
				};
				static const wchar_t *replayRecordGE[lineCountOn-1] = {
					L"Der experimentelle",
					L"Aufnahmemodus wurde",
					L"aktiviert. Allerdings",
					L"funktioniert er nur, wenn du auch",
					L"den main hook im Code",
					L"aktiviert hast. Wir empfehlen",
					L"dir, ihn wieder auszuschalten.",
				};
				static const wchar_t *replayRecordFR[lineCountOn-1] = {
					L"La fonction expérimentale pour",
					L"enregistrer des Replay a été",
					L"activée. Par contre elle ne marche",
					L"pas si vous ne la réactivez pas",
					L"dans le code, donc vous perdez",
					L"probablement votre temps là,",
					L"donc désactivez-là.",
				};
				static const wchar_t *replayRecordSP[lineCountOn] = {
					L"El modo experimental de",
					L"grabación de Replay ha sido",
					L"activado. Sin embargo,",
					L"no funciona si no la reactivas",
					L"dentro del código del juego,",
					L"entonces probablemente estás",
					L"perdiendo tu tiempo.",
					L"Recomendamos apagarla.",
				};
				static const wchar_t *replayRecordIT[lineCountOn-3] = {
					L"La funzionalità sperimentale di",
					L"Registrazione Replay è stata",
					L"attivata, ma se non attivi l'hook",
					L"nel codice essa non funzionerà.",
					L"Ti suggeriamo di spegnerla.",
				};
				static const wchar_t *replayRecordDU[lineCountOn] = {
					L"De experimentele Replay",
					L"Recording is nu geactiveerd.",
					L"Dit staat normaal uitgeschald,",
					L"wij raden ook aan om dit",
					L"uitgeschakeld te houden.",
					L"Dit is omdat de Replay Recorder",
					L"niet goed werkt en het daardoor de",
					L"speel ervaring kan verminderen",
				};
				static const wchar_t *replayRecordOffJP[lineCountOff] = {
					L"リプレイろく画きのうがオフになりました。",
					L" ",
				};
				static const wchar_t *replayRecordOffEN[lineCountOff] = {
					L"Replay Recording",
					L"turned off.",
				};
				static const wchar_t *replayRecordOffGE[lineCountOff] = {
					L"Der Aufnahmemodus",
					L"wurde deaktiviert.",
				};
				static const wchar_t *replayRecordOffFR[lineCountOff] = {
					L"Enregistrement des",
					L"Replays désactivé.",
				};
				static const wchar_t *replayRecordOffSP[lineCountOff] = {
					L"Grabación de Replays",
					L"desactivada.",
				};
				static const wchar_t *replayRecordOffIT[lineCountOff] = {
					L"Registrazione Replay",
					L"disattivata.",
				};
				static const wchar_t *replayRecordOffDU[lineCountOff] = {
					L"Replay Recording is nu",
					L"gedeactiveerd.",
				};

				if (isReplayEnabled != 100) {
					isReplayEnabled = 100;
					OSReport("Replay Recording enabled!\n");
					// Language-related things
					if(GetGameLanguage(4) == 0) { // Japanese
						showSecretMessage(L"げんざいこうじちゅう...", replayRecordJP, lineCountOn);
					}
					if(GetGameLanguage(4) == 1) { // English
						showSecretMessage(L"Well...", replayRecordEN, lineCountOn);
					}
					if(GetGameLanguage(4) == 2) { // German
						showSecretMessage(L"Nun...", replayRecordGE, lineCountOn-1);
					}
					if(GetGameLanguage(4) == 3) { // French
						showSecretMessage(L"Euh...", replayRecordFR, lineCountOn-1);
					}
					if(GetGameLanguage(4) == 4) { // Spanish
						showSecretMessage(L"Bueno...", replayRecordSP, lineCountOn);
					}
					if(GetGameLanguage(4) == 5) { // Italian
						showSecretMessage(L"UT", replayRecordIT, lineCountOn-3);
					}
					if(GetGameLanguage(4) == 6) { // Dutch
						showSecretMessage(L"Nou...", replayRecordDU, lineCountOn);
					}
				} else {
					isReplayEnabled = 0;
					OSReport("Replay Recording disabled!\n");
					// Language-related things
					if(GetGameLanguage(4) == 0) { // Japanese
						showSecretMessage(L"オーケー", replayRecordOffJP, lineCountOff);
					}
					if(GetGameLanguage(4) == 1) { // English
						showSecretMessage(L"Duh!", replayRecordOffEN, lineCountOff);
					}
					if(GetGameLanguage(4) == 2) { // German
						showSecretMessage(L"Duh!", replayRecordOffGE, lineCountOff);
					}
					if(GetGameLanguage(4) == 3) { // French
						showSecretMessage(L"Duh!", replayRecordOffFR, lineCountOff);
					}
					if(GetGameLanguage(4) == 4) { // Spanish
						showSecretMessage(L"Duh!", replayRecordOffSP, lineCountOff);
					}
					if(GetGameLanguage(4) == 5) { // Italian
						showSecretMessage(L"Duh!", replayRecordOffIT, lineCountOff);
					}
					if(GetGameLanguage(4) == 6) { // Dutch
						showSecretMessage(L"Duh!", replayRecordOffDU, lineCountOff);
					}
				}
			}
			return;
		} else {
			secretCodeIndex = 0;
		}
	}

	if (nowPressed & WPAD_MINUS) {
		minusCount++;
		if (minusCount >= 16) {
			minusCount = 0;

			enableDebugMode = !enableDebugMode;

			if (enableDebugMode) {
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_GET_PRIZE, 1);

				const int msgCount = 9;
				const int msgCount2 = 9;
				static const wchar_t *debugModeJP[msgCount] = {
					L"ひみつのデバッグモードを",
					L"オンにしました。 このモードは",
					L"デバッグにつかったり、ステージでミス",
					L"しないようにするためにつくりました。",
					L"ーつかいかたー",
					L"B + うえ - パワーアップへんこう",
					L"B + した - コースクリア",
					L"B + ひだり - 当たりはんていひょうじ",
					L" ",
				};
				static const wchar_t *debugModeJP2[msgCount2] = {
					L"B + みぎ - スターせいせい",
					L"B + 1 - おんがくをさくじょ",
					L"ほかにもいくつかチートはありますが、",
					L"それらはデバッグ用のため",
					L"つかいどころはないとおもいます。",
					L"それにしてもきみ、",
					L"よくここをみつけたね！",
					L"",
					L"    2020/5/16 レッドストーンマット",
				};
				static const wchar_t *debugModeEN[msgCount] = {
					L"You've enabled the Totally",
					L"Secret Debug Mode.",
					L"I used it to make some debugging",
					L"and to stop dying on Mullkaw's",
					L"levels. Here's how it works,",
					L"hold these buttons to:",
					L"B + UP - Change your powerup",
					L"B + DOWN - Beat the level",
					L"B + LEFT - turn on/off collisions",
				};
				static const wchar_t *debugModeEN2[msgCount2] = {
					L"B + RIGHT - Spawn a star",
					L"B + ONE - remove musics",
					L"There's a few other things but",
					L"they're for debugging and",
					L"useless for you.",
					L"It's pretty funny to hide",
					L"things like that tbh.",
					L"",
					L"    RedStoneMatt, May 16th 2020",
				};
				static const wchar_t *debugModeGE[msgCount] = {
					L"Du hast den total geheimen",
					L"Debug-Modus aktiviert.",
					L"Ich hab ihn benutzt, um ein wenig",
					L"zu debuggen und, um nicht",
					L"in Mullkaws Levels zu verlieren.",
					L"So funktioniert's, halte folgend",
					L"Knöpfe gedrückt:",
					L"B + OBEN - Ändere dein Power-Up",
					L"B + UNTEN - Beende das Level",
				};
				static const wchar_t *debugModeGE2[msgCount2] = {
					L"B + LINKS - Schalte Kollisionen an/aus",
					L"B + RECHTS - Erzeuge einen Stern",
					L"B + EINS - Entferne die Musik",
					L"Es gibt noch ein paar andere",
					L"Dinge, doch die sind für Debugging",
					L"und daher nutzlos für dich.",
					L"Ganz schön lustig, Sachen",
					L"wie diese zu verstecken,",
					L"um ehrlich zu sein.",
				};
				static const wchar_t *debugModeFR[msgCount] = {
					L"Vous avez activé le Debug",
					L"Mode Totalement Secret.",
					L"Je l'ai utilisé pour débuguer",
					L"et pour éviter de mourir dans les",
					L"niveaux de Mullkaw.",
					L"Voilà comment ça marche:",
					L"B + HAUT - Changer de powerup",
					L"B + BAS - Finir le niveau",
					L"B + GAUCHE - montrer collisions",
				};
				static const wchar_t *debugModeFR2[msgCount2] = {
					L"B + DROITE - Créer une étoile",
					L"B + UN - Désactiver les musiques",
					L"Y'a quelques autres trucs",
					L"mais flemme et ça te servira",
					L"à que dalle mon pote.",
					L"J'aime bien cacher des",
					L"petits trucs comme ça :p",
					L"",
					L"    RedStoneMatt, 16 Mai 2020",
				};
				static const wchar_t *debugModeSP[msgCount] = {
					L"Has activado el Modo Debug",
					L"Súper Secreto. Lo usé para",
					L"debuguear algunas cosas",
					L"y para dejar de morir en los niveles",
					L"de Mullkaw. Funciona así, mantén",
					L"apretados estos botones para:",
					L"B + ARRIBA - Cambia de poder",
					L"B + ABAJO - Pasar el nivel",
					L"B + IZQ. - Act./desct. colisiones",
				};
				static const wchar_t *debugModeSP2[msgCount2] = {
					L"B + DERECHA - Crear una estrella",
					L"B + UNO - Quitar la música",
					L"Hay unas cuantas otras",
					L"cosas pero son para debuguear",
					L"y no te servirán a ti.",
					L"La verdad es gracioso",
					L"esconder cosas así.",
					L"",
					L"RedStoneMatt, 16 de mayo de 2020",
				};
				static const wchar_t *debugModeIT[msgCount] = {
					L"Hai attivato la Modalità di Debug",
					L"Super Segreta. L'ho usata per",
					L"debuggare un paio di cose e per",
					L"non morire continuamente nei livelli",
					L"di Mullkaw. Funziona così, premi",
					L"questi pulsanti per:",
					L"B+UP - Cambiare powerup",
					L"B+DOWN - Completare il livello",
					L"B+LEFT - Spegnere/accendere le",
				};
				static const wchar_t *debugModeIT2[msgCount2] = {
					L"collisioni",
					L"B+RIGHT - Spawna una stella",
					L"B+1 - Rimuovi la musica",
					L"Ci sono altre combo ma sono per",
					L"debugging e quindi inutili per te.",
					L"È abbastanza divertente",
					L"nascondere cose del genere tbh.",
					L"",
					L"   RedStoneMatt, 16 maggio 2020",
				};
				static const wchar_t *debugModeDU[msgCount] = {
					L"Je hebt het geheime",
					L"Debug Menu geactiveerd!",
					L"Ik heb dit gebruikt om bugs op",
					L"te lossen en om te stoppen met",
					L"doodgaan in Mullkaw's levels.",
					L"Zo werkt het:",
					L"B + OMHOOG - Verander powerup",
					L"B + OMLAAG - level gehaald",
					L"B + LINKS - act./deact. collisions",
				};
				static const wchar_t *debugModeDU2[msgCount2] = {
					L"B + RIGHT - creëer een ster",
					L"B + EEN - Stop de Muziek",
					L"Er zijn nog een paar dingen",
					L"maar deze hebben geen nut ",
					L"tenzij je aan het debuggen bent.",
					L"Het is best grappig om dingen",
					L"zoals dit te verstoppen xD.",
					L"",
					L"    RedStoneMatt, 16 maart 2020",
				};
				// Language-related things
				if(GetGameLanguage(4) == 0) { // Japanese
					showSecretMessage(L"おめでとう ! ", debugModeJP, msgCount, debugModeJP2, msgCount2);
				}
				if(GetGameLanguage(4) == 1) { // English
					showSecretMessage(L"Yay!", debugModeEN, msgCount, debugModeEN2, msgCount2);
				}
				if(GetGameLanguage(4) == 2) { // German
					showSecretMessage(L"Yay!", debugModeGE, msgCount, debugModeGE2, msgCount2);
				}
				if(GetGameLanguage(4) == 3) { // French
					showSecretMessage(L"Yay!", debugModeFR, msgCount, debugModeFR2, msgCount2);
				}
				if(GetGameLanguage(4) == 4) { // Spanish
					showSecretMessage(L"Yay!", debugModeSP, msgCount, debugModeSP2, msgCount2);
				}
				if(GetGameLanguage(4) == 5) { // Italian
					showSecretMessage(L"Yay!", debugModeIT, msgCount, debugModeIT2, msgCount2);
				}
				if(GetGameLanguage(4) == 6) { // Dutch
					showSecretMessage(L"Yay!", debugModeDU, msgCount, debugModeDU2, msgCount2);
				}
			} else {
				const int msgCount = 5;
				static const wchar_t *debugModeOffJP[msgCount] = {
					L"ひみつのデバッグモードをオフにしました。",
					L"",
					L"ざんねんですが、もう",
					L"チートはつかえません ! ",
					L"またこんど ! ",
				};
				static const wchar_t *debugModeOffEN[msgCount] = {
					L"You've turned off the Totally",
					L"Secret Debug Mode.",
					L"",
					L"Sad, you won't be able to cheat",
					L"anymore ! See you next time !",
				};
				static const wchar_t *debugModeOffGE[msgCount] = {
					L"Du hast den total geheimen",
					L"Secret Debug Mode.",
					L"",
					L"Zu schade, jetzt wirst du nicht mehr",
					L"schummeln können. Auf Wiedersehen!",
				};
				static const wchar_t *debugModeOffFR[msgCount] = {
					L"Vous avez désactivé le Debug",
					L"Mode Totalement Secret.",
					L"",
					L"Dommage, tu ne vas plus pouvoir",
					L"tricher ! À la prochaine !",
				};
				static const wchar_t *debugModeOffSP[msgCount] = {
					L"Has desactivado el Modo",
					L"Debug Súper Secreto.",
					L"",
					L"¡Qué pena, ya no podrás hacer",
					L"trampa! ¡Hasta la próxima!",
				};
				static const wchar_t *debugModeOffIT[msgCount] = {
					L"Hai spento la Modalità di",
					L"Debug Super Segreta.",
					L"",
					L"Peccato, non potrai più barare!",
					L"Alla prossima!",
				};
				static const wchar_t *debugModeOffDU[msgCount] = {
					L"Het geheime Debug Menu",
					L"is nu uitgeschakeld",
					L"",
					L"Helaas, je kunt nu niet meer ",
					L"vals spelen! Tot de volgende keer!",
				};
				static const wchar_t *hiddenMsg[] = {
					L"Treeki used to hide a message",
					L"right here, and i totally understand",
					L"him because it's so satisfying to hide",
					L"things for secret hunters.",
					L"Honestly i'd be kinda surprised if someone",
					L"read this since there's not so much secret hunters",
					L"in our 'modern days'",
					L"I wonder what will be the next thing i'll add here :>",
					L"   -RedStoneMatt, no idea when because i just realise i forgot to put when i wrote this message but it probably was in May 2020.",
				};
				// Language-related things
				if(GetGameLanguage(4) == 0) { // Japanese
					showSecretMessage(L"\x0B\x014A\xBEEF", debugModeOffJP, msgCount, hiddenMsg, 0);
				}
				if(GetGameLanguage(4) == 1) { // English
					showSecretMessage(L"OwO", debugModeOffEN, msgCount, hiddenMsg, 0);
				}
				if(GetGameLanguage(4) == 2) { // German
					showSecretMessage(L"UwU", debugModeOffGE, msgCount, hiddenMsg, 0);
				}
				if(GetGameLanguage(4) == 3) { // French
					showSecretMessage(L"ÈwÉ", debugModeOffFR, msgCount, hiddenMsg, 0);
				}
				if(GetGameLanguage(4) == 4) { // Spanish
					showSecretMessage(L"ÉwÈ", debugModeOffSP, msgCount, hiddenMsg, 0);
				}
				if(GetGameLanguage(4) == 5) { // Italian
					showSecretMessage(L"^w^", debugModeOffIT, msgCount, hiddenMsg, 0);
				}
				if(GetGameLanguage(4) == 6) { // Dutch
					showSecretMessage(L"\\^o^/", debugModeOffDU, msgCount, hiddenMsg, 0);
				}
			}
		}
	} else if (nowPressed & WPAD_ONE) {
		MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT_AUTO, 1);
		willExit = true;
		state.setState(&StateID_HideSectionWait);
	} else if ((nowPressed & WPAD_LEFT) && canScrollLeft()) {
		currentSection = sectionIndices[--currentSectionIndex];
		willExit = false;
		state.setState(&StateID_HideSectionWait);
	} else if ((nowPressed & WPAD_RIGHT) && canScrollRight()) {
		currentSection = sectionIndices[++currentSectionIndex];
		willExit = false;
		state.setState(&StateID_HideSectionWait);
	}
}
void dWMStarCoin_c::endState_Wait() { }

void dWMStarCoin_c::beginState_HideSectionWait() {
	layout.enableNonLoopAnim(HIDE_SECTION);
	if (willExit) {
		hideLeftArrow();
		hideRightArrow();
	} else {
		setLeftArrowVisible(canScrollLeft());
		setRightArrowVisible(canScrollRight());
	}
}
void dWMStarCoin_c::executeState_HideSectionWait() {
	if (!layout.isAnimOn(HIDE_SECTION)) {
		if (willExit)
			state.setState(&StateID_HideWait);
		else
			state.setState(&StateID_ShowSectionWait);
	}
}
void dWMStarCoin_c::endState_HideSectionWait() { }

void dWMStarCoin_c::beginState_HideWait() {
	layout.enableNonLoopAnim(SHOW_ALL, true);
	layout.grpHandlers[SHOW_ALL].frameCtrl.flags = 3; // NO_LOOP | REVERSE
}
void dWMStarCoin_c::executeState_HideWait() {
	if (!layout.isAnimOn(SHOW_ALL))
		state.setState(&StateID_Hidden);
}
void dWMStarCoin_c::endState_HideWait() {
	visible = false;
}


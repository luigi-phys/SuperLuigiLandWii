#include <game.h>
#include <stage.h>

extern u8 LastLevelPlayed[2];
extern "C" int CanExitThisLevel();
extern int previewID;

void restartLevel() {

	LastPowerupStoreType = LOSE_LEVEL;


	if(GetActiveRemocon()->heldButtons & WPAD_TWO || GetActiveRemocon()->heldButtons & WPAD_A) {
		ExitStage(WORLD_MAP, 0, LOSE_LEVEL, WIPE_BOWSER);
		return;
	}

	if(((!Player_Lives[0]) || 
		((Player_Active[1]) && !Player_Lives[1]) ||
		((Player_Active[2]) && !Player_Lives[2]) ||
		((Player_Active[3]) && !Player_Lives[3]))) {
		ExitStage(WORLD_MAP, 0, LOSE_LEVEL, WIPE_BOWSER);
		return;
	}

	StartLevelInfo sl;
	sl.replayTypeMaybe = 0;
	sl.entrance = 0xFF;
	sl.areaMaybe = 0;
	sl.unk4 = 0;
	sl.purpose = 0;

	sl.world1 = LastLevelPlayed[0];
	sl.world2 = LastLevelPlayed[0];
	sl.level1 = LastLevelPlayed[1];
	sl.level2 = LastLevelPlayed[1];

	ActivateWipe(WIPE_BOWSER);

	previewID = (GameMgrP->eight.checkpointEntranceID != 255 && GameMgrP->eight.checkpointWorldID == LastLevelPlayed[0] && GameMgrP->eight.checkpointLevelID == LastLevelPlayed[1]);

	DoStartLevel(GetGameMgr(), &sl);
}

void exitLevel() {
	if(CanExitThisLevel()) {
		ExitStage(WORLD_MAP, 0, EXIT_LEVEL, 5);
	}
	else {
		int isGameOver = (Player_Lives[0] - 1) + (((Player_Active[1] - 1) == 1) ? (Player_Lives[1] - 1) : 0) + (((Player_Active[2] - 1) == 1) ? (Player_Lives[2] - 1) : 0) + (((Player_Active[3] - 1) == 1) ? (Player_Lives[3] - 1) : 0); //Get if there's a Game Over pending
		if(isGameOver > 0) {
			Player_Lives[0]--;
			if(Player_Active[1]) {
				Player_Lives[1]--;
			}
			if(Player_Active[2]) {
				Player_Lives[2]--;
			}
			if(Player_Active[3]) {
				Player_Lives[3]--;
			}
		}
		ExitStage(WORLD_MAP, 0, LOSE_LEVEL, WIPE_BOWSER);
	}
}
#include <game.h>
#include <stage.h>

extern u8 LastLevelPlayed[2];		//Current Level's world and level ID
extern "C" int CanExitThisLevel();	//Check if you already completed the level you're in
extern int previewID;				//Which preview to use in the next preGame screen

void restartLevel() {				//Executed after the player's death animation

	LastPowerupStoreType = LOSE_LEVEL;							//Make the game remember how much of a looser you are for dying in the level


	int nowPressed = Remocon_GetPressed(GetActiveRemocon());	//Get the current pressed buttons
	if(GetActiveRemocon()->heldButtons == 0x0100) {				//If the (2) button is held
		ExitStage(WORLD_MAP, 0, LOSE_LEVEL, WIPE_BOWSER);		//Go back to the worldmap
		return;													//stop here, don't do what's after that piece of code
	}

	StartLevelInfo sl;					//Create a level starting info
	sl.replayTypeMaybe = 0;				//No replay
	sl.entrance = 0xFF;					//Setting it to 0xFF makes it use the entrance of the area if the checkpoint wasn't checked, and if it was then use the checkpoint entrance
	sl.areaMaybe = 0;					//Use Area 0, as every level duh
	sl.unk4 = 0;						//No idea of what is this
	sl.purpose = 0;						//Treeki could you give better names pleaaaase :c

	sl.world1 = LastLevelPlayed[0];		//Getting the last level played (so the current one) world id, then store it in the level starting info
	sl.world2 = LastLevelPlayed[0];		//^
	sl.level1 = LastLevelPlayed[1];		//Getting the last level played (so the current one) level id, then store it in the level starting info
	sl.level2 = LastLevelPlayed[1];		//^

	ActivateWipe(WIPE_BOWSER);			//Activate Bowser Wipe

	if(GameMgrP->eight.checkpointEntranceID != 255 && GameMgrP->eight.checkpointWorldID == LastLevelPlayed[0] && GameMgrP->eight.checkpointLevelID == LastLevelPlayed[1]) { //If the checkpoint was took
		previewID = 1;					//Use the Checkpoint Preview Picture
	}
	else {								//Otherwise
		previewID = 0;					//Do not use the Checkpoint Preview Picture
	}

	DoStartLevel(GetGameMgr(), &sl);	//Restart the level using the level starting info we previously created
}

void exitLevel() {								//Executed after pressing "Exit" in the level pause menu
	int canI = CanExitThisLevel();				//Check if you can exit the level because you already completed it
	if(canI) {									//If yes
		ExitStage(WORLD_MAP, 0, EXIT_LEVEL, 5);	//Then exit as you do in the original game
	}
	else {										//Otherwise
		int isGameOver = (Player_Lives[0] - 1) + (((Player_Active[1] - 1) == 1) ? (Player_Lives[1] - 1) : 0) + (((Player_Active[2] - 1) == 1) ? (Player_Lives[2] - 1) : 0) + (((Player_Active[3] - 1) == 1) ? (Player_Lives[3] - 1) : 0); //Get if there's a Game Over pending
		if(isGameOver > 0) {					//if there's no Game Over pending (isGameOver equals to 0 if the total lives of all the active players is 0)
			Player_Lives[0]--;					//Remove a life to Player 1 (The Player 1 always being ative, no need to check if he's active or not)
			if(Player_Active[1]) {				//If the Player 2 is active
				Player_Lives[1]--;				//Remove a life to Player 2
			}
			if(Player_Active[2]) {				//If the Player 3 is active
				Player_Lives[2]--;				//Remove a life to Player 3
			}
			if(Player_Active[3]) {				//If the Player 4 is active
				Player_Lives[3]--;				//Remove a life to Player 4
			}
		}
		ExitStage(WORLD_MAP, 0, LOSE_LEVEL, WIPE_BOWSER); //Exit to the worldmap as if you loose the level because you haven't already completed it
	}
}
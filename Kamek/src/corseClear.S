.text
#include <asm_setup.S>
.extern GetGameLanguage

.extern corseClearBrlyt_English
.extern corseClearBrlyt_German
.extern corseClearBrlyt_French
.extern corseClearBrlyt_Spanish
.extern corseClearBrlyt_Italian
.extern ContinueFromCorseClearBrlyt

.extern corseClearBrlan_English
.extern corseClearBrlan_German
.extern corseClearBrlan_French
.extern corseClearBrlan_Spanish
.extern corseClearBrlan_Italian
.extern ContinueFromCorseClearBrlan

.extern strncat
.extern loadAnimations__Q23m2d13EmbedLayout_cFPPCci

.global DutchCorseClearBrlyt
DutchCorseClearBrlyt:
	bl GetGameLanguage
	clrlwi r0, r3, 24
	cmpwi r0, 3
	beq _corseClearBrlyt_French
	cmpwi r0, 2
	beq _corseClearBrlyt_German
	cmpwi r0, 5
	beq _corseClearBrlyt_Italian
	cmpwi r0, 4
	beq _corseClearBrlyt_Spanish
	cmpwi r0, 6
	beq corseClearBrlyt_Dutch
	b corseClearBrlyt_English
_corseClearBrlyt_French:
	b corseClearBrlyt_French
_corseClearBrlyt_German:
	b corseClearBrlyt_German
_corseClearBrlyt_Italian:
	b corseClearBrlyt_Italian
_corseClearBrlyt_Spanish:
	b corseClearBrlyt_Spanish
corseClearBrlyt_Dutch:
	addi r3, r1, 8
	lis r4, aNedeu_00_0@h
	ori r4, r4, aNedeu_00_0@l
	li r5, 0x63
	bl strncat
	b ContinueFromCorseClearBrlyt

.global DutchCorseClearBrlan
DutchCorseClearBrlan:
	bl GetGameLanguage
	clrlwi r0, r3, 24
	cmpwi r0, 3
	beq _corseClearBrlan_French
	cmpwi r0, 2
	beq _corseClearBrlan_German
	cmpwi r0, 5
	beq _corseClearBrlan_Italian
	cmpwi r0, 4
	beq _corseClearBrlan_Spanish
	cmpwi r0, 6
	beq corseClearBrlan_Dutch
	b corseClearBrlan_English
_corseClearBrlan_French:
	b corseClearBrlan_French
_corseClearBrlan_German:
	b corseClearBrlan_German
_corseClearBrlan_Italian:
	b corseClearBrlan_Italian
_corseClearBrlan_Spanish:
	b corseClearBrlan_Spanish
corseClearBrlan_Dutch:
	addi r3, r30, 0x70
	lis r4, corseClearAnim_Dutch@h
	ori r4, r4, corseClearAnim_Dutch@l
	li r5, 3
	bl loadAnimations__Q23m2d13EmbedLayout_cFPPCci
	b ContinueFromCorseClearBrlan

.data 
aNedeu_00_0:
	.string "NedEU_00"

aCorseclear_ned:
	.string "corseClear_NedEU_00_inText.brlan"
aCorseclear_n_0:
	.string "corseClear_NedEU_00_loopText.brlan"
aCorseclear_n_1:
	.string "corseClear_NedEU_00_outText.brlan"

corseClearAnim_Dutch:
	.long aCorseclear_ned
	.long aCorseclear_n_0
	.long aCorseclear_n_1
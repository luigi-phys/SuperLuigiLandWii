.text
.align 4
#include <asm_setup.S>

.extern ContinueFromPMLanguage
.extern PM_OtherLanguage
.extern GetGameLanguage

.global PM_ChangeForLanguage
PM_ChangeForLanguage:
	mr		r20, r6					#Save r6 into r20 before it get erased by GetGameLanguage
	mr		r21, r7					#Save r7 into r21 before it get erased by GetGameLanguage
	li		r3, 4					#Set r3 to 4 (because yes)
	bl		GetGameLanguage			#Get the game language
	cmpwi	r3, 0					#If it's 0 (=Japanese)
	beq		PM_Japanese				#Then go to PM_Japanese
	cmpwi	r3, 2					#If it's 2 (=German)
	beq		PM_Japanese				#Then go to PM_Japanese
	cmpwi	r3, 4					#If it's 4 (=Spanish)
	beq		PM_Japanese				#Then go to PM_Japanese
	cmpwi	r3, 5					#If it's 5 (=Italian)
	beq		PM_Japanese				#Then go to PM_Japanese
	cmpwi	r3, 6					#If it's 6 (=Dutch)
	beq		PM_Japanese				#Then go to PM_Japanese
	addi	r3, r31, 0x70			#Else, restore r3
	mr		r6, r20					#Restore r6 from r20
	mr		r7, r21					#Restore r7 from r21
	li		r20, 0					#Reset r20
	li		r21, 0					#Reset r21
	lwz		r12, 0x78(r31)			#Restore r12
	b		PM_OtherLanguage		#Go back to the original function, where it sets r4 to "pauseMenu_16.brlyt"
PM_Japanese:
	addi	r3, r31, 0x70			#Restore r3
	mr		r6, r20					#Restore r6 from r20
	mr		r7, r21					#Restore r7 from r21
	li		r20, 0					#Reset r20
	li		r21, 0					#Reset r21
	lwz		r12, 0x78(r31)			#Restore r12
	lis		r4, PM_JPString@h		#Set r4 to "pauseMenu_16-JP.brlyt"
	ori		r4, r4, PM_JPString@l	#Set r4 to "pauseMenu_16-JP.brlyt"
	b		ContinueFromPMLanguage	#Go back to the original function, after the line where r4 was set

# That was originally intended for Japanese only as it has very long names,but it ended up being on every language except French and English


.global PM_JPString
PM_JPString:
	.string "pauseMenu_16-JP.brlyt"
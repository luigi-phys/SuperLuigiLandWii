#Done through dolpatches
#Please check LuigiLand.xml

.global DutchPalCheck #b'd at 0x800B46D4, wrote at 0x80349098
DutchPalCheck:
	cmpwi r0, 6				#2C000006 ->0x80349098
	beq doDutch				#4182000C ->0x8034909C
	mr r3, 31				#7FE3FB78 ->0x803490A0
	b loc_800B46D8			#4BD6B634 ->0x803490A4
doDutch:
	mr r3, 31				#7FE3FB78 ->0x803490A8
	lis r4, aNedeu@h		#3C808034 ->0x803490AC
	ori r4, r4, aNedeu@l	#608490C0 ->0x803490B0
	li r5, 0x63				#38A00063 ->0x803490B4
	bl strncat_802E1D58		#4bf98CA1 ->0x803490B8
	b loc_800B4734			#4BD6B678 ->0x803490BC


.global aNedeu				#4E656445 ->0x803490C0
aNedeu:						#552F0000 ->0x803490C4
	.string "NedEU/"
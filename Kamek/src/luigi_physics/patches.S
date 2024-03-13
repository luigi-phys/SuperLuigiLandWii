.text

#include <asm_setup.S>

.align 4
.extern luigi__x_speed_inc

.align 4
.extern luigi__swim_y_speed_add

.global OverrideLuigiXSpeedInc
OverrideLuigiXSpeedInc:
    lis       r3, luigi__x_speed_inc@ha
    lfs       f0, luigi__x_speed_inc@l(r3)
    blr


.global OverrideLuigiSwimYSpeedAdd
OverrideLuigiSwimYSpeedAdd:
    lis       r3, luigi__swim_y_speed_add@ha
    lfs       f1, luigi__swim_y_speed_add@l(r3)
    blr

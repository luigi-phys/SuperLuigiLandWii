#include <game.h>

extern "C" {

float luigi__x_speed_inc = 0.07f;
float luigi__swim_y_speed_add = 1.8f;

extern const float FLOAT_ARRAY_802eedc0[2];

void daPlBase_c__slipPowerSet_1(daPlBase_c *_this)
{
    if (_this->isSaka())
    {
        _this->x_speed_inc = _this->getSakaMoveAccele((_this->mSpeedF < 0.0) ? 1 : 0);
        int walk_dir;
        if (_this->input.getLRMovementDirection(&walk_dir))
            _this->x_speed_inc *= FLOAT_ARRAY_802eedc0[_this->collMgr.getSakaUpDown(walk_dir)];
    }
    else
    {
        if (_this->testFlag(0x31))
            _this->x_speed_inc = 0.05;
        else
            _this->x_speed_inc = 0.09;
    }

    _this->icePowerChange(1);
}

void daPlBase_c__slipPowerSet_0(daPlBase_c *_this)
{
    if (abs(_this->mSpeedF) > _this->getSpeedData()->_8)
    {
        _this->x_speed_inc = 0.75;
        return;
    }

    {
        // HACK: Patching addresses 0x802ef118 & 0x802ef208 using Kamek is not enough
        // as Kamek is not able to perform patches before the ctors of the original game run
        _this->mpSpeedDataNormal->_c._c = 0.04f;
    }

    PlayerSpeedDataInner local_4c;
    _this->getSpeedDataInner(&local_4c);

    int walk_dir;
    if (_this->input.getLRMovementDirection(&walk_dir))
    {
        if (_this->mSpeedF * daPlBase_c::sc_DirSpeed[_this->direction] < 0.0)
        {
            _this->x_speed_inc = local_4c._c;
            if (_this->testFlag(0x89))
                _this->x_speed_inc *= 3;

            return;
        }

        if (_this->isSaka())
        {
            _this->x_speed_inc = _this->getSakaMoveAccele((_this->mSpeedF < 0.0) ? 1 : 0) * FLOAT_ARRAY_802eedc0[_this->collMgr.getSakaUpDown(walk_dir)];
            _this->icePowerChange(0);
            return;
        }

        const float x_speed_1_mag = abs(_this->mSpeedF);
        const float x_speed_2_mag = abs(_this->mMaxSpeedF);

        if (x_speed_1_mag < 0.5)
        {
            _this->x_speed_inc = local_4c._10;
        }
        else if (x_speed_1_mag < _this->getSpeedData()->_0)
        {
            if (_this->input.getHeldOne())
                _this->x_speed_inc = local_4c._18;
            else
                _this->x_speed_inc = local_4c._14;
        }
        else if (x_speed_1_mag < _this->getSpeedData()->_4)
        {
            if (x_speed_2_mag >= _this->getSpeedData()->_4)
                _this->x_speed_inc = local_4c._1c;
            else
                _this->x_speed_inc = local_4c._0;
        }
        else
        {
            if (x_speed_2_mag >= _this->getSpeedData()->_4)
                _this->x_speed_inc = local_4c._20;
            else
                _this->x_speed_inc = local_4c._0;
        }
    }
    else
    {
        if (_this->isSaka())
            _this->x_speed_inc = _this->getSakaStopAccele((_this->mSpeedF < 0.0) ? 1 : 0);
        else if (_this->mSpeedF * daPlBase_c::sc_DirSpeed[_this->direction] < 0.0)
            _this->x_speed_inc = local_4c._8;
        else if (abs(_this->mSpeedF) < _this->getSpeedData()->_0)
            _this->x_speed_inc = local_4c._4;
        else
            _this->x_speed_inc = local_4c._0;

        if (_this->testFlag(0x89))
            _this->x_speed_inc *= 3;

        _this->icePowerChange(0);
    }
}

}

// ------------------------------------------------------------------------------------------------
static int s_jumpAnmID[4] = {-1, -1, -1, -1};
static float s_jumpAnmRate[4] = {0.0f, 0.0f, 0.0f, 0.0f};

class sLib
{
public:
    static bool chase(float *value, float target, float step);
};

extern "C" {

void dAcPy_c___jumpSet_patch(dAcPy_c *_this)
{
    _this->mAction = 0;
    s_jumpAnmID[_this->which_player] = -1;
    s_jumpAnmRate[_this->which_player] = 0.5;
    _this->rot.x = 0;
    _this->rot.y = _this->getMukiAngle(_this->direction);
    _this->setWaterWalkFlag();
}

void dAcPy_c__jumpAirExec_patch(dAcPy_c *_this, int status_bit)
{
    _this->clearFlag(status_bit);
    _this->modelHandler.setAnm(6, 10.0, 0.0);
    s_jumpAnmID[_this->which_player] = 6; // New addition
}

extern int DWORD_ARRAY_802f5108[3];

int dAcPy_c__getJump2AnimID(dAcPy_c *_this)
{
    if (_this->testFlag(0xd))
        return 68;

    if (_this->testFlag(0xf))
        return 119;

    if (_this->jumpSoundRelated == 2)
        return -1;

    return DWORD_ARRAY_802f5108[_this->jumpSoundRelated];
}

int dAcPy_c__getJump3AnimID(dAcPy_c *_this)
{
    if (_this->testFlag(0xd))
        return 84;

    if (_this->testFlag(0xf))
        return 119;

    if (_this->jumpSoundRelated == 2)
        return -1;

    return 158;
}

int dAcPy_c__getPhysicsAnimType(dAcPy_c *_this)
{
    if ((_this->_10d4 & 1) == 0
      //&& !_this->forceNeutralJumpFall() // TODO: This occurs in states such as triple jump, figure out equivalent in NSMBW (May be unnecessary)
        && _this->isStar() == 0)
    {
        if (_this->input.getHeldTwoModifiedByFlags())
            return 0;
        else
            return 1;
    }
    return 2;
}

void dAcPy_c__jumpStartWithPhysics(dAcPy_c *_this)
{
    if (_this->testFlag(0xc))
        _this->mAction = 1;

    else if (_this->testFlag(0x10))
    {
        if (_this->modelHandler.mdlClass->isAnmStop())
        {
            _this->modelHandler.setAnm(6, 3.0, 0.0);
            s_jumpAnmID[_this->which_player] = 6;
            _this->mAction = 1;
        }
    }
    else
    {
        if (_this->speed.y < 0.0)
        {
            const int anmID = dAcPy_c__getJump2AnimID(_this);
            if (anmID != -1)
            {
                if (anmID == 68)
                    _this->modelHandler.setAnm(68, 10.0, 0.0);
                else
                    _this->modelHandler.setAnm(anmID, 0.0);

                s_jumpAnmID[_this->which_player] = anmID;
                _this->mAction = 1;
            }
        }
        if (dAcPy_c__getPhysicsAnimType(_this) == 0 &&
            (_this->modelHandler.mdlClass->isAnmStop() || _this->speed.y < 2.5))
        {
            const int anmID = dAcPy_c__getJump3AnimID(_this);
            if (anmID != -1)
            {
                switch (anmID)
                {
                case 84:
                    _this->modelHandler.setAnm(84, 3.0, 0.0); // Not 100% accurate, but perhaps good enough
                    break;
                default:
                    _this->modelHandler.setAnm(anmID, 0.0); // Not 100% accurate, but perhaps good enough
                    break;
                case 158:
                    _this->modelHandler.setAnm(158, 10.0, 0.0);
                }
                s_jumpAnmID[_this->which_player] = dAcPy_c__getJump2AnimID(_this);
                _this->mAction = 1;
            }
        }
    }
    _this->jumpExecAir();
}

void dAcPy_c__jumpExecAirWithPhysics(dAcPy_c *_this)
{
    if (_this->jumpSoundRelated == 2)
    {
        u8 dir = _this->direction;
        if (dir != _this->_27cc || (_this->_10d4 >> 1 & 1) != 0)
        {
            _this->_27cc = dir;
            _this->modelHandler.setAnm(
                11,
                0.0,
                _this->modelHandler.mdlClass->getAnmFrameMax() - 1.0);
            s_jumpAnmID[_this->which_player] = 11;
            //_this->setForceNeutralJumpFall(false); // TODO: Figure out equivalent in NSMBW (May be unnecessary)
        }
        if (_this->modelHandler.mdlClass->isAnmStop())
        {
            //_this->setForceNeutralJumpFall(false); // TODO: Figure out equivalent in NSMBW (May be unnecessary)
        }
    }
    switch (dAcPy_c__getPhysicsAnimType(_this))
    {
    case 0:
    {
        const int anmID = dAcPy_c__getJump3AnimID(_this);
        if (anmID != -1)
        {
            const float rate = dPlayerModelHandler_c::getAnmRate(anmID);
            float blendDuration;
            switch (anmID)
            {
            case 84:
                blendDuration = 3.0; // Not 100% accurate, but perhaps good enough
                break;
            default:
                blendDuration = dPlayerModelHandler_c::getAnmBlendDuration(anmID);
                break;
            case 158:
                blendDuration = 10.0;
            }
            sLib::chase(&(s_jumpAnmRate[_this->which_player]), 1.5, 0.02);
            _this->modelHandler.setAnm(
                anmID,
                s_jumpAnmRate[_this->which_player] * rate,
                blendDuration,
                0.0);
        }

        if (_this->modelHandler.mdlClass->checkAnmFrame(8.0) ||
            _this->modelHandler.mdlClass->checkAnmFrame(19.0))
        {
            _this->startSound(284, 0); // 284 -> SE_PLY_PRPL_FLY
        }
    }
        break;
    case 1:
        if (s_jumpAnmID[_this->which_player] != -1)
        {
            _this->modelHandler.setAnm(s_jumpAnmID[_this->which_player], 5.0, 0.0);
        } // Fall-through
    default:
        s_jumpAnmRate[_this->which_player] = 0.5;
        break;
    }

    _this->jumpExecAir();
}

}

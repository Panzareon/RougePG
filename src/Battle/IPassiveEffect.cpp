#include "Battle/IPassiveEffect.h"

IPassiveEffect::IPassiveEffect()
{
    //ctor
}

IPassiveEffect::~IPassiveEffect()
{
    //dtor
}

bool IPassiveEffect::IsEquipment()
{
    return false;
}

void IPassiveEffect::OnEffectStart()
{
    //NOOP
}

void IPassiveEffect::OnBattleFinished(Entity* target)
{
    //NOOP
}

bool IPassiveEffect::ShowEnemyHealth(bool base)
{
    return base;
}

float IPassiveEffect::Movementspeed(float base)
{
    return base;
}

bool IPassiveEffect::DeleteEffect()
{
    return true;
}

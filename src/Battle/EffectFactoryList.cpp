#include "Battle/EffectFactoryList.h"
#include "Exception/InvalidArgumentException.h"
#include "Battle/Entity.h"
#include "Controller/Enums.h"
#include "Battle/PassiveEffect.h"
#include "Battle/EffectFactory.h"
#include "Battle/EffectFactoryPassive.h"
#include <random>
#include "Controller/GameController.h"
#include "SceneManager/SceneManagerStatus.h"

namespace PassiveEffectFunctions
{
    float BuffAttribute(float baseValue,BattleEnums::Attribute attr, float strength, BattleEnums::Attribute toBuff)
    {
        if(toBuff != attr)
            return baseValue;
        return baseValue * (1 + strength);
    }
    float BuffResistance(float baseValue, BattleEnums::AttackType type, float strength, BattleEnums::AttackType toBuff)
    {
        if(toBuff != type)
            return baseValue;
        return baseValue + strength;
    }
    float DebuffAttribute(float baseValue,BattleEnums::Attribute attr, float strength, BattleEnums::Attribute toBuff)
    {
        if(toBuff != attr)
            return baseValue;
        return baseValue * (1 - strength);
    }
    float DebuffResistance(float baseValue, BattleEnums::AttackType type, float strength, BattleEnums::AttackType toBuff)
    {
        if(toBuff != type)
            return baseValue;
        return baseValue - strength;
    }
    void Heal(Entity* target, IPassiveEffect* passiveEffect, int hp)
    {
        target->Heal(hp);
    }
    void DamageOverTime(Entity* user, Entity* target, IPassiveEffect* passiveEffect, int dmg, BattleEnums::AttackType type)
    {
        Attack att(dmg, false, target, type);
        user->AttackEntity(target, &att);
    }
    void Taunt(Attack* att, Entity* user, Entity* taunt)
    {
        att->m_target = taunt;
    }
    float SubtractMultiplier(float baseValue, float multiplier)
    {
        return baseValue * (1 - multiplier);
    }
    void AddAttackType(Attack* att, Entity* attacker, BattleEnums::AttackType type, bool physical)
    {
        if(physical == att->m_physical)
        {
            att->m_type.insert(type);
        }
    }
    void MultiplyDmg(Attack* att, Entity* attacker, float multiply, bool physical)
    {
        if(physical == att->m_physical)
        {
            att->m_dmg *= multiply;
        }
    }
    int PreventDamage(Attack* att, Entity* target, Entity* attacker, int baseAmount, std::vector<float>* preventAmount, PassiveEffect* effect)
    {
        if(preventAmount->at(0) > baseAmount)
        {
            preventAmount->at(0) -= baseAmount;
            baseAmount = 0;
        }
        else
        {
            //No longer prevents damage
            baseAmount -= (int)preventAmount->at(0);
            preventAmount->at(0) = 0.0f;
            effect->SetDuration(0);
        }

        return baseAmount;
    }
    int ReduceDamage(Attack* att, Entity* target, Entity* attacker, int baseAmount, int reduceTo)
    {
        if(baseAmount > reduceTo)
        {
            return reduceTo;
        }
        else
        {
            return baseAmount;
        }
    }
    int IgnoreDamage(Attack* att, Entity* target, Entity* attacker, int baseAmount, std::vector<float>* preventTimes, PassiveEffect* effect, BattleEnums::AttackType attackType)
    {
        if (std::find(att->m_type.begin(), att->m_type.end(), attackType) == att->m_type.end())
        {
            return baseAmount;
        }

        if (preventTimes->at(0) == 1)
        {
            //No longer prevents damage
            baseAmount = 0;
            preventTimes->at(0) = 0;
            effect->SetDuration(0);
        }
        else if (preventTimes->at(0) > 1)
        {
            baseAmount = 0;
            preventTimes->at(0)--;
        }

        return baseAmount;
    }
    int ReduceDamageBy(Attack* att, Entity* target, Entity* attacker, int baseAmount, int reduceBy)
    {
        if(baseAmount > reduceBy)
        {
            return baseAmount - reduceBy;
        }
        else
        {
            return 0;
        }
    }
}

namespace EffectFunctions
{
    //Strength: one value with strength of dmg
    void DealMagicDmg(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType type)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            Attack att((int)strength->at(0) + user->GetAttribute(BattleEnums::Attribute::Int), false, targets->at(i), type);
            user->AttackEntity(targets->at(i), &att);
        }
    }

    //Strength: one value with strength of dmg
    void DealPhysicalDmg(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType type)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            Attack att((int)strength->at(0) + user->GetAttribute(BattleEnums::Attribute::Strength), true, targets->at(i), type);
            user->AttackEntity(targets->at(i), &att);
        }
    }

    //Strength: two values, first strength of dmg, second percent of heal from dmg
    void DrainMagicDmg(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType type)
    {
        int dmgAmmount = 0;
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            Attack att((int)strength->at(0) + user->GetAttribute(BattleEnums::Attribute::Int), false, targets->at(i), type);
            user->AttackEntity(targets->at(i), &att);
            dmgAmmount += att.m_finalDmg;
        }

        int heal = (int)std::ceil(dmgAmmount * strength->at(1));
        user->Heal(heal);
    }

    //Strength: two values, first strength of dmg, second percent of heal from dmg
    void DrainPhysicalDmg(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType type)
    {
        int dmgAmmount = 0;
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            Attack att((int)strength->at(0) + user->GetAttribute(BattleEnums::Attribute::Strength), true, targets->at(i), type);
            user->AttackEntity(targets->at(i), &att);
            dmgAmmount += att.m_finalDmg;
        }

        int heal = (int)std::ceil(dmgAmmount * strength->at(1));
        user->Heal(heal);
    }

    //Strength: one value with strength of heal
    void Heal(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            targets->at(i)->Heal((int)strength->at(0));
        }
    }

    //Strength: two values, first duration of heal, second: strength of heal
    void Regenerate(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddOnTurnEffect(new std::function<void(Entity*, IPassiveEffect*)>(
                std::bind(&PassiveEffectFunctions::Heal,std::placeholders::_1,std::placeholders::_2,strength->at(1))));
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //Strength: two values, fist: duration in turns, second: strength of Buff
    void BuffAttribute(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::Attribute attribute)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddAttributeEffect(new std::function<float(float,BattleEnums::Attribute)>(
                std::bind(&PassiveEffectFunctions::BuffAttribute,std::placeholders::_1,std::placeholders::_2,strength->at(1), attribute)));
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //Strength: two values, fist: duration in turns, second: strength of Buff
    void BuffResistance(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType type)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddGetResistance(new std::function<float(float,BattleEnums::AttackType)>(
                std::bind(&PassiveEffectFunctions::BuffResistance,std::placeholders::_1,std::placeholders::_2,strength->at(1), type)));
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //Strength: two values, fist: duration in turns, second: strength of Buff
    void DebuffAttribute(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::Attribute attribute)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddAttributeEffect(new std::function<float(float,BattleEnums::Attribute)>(
                std::bind(&PassiveEffectFunctions::DebuffAttribute,std::placeholders::_1,std::placeholders::_2,strength->at(1), attribute)));
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //Strength: two values, fist: duration in turns, second: strength of Buff
    void DebuffResistance(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType type)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddGetResistance(new std::function<float(float,BattleEnums::AttackType)>(
                std::bind(&PassiveEffectFunctions::DebuffResistance,std::placeholders::_1,std::placeholders::_2,strength->at(1), type)));
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //Strength: two values, fist: duration in turns, second: damage
    void DamageOverTime(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType type)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddOnTurnEffect(new std::function<void(Entity*, IPassiveEffect*)>(
                std::bind(&PassiveEffectFunctions::DamageOverTime,user,std::placeholders::_1,std::placeholders::_2,strength->at(1), type)));
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    void Taunt(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddAttack(new std::function<void(Attack*, Entity*)>(
                std::bind(&PassiveEffectFunctions::Taunt,std::placeholders::_1,std::placeholders::_2,user)));
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //strength is one value, the amount of damage negated
    void PreventDamage(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, -1, effect);
            std::vector<float>* values = new std::vector<float>();
            values->push_back(strength->at(0));
            eff->AddOnLooseHp(new std::function<int(Attack*, Entity*, Entity*, int)>(
                std::bind(&PassiveEffectFunctions::PreventDamage,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,values,eff)));
            eff->AddDescription("effect.1101.additional_desc", values);

            //Should be called after Reduce Damage effect
            eff->SetActivationPriority(200);
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //strength is one value, the number of turns
    void ReduceDamage(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, int reduceTo)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddOnLooseHp(new std::function<int(Attack*, Entity*, Entity*, int)>(
                std::bind(&PassiveEffectFunctions::ReduceDamage,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, reduceTo)));

            //Should be called before Prevent Damage effect but after resistances
            eff->SetActivationPriority(150);
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    //strength is two values, the first is the number of turns, the second the amount to reduce by
    void ReduceDamageBy(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, (int)strength->at(0), effect);
            eff->AddOnLooseHp(new std::function<int(Attack*, Entity*, Entity*, int)>(
                std::bind(&PassiveEffectFunctions::ReduceDamageBy,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, (int)strength->at(1))));

            //Should be called before other Prevent Damage and reduce damage effects
            eff->SetActivationPriority(125);
            targets->at(i)->AddPassiveEffect(eff);
        }
    }

    void AnalyzeEnemy(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect)
    {
        for(unsigned int i = 0; i < targets->size(); i++)
        {
            targets->at(i)->Analyze();
        }
        if(targets->size() > 0 )
        {
            SceneManager* sm = GameController::getInstance()->GetActiveSceneManager();
            SceneManagerBattle* battle = dynamic_cast<SceneManagerBattle*>(sm);
            if(battle != nullptr)
            {
                SceneManagerStatus* status = new SceneManagerStatus(battle, targets->at(0));
                GameController::getInstance()->LoadSceneManager(status);
            }
        }
    }

    //strength is one value, number of attacks that are shielded
    void IgnoreDamage(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, NamedItem* effect, BattleEnums::AttackType attackType)
    {
        for (unsigned int i = 0; i < targets->size(); i++)
        {
            PassiveEffect* eff = new PassiveEffect(true, -1, effect);
            std::vector<float>* values = new std::vector<float>();
            values->push_back(strength->at(0));
            eff->AddOnLooseHp(new std::function<int(Attack*, Entity*, Entity*, int)>(
                std::bind(&PassiveEffectFunctions::IgnoreDamage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, values, eff, attackType)));
            eff->AddDescription("effect.1111.additional_desc", values);

            //Should be called after Prevent Damage effect
            eff->SetActivationPriority(250);
            targets->at(i)->AddPassiveEffect(eff);
        }
    }
}

namespace PassiveSkillEffectFunctions
{
    //Strength: one value hp to heal
    void HealAfterBattle(std::vector<float>* strength, PassiveEffect* target)
    {
        target->AddOnBattleFinished(new std::function<void(Entity*)>(
                std::bind(&PassiveEffectFunctions::Heal,std::placeholders::_1,nullptr,strength->at(0))));
    }
    //Strength: one value hp to heal
    void HealAfterTurn(std::vector<float>* strength, PassiveEffect* target)
    {
        target->AddOnTurnEffect(new std::function<void(Entity*, IPassiveEffect*)>(
                std::bind(&PassiveEffectFunctions::Heal,std::placeholders::_1,std::placeholders::_2,strength->at(0))));
    }
    //Strength: %movement speed
    void Movementspeed(std::vector<float>* strength, PassiveEffect* target)
    {
        target->SetMovementSpeedMultiplier(1 + strength->at(0));
    }
    //Strength no values
    void ShowHp(std::vector<float>* strength, PassiveEffect* target)
    {
        target->SetShowEnemyHealth(true);
    }
    //Strength no values
    void ShowMap(std::vector<float>* strength, PassiveEffect* target)
    {
        target->SetShowCompleteMap(true);
    }
    //Strength mp cost multiplier
    void MpCostMultiplier(std::vector<float>* strength, PassiveEffect* target)
    {
        target->AddGetNeededMp(new std::function<float(float)>(
                std::bind(&PassiveEffectFunctions::SubtractMultiplier,std::placeholders::_1,strength->at(0))));
    }
}
EffectFactoryList* EffectFactoryList::m_instance = 0;


EffectFactoryList::EffectFactoryList()
{
    //ctor
    //Add all the Effects for the game
    std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>* func;
    std::function<void(std::vector<float>* strength, PassiveEffect* target)>* passiveFunc;
    StrengthCalculation* calc;
    EffectFactoryBase* newEffect;

    //Add Fire Damage
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DealMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Fire));
    newEffect = new EffectFactory(func, 1);
    calc = newEffect->GetStrengthCalculation();
    calc->AddStrengthValue(1.0f, 100.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);;

    //Add Air Damage
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DealMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Air));
    newEffect = new EffectFactory(func, 2);
    calc = newEffect->GetStrengthCalculation();
    calc->AddStrengthValue(1.0f, 100.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);;

    //Add Water Damage
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DealMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Water));
    newEffect = new EffectFactory(func, 3);
    calc = newEffect->GetStrengthCalculation();
    calc->AddStrengthValue(1.0f, 100.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Earth Damage
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DealMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Earth));
    newEffect = new EffectFactory(func, 4);
    calc = newEffect->GetStrengthCalculation();
    calc->AddStrengthValue(1.0f, 100.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Physical Damage
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DealPhysicalDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Physical));
    newEffect = new EffectFactory(func, 5);
    calc = newEffect->GetStrengthCalculation();
    calc->AddStrengthValue(1.0f, 100.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Burn effect
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DamageOverTime,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Fire));
    newEffect = new EffectFactory(func, 11);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Damage of burn effect
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Poison effect
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DamageOverTime,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Earth));
    newEffect = new EffectFactory(func, 12);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Damage of burn effect
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Fire Damage with leech
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DrainMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Fire));
    newEffect = new EffectFactory(func, 51, 0.3f);
    calc = newEffect->GetStrengthCalculation();
    //Base Damage
    calc->AddStrengthValue(3.0f, 100.0f);
    //Everything from 5% to 300% of Damage dealt heal
    calc->AddStrengthValue(0.05f, 3.0f, 0.0f, 10.0f, StrengthCalculation::CombiningType::ADD);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);;

    //Add Air Damage with leech
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DrainMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Air));
    newEffect = new EffectFactory(func, 52, 0.3f);
    calc = newEffect->GetStrengthCalculation();
    //Base Damage
    calc->AddStrengthValue(3.0f, 100.0f);
    //Everything from 5% to 300% of Damage dealt heal
    calc->AddStrengthValue(0.05f, 3.0f, 0.0f, 10.0f, StrengthCalculation::CombiningType::ADD);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);;

    //Add Water Damage with leech
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DrainMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Water));
    newEffect = new EffectFactory(func, 53, 0.3f);
    calc = newEffect->GetStrengthCalculation();
    //Base Damage
    calc->AddStrengthValue(3.0f, 100.0f);
    //Everything from 5% to 300% of Damage dealt heal
    calc->AddStrengthValue(0.05f, 3.0f, 0.0f, 10.0f, StrengthCalculation::CombiningType::ADD);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Earth Damage with leech
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DrainMagicDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Earth));
    newEffect = new EffectFactory(func, 54, 0.3f);
    calc = newEffect->GetStrengthCalculation();
    //Base Damage
    calc->AddStrengthValue(3.0f, 100.0f);
    //Everything from 5% to 300% of Damage dealt heal
    calc->AddStrengthValue(0.05f, 3.0f, 0.0f, 10.0f, StrengthCalculation::CombiningType::ADD);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Physical Damage with leech
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DrainPhysicalDmg,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Physical));
    newEffect = new EffectFactory(func, 55, 0.3f);
    calc = newEffect->GetStrengthCalculation();
    //Base Damage
    calc->AddStrengthValue(3.0f, 100.0f);
    //Everything from 5% to 300% of Damage dealt heal
    calc->AddStrengthValue(0.05f, 3.0f, 0.0f, 10.0f, StrengthCalculation::CombiningType::ADD);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Damage);
    m_effects.push_back(newEffect);

    //Add Strength Buff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Strength));
    newEffect = new EffectFactory(func, 101);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(10.0f);
    //Strength Buff fits Fire and Earth or Physical
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffOffense);
    m_effects.push_back(newEffect);

    //Add Defense Buff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Defense));
    newEffect = new EffectFactory(func, 102);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(10.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Add Magic defense Buff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::MagicDefense));
    newEffect = new EffectFactory(func, 103);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(10.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Add Int Buff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Int));
    newEffect = new EffectFactory(func, 104);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(10.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffMagic);
    m_effects.push_back(newEffect);

    //Add Speed Buff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Speed));
    newEffect = new EffectFactory(func, 105);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(10.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffOffense);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffMagic);
    m_effects.push_back(newEffect);

    //Add Elemental Resistance Buffs
    //Physical Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Physical));
    newEffect = new EffectFactory(func, 151);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(5.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Water Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Water));
    newEffect = new EffectFactory(func, 152);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(5.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Air Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Air));
    newEffect = new EffectFactory(func, 153);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(5.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Earth Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Earth));
    newEffect = new EffectFactory(func, 154);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(5.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Fire Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::BuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Fire));
    newEffect = new EffectFactory(func, 155);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 300% buff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(5.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);



    //Heal
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(&EffectFunctions::Heal);
    newEffect = new EffectFactory(func, 1001);
    calc = newEffect->GetStrengthCalculation();
    //Everything from 1 to 300 hp heal
    calc->AddStrengthValue(1.0f, 300.0f, 1.0f);
    calc->SetMultiplier(2.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Heal);
    m_effects.push_back(newEffect);

    //Regenerate
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(&EffectFunctions::Regenerate);
    newEffect = new EffectFactory(func, 1002);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 1 to 30 hp heal per turn
    calc->AddStrengthValue(1.0f, 30.0f, 1.0f);
    calc->SetMultiplier(1.5f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Heal);
    m_effects.push_back(newEffect);


    //Shield Damage
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(&EffectFunctions::PreventDamage);
    newEffect = new EffectFactory(func, 1101, 0.6f);
    calc = newEffect->GetStrengthCalculation();
    //Amount of hp shielded
    calc->AddStrengthValue(1.0f, 300.0f, 1.0f);
    calc->SetMultiplier(3.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Heal);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Reduce damage to 1
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::ReduceDamage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, 1));
    newEffect = new EffectFactory(func, 1102, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Duration of buff
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(10.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);


    //Reduce damage by
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::ReduceDamageBy, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    newEffect = new EffectFactory(func, 1103, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Duration of buff
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    //Reduction of damage
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(1.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);


    //Ignore Physical Damage
    func = new std::function<void(std::vector<float> * strength, Entity * user, std::vector<Entity*> * targets, NamedItem * effect)>(std::bind(&EffectFunctions::IgnoreDamage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, BattleEnums::AttackType::Physical));
    newEffect = new EffectFactory(func, 1111, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Number of times the damage is ignored
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(3.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Ignore Water Damage
    func = new std::function<void(std::vector<float> * strength, Entity * user, std::vector<Entity*> * targets, NamedItem * effect)>(std::bind(&EffectFunctions::IgnoreDamage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, BattleEnums::AttackType::Water));
    newEffect = new EffectFactory(func, 1112, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Number of times the damage is ignored
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(3.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Ignore Air Damage
    func = new std::function<void(std::vector<float> * strength, Entity * user, std::vector<Entity*> * targets, NamedItem * effect)>(std::bind(&EffectFunctions::IgnoreDamage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, BattleEnums::AttackType::Air));
    newEffect = new EffectFactory(func, 1113, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Number of times the damage is ignored
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(3.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Ignore Earth Damage
    func = new std::function<void(std::vector<float> * strength, Entity * user, std::vector<Entity*> * targets, NamedItem * effect)>(std::bind(&EffectFunctions::IgnoreDamage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, BattleEnums::AttackType::Earth));
    newEffect = new EffectFactory(func, 1114, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Number of times the damage is ignored
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(3.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);

    //Ignore Fire Damage
    func = new std::function<void(std::vector<float> * strength, Entity * user, std::vector<Entity*> * targets, NamedItem * effect)>(std::bind(&EffectFunctions::IgnoreDamage, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, BattleEnums::AttackType::Fire));
    newEffect = new EffectFactory(func, 1115, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Number of times the damage is ignored
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(3.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Buff);
    newEffect->AddEffectType(BattleEnums::EffectType::BuffDefense);
    m_effects.push_back(newEffect);


    //Add Strength Debuff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Strength));
    newEffect = new EffectFactory(func, 10001);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Debuff for something between 2 and 66%
    calc->AddStrengthValue(0.02f, 0.66f);
    calc->SetMultiplier(15.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffOffense);
    m_effects.push_back(newEffect);

    //Add Defense Debuff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Defense));
    newEffect = new EffectFactory(func, 10002);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Debuff for something between 2 and 66%
    calc->AddStrengthValue(0.02f, 0.66f);
    calc->SetMultiplier(15.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffDefense);
    m_effects.push_back(newEffect);

    //Add Magic Defense Debuff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::MagicDefense));
    newEffect = new EffectFactory(func, 10003);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Debuff for something between 2 and 66%
    calc->AddStrengthValue(0.02f, 0.66f);
    calc->SetMultiplier(15.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffDefense);
    m_effects.push_back(newEffect);

    //Add Int Debuff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Int));
    newEffect = new EffectFactory(func, 10004);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Debuff for something between 2 and 66%
    calc->AddStrengthValue(0.02f, 0.66f);
    calc->SetMultiplier(15.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffMagic);
    m_effects.push_back(newEffect);

    //Add Speed Debuff
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffAttribute,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::Attribute::Speed));
    newEffect = new EffectFactory(func, 10005);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Debuff for something between 2 and 66%
    calc->AddStrengthValue(0.02f, 0.66f);
    calc->SetMultiplier(15.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffOffense);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffMagic);
    m_effects.push_back(newEffect);


    //Add Elemental Resistance Debuffs
    //Physical Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Physical));
    newEffect = new EffectFactory(func, 10051);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 66% debuff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(7.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffDefense);
    m_effects.push_back(newEffect);

    //Water Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Water));
    newEffect = new EffectFactory(func, 10052);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 66% debuff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(7.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffDefense);
    m_effects.push_back(newEffect);

    //Air Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Air));
    newEffect = new EffectFactory(func, 10053);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 66% debuff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(7.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffDefense);
    m_effects.push_back(newEffect);

    //Earth Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Earth));
    newEffect = new EffectFactory(func, 10054);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 66% debuff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(7.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffDefense);
    m_effects.push_back(newEffect);

    //Fire Defense
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(std::bind(&EffectFunctions::DebuffResistance,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, BattleEnums::AttackType::Fire));
    newEffect = new EffectFactory(func, 10055);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(2.0f, 10.0f, 1.0f);
    //Everything from 5% to 66% debuff
    calc->AddStrengthValue(0.05f, 3.0f);
    calc->SetMultiplier(7.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffDefense);
    m_effects.push_back(newEffect);

    //Taunt enemy
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(&EffectFunctions::Taunt);
    newEffect = new EffectFactory(func, 10101);
    calc = newEffect->GetStrengthCalculation();
    //Number of turns: from 2 to 10 with step of 1
    calc->AddStrengthValue(1.0f, 10.0f, 1.0f);
    calc->SetMultiplier(5.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    newEffect->AddEffectType(BattleEnums::EffectType::DebuffOffense);
    m_effects.push_back(newEffect);


    //Analyze enemy
    func = new std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>*targets, NamedItem* effect)>(&EffectFunctions::AnalyzeEnemy);
    newEffect = new EffectFactory(func, 10201);
    calc = newEffect->GetStrengthCalculation();
    calc->SetMultiplier(20.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Debuff);
    m_effects.push_back(newEffect);




    //Heal after battle
    passiveFunc = new std::function<void(std::vector<float>* strength, PassiveEffect* target)>(&PassiveSkillEffectFunctions::HealAfterBattle);
    newEffect = new EffectFactoryPassive(passiveFunc, 100001);
    calc = newEffect->GetStrengthCalculation();
    //Everything from 1 to 300 hp heal
    calc->AddStrengthValue(1.0f, 300.0f, 1.0f);
    calc->SetMultiplier(3.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Passive);
    m_effects.push_back(newEffect);

    //Heal on turn
    passiveFunc = new std::function<void(std::vector<float>* strength, PassiveEffect* target)>(&PassiveSkillEffectFunctions::HealAfterTurn);
    newEffect = new EffectFactoryPassive(passiveFunc, 100002, 0.4f);
    calc = newEffect->GetStrengthCalculation();
    //Everything from 1 to 300 hp heal
    calc->AddStrengthValue(1.0f, 300.0f, 1.0f);
    calc->SetMultiplier(10.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Passive);
    m_effects.push_back(newEffect);

    //Movement boost
    passiveFunc = new std::function<void(std::vector<float>* strength, PassiveEffect* target)>(&PassiveSkillEffectFunctions::Movementspeed);
    newEffect = new EffectFactoryPassive(passiveFunc, 100101);
    calc = newEffect->GetStrengthCalculation();
    //Everything from 1 to 50 % more speed
    calc->AddStrengthValue(0.01f, 0.5f);
    calc->SetMultiplier(100.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddEffectType(BattleEnums::EffectType::Passive);
    m_effects.push_back(newEffect);

    //Show hp bar
    passiveFunc = new std::function<void(std::vector<float>* strength, PassiveEffect* target)>(&PassiveSkillEffectFunctions::ShowHp);
    newEffect = new EffectFactoryPassive(passiveFunc, 100102);
    calc = newEffect->GetStrengthCalculation();
    calc->SetMultiplier(20.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddEffectType(BattleEnums::EffectType::Passive);
    m_effects.push_back(newEffect);

    //Reduce MP cost
    passiveFunc = new std::function<void(std::vector<float>* strength, PassiveEffect* target)>(&PassiveSkillEffectFunctions::MpCostMultiplier);
    newEffect = new EffectFactoryPassive(passiveFunc, 100103, 0.6f);
    calc = newEffect->GetStrengthCalculation();
    calc->AddStrengthValue(0.01f, 0.75f);
    calc->SetMultiplier(100.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddAttackType(BattleEnums::AttackType::Water);
    newEffect->AddEffectType(BattleEnums::EffectType::Passive);
    m_effects.push_back(newEffect);

    //Show complete map
    passiveFunc = new std::function<void(std::vector<float>* strength, PassiveEffect* target)>(&PassiveSkillEffectFunctions::ShowMap);
    newEffect = new EffectFactoryPassive(passiveFunc, 100104);
    calc = newEffect->GetStrengthCalculation();
    calc->SetMultiplier(20.0f);
    newEffect->AddAttackType(BattleEnums::AttackType::Physical);
    newEffect->AddAttackType(BattleEnums::AttackType::Earth);
    newEffect->AddAttackType(BattleEnums::AttackType::Air);
    newEffect->AddAttackType(BattleEnums::AttackType::Fire);
    newEffect->AddEffectType(BattleEnums::EffectType::Passive);
    m_effects.push_back(newEffect);
}

EffectFactoryList::~EffectFactoryList()
{
    //dtor
    for(unsigned int i = 0; i < m_effects.size(); i++)
    {
        delete m_effects[i];
    }
}

EffectFactoryBase* EffectFactoryList::getRandom(BattleEnums::AttackType attackType, BattleEnums::EffectType effectType)
{
    float maxChance = 0.0f;
    for(unsigned int i = 0; i < m_effects.size(); i++)
    {
        if(m_effects[i]->DoesContainAttackType(attackType) && m_effects[i]->DoesContainEffectType(effectType))
            maxChance += m_effects[i]->GetChance();
    }
    if(maxChance == 0)
    {
        std::string msg = "No Effect with the AttackType:";
        msg.append(std::to_string((int)attackType));
        msg.append(" and the EffectType:");
        msg.append(std::to_string((int)effectType));
        msg.append(" avaliable!");
        throw InvalidArgumentException(msg);
    }
    float chance = (rand() / ((float)RAND_MAX)) * maxChance;
    for(unsigned int i = 0; i < m_effects.size(); i++)
    {
        if(m_effects[i]->DoesContainAttackType(attackType) && m_effects[i]->DoesContainEffectType(effectType))
        {
            chance -= m_effects[i]->GetChance();
            if(chance <= 0.0f)
                return m_effects[i];
        }
    }
    throw InvalidArgumentException("EffectFactory not found");
}

EffectFactoryBase* EffectFactoryList::getRandom(BattleEnums::AttackType attackType, BattleEnums::EffectType effectType, bool needTarget)
{
    float maxChance = 0.0f;
    for(unsigned int i = 0; i < m_effects.size(); i++)
    {
        if(m_effects[i]->DoesContainAttackType(attackType) && m_effects[i]->DoesContainEffectType(effectType) && m_effects[i]->DoesNeedTarget() == needTarget)
            maxChance += m_effects[i]->GetChance();
    }
    if(maxChance == 0.0f)
    {
        std::string msg = "No Effect with the AttackType:";
        msg.append(std::to_string((int)attackType));
        msg.append(", the EffectType:");
        msg.append(std::to_string((int)effectType));
        msg.append(" and the Target:");
        msg.append(std::to_string((int)needTarget));
        msg.append(" avaliable!");
        throw InvalidArgumentException(msg);
    }
    float chance = (rand() / ((float)RAND_MAX)) * maxChance;
    for(unsigned int i = 0; i < m_effects.size(); i++)
    {
        if(m_effects[i]->DoesContainAttackType(attackType) && m_effects[i]->DoesContainEffectType(effectType) && m_effects[i]->DoesNeedTarget() == needTarget)
        {
            chance -= m_effects[i]->GetChance();
            if(chance <= 0.0f)
                return m_effects[i];
        }
    }
    throw InvalidArgumentException("EffectFactory not found");
}

EffectFactoryBase* EffectFactoryList::getWithId(int id)
{
    for(auto it = m_effects.begin(); it != m_effects.end(); it++)
    {
        if((*it)->GetId() == id)
        {
            return *it;
        }
    }
    std::string msg = "No Effect with id:";
    msg.append(std::to_string(id));
    throw InvalidArgumentException(msg);
}


EffectFactoryList* EffectFactoryList::GetInstance()
{
    if(m_instance == 0)
    {
        m_instance = new EffectFactoryList();
    }
    return m_instance;
}

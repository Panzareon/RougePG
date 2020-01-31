#include "Party/ItemFactory.h"
#include "Exception/InvalidArgumentException.h"
#include "Controller/MathHelper.h"
#include "Controller/Localization.h"
#include "Battle/PassiveEffect.h"

namespace PassiveEffectFunctions
{
    void AddAttackType(Attack* att, Entity* attacker, BattleEnums::AttackType type, bool physical);
    void Heal(Entity* target, IPassiveEffect* passiveEffect, int hp);
    void MultiplyDmg(Attack* att, Entity* attacker, float multiply, bool physical);
    int ReduceDamageBy(Attack* att, Entity* target, Entity* attacker, int baseAmount, int reduceBy);
}

ItemFactory* ItemFactory::m_instance = nullptr;

ItemFactory::ItemFactory()
{
    //ctor
    m_equipmentIds[Equipment::MainHand][StartingItem].push_back(1);
    m_equipmentIds[Equipment::MainHand][StartingItem].push_back(2);
    m_equipmentIds[Equipment::MainHand][StartingItem].push_back(3);
    m_equipmentIds[Equipment::MainHand][StartingItem].push_back(4);

    m_equipmentIds[Equipment::MainHand][BaseItem].push_back(101);
    m_equipmentIds[Equipment::MainHand][BaseItem].push_back(102);
    m_equipmentIds[Equipment::MainHand][BaseItem].push_back(103);
    m_equipmentIds[Equipment::MainHand][BaseItem].push_back(104);

    m_equipmentIds[Equipment::MainHand][SpecialItem].push_back(1001);
    m_equipmentIds[Equipment::MainHand][SpecialItem].push_back(1101);
    m_equipmentIds[Equipment::MainHand][SpecialItem].push_back(1201);
    m_equipmentIds[Equipment::MainHand][SpecialItem].push_back(1301);
    //TODO: init all ItemIds
}

ItemFactory::~ItemFactory()
{
    //dtor
}
ItemFactory* ItemFactory::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new ItemFactory();
    }
    return m_instance;
}

Item* ItemFactory::GetRandomItem(Item::ItemType type, ItemRarity rarity)
{
    if(type == Item::ItemTypeEquipment)
        return GetRandomEquipment(rarity);
    int random = rand() % m_itemIds[type][rarity].size();
    int itemId = m_itemIds[type][rarity][random];
    return GetItem(type, itemId);
}

Item* ItemFactory::GetRandomEquipment(ItemRarity rarity)
{
    int random = rand() % Equipment::EQUIPMENT_POSITION_END;
    return GetRandomEquipment((Equipment::EquipmentPosition) random, rarity);
}

Item* ItemFactory::GetRandomEquipment(Equipment::EquipmentPosition pos, ItemRarity rarity)
{
    int random = rand() % m_equipmentIds[pos][rarity].size();
    int itemId = m_equipmentIds[pos][rarity][random];
    return GetEquipment(pos, itemId);
}

Item* ItemFactory::GetItem(Item::ItemType type, int itemId)
{
    Item* retval = new Item(itemId, type);
    LoadItem(type, itemId, retval);

    switch(itemId)
    {
        //TODO: Init Item with Id itemId

    default:
        std::string msg("Invalid ItemId ");
        msg.append(std::to_string(itemId));
        msg.append(" given in GetItem.");
        throw GenericException(msg);
    }
    return retval;
}

Item* ItemFactory::GetEquipment(Equipment::EquipmentPosition pos, int itemId)
{
    Localization* localization = Localization::GetInstance();
    Equipment* retval = new Equipment(itemId,pos);
    LoadEquipment(pos, itemId, retval);
    SkillGenerator* gen = retval->GetSkillGenerator();

    MathHelper* mathHelper = MathHelper::GetInstance();
    switch(itemId)
    {
    case 1:
        retval->SetAttributeBuff(BattleEnums::AttributeStrength, mathHelper->GetRandomInt(6,3,true));
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(3,0,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(3,0,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(3,0,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(3,1,true));
        break;

    case 2:
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(6,3,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(6,3,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(6,3,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(3,1,true));
        break;

    case 3:
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(3,0,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(6,3,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(6,3,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(6,3,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(3,1,true));
        break;

    case 4:
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(3,0,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxMp, mathHelper->GetRandomInt(6,3,true));
        retval->SetAttributeBuff(BattleEnums::AttributeInt, mathHelper->GetRandomInt(6,3,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(3,0,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(3,1,true));
        break;

    case 101:
        retval->SetAttributeBuff(BattleEnums::AttributeStrength, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(5,2,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(4,2,true));
        break;

    case 102:
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(10,5,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(4,2,true));
        break;

    case 103:
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(10,5,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(4,2,true));
        break;

    case 104:
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxMp, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeInt, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(5,2,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(4,2,true));
        break;

    case 1001:
        retval->SetAttributeBuff(BattleEnums::AttributeStrength, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(5,2,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(5,2,true));
        break;

    case 1101:
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(10,5,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(5,2,true));
        break;

    case 1201:
        retval->SetAttributeBuff(BattleEnums::AttributeDefense, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxHp, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(10,5,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(5,2,true));
        break;

    case 1301:
        retval->SetAttributeBuff(BattleEnums::AttributeMagicDefense, mathHelper->GetRandomInt(5,2,true));
        retval->SetAttributeBuff(BattleEnums::AttributeMaxMp, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeInt, mathHelper->GetRandomInt(10,5,true));
        retval->SetAttributeBuff(BattleEnums::AttributeSpeed, mathHelper->GetRandomInt(5,2,true));

        retval->AddSkillsToLearn(mathHelper->GetRandomInt(5,2,true));
        break;


        //TODO: Init Equipment with Id itemId
    default:
        std::string msg("Invalid ItemId ");
        msg.append(std::to_string(itemId));
        msg.append(" given in GetEquipment.");
        throw GenericException(msg);
    }
    return retval;
}

void ItemFactory::LoadItem(Item::ItemType type, int id, Item* item)
{
    if(item->GetItemType() == Item::ItemTypeEquipment)
    {
        Equipment* equipment = (Equipment*)item;
        LoadEquipment(equipment->GetEquipmentPosition(), id, equipment);
    }
    else
    {
        switch(id)
        {
            //TODO: Init Item with Id itemId

        default:
            std::string msg("Invalid ItemId ");
            msg.append(std::to_string(id));
            msg.append(" given in LoadItem.");
            throw GenericException(msg);
        }
    }
}

void ItemFactory::LoadEquipment(Equipment::EquipmentPosition pos, int id, Equipment* equipment)
{
    Localization* localization = Localization::GetInstance();
    SkillGenerator* gen = equipment->GetSkillGenerator();

    switch(id)
    {
    case 1:
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.7f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.09f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.09f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.8f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.05f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.5f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.2f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.3f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.5f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.5f, false);

        equipment->SetSkillStrength(10);

        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::MultiplyDmg,std::placeholders::_1,std::placeholders::_2,1.5f, true)));
        break;

    case 2:
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.18f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.02f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.3f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.025f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.025f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetPassive, 0.3f);


        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.8f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.05f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.5f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.4f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.3f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.7f, false);

        equipment->SetSkillStrength(10);


        //Adding effect that reduces damage by 1
        {
            int reduceBy = 1;
            std::vector<float> values;
            values.push_back(reduceBy);
            PassiveEffect* eff = new PassiveEffect(true, -1, localization->GetLocalization("effect.1103"), localization->GetLocalizationWithFloats("effect.1103.desc2", &values), true);
            eff->AddOnLooseHp(new std::function<int(Attack*, Entity*, Entity*, int)>(
                std::bind(&PassiveEffectFunctions::ReduceDamageBy,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, reduceBy)));
            //Should be called before other Prevent Damage and reduce damage effects
            eff->SetActivationPriority(125);
            eff->StaysAfterBattle();
            equipment->AddAdditionalEffect(std::shared_ptr<IPassiveEffect>(eff));
        }
        break;

    case 3:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.08f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.3f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.45f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.01f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.35f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.25f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.3f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.8f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.3f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.7f, false);

        equipment->SetSkillStrength(10);
        break;

    case 4:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.55f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.09f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.29f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.04f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.24f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.24f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.24f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.24f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.7f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.3f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.7f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.3f, false);

        equipment->SetSkillStrength(10);

        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::MultiplyDmg,std::placeholders::_1,std::placeholders::_2,1.5f, false)));
        break;

    case 101:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.7f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.09f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.09f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.8f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.05f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.5f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.2f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.3f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.5f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.5f, false);

        equipment->SetSkillStrength(20);

        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::MultiplyDmg,std::placeholders::_1,std::placeholders::_2,1.75f, true)));
        break;

    case 102:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.2f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.3f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.025f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.025f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetPassive, 0.25f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.8f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.05f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.5f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.4f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.3f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.7f, false);

        equipment->SetSkillStrength(20);

        //Adding effect that reduces damage by 2
        {
            int reduceBy = 2;
            std::vector<float> values;
            values.push_back(reduceBy);
            PassiveEffect* eff = new PassiveEffect(true, -1, localization->GetLocalization("effect.1103"), localization->GetLocalizationWithFloats("effect.1103.desc2", &values), true);
            eff->AddOnLooseHp(new std::function<int(Attack*, Entity*, Entity*, int)>(
                std::bind(&PassiveEffectFunctions::ReduceDamageBy,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, reduceBy)));
            //Should be called before other Prevent Damage and reduce damage effects
            eff->SetActivationPriority(125);
            eff->StaysAfterBattle();
            equipment->AddAdditionalEffect(std::shared_ptr<IPassiveEffect>(eff));
        }
        break;

    case 103:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.08f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.3f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.48f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.02f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.01f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.35f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.25f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.3f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.8f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.3f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.7f, false);

        equipment->SetSkillStrength(20);
        break;

    case 104:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.55f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.09f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.29f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.04f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.24f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.24f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.24f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.24f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.7f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.3f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.7f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.3f, false);

        equipment->SetSkillStrength(20);

        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::MultiplyDmg,std::placeholders::_1,std::placeholders::_2,1.75f, false)));
        break;

    case 1001:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.7f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.09f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.09f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.4f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.5f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.05f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.5f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.2f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.3f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.5f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.5f, false);

        equipment->SetSkillStrength(30);

        equipment->SetTypeResistance(BattleEnums::AttackTypeFire, 1.3f);
        //Physical attacks get additional type fire, if this item is equipped
        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::AddAttackType,std::placeholders::_1,std::placeholders::_2,BattleEnums::AttackTypeFire, true)));
        //Physical attacks deal double damage
        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::MultiplyDmg,std::placeholders::_1,std::placeholders::_2,2.0f, true)));
        break;

    case 1101:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.25f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.3f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.025f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.025f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetPassive, 0.2f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.4f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.5f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.05f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.5f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.4f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.3f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.7f, false);

        equipment->SetSkillStrength(30);

        equipment->SetTypeResistance(BattleEnums::AttackTypeFire, 2.0f);

        //Adding effect that reduces damage by 4
        {
            int reduceBy = 4;
            std::vector<float> values;
            values.push_back(reduceBy);
            PassiveEffect* eff = new PassiveEffect(true, -1, localization->GetLocalization("effect.1103"), localization->GetLocalizationWithFloats("effect.1103.desc2", &values), true);
            eff->AddOnLooseHp(new std::function<int(Attack*, Entity*, Entity*, int)>(
                std::bind(&PassiveEffectFunctions::ReduceDamageBy,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4, reduceBy)));
            //Should be called before other Prevent Damage and reduce damage effects
            eff->SetActivationPriority(125);
            eff->StaysAfterBattle();
            equipment->AddAdditionalEffect(std::shared_ptr<IPassiveEffect>(eff));
        }
        break;

    case 1201:

        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.08f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.3f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.1f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.48f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.02f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.01f);

        gen->AddSkillAttackType(BattleEnums::AttackTypePhysical, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeWater, 0.35f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 0.05f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeEarth, 0.25f);
        gen->AddSkillAttackType(BattleEnums::AttackTypeAir, 0.3f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffDefense, 0.1f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.8f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.3f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.7f, false);

        equipment->SetSkillStrength(30);

        //Heals 2 hp every turn
        equipment->AddOnTurnEffect(new std::function<void(Entity*, IPassiveEffect*)>(
                std::bind(&PassiveEffectFunctions::Heal,std::placeholders::_1,std::placeholders::_2,2)));
        break;

    case 1301:
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamEntity, 0.55f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeamRandomEntity, 0.05f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeam, 0.01f);
        gen->AddSkillTarget(BattleEnums::TargetSelf, 0.09f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetOwnTeamRandomEntity, 0.005f);
        gen->AddSkillTarget(BattleEnums::TargetEnemyTeam, 0.29f);

        gen->AddSkillAttackType(BattleEnums::AttackTypeFire, 1.0f);
        gen->AddSkillEffectType(BattleEnums::EffectTypeBuffOffense, 0.7f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeHeal, 0.3f, true);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDamage, 0.7f, false);
        gen->AddSkillEffectType(BattleEnums::EffectTypeDebuff, 0.3f, false);

        equipment->SetSkillStrength(30);
        //Magic attacks have additional Fire type if this item is equipped
        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::AddAttackType,std::placeholders::_1,std::placeholders::_2,BattleEnums::AttackTypeFire, false)));
        //Magic attacks deal double damage
        equipment->AddAttack(new std::function<void(Attack* att, Entity* attacker)>(
                std::bind(&PassiveEffectFunctions::MultiplyDmg,std::placeholders::_1,std::placeholders::_2,2.0f, false)));
        break;


        //TODO: Init Equipment with Id itemId
    default:
        std::string msg("Invalid ItemId ");
        msg.append(std::to_string(id));
        msg.append(" given in LoadEquipment.");
        throw GenericException(msg);
    }
}


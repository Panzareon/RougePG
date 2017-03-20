#ifndef EFFECTFACTORY_H
#define EFFECTFACTORY_H

#include <vector>
#include <set>
#include <functional>

#include "Effect.h"
#include "StrengthCalculation.h"

class EffectFactory
{
    public:
        EffectFactory(std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, Effect* effect)>* func, int id);
        virtual ~EffectFactory();

        //Returns new Instance of Effect with a strength defined by the value given, needs to be deleted
        Effect* GetEffectWithValue(float value, BattleEnums::Target target);

        void AddAttackType(BattleEnums::AttackType type);
        void AddEffectType(BattleEnums::EffectType type);
        void SetNeedTarget(bool need);

        bool DoesContainAttackType(BattleEnums::AttackType type);
        bool DoesContainEffectType(BattleEnums::EffectType type);
        //If the effect can be used with TargetNone
        bool DoesNeedTarget();

        int GetId();

        StrengthCalculation* GetStrengthCalculation();
    protected:
    private:
        //Function Pointer to Function that implements the Effect
        std::function<void(std::vector<float>* strength, Entity* user, std::vector<Entity*>* targets, Effect* effect)>* m_effectFunction;

        std::set<BattleEnums::AttackType> m_attackType;
        std::set<BattleEnums::EffectType> m_effectType;
        bool m_needsTarget;
        StrengthCalculation m_strengthCalculation;

        int m_id;
};

#endif // EFFECTFACTORY_H

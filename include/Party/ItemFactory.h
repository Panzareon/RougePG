#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include "Item.h"
#include "Equipment.h"
#include <vector>
#include <map>

class ItemFactory
{
    public:
        virtual ~ItemFactory();
        static ItemFactory* GetInstance();

        enum ItemRarity{StartingItem, BaseItem, SpecialItem};

        Item* GetRandomItem(Item::ItemType type, ItemRarity rarity);
        Item* GetRandomEquipment(ItemRarity rarity);
        Item* GetRandomEquipment(Equipment::EquipmentPosition pos, ItemRarity rarity);

        //Creates an item
        Item* GetItem(Item::ItemType type, int id);
        //Creates equipment
        Item* GetEquipment(Equipment::EquipmentPosition pos, int id);
        //Sets values for an item
        void LoadItem(Item::ItemType type, int id, Item* item);
        //Sets values for equipment
        void LoadEquipment(Equipment::EquipmentPosition pos, int id, Equipment* item);

    protected:
        static ItemFactory* m_instance;
        ItemFactory();

        std::map<Item::ItemType, std::map<ItemRarity, std::vector<int> > > m_itemIds;
        std::map<Equipment::EquipmentPosition, std::map<ItemRarity, std::vector<int> > > m_equipmentIds;

    private:
};

#endif // ITEMFACTORY_H

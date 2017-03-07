#ifndef TEXTURELIST_H
#define TEXTURELIST_H

#include <map>
#include "Texture.h"

class TextureList
{
    public:
        //List of Texture file names
        enum TextureFiles{
            DungeonTileMap,DebugTileMap,

            HeroSpriteSheet,
            EnemySpriteSheet,

            BatBattleSprite, DeadWizardBattleSprite, StoneGolemBattleSprite,
            DefaultBattleSprite,

            TargetCursor,

            FireballAnimation,
            WaterAnimation,
            EarthAnimation,
            AirAnimation,
            SwordAnimation,

            DungeonBattleBackground,

            GameOverScreen,
            InGameMenu,
            InGameMenuSelected,
            EquipmentMenu,
            EquipmentSelected
        };
        virtual ~TextureList();


        static Texture* getTexture(TextureFiles file);
    protected:
    private:
        static std::map<TextureFiles, Texture*> m_textures;
        TextureList();
};

#endif // TEXTURELIST_H

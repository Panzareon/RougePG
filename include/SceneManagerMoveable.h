#ifndef SCENEMANAGERMOVEABLE_H
#define SCENEMANAGERMOVEABLE_H

#include <SceneManager.h>
#include "Map.h"
#include "AnimatedNode.h"
#include "MapEvent.h"
#include <vector>


class SceneManagerMoveable : public SceneManager
{
    public:
        SceneManagerMoveable(int tileWidth, int tileHeight);
        virtual ~SceneManagerMoveable();
        virtual void Tick();
    protected:
        void UpdateCamPosition();
        Map m_map;
        AnimatedNode* m_hero;
        Enums::Direction m_heroDirection;
        std::vector<MapEvent*> m_events;

        int m_minViewPosX;
        int m_minViewPosY;
        int m_maxViewPosX;
        int m_maxViewPosY;

        TileMap* m_tileMap;
        TileMap* m_tileMapItems;
        TileMap* m_tileMapAboveHero;
        TileMap* m_tileMapAboveWall;
        TileMap* m_tileMapWallDecoration;


        #ifdef DEBUG_FLAG
        Node* m_roomNumberNode;
        TileMap* m_tileMapRoomNumber;
        #endif // DEBUG_FLAG


        Node* m_belowHero;
        Node* m_eventLayer;
        Node* m_aboveHero;
    private:
};

#endif // SCENEMANAGERMOVEABLE_H

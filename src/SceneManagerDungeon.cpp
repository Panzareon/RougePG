#include "SceneManagerDungeon.h"

#include "DrawableNode.h"
#include "AnimatedNode.h"
#include "TileMap.h"
#include "MapFillDungeon.h"
#include "TextureList.h"
#include "MapEventStairs.h"
#include "MapEventEnemy.h"
#include "MapEventChest.h"

#include <iostream>


SceneManagerDungeon::SceneManagerDungeon(sf::RenderTarget * target, int windowWidth, int windowHeight, int tileWidth, int tileHeight, unsigned int seed): SceneManagerMoveable(target, windowWidth, windowHeight, tileWidth, tileHeight), m_generator(&m_map, seed)
{
    //ctor
    //Define Tile Maps
    SetMemberStats();
    m_map.init(5);
    m_tileMap = new TileMap();
    m_tileMapItems = new TileMap();
    m_tileMapAboveHero = new TileMap();
    m_tileMapAboveWall = new TileMap();
    m_tileMapWallDecoration = new TileMap();

    m_tileMap->setTexture(TextureList::getTexture(TextureList::DungeonTileMap));
    m_tileMapItems->setTexture(TextureList::getTexture(TextureList::DungeonTileMap));
    m_tileMapAboveHero->setTexture(TextureList::getTexture(TextureList::DungeonTileMap));
    m_tileMapAboveWall->setTexture(TextureList::getTexture(TextureList::DungeonTileMap));
    m_tileMapWallDecoration->setTexture(TextureList::getTexture(TextureList::DungeonTileMap));

    //Build Scene Graph
    m_mainNode = new Node();
    m_belowHero = new Node();
    m_mainNode->addChild(m_belowHero);

    m_eventLayer = new Node();
    m_mainNode->addChild(m_eventLayer);
    m_mainNode->addChild(m_animationNode);

    m_aboveHero = new Node();
    m_mainNode->addChild(m_aboveHero);

    m_belowHero->addChild(new DrawableNode(m_tileMap));
    m_belowHero->addChild(new DrawableNode(m_tileMapItems));

    m_aboveHero->addChild(new DrawableNode(m_tileMapAboveHero));
    m_aboveHero->addChild(new DrawableNode(m_tileMapAboveWall));
    m_aboveHero->addChild(new DrawableNode(m_tileMapWallDecoration));


    //Add Hero
    sf::Sprite hero;
    Texture* tex = TextureList::getTexture(TextureList::HeroSpriteSheet);
    hero.setTexture(*tex);
    hero.setTextureRect(sf::IntRect(15,13,32,36));
    m_hero = new AnimatedNode(&hero, tex->GetNumberAnimationSteps());
    m_hero->setBoundingBox(sf::FloatRect(8.0f,22.0f,16.0f,16.0f));
    m_eventLayer->addChild(m_hero);

    //Generate Map

    if(tileHeight > 200 || tileWidth > 200)
        m_generator.FasterCellularAutomata(0.45f);
    else
        m_generator.CellularAutomata(0.45f);

    m_generator.NumberRooms();

    m_generator.PlaceStartingAndEndPosition();

    sf::Transform heroTransform;
    //Place Hero at Startposition
    heroTransform.translate(m_map.m_startX * TileMap::GetTileWith(), m_map.m_startY * TileMap::GetTileWith() - 14);
    m_hero->setTransform(heroTransform);
    UpdateCamPosition();

    m_mapFill = new MapFillDungeon(&m_map);
    //Fill Base Layer with walkable Tile
    m_mapFill->FillLayer(MapFill::Ground, 0);
    //Fill Wall
    m_mapFill->FillLayer(MapFill::Wall, 0,3);
    //Fill Wall Topping
    m_mapFill->FillLayer(MapFill::WallTopping, 3);
    //Add random Items
    m_mapFill->FillLayer(MapFill::AdditionalItems, 1,2,4);

    //Place Stairs
    m_mapFill->PlaceItemAt(1,2,4,MapFill::TileStairsUp,m_map.m_startX, m_map.m_startY);
    m_mapFill->PlaceItemAt(1,2,4,MapFill::TileStairsDown,m_map.m_endX, m_map.m_endY);
    m_events.push_back(new MapEventStairs(false, m_map.m_startX, m_map.m_startY));
    m_events.push_back(new MapEventStairs(true, m_map.m_endX, m_map.m_endY));


    //Place Chests
    PlaceChest();

    m_map.writeToTileMap(*m_tileMap,0);
    m_map.writeToTileMap(*m_tileMapItems,1);
    m_map.writeToTileMap(*m_tileMapAboveHero,2);
    sf::Color halfTransparent(255,255,255,220);
    m_map.writeToTileMap(*m_tileMapAboveWall,3, halfTransparent);
    m_map.writeToTileMap(*m_tileMapWallDecoration,4);


    SpawnEnemy();
}

SceneManagerDungeon::~SceneManagerDungeon()
{
    //dtor
    delete m_mapFill;
}

void SceneManagerDungeon::SpawnEnemy()
{
    std::pair<int, int>* pos = m_generator.GetFreePosition();

    sf::Sprite sprite;
    Texture* tex = TextureList::getTexture(TextureList::EnemySpriteSheet);
    sprite.setTexture(*tex);
    sprite.setTextureRect(sf::IntRect(15,13,32,36));
    Node* enemy = new AnimatedNode(&sprite, tex->GetNumberAnimationSteps());
    enemy->setBoundingBox(sf::FloatRect(8.0f,22.0f,16.0f,16.0f));
    m_eventLayer->addChild(enemy);

    sf::Transform enemyTransform;
    //Place Enemy at Position
    enemyTransform.translate(pos->first * TileMap::GetTileWith(), pos->second * TileMap::GetTileWith() - 14);
    enemy->setTransform(enemyTransform);

    MapEventEnemy* mapEvent = new MapEventEnemy(&m_map, enemy,  256.0f);
    m_events.push_back(mapEvent);
}

void SceneManagerDungeon::PlaceChest()
{
    std::pair<int, int>* pos = m_generator.GetFreePosition();
    std::cout << "Chest at " << pos->first << " " << pos->second << std::endl;
    m_mapFill->PlaceItemAt(1,2,4,MapFill::TileChest,pos->first, pos->second);
    m_events.push_back(new MapEventChest(pos->first, pos->second));
}

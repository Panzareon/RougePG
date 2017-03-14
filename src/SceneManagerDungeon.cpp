#include "SceneManagerDungeon.h"

#include "DrawableNode.h"
#include "AnimatedNode.h"
#include "TileMap.h"
#include "MapFillDungeon.h"
#include "TextureList.h"
#include "MapEventStairs.h"
#include "MapEventEnemy.h"
#include "MapEventHero.h"
#include "MapEventChest.h"
#include "GameController.h"
#include "EnemyFactory.h"
#include "DungeonConfiguration.h"

#include <iostream>


SceneManagerDungeon::SceneManagerDungeon(int tileWidth, int tileHeight, unsigned int seed, int lvlId, DungeonConfiguration* config): SceneManagerMoveable(tileWidth, tileHeight), m_generator(&m_map, seed)
{
    //ctor
    //Define Tile Maps
    SetMemberStats();


    #ifdef DEBUG_FLAG

    m_map.init(6);

    #else

    m_map.init(5);

    #endif // DEBUG_FLAG
    m_dungeonConfig = config;
    m_lvlId = lvlId;
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

    #ifdef DEBUG_FLAG

    TileMap* tileMapRoomNumber = new TileMap();
    tileMapRoomNumber->setTexture(TextureList::getTexture(TextureList::DebugTileMap));
    m_roomNumberNode = new DrawableNode(tileMapRoomNumber);
    m_roomNumberNode->setVisibility(false);
    m_belowHero->addChild(m_roomNumberNode);

    #endif // DEBUG_FLAG

    m_belowHero->addChild(new DrawableNode(m_tileMapItems));

    m_aboveHero->addChild(new DrawableNode(m_tileMapAboveHero));
    m_aboveHero->addChild(new DrawableNode(m_tileMapAboveWall));
    m_aboveHero->addChild(new DrawableNode(m_tileMapWallDecoration));


    //Add Hero
    sf::Sprite hero;
    Texture* tex = TextureList::getTexture(TextureList::HeroSpriteSheet);
    hero.setTexture(*tex);
    hero.setTextureRect(sf::IntRect(0,0,32,36));
    m_hero = new AnimatedNode(&hero, tex->GetNumberAnimationSteps());
    m_hero->setBoundingBox(sf::FloatRect(8.0f,20.0f,16.0f,16.0f));
    m_eventLayer->addChild(m_hero);

    //Generate Map

    if(tileHeight > 200 || tileWidth > 200)
        m_generator.FasterCellularAutomata(0.45f);
    else
        m_generator.CellularAutomata(0.45f);

    m_generator.NumberRooms();


    m_mapFill = new MapFillDungeon(&m_map);
    //Fill Base Layer with walkable Tile
    m_mapFill->FillLayer(MapFill::Ground, 0);
    //Fill Wall
    m_mapFill->FillLayer(MapFill::Wall, 0,3);
    //Fill Wall Topping
    m_mapFill->FillLayer(MapFill::WallTopping, 3);


    #ifdef DEBUG_FLAG

    for(int x = 0; x < m_map.GetWidth(); x++)
    {
        for(int y = 0; y < m_map.GetHeight(); y++)
            m_map.SetTileId(x,y,m_map.GetRoomNr(x,y), 5);
    }

    m_map.writeToTileMap(*tileMapRoomNumber,5);

    #endif // DEBUG_FLAG

    m_generator.PlaceStartingAndEndPosition();

    sf::Transform heroTransform;
    //Place Hero at Startposition
    heroTransform.translate(m_map.m_startX * TileMap::GetTileWith(), m_map.m_startY * TileMap::GetTileWith() - 14);
    m_hero->setTransform(heroTransform);
    UpdateCamPosition();

    //Add random Items
    m_mapFill->FillLayer(MapFill::AdditionalItems, 1,2,4);

    //Place Stairs
    m_mapFill->PlaceItemAt(1,2,4,MapFill::TileStairsUp,m_map.m_startX, m_map.m_startY, false);
    m_mapFill->PlaceItemAt(1,2,4,MapFill::TileStairsDown,m_map.m_endX, m_map.m_endY, false);
    m_events.push_back(new MapEventStairs(false, m_map.m_startX, m_map.m_startY));
    m_events.push_back(new MapEventStairs(true, m_map.m_endX, m_map.m_endY));


    //Place Boss at Stairs
    sf::Sprite sprite;
    tex = TextureList::getTexture(TextureList::BossSpriteSheet);
    sprite.setTexture(*tex);
    sprite.setTextureRect(sf::IntRect(0,0,32,32));
    Node* enemy = new AnimatedNode(&sprite, tex->GetNumberAnimationSteps());
    enemy->setBoundingBox(sf::FloatRect(0.0f,0.0f,32.0f,32.0f));
    m_eventLayer->addChild(enemy);

    sf::Transform enemyTransform;
    //Place Enemy at Position
    enemyTransform.translate(m_map.m_endX * TileMap::GetTileWith(), m_map.m_endY * TileMap::GetTileWith());
    enemy->setTransform(enemyTransform);

    std::vector<Entity*>* enemies = new std::vector<Entity*>();
    //TODO: get Entities of this Map from somewhere else
    Entity* e = m_dungeonConfig->GetDungeonEnemy(m_lvlId);
    e->SetTeamId(1);
    enemies->push_back(e);
    e = m_dungeonConfig->GetDungeonBoss(m_lvlId);
    e->SetTeamId(1);
    enemies->push_back(e);
    e = m_dungeonConfig->GetDungeonEnemy(m_lvlId);
    e->SetTeamId(1);
    enemies->push_back(e);


    MapEventEnemy* mapEvent = new MapEventEnemy(&m_map, enemy,  0.0f, enemies);
   m_events.push_back(mapEvent);



    //Place Chests
    PlaceChest();

    //Place additional heroes
    PlaceHero();

    m_map.writeToTileMap(*m_tileMap,0);
    m_map.writeToTileMap(*m_tileMapItems,1);
    m_map.writeToTileMap(*m_tileMapAboveHero,2);
    sf::Color halfTransparent(255,255,255,Configuration::GetInstance()->GetWallTransparency() * 255);
    m_map.writeToTileMap(*m_tileMapAboveWall,3, halfTransparent);
    m_map.writeToTileMap(*m_tileMapWallDecoration,4);


    for(int i = 0; i < 10; i++)
    {
        SpawnEnemy();
    }
}

SceneManagerDungeon::~SceneManagerDungeon()
{
    //dtor
    delete m_mapFill;
}

void SceneManagerDungeon::Tick()
{
    m_timeToNextSpawn -= GameController::getInstance()->GetTickTimeSeconds();
    if(m_timeToNextSpawn <= 0.0f)
    {
        SpawnEnemy();
    }
    SceneManagerMoveable::Tick();

    #ifdef DEBUG_FLAG

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::X))
    {
        m_roomNumberNode->setVisibility(true);
    }
    else
    {
        m_roomNumberNode->setVisibility(false);
    }

    #endif // DEBUG_FLAG
}

void SceneManagerDungeon::SpawnEnemy()
{
    std::cout << "Spawn Enemy" << std::endl;
    sf::FloatRect heroPos = m_hero->getGlobalBoundingBox();
    float x = heroPos.left + heroPos.width/2;
    x /= TileMap::GetTileWith();
    float y = heroPos.top + heroPos.height/2;
    y /= TileMap::GetTileWith();
    int playerRoomNumber = m_map.GetRoomNr(x,y);
    std::pair<int, int>* pos;
    int nrTries = 0;
    //Try 10 Times to find a Position not in the same room as the Hero
    do
    {
        pos = m_generator.GetFreePosition();
    }
    while(m_map.GetRoomNr(pos->first, pos->second) == playerRoomNumber && nrTries < 10);

    SpawnEnemy(pos->first, pos->second, m_lvlId, 110.0f, 220.0f, 4, 2);

    m_timeToNextSpawn = rand()%10 + 10;
}

void SceneManagerDungeon::SpawnEnemy(int x, int y, int lvl, float movementSpeed, float followSpeed, int followRange, int nr)
{
    sf::Sprite sprite;
    Texture* tex = TextureList::getTexture(TextureList::EnemySpriteSheet);
    sprite.setTexture(*tex);
    sprite.setTextureRect(sf::IntRect(0,0,32,32));
    Node* enemy = new AnimatedNode(&sprite, tex->GetNumberAnimationSteps());
    enemy->setBoundingBox(sf::FloatRect(4.0f,16.0f,20.0f,16.0f));
    m_eventLayer->addChild(enemy);

    sf::Transform enemyTransform;
    //Place Enemy at Position
    enemyTransform.translate(x * TileMap::GetTileWith(), y * TileMap::GetTileWith() - 14);
    enemy->setTransform(enemyTransform);

    std::vector<Entity*>* enemies = new std::vector<Entity*>();
    //TODO: get Entities of this Map from somewhere else
    Entity* e;
    for(int i = 0; i < nr; i++)
    {
        e = m_dungeonConfig->GetDungeonEnemy(lvl);
        e->SetTeamId(1);
        enemies->push_back(e);
    }


    MapEventEnemy* mapEvent = new MapEventEnemy(&m_map, enemy,  movementSpeed, enemies);
    m_events.push_back(mapEvent);
    mapEvent->FollowPlayer(true, followRange, followSpeed);
}

void SceneManagerDungeon::PlaceChest()
{
    std::pair<int, int>* pos;
    //Add a maximum number of tries to prevent endless loop
    //For the first 100 try to find a dead end to put the chest into
    int nrTries = 0;
    bool placed;
    do
    {
        nrTries++;
        pos = m_generator.GetFreePosition(nrTries < 100);
        int roomNr = m_map.GetRoomNr(pos->first, pos->second);
        if((roomNr == m_map.m_endRoomNr || roomNr == m_map.m_startRoomNr) && nrTries < 200)
        {
            //Do not place chest in start or end Room
            continue;
        }
        placed = m_mapFill->PlaceItemAt(1,2,4,MapFill::TileChest,pos->first, pos->second);
    }
    while(!placed && nrTries < 200);
    std::cout << "Chest at " << pos->first << " " << pos->second << " nr tries:" << nrTries << " placeable: " << placed << std::endl;
    m_events.push_back(new MapEventChest(pos->first, pos->second));

    //look for adjacent free tile
    if(!m_map.DoesCollide(pos->first + 1, pos->second))
    {
        pos->first +=1;
    }
    else if(!m_map.DoesCollide(pos->first - 1, pos->second))
    {
        pos->first -=1;
    }
    else if(!m_map.DoesCollide(pos->first, pos->second + 1))
    {
        pos->second +=1;
    }
    else if(!m_map.DoesCollide(pos->first, pos->second - 1))
    {
        pos->second -=1;
    }

    SpawnEnemy(pos->first,pos->second,m_lvlId + 2, 0.0f, 256.0f,2,3);
}

void SceneManagerDungeon::PlaceHero()
{
    std::pair<int, int>* pos;
    //Add a maximum number of tries to prevent endless loop
    //For the first 100 try to find a dead end to put the hero into
    int nrTries = 0;
    int roomNr;
    do
    {
        nrTries++;
        pos = m_generator.GetFreePosition(nrTries < 100);
        roomNr = m_map.GetRoomNr(pos->first, pos->second);
    }
    while((roomNr == m_map.m_endRoomNr || roomNr == m_map.m_startRoomNr) && nrTries < 200);
    std::cout << "Hero at " << pos->first << " " << pos->second << " nr tries:" << nrTries << std::endl;

    //Display player sprite
    sf::Sprite* sprite = new sf::Sprite();
    Texture* tex = TextureList::getTexture(TextureList::HeroSpriteSheet);
    sprite->setTexture(*tex);
    sprite->setTextureRect(sf::IntRect(0,0,32,32));
    Node* hero = new DrawableNode(sprite);
    m_eventLayer->addChild(hero);

    sf::Transform heroTransform;
    //Place Enemy at Position
    heroTransform.translate(pos->first * TileMap::GetTileWith(), pos->second * TileMap::GetTileWith() - 14);
    hero->setTransform(heroTransform);


    m_events.push_back(new MapEventHero(hero, pos->first, pos->second));
}

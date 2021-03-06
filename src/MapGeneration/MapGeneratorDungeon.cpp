#include "MapGeneration/MapGeneratorDungeon.h"
#include "Exception/GenericException.h"
#include "MapGeneration/MapFillDungeon.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <cmath>

MapGeneratorDungeon::MapGeneratorDungeon()
{

}


MapGeneratorDungeon::MapGeneratorDungeon(Map* map, unsigned int seed)
{
    //ctor
    Init(map,seed);
}

MapGeneratorDungeon::~MapGeneratorDungeon()
{
    //dtor
}

void MapGeneratorDungeon::Init(Map* map, unsigned int seed)
{
    m_width = map->GetWidth();
    m_height = map->GetHeight();
    m_MGUtil.SetSize(m_width, m_height);
    m_map = map;
    m_map->AddCollidingType(MapFillDungeon::Wall);
    m_map->AddCollidingType(MapFillDungeon::BlockingItem);

    if(seed == 0)
        std::srand((unsigned int)std::time(0));
    else
        std::srand(seed);
}

void MapGeneratorDungeon::CellularAutomata(float startPercent)
{
    std::cout << "Starting Generation" << std::endl;
    CellularAutomataStart(startPercent);
    for(int i = 0; i < 5; i++)
    {
        CellularAutomataStep(5, 1, 0.67f);
    }
    std::cout << "Connecting Rooms" << std::endl;
    ConnectAllRooms(true);
    for(int i = 0; i < 3; i++)
    {
        CellularAutomataStep(5);
        ConnectAllRooms(false);
    }

          //AlternativeImplementation
/*            generator.CellularAutomataStart(0.45f);
            for(int i = 0; i < 5; i++)
            {
                generator.CellularAutomataStep(5,1,0.67f);
            }
            for(int i = 0; i < 3; i++)
            {
                generator.CellularAutomataStep(5);
            }

            generator.ConnectTwoPoints(5,5,m_width - 5,m_height - 5);

            generator.RemoveUnconnectedRooms();*/
}

void MapGeneratorDungeon::FasterCellularAutomata(float startPercent)
{
    std::cout << "Starting Generation" << std::endl;
    CellularAutomataStart(startPercent);
    for(int i = 0; i < 8; i++)
    {
        CellularAutomataStep(5, 1, 0.75f);
    }
    MorphologicalCloseOperator();
    CellularAutomataStep(6);
    //Remove too small rooms
    CellularAutomataStep(8,-1,1.0, true);
    ConnectAllRooms(true);
}


void MapGeneratorDungeon::CellularAutomataStart(float startPercent)
{
    for(int i = 0; i < m_width; i++)
    {
        for(int j = 0; j < m_height; j++)
        {
            if(std::rand()/((float) RAND_MAX) < startPercent)
                m_map->SetTileToType(i,j,MapFillDungeon::Wall);
            else
                m_map->SetTileToType(i,j,MapFillDungeon::Space);
        }
    }
}

void MapGeneratorDungeon::CellularAutomataStep(int minWallTiles, int orMaxWallTiles, float chanceAtThreshhold, bool onlyChangeToWall)
{
    //new Array for the new map

    int** newTiles = new int*[m_width];
    for(int i = 0; i < m_width; i++)
        newTiles[i] = new int[m_height];


    bool setWall;
    int newNrWall=0;
    int newNrSpace=0;
    int nrWall;
    for(int i = 0; i < m_width; i++)
    {
        for(int j = 0; j < m_height; j++)
        {
            nrWall = NumberOfAdjacentWalls(i,j);

            if((onlyChangeToWall && m_map->DoesCollide(i,j)) || nrWall > minWallTiles || nrWall < orMaxWallTiles)
            {
                setWall = true;
            }
            else if(nrWall == minWallTiles || nrWall == orMaxWallTiles)
            {
                if(chanceAtThreshhold >= 1.0f)
                {
                    setWall = true;
                }
                else if(chanceAtThreshhold <= 0.0f)
                {
                    setWall = false;
                }
                else
                {
                    setWall = std::rand()/((float) RAND_MAX) < chanceAtThreshhold;
                }
            }
            else
            {
                setWall = false;
            }
            if(setWall)
            {
                newTiles[i][j] = 0;
                newNrWall++;
            }
            else
            {
                newTiles[i][j] = 1;
                newNrSpace++;
            }
        }
    }
    for(int i = 0; i < m_width; i++)
        for(int j = 0; j < m_height; j++)
        {
            if(newTiles[i][j] == 0)
                m_map->SetTileToType(i,j,MapFillDungeon::Wall);
            else
                m_map->SetTileToType(i,j,MapFillDungeon::Space);
        }
}

int MapGeneratorDungeon::NumberOfAdjacentWalls(int i, int j)
{
    int nrWall = 0;
    for(int x = i - 1; x <= i + 1; x ++)
    {
        for(int y = j - 1; y <= j + 1; y++)
        {
            if(x < 0 || y < 0 || x >= m_width || y >= m_height)
            {
                nrWall++;
            }
            else if(m_map->DoesCollide(x,y))
            {
                nrWall++;
            }
        }
    }
    return nrWall;
}


void MapGeneratorDungeon::ConnectedRoomsRectagle(int roomSizeX, int roomSizeY, int nrRooms, int randomDifference)
{
    int width, height;
    for(int i = 0; i < nrRooms; i++)
    {
        width = roomSizeX + rand()%randomDifference - randomDifference/2;
        height = roomSizeY + rand()%randomDifference - randomDifference/2;
        int x,y;
        int nrTries = 0;
        do
        {
            x = rand() % (m_width - width);
            y = rand() % (m_height - height);
            nrTries++;
        }
        while (!IsRoomFree(x,y,width, height) && nrTries < 100);
        if(nrTries >= 100)
            break;
        AddRectangleRoom(x,y,width, height);
    }
    ConnectAllRooms(true);
}

void MapGeneratorDungeon::AddRectangleRoom(int x, int y, int width, int height)
{
    for(int i = x; i < x + width; i++)
    {
        for(int j = y; j < y + height; j++)
        {
            m_map->SetTileToType(i,j,MapFillDungeon::Space);
        }
    }
}

bool MapGeneratorDungeon::IsRoomFree(int x, int y, int width, int height)
{
    for(int i = x - 1; i < x + width + 1; i++)
    {
        for(int j = y - 1; j < y + height + 1; j++)
        {
            if(i >= 0 && i < m_width && j >= 0 && j < m_height && !m_map->DoesCollide(i,j))
                return false;
        }
    }
    return true;
}

void MapGeneratorDungeon::ConnectedRooms(int roomSizeX, int roomSizeY, int nrRooms, int randomDifference)
{
    int width, height;
    for(int i = 0; i < nrRooms; i++)
    {
        width = roomSizeX + rand()%randomDifference - randomDifference/2;
        height = roomSizeY + rand()%randomDifference - randomDifference/2;
        int x,y;
        int nrTries = 0;
        do
        {
            x = rand() % (m_width - width);
            y = rand() % (m_height - height);
            nrTries++;
        }
        while (!IsRoomFree(x,y,width, height) && nrTries < 100);
        if(nrTries >= 100)
            break;
        AddRoom(x,y,width, height);
    }
    ConnectAllRooms(true, 10, true);
}

void MapGeneratorDungeon::AddRoom(int x, int y, int width, int height)
{
    AddRectangleRoom(x,y,width,height);

    if (rand() % 10 > 0)
    {
        m_map->SetTileToType(x,y,MapFillDungeon::Wall);
    }
    if (rand() % 10 > 0)
    {
        m_map->SetTileToType(x+width-1,y,MapFillDungeon::Wall);
    }
    if (rand() % 10 > 0)
    {
        m_map->SetTileToType(x+width-1,y+height-1,MapFillDungeon::Wall);
    }
    if (rand() % 10 > 0)
    {
        m_map->SetTileToType(x,y+height-1,MapFillDungeon::Wall);
    }
    for(int i = x +1; i < x + width - 1; i++)
    {
        SetIrregularTile(i,y,true);
        SetIrregularTile(i,y+height-1,true);
    }
    for(int j = y +1; j < y + height - 1; j++)
    {
        SetIrregularTile(x,j,false);
        SetIrregularTile(x+width-1,j,false);
    }
    RemoveNotConnectedSpaceFromRoom(x,y,width,height);
}

void MapGeneratorDungeon::SetIrregularTile(int x, int y, bool horizontal)
{
    if (horizontal)
    {
        if ((rand()%10 < 5 ||
             m_map->DoesCollide(x-1,y) && NumberOfAdjacentWalls(x-1,y) == 4) &&
            (m_map->DoesCollide(x-1,y) || NumberOfAdjacentWalls(x-1,y) != 4))
        {
            m_map->SetTileToType(x,y,MapFillDungeon::Wall);
        }
    }
    else
    {
        if ((rand()%10 < 5 ||
             m_map->DoesCollide(x,y-1) && NumberOfAdjacentWalls(x,y-1) == 4) &&
            (m_map->DoesCollide(x,y-1) || NumberOfAdjacentWalls(x,y-1) != 4))
        {
            m_map->SetTileToType(x,y,MapFillDungeon::Wall);
        }
    }
}

void MapGeneratorDungeon::MorphologicalCloseOperator()
{
    CellularAutomataStep(9);
    CellularAutomataStep(1);
}

void MapGeneratorDungeon::RemoveNotConnectedSpaceFromRoom(int x, int y, int width, int height, int setToValue)
{
    //flags for every Tile: 0 = not checked; 1 = checked; 2 = wall
    int** checkedTiles = new int*[width];
    for(int i = 0; i < width; i++)
    {
        checkedTiles[i] = new int[height];
        for(int j = 0; j < height; j++)
        {
            if(m_map->DoesCollide(i+x,j+y))
                checkedTiles[i][j] = 2;
            else
                checkedTiles[i][j] = 0;
        }
    }

    MapGeneratorUtil util;
    util.SetSize(width,height);
    util.SetTilesToChecked(checkedTiles, width/2, height/2);

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            if(checkedTiles[i,j] == 0)
            {
                m_map->SetTileToType(i+x, j+y, setToValue);
            }
        }
    }
    for(int i = 0; i < width; i++)
        delete[] checkedTiles[i];
    delete[] checkedTiles;
}


void MapGeneratorDungeon::ConnectAllRooms(bool straight, int maxRemovedTiles, bool withClosest)
{
    std::cout << "Start connecting all" << std::endl;
    sf::Clock clock;
    //flags for every Tile: 0 = not checked; 1 = checked; 2 = wall
    int** checkedTiles = new int*[m_width];
    for(int i = 0; i < m_width; i++)
    {
        checkedTiles[i] = new int[m_height];
        for(int j = 0; j < m_height; j++)
        {
            if(m_map->DoesCollide(i,j))
                checkedTiles[i][j] = 2;
            else
                checkedTiles[i][j] = 0;
        }
    }
    bool finished = false;
    //To have most of the Rooms in the starting list: Start with at least 1/4 of all Tiles must be checked
    int mod = 4;
    do
    {
        int checkX = std::rand() % m_width;
        int checkY = std::rand() % m_height;

        if(checkedTiles[checkX][checkY] == 0)
        {
            if(m_MGUtil.GetNumberOfConnected(checkedTiles, checkX, checkY, 0, m_width * m_height / mod) < m_width * m_height / mod)
            {
                mod++;
            }
            else
            {
                m_MGUtil.SetTilesToChecked(checkedTiles, checkX, checkY);
                finished = true;
            }
        }

    }
    while(!finished);

    int remainingTiles;
    int checkNr = 0;
    int combinedCheckNr = 0;

    std::cout << "Check if all connected: " << straight << ";" << maxRemovedTiles << ";" << withClosest << std::endl;
    do
    {
        checkNr++;
        combinedCheckNr++;
        int checkX = std::rand() % m_width;
        int checkY = std::rand() % m_height;

        if(checkNr > 30)
        {
            for(int x = 0; x < m_width; x++)
                for(int y = 0; y < m_height; y++)
                    if(checkedTiles[x][y] == 0 && rand() % remainingTiles == 0)
                    {
                        checkX = x;
                        checkY = y;
                    }
        }

        if(checkedTiles[checkX][checkY] == 0)
        {
            CheckTiles(checkedTiles, checkX, checkY, straight, withClosest);
            checkNr = 0;
        }

        finished = true;
        remainingTiles = 0;
        for(int i = 0; i < m_width; i++)
        {
            for(int j = 0; j < m_height; j++)
            {
                if(checkedTiles[i][j] == 0)
                    remainingTiles++;
                if(remainingTiles > maxRemovedTiles)
                    finished = false;
            }
        }
    }
    while(!finished && combinedCheckNr < 10000);

    for(int i = 0; i < m_width; i++)
    {
        for(int j = 0; j < m_height; j++)
        {
            if(checkedTiles[i][j] == 0)
                m_map->SetTileToType(i,j,MapFillDungeon::Wall);
        }
    }

    std::cout << "Finished connecting all, took " << clock.getElapsedTime().asSeconds() << "s" << std::endl;

    for(int i = 0; i < m_width; i++)
        delete[] checkedTiles[i];
    delete[] checkedTiles;
}

void MapGeneratorDungeon::RemoveUnconnectedRooms()
{

    int** checkedTiles = new int*[m_width];
    for(int i = 0; i < m_width; i++)
    {
        checkedTiles[i] = new int[m_height];
        for(int j = 0; j < m_height; j++)
        {
            if(m_map->DoesCollide(i,j))
                checkedTiles[i][j] = 2;
            else
                checkedTiles[i][j] = 0;
        }
    }
    bool finished = false;
    //To have most of the Rooms in the starting list: Start with at least 1/4 of all Tiles must be checked
    int mod = 4;
    do
    {
        int checkX = std::rand() % m_width;
        int checkY = std::rand() % m_height;

        if(checkedTiles[checkX][checkY] == 0)
        {
            if(m_MGUtil.SetTilesToChecked(checkedTiles, checkX, checkY) < m_width * m_height / mod)
            {
                for(int i = 0; i < m_width; i++)
                {
                    for(int j = 0; j < m_height; j++)
                    {
                        if(m_map->DoesCollide(i,j))
                            checkedTiles[i][j] = 2;
                        else
                            checkedTiles[i][j] = 0;
                    }
                }
                mod++;
            }
            else
            {
                finished = true;
            }
        }

    }
    while(!finished);

    for(int i = 0; i < m_width; i++)
        for(int j = 0; j < m_height; j++)
        {
            if(checkedTiles[i][j] == 0)
                m_map->SetTileToType(i,j,MapFillDungeon::Wall);
        }


    for(int i = 0; i < m_width; i++)
        delete[] checkedTiles[i];
    delete[] checkedTiles;
}


void MapGeneratorDungeon::ConnectTwoPoints(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
    m_map->SetTileToType(x1,y1,MapFillDungeon::Space);
    m_map->SetTileToType(x2,y2,MapFillDungeon::Space);
    //flags for every Tile: 0 = not checked; 1 = checkedStartPoint; 2 = wall; 3 = checkedEndPoint
    int** checkedTiles = new int*[m_width];
    for(int i = 0; i < m_width; i++)
    {
        checkedTiles[i] = new int[m_height];
        for(int j = 0; j < m_height; j++)
        {
            if(m_map->DoesCollide(i,j))
                checkedTiles[i][j] = 2;
            else
                checkedTiles[i][j] = 0;
        }
    }

    m_MGUtil.SetTilesToChecked(checkedTiles, x1, y1);
    //Check if already connected
    if(checkedTiles[x2][y2] == 0)
    {
        m_MGUtil.SetTilesToChecked(checkedTiles, x2, y2, 0, 3);

        bool finished = false;
        do
        {
            sf::Vector2u closestFrom1 = m_MGUtil.GetClosestToPoint(checkedTiles,3, x1,y1);
            sf::Vector2u closestFrom2 = m_MGUtil.GetClosestToPoint(checkedTiles,1, closestFrom1.x, closestFrom1.y);
            sf::Vector2u newMidpoint = m_MGUtil.GetClosestToPoint(checkedTiles, 0, closestFrom1.x, closestFrom1.y);

            if (closestFrom1 == sf::Vector2u() || closestFrom2 == sf::Vector2u())
            {
                break;
            }

            float distance = sqrtf((closestFrom1.x - closestFrom2.x) * (closestFrom1.x - closestFrom2.x) + (closestFrom1.y - closestFrom2.y) * (closestFrom1.y - closestFrom2.y));
            float distance2 = sqrtf((closestFrom1.x - newMidpoint.x) * (closestFrom1.x - newMidpoint.x) + (closestFrom1.y - newMidpoint.y) * (closestFrom1.y - newMidpoint.y));
            std::cout << distance << " vs. " << distance2 << std::endl;

            if((newMidpoint.x == 0 && newMidpoint.y == 0) || distance < distance2)
            {
                DigTunnel(closestFrom1.x, closestFrom1.y, closestFrom2.x, closestFrom2.y);
                finished = true;
            }
            else
            {
                DigTunnel(closestFrom1.x, closestFrom1.y, newMidpoint.x, newMidpoint.y);


                for(int i = 0; i < m_width; i++)
                {
                    for(int j = 0; j < m_height; j++)
                    {
                        if(m_map->DoesCollide(i,j))
                            checkedTiles[i][j] = 2;
                        else
                            checkedTiles[i][j] = 0;
                    }
                }
                m_MGUtil.SetTilesToChecked(checkedTiles, x1, y1);
                //should not happen....
                if(checkedTiles[x2][y2] == 1)
                    finished = true;
                m_MGUtil.SetTilesToChecked(checkedTiles, x2, y2, 0, 3);
            }
        }
        while (!finished);
    }


    for(int i = 0; i < m_width; i++)
        delete[] checkedTiles[i];
    delete[] checkedTiles;
}
void MapGeneratorDungeon::NumberRooms()
{
    //flags for every Tile: 0 = not checked; 1 = wall; 2 = outer line of Room; 3 or greater Room Nr
    int** checkedTiles = new int*[m_width];
    for(int i = 0; i < m_width; i++)
    {
        checkedTiles[i] = new int[m_height];
        for(int j = 0; j < m_height; j++)
        {
            if(m_map->DoesCollide(i,j))
                checkedTiles[i][j] = 1;
            else
                checkedTiles[i][j] = 0;
        }
    }

    int nrWall;
    int roomId = 3;
    for(int d = 5; d >= 0; d--)
    {
        for(int i = 0; i < m_width; i++)
        {
            for(int j = 0; j < m_height; j++)
            {
                if(!m_map->DoesCollide(i,j))
                {
                    nrWall = 0;
                    for(int x = i - d; x <= i + d; x ++)
                    {
                        for(int y = j - d; y <= j + d; y++)
                        {
                            if(x < 0 || y < 0 || x >= m_width || y >= m_height)
                            {
                                nrWall++;
                            }
                            else if(m_map->DoesCollide(x,y))
                            {
                                nrWall++;
                            }
                        }
                    }
                    if(nrWall >= 1)
                    {
                        checkedTiles[i][j] = 2;
                    }
                }
            }
        }


        int** lastTiles = new int*[m_width];
        for(int i = 0; i < m_width; i++)
        {
            lastTiles[i] = new int[m_height];
        }
        bool finished = false;
        while(!finished)
        {
            finished = true;
            for(int i = 0; i < m_width; i++)
            {
                for(int j = 0; j < m_height; j++)
                {
                    lastTiles[i][j] = checkedTiles[i][j];
                }
            }
            for(int i = 0; i < m_width; i++)
            {
                for(int j = 0; j < m_height; j++)
                {
                    if(checkedTiles[i][j] == 0)
                    {
                        int room = 0;
                        int room1 = 0;
                        int room1nr = 0;
                        int room2 = 0;
                        int room2nr = 0;
                        for(int x = i - 1; x <= i + 1; x ++)
                        {
                            for(int y = j - 1; y <= j + 1; y++)
                            {
                                if(x >= 0 && y >= 0 && x < m_width && y < m_height)
                                {
                                    room = lastTiles[x][y];
                                    if(room > 2)
                                    {
                                        if(room1 == 0 || room1 == room)
                                        {
                                            room1 = room;
                                            room1nr++;
                                        }
                                        else if(room2 == 0 || room2 == room)
                                        {
                                            room2 = room;
                                            room2nr++;
                                        }
                                    }
                                }
                            }
                        }
                        if(room1 != 0)
                        {
                            finished = false;
                            if(room1nr > room2nr)
                            {
                                checkedTiles[i][j] = room1;
                            }
                            else
                            {
                                checkedTiles[i][j] = room2;
                            }
                        }
                    }
                }
            }
        }
        for(int i = 0; i < m_width; i++)
            delete[] lastTiles[i];
        delete[] lastTiles;




        int checkX = 0;
        int checkY = 0;
        finished = false;
        do
        {

            for(int i = 0; i < m_width; i++)
            {
                for(int j = 0; j < m_height; j++)
                {
                    if(checkedTiles[i][j] == 0)
                    {
                        checkX = i;
                        checkY = j;
                        i = m_width;
                        j = m_height;
                    }
                }
            }

            if(checkedTiles[checkX][checkY] == 0)
            {
                int room = 0;
                int room1 = 0;
                int room1nr = 0;
                int room2 = 0;
                int room2nr = 0;
                for(int x = checkX - 1; x <= checkX + 1; x ++)
                {
                    for(int y = checkY - 1; y <= checkY + 1; y++)
                    {
                        if(x >= 0 && y >= 0 && x < m_width && y < m_height)
                        {
                            room = checkedTiles[x][y];
                            if(room > 2)
                            {
                                if(room1 == 0 || room1 == room)
                                {
                                    room1 = room;
                                    room1nr++;
                                }
                                else if(room2 == 0 || room2 == room)
                                {
                                    room2 = room;
                                    room2nr++;
                                }
                            }
                        }
                    }
                }
                if(room1 != 0)
                {
                    finished = false;
                    if(room1nr > room2nr)
                    {
                        checkedTiles[checkX][checkY] = room1;
                    }
                    else
                    {
                        checkedTiles[checkX][checkY] = room2;
                    }
                }
                else
                {
                    m_MGUtil.SetTilesToChecked(checkedTiles, checkX, checkY, 0, roomId);
                    roomId++;
                }
            }
            finished = true;
            for(int i = 0; i < m_width && finished; i++)
            {
                for(int j = 0; j < m_height && finished; j++)
                {
                    if(checkedTiles[i][j] == 0)
                    {
                        finished = false;
                    }
                }
            }

        }
        while(!finished);




        for(int i = 0; i < m_width && finished; i++)
        {
            for(int j = 0; j < m_height && finished; j++)
            {
                if(checkedTiles[i][j] == 2)
                {
                    checkedTiles[i][j] = 0;
                }
            }
        }




    }

    //Setting of the rooms
    for(int i = 0; i < m_width; i++)
        for(int j = 0; j < m_height; j++)
        {
            if(checkedTiles[i][j] > 2)
            {
                m_map->SetRoomNr(i,j, checkedTiles[i][j] - 2);
            }
        }

    //Set adjacent
    int xAdd, yAdd, roomAdjacentNumber;
    for(int i = 0; i < m_width; i++)
        for(int j = 0; j < m_height; j++)
        {
            if(checkedTiles[i][j] > 2)
            {
                for(int k = 0; k < 2; k++)
                {
                    xAdd = 0;
                    yAdd = 0;
                    switch(k)
                    {
                    case 0:
                        xAdd = 1;
                        break;
                    case 1:
                        yAdd = 1;
                        break;
                    }
                    if(i + xAdd >= 0 && i + xAdd < m_width && j + yAdd >= 0 && j + yAdd < m_height)
                    {
                        roomAdjacentNumber = checkedTiles[i + xAdd][j + yAdd];
                        if(roomAdjacentNumber > 2)
                        {
                            if(roomAdjacentNumber != checkedTiles[i][j])
                            {
                                m_map->RoomsAreAdjacent(checkedTiles[i][j] - 2, roomAdjacentNumber - 2);
                            }
                        }
                    }

                }
            }
        }


    for(int i = 0; i < m_width; i++)
        delete[] checkedTiles[i];
    delete[] checkedTiles;
}

void MapGeneratorDungeon::DigTunnel(int fromX, int fromY, int toX, int toY)
{
    if(abs(fromX - toX) > abs(fromY - toY))
    {
        if(fromX > toX)
        {
            int x = toX;
            int y = toY;
            toX = fromX;
            toY = fromY;
            fromX = x;
            fromY = y;
        }

        int yStep = 1;
        if(fromY > toY)
            yStep = -1;

        int dX = abs(toX - fromX);
        int dY = abs(toY - fromY);
        int error = dX / 2;
        int y = fromY;
        for(int x = fromX; x <= toX; x++)
        {
            m_map->SetTileToType(x,y,MapFillDungeon::Space);
            error -= dY;
            if(error < 0)
            {
                error += dX;
                y += yStep;
                m_map->SetTileToType(x,y,MapFillDungeon::Space);
            }
        }
    }
    else
    {
        if(fromY > toY)
        {
            int x = toX;
            int y = toY;
            toX = fromX;
            toY = fromY;
            fromX = x;
            fromY = y;
        }

        int xStep = 1;
        if(fromX > toX)
            xStep = -1;

        int dX = abs(toX - fromX);
        int dY = abs(toY - fromY);
        int error = dY / 2;
        int x = fromX;
        for(int y = fromY; y <= toY; y++)
        {
            m_map->SetTileToType(x,y,MapFillDungeon::Space);
            error -= dX;
            if(error < 0)
            {
                error += dY;
                x += xStep;
                m_map->SetTileToType(x,y,MapFillDungeon::Space);
            }
        }
    }
}


void MapGeneratorDungeon::CheckTiles(int** checkArray, int x, int y, bool straight, bool withClosest)
{
    if (withClosest)
    {
        sf::Vector2u startPoint = sf::Vector2u(x,y);
        sf::Vector2u endPoint;
        for(int i = 0; i < 4; i++)
        {
            endPoint = m_MGUtil.GetClosestToPoint(checkArray, 0, startPoint.x,startPoint.y);
            startPoint = m_MGUtil.GetClosestToPoint(checkArray, 1, endPoint.x,endPoint.y);
            if(endPoint == sf::Vector2u() || startPoint == sf::Vector2u())
            {
                return;
            }
        }

        if (checkArray[startPoint.x][startPoint.y] == 1)
        {
            ConnectTwoPoints(startPoint.x, startPoint.y, endPoint.x, endPoint.y);
            for(int i = 0; i < m_width; i++)
            {
                for(int j = 0; j < m_height; j++)
                {
                    if(m_map->DoesCollide(i,j))
                        checkArray[i][j] = 2;
                    else
                        checkArray[i][j] = 0;
                }
            }
            m_MGUtil.SetTilesToChecked(checkArray,startPoint.x,startPoint.y, 0, 1, true);
        }
    }
    else
    {
        int direction = std::rand() % 4;

        int xChange = 0;
        int yChange = 0;
        if(direction == 0)
        {
            yChange = -1;
        }
        else if(direction == 1)
        {
            yChange = 1;
        }
        else if(direction == 2)
        {
            xChange = -1;
        }
        else
        {
            xChange = 1;
        }

        //go as long as unchecked Space
        while(x > 0 && y > 0 && x < m_width - 1 && y < m_height - 1 && checkArray[x][y] == 0)
        {
            x += xChange;
            y += yChange;
        }
        if(x > 0 && y > 0 && x < m_width - 1 && y < m_height - 1 && checkArray[x][y] != 0)
        {
            if(straight)
            {
                //Go as long as there is a Wall
                while(x > 0 && y > 0 && x < m_width - 1 && y < m_height - 1 && checkArray[x][y] == 2)
                {
                    x += xChange;
                    y += yChange;
                }
                //if after Wall is checked Space go back to the start and set to space
                if(x > 0 && y > 0 && x < m_width - 1 && y < m_height - 1 && checkArray[x][y] == 1)
                {
                    do
                    {
                        x -= xChange;
                        y -= yChange;
                        checkArray[x][y] = 0;
                        m_map->SetTileToType(x,y,MapFillDungeon::Space);
                    }
                    while(x > 0 && y > 0 && x < m_width - 1 && y < m_height - 1 && checkArray[x - xChange][y - yChange] != 0);
                    m_MGUtil.SetTilesToChecked(checkArray,x,y);
                }
            }
            else
            {
                checkArray[x][y] = 0;
                m_map->SetTileToType(x,y,MapFillDungeon::Space);
                if(x > 0 && y > 0 && x < m_width - 1 && y < m_height - 1 )
                    if(checkArray[x-1][y] == 1 || checkArray[x+1][y] == 1 || checkArray[x][y-1] == 1 || checkArray[x][y+1] == 1)
                        m_MGUtil.SetTilesToChecked(checkArray,x,y);
            }
        }
    }
}

void MapGeneratorDungeon::PlaceStartingAndEndPosition()
{
    int nr = 0;
    std::map<int,MapRoom>* rooms = m_map->GetAllRooms();
    MapRoom* startRoom = nullptr;
    MapRoom* endRoom = nullptr;
    int startNr;
    int endNr;
    if(rooms->size() >= 2)
    {

        for(auto it = rooms->begin(); it != rooms->end(); it++)
        {
            if(it->second.GetNumberOfAdjacentRooms() == 1)
            {
                nr++;
            }
        }

        size_t maxNr = nr;
        //is there a place for starting and end Position
        if(nr < 2)
        {
            maxNr = rooms->size();
        }
        startNr = rand() % maxNr;
        endNr = rand() % (maxNr - 1);
        if(endNr >= startNr)
            endNr++;
        for(auto it = rooms->begin(); it != rooms->end(); it++)
        {
            if(nr < 2 || it->second.GetNumberOfAdjacentRooms() == 1)
            {
                if(startNr == 0)
                    startRoom = &it->second;
                startNr--;
                if(endNr == 0)
                    endRoom = &it->second;
                endNr--;
            }
        }
    }
    else if(rooms->size() == 1)
    {
        startRoom = &rooms->begin()->second;
        endRoom = &rooms->begin()->second;
    }
    else
    {
        throw GenericException("No rooms on this map to place starting or end position!");
    }
    std::pair<int,int>* pos;
    if(startRoom != nullptr)
    {
        pos = startRoom->GetRandomPosition();
        if(m_map->DoesCollide(pos->first, pos->second))
        {
            throw GenericException("Invalid starting position found!");
        }
        m_map->m_startX = pos->first;
        m_map->m_startY = pos->second;
        m_map->m_startRoomNr = m_map->GetRoomNr(m_map->m_startX, m_map->m_startY);
        m_map->SetTileToType(m_map->m_startX, m_map->m_startY, MapFillDungeon::WalkthroughItem);
    }
    else
    {
        //This should not happen
        throw GenericException("No starting position found!");
    }
    if(endRoom != nullptr)
    {
        pos = endRoom->GetRandomPosition();
        if(m_map->DoesCollide(pos->first, pos->second))
        {
            throw GenericException("Invalid end position found!");
        }
        m_map->m_endX = pos->first;
        m_map->m_endY = pos->second;
        m_map->m_endRoomNr = m_map->GetRoomNr(m_map->m_endX, m_map->m_endY);
        m_map->SetTileToType(m_map->m_endX, m_map->m_endY, MapFillDungeon::WalkthroughItem);
    }
    else
    {
        //This should not happen
        throw GenericException("No end position found!");
    }
}

std::pair<int,int>* MapGeneratorDungeon::GetFreePosition(bool deadEnd)
{
    std::map<int,MapRoom>* rooms = m_map->GetAllRooms();
    size_t roomsSize = rooms->size();
    if(deadEnd)
    {
        roomsSize = 0;
        for(auto it = rooms->begin(); it != rooms->end(); it++)
        {
            if(it->second.GetNumberOfAdjacentRooms() == 1)
            {
                roomsSize++;
            }
        }
        if(roomsSize == 0)
        {
            roomsSize = rooms->size();
        }
    }
    int i = rand() % roomsSize + 1;
    int nr = 0;
    std::pair<int,int>* pos = nullptr;
    //100 tries to find free space
    while(nr < 100)
    {
        if(deadEnd && roomsSize != rooms->size())
        {
            int randNr = nr;
            for(auto it = rooms->begin(); it != rooms->end() && randNr >= 0; it++)
            {
                if(it->second.GetNumberOfAdjacentRooms() == 1)
                {
                    if(randNr == 0)
                    {
                        pos = it->second.GetRandomPosition();
                    }
                    randNr--;
                }
            }
        }
        else
        {
            pos = rooms->at(i).GetRandomPosition();
        }
        if(!m_map->DoesCollide(pos->first, pos->second))
            return pos;
    }
    //if no free space is found, return last one
    return pos;
}

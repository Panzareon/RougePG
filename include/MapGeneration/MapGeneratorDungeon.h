#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include "TileMap.h"
#include "MapGeneratorUtil.h"
#include "Map.h"

//A class to generate Basic Layout of Maps
class MapGeneratorDungeon
{
    public:
        MapGeneratorDungeon();
        MapGeneratorDungeon(Map* map, unsigned int seed = 0);
        virtual ~MapGeneratorDungeon();

        void Init(Map* map, unsigned int seed);

        void CellularAutomata(float startPercent);
        //Faster Generation for Maps > than 200 x 200
        void FasterCellularAutomata(float startPercent);
        void CellularAutomataStart(float startPercent);
        void CellularAutomataStep(int minWallTiles, int orMaxWallTiles = -1, float chanceAtThreshhold = 1.0f, bool onlyChangeToWall = false);

        //Creates rectangle rooms and connects them. randomDifference is the allowed change for height and width
        void ConnectedRoomsRectagle(int roomSizeX, int roomSizeY, int nrRooms, int randomDifference);
        //Creates not rectangle rooms and connects them. randomDifference is the allowed change for height and width
        void ConnectedRooms(int roomSizeX, int roomSizeY, int nrRooms, int randomDifference);
        void AddRectangleRoom(int x, int y, int width, int height);
        void AddRoom(int x, int y, int width, int height);
        void SetIrregularTile(int x, int y, bool horizontal);
        bool IsRoomFree(int x, int y, int width, int height);

        void MorphologicalCloseOperator();

        //has really bad Performance, not recommended to use with Maps larger than 200x200
        void ConnectAllRooms(bool straight, int maxRemovedTiles = 10, bool withClosest = false);

        void RemoveUnconnectedRooms();

        void ConnectTwoPoints(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

        void RemoveNotConnectedSpaceFromRoom(int x, int y, int width, int height, int setToValue = 0);


        void DigTunnel(int fromX, int fromY, int toX, int toY);

        void NumberRooms();

        int NumberOfAdjacentWalls(int x, int y);

        void PlaceStartingAndEndPosition();
        //for deadEnd == true look for a Room without adjacent rooms
        std::pair<int,int>* GetFreePosition(bool deadEnd = false);

    protected:
    private:
        int m_width;
        int m_height;


        MapGeneratorUtil m_MGUtil;
        Map* m_map;


        //Connect from given tile to already checked
        void CheckTiles(int** checkArray, int x, int y, bool straight, bool withClosest);



};

#endif // MAPGENERATOR_H

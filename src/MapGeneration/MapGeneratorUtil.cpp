#include "MapGeneration/MapGeneratorUtil.h"
#include <iostream>

MapGeneratorUtil::MapGeneratorUtil()
{
    //ctor
}

MapGeneratorUtil::~MapGeneratorUtil()
{
    //dtor
    for(int x = 0; x < m_width; x++)
    {
        delete[] m_checked2[x];
    }
    delete[] m_checked2;
}

void MapGeneratorUtil::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_checked2 = new int*[m_width];
    for(int x = 0; x < m_width; x++)
    {
        m_checked2[x] = new int[m_height];
    }
}


int MapGeneratorUtil::SetTilesToChecked(int** checkArray, int x, int y, int fromId, int toId, bool useToId)
{
    if(checkArray[x][y] != fromId && (!useToId || checkArray[x][y] != toId))
        return 0;
    int retval = 0;
    std::vector<sf::Vector2u> checked;
    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_height; y++)
            m_checked2[x][y] = 0;
    }
    for(int x = 0; x < m_width; x++)
        for(int y = 0; y < m_height; y++)
            m_checked2[x][y] = 0;
    checked.push_back(sf::Vector2u(x,y));
    checkArray[x][y] = toId;
    while(checked.size() > 0)
    {
        retval ++;
        sf::Vector2u pos = checked.back();
        checked.pop_back();
        int x = pos.x;
        int y = pos.y;

        if(x > 0 && (checkArray[x-1][y] == fromId || (useToId && checkArray[x-1][y] == toId)))
        {
            sf::Vector2u add(x-1,y);
            if(m_checked2[x-1][y] == 0)
            {
                checked.push_back(add);
                checkArray[x-1][y] = toId;
                m_checked2[x-1][y] = 1;
            }
        }
        if(y > 0 && (checkArray[x][y-1] == fromId || (useToId && checkArray[x][y-1] == toId)))
        {
            sf::Vector2u add(x,y-1);
            if(m_checked2[x][y-1] == 0)
            {
                checked.push_back(add);
                checkArray[x][y-1] = toId;
                m_checked2[x][y-1] = 1;
            }
        }
        if(x < m_width - 1 && (checkArray[x+1][y] == fromId || (useToId && checkArray[x+1][y] == toId)))
        {
            sf::Vector2u add(x+1,y);
            if(m_checked2[x+1][y] == 0)
            {
                checked.push_back(add);
                checkArray[x+1][y] = toId;
                m_checked2[x+1][y] = 1;
            }
        }
        if(y < m_height - 1 && (checkArray[x][y+1] == fromId || (useToId && checkArray[x][y+1] == toId)))
        {
            sf::Vector2u add(x,y+1);
            if(m_checked2[x][y+1] == 0)
            {
                checked.push_back(add);
                checkArray[x][y+1] = toId;
                m_checked2[x][y+1] = 1;
            }
        }
    }
    return retval;
}
int MapGeneratorUtil::GetNumberOfConnected(int** checkArray, int x, int y, int checkId, int maxCount)
{
    if(checkArray[x][y] != checkId)
        return 0;
    int retval = 0;
    std::vector<sf::Vector2u> checked;
    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_height; y++)
            m_checked2[x][y] = 0;
    }
    checked.push_back(sf::Vector2u(x,y));
    while(checked.size() > 0)
    {
        retval++;
        sf::Vector2u pos = checked.back();
        checked.pop_back();
        int x = pos.x;
        int y = pos.y;

        if(x > 0 && checkArray[x-1][y] == checkId)
        {
            sf::Vector2u add(x-1,y);
            if(m_checked2[x-1][y] == 0)
            {
                checked.push_back(add);
                m_checked2[x-1][y] = 1;
            }
        }
        if(y > 0 && checkArray[x][y-1] == checkId)
        {
            sf::Vector2u add(x,y-1);
            if(m_checked2[x][y-1] == 0)
            {
                checked.push_back(add);
                m_checked2[x][y-1] = 1;
            }
        }
        if(x < m_width - 1 && checkArray[x+1][y] == checkId)
        {
            sf::Vector2u add(x+1,y);
            if(m_checked2[x+1][y] == 0)
            {
                checked.push_back(add);
                m_checked2[x+1][y] = 1;
            }
        }
        if(y < m_height - 1 && checkArray[x][y+1] == checkId)
        {
            sf::Vector2u add(x,y+1);
            if(m_checked2[x][y+1] == 0)
            {
                checked.push_back(add);
                m_checked2[x][y+1] = 1;
            }
        }
        if(retval + checked.size() >= maxCount)
            break;
    }
    return retval + checked.size();
}
sf::Vector2f MapGeneratorUtil::GetMidpointOf(int** tiles, int checkId)
{
    int x = 0;
    int y = 0;
    int nr = 0;

    for(int i = 0; i < m_width; i++)
    {
        for(int j = 0; j < m_height; j++)
        {
            if(tiles[i][j] == checkId)
            {
                x += i;
                y += j;
                nr++;
            }
        }
    }
    float retX = 0.0f, retY = 0.0f;
    if(nr > 0)
    {
        retX = x / (float)nr;
        retY = y / (float)nr;

    }
    return sf::Vector2f(retX, retY);
}

sf::Vector2u MapGeneratorUtil::GetClosestToPoint(int** tiles, int checkId, int x, int y)
{
    if(tiles[x][y] == checkId)
        return sf::Vector2u(x,y);
    for (int d = 1; d<=m_width*m_height; d++)
    {
        for (int i = 0; i < d + 1; i++)
        {
            int x1 = x - d + i;
            int y1 = y - i;

            if(y1 >= 0 && x1 >= 0)
                if(tiles[x1][y1] == checkId)
                    return sf::Vector2u(x1, y1);

            x1 = x + d - i;
            y1 = y + i;

            if(y1 < m_height && x1 < m_width)
                if(tiles[x1][y1] == checkId)
                    return sf::Vector2u(x1, y1);
        }


        for (int i = 1; i < d; i++)
        {
            int x1 = x - i;
            int y1 = y + d - i;

            if(y1 < m_height && x1 > 0)
                if(tiles[x1][y1] == checkId)
                    return sf::Vector2u(x1, y1);

            x1 = x + d - i;
            y1 = y - i;

            if(y1 >= 0 && x1 < m_width)
                if(tiles[x1][y1] == checkId)
                    return sf::Vector2u(x1, y1);
        }
    }
    return sf::Vector2u(0,0);
}
//direction from 0 (east) to 7 (south-east)
sf::Vector2u MapGeneratorUtil::GetClosestInDirection(int** tiles, int checkId, int x, int y, int direction)
{
    if(tiles[x][y] == checkId)
        return sf::Vector2u(x,y);
    int xChange, yChange, xAddChange1, yAddChange1, xAddChange2, yAddChange2 = 0;
    if(direction == 0)
    {
        xChange = 1;
        yAddChange1 = 1;
        yAddChange2 = -1;
    }
    else if(direction == 1)
    {
        xChange = 1;
        yChange = -1;
        yAddChange1 = 1;
        xAddChange2 = -1;
    }
    else if(direction == 2)
    {
        yChange = -1;
        xAddChange1 = 1;
        xAddChange2 = -1;
    }
    else if(direction == 3)
    {
        xChange = -1;
        yChange = -1;
        yAddChange1 = 1;
        xAddChange2 = 1;
    }
    else if(direction == 4)
    {
        xChange = -1;
        yAddChange1 = 1;
        yAddChange2 = -1;
    }
    else if(direction == 5)
    {
        xChange = -1;
        yChange = 1;
        xAddChange1 = 1;
        yAddChange2 = -1;
    }
    else if(direction == 6)
    {
        yChange = 1;
        xAddChange1 = 1;
        xAddChange2 = -1;
    }
    else if(direction == 7)
    {
        xChange = 1;
        yChange = 1;
        xAddChange1 = -1;
        yAddChange2 = -1;
    }
    int addMultiplier = 0;
    int xCheck, yCheck;
    while(x > 0 && y > 0 && x < m_width && y < m_height)
    {
        addMultiplier++;
        x += xChange;
        y += yChange;
        for(int i = 0; i < addMultiplier / 2; i++)
        {
            xCheck = x + i*xAddChange1;
            yCheck = y + i*yAddChange1;
            if(xCheck > 0 && yCheck > 0 && xCheck < m_width && yCheck < m_height)
                if(tiles[xCheck][yCheck] == checkId)
                    return sf::Vector2u(xCheck, yCheck);
        }
        for(int i = 1; i < addMultiplier / 2; i++)
        {
            xCheck = x + i*xAddChange2;
            yCheck = y + i*yAddChange2;
            if(xCheck > 0 && yCheck > 0 && xCheck < m_width && yCheck < m_height)
                if(tiles[xCheck][yCheck] == checkId)
                    return sf::Vector2u(xCheck, yCheck);
        }
    }
    return sf::Vector2u(0,0);
}

bool MapGeneratorUtil::WouldPointBlockPath(int** tiles, int x, int y, int checkId)
{
    int i = GetNumberOfConnected(tiles, x, y, checkId);
    if(i > 1)
    {
        int x2 = x;
        int y2 = y;
        tiles[x][y]++;
        if(x > 0 && tiles[x-1][y] == checkId)
        {
            x2--;
        }
        else if(x < m_width - 1 && tiles[x+1][y] == checkId)
        {
            x2++;
        }
        else if(y > 0 && tiles[x][y-1] == checkId)
        {
            y2--;
        }
        else if(y < m_height - 1 && tiles[x][y+1] == checkId)
        {
            y2++;
        }
        int j = GetNumberOfConnected(tiles, x2, y2, checkId);
        tiles[x][y]--;
        if(i == j + 1)
            return false;
    }
    return true;
}



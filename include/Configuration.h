#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <SFML/Graphics.hpp>
#include "FileConfig.h"

class Configuration : FileConfig
{
    public:
        enum Keys{MoveUp, MoveDown, MoveRight,MoveLeft,Accept,Cancel, KEYS_END};
        virtual ~Configuration();
        static Configuration* GetInstance();
        sf::Font* GetFont();

        void SetLastKeyPressed(sf::Keyboard::Key key);
        void ResetLastKey();
        sf::Keyboard::Key GetLastKey();


        bool SetKeyConfig(Keys key, sf::Keyboard::Key toSet);
        sf::Keyboard::Key GetKey(Keys key);
        sf::Keyboard::Key GetDefaultKey(Keys key);

        int GetMaxPartySize();

        int GetNumberKeys();

        std::string KeyToString(Keys key);
        std::string KeyToString(sf::Keyboard::Key key);

        float GetWallTransparency();

    protected:
        Configuration();
        static Configuration* m_instance;
    private:
        sf::Keyboard::Key m_lastPressed;
        sf::Font m_font;

        float m_wallTransparency;

        std::vector<sf::Keyboard::Key> m_keys;
        std::vector<sf::Keyboard::Key> m_defaultKeys;

        int m_maxPartySize;
};

#endif // CONFIGURATION_H

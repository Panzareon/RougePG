#ifndef GUINODE_H
#define GUINODE_H

#include "Node.h"
#include <vector>
#include <functional>

class MenuNode : public Node
{
    public:
        MenuNode(int width);
        virtual ~MenuNode();
        void ResetOptions();
        void AddOption(std::string name, std::function<void()> func, bool available = true);
        void AddDisabledOption(std::string name);
        void CancelAvailable(bool cancel);

        void CheckKeyboardInput();
        void MoveUp();
        void MoveDown();
        void Use();

    protected:
    private:
        virtual void onDraw(sf::RenderTarget& target, const sf::Transform& transform) const;
        virtual void UpdateBackground();

        sf::RectangleShape m_background;


        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_optionHeight;
        unsigned int m_selected;
        unsigned int m_scrollPosition;
        unsigned int m_maxShownNumber;

        std::vector<std::string> m_optionName;
        std::vector<bool> m_optionAvailable;
        std::vector<std::function<void()>> m_optionFunction;
        bool m_cancelAvailable;

        sf::Color m_backgroundColor;
        sf::Color m_foregroundColor;
        sf::Color m_foregroundColorDisabled;
        sf::Color m_selectedColor;
};

#endif // GUINODE_H

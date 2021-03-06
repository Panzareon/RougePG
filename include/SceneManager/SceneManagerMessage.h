#ifndef SCENEMANAGERMESSAGE_H
#define SCENEMANAGERMESSAGE_H

#include "SceneManager.h"
#include "SceneGraph/TextNode.h"
#include "SceneGraph/MenuNode.h"
#include <functional>


class SceneManagerMessage : public SceneManager
{
    public:
        SceneManagerMessage(std::string toDisplay, float minDisplayTime = 0.5f);
        virtual ~SceneManagerMessage();

        virtual void Tick();
        virtual bool IsFinished();

        //returns true if Scene Manager below should be displayed first
        virtual bool IsTransparent();
        //returns false if Tick should be called for the Scene Manager below
        virtual bool PausesSceneManagerBelow();

        void OnAccept(std::function<void()> func);

        void AddMenuNode(MenuNode* node);
    protected:
        TextNode* m_text;
        sf::RectangleShape* m_background;
        Node* m_backgroundNode;
        MenuNode* m_menu;

        float m_displayedTime;
        float m_minDisplayTime;

        std::function<void()> m_onAccept;

        bool m_finished;
    private:
};

#endif // SCENEMANAGERMESSAGE_H

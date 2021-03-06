#include "SceneGraph/AnimatedNode.h"
#include "Controller/ShaderList.h"

AnimatedNode::AnimatedNode(sf::Sprite* sprite, int numberFrames)
{
    //ctor
    m_elapsedSeconds = 0.0f;
    m_framesPerSecond = 4;

    //Load shader
    if(sf::Shader::isAvailable())
        m_shader = ShaderList::GetShader(ShaderList::AnimatedSpriteShader);

    SetSprite(sprite, numberFrames);
}

AnimatedNode::~AnimatedNode()
{
    //dtor
}

void AnimatedNode::SetSprite(sf::Sprite* sprite, int numberFrames)
{
    m_sprite = *sprite;
    //Set number of Frames to circle through
    m_numberFrames = numberFrames;
}

void AnimatedNode::Tick(float seconds)
{
    m_elapsedSeconds += seconds;
}

void AnimatedNode::SetAnimationSpeed(float framesPerSecond)
{
    m_framesPerSecond = framesPerSecond;
}

void AnimatedNode::onDraw(sf::RenderTarget& target, const sf::Transform& transform) const
{
    sf::RenderStates rs(transform);
    if(sf::Shader::isAvailable())
    {
        rs.shader = m_shader;
        m_shader->setParameter("frameId", (float((int)(m_elapsedSeconds*m_framesPerSecond) % m_numberFrames)));
        m_shader->setParameter("numberFrames", (float)m_numberFrames);
    }
    target.draw(m_sprite, rs);
}

void AnimatedNode::SetDirection(Enums::Direction direction)
{
    int textureDeltaY = 0;
    switch(direction)
    {
    case Enums::Direction::East:
        textureDeltaY = 1;
        break;
    case Enums::Direction::South:
        textureDeltaY = 2;
        break;
    case Enums::Direction::West:
        textureDeltaY = 3;
        break;
    }
    sf::IntRect rect = m_sprite.getTextureRect();
    rect.top = textureDeltaY * rect.height;
    m_sprite.setTextureRect(rect);
}

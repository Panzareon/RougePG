#include "SceneGraph/Node.h"
#include <algorithm>

Node::Node()
{
    //ctor
    m_transform = sf::Transform::Identity;
    m_parent = 0;
    m_visible = true;
}

Node::~Node()
{
    //dtor
    for (std::size_t i = 0; i < m_children.size(); ++i)
        delete m_children[i];
}
void Node::setVisibility(bool visible)
{
    m_visible = visible;
}

void Node::draw(sf::RenderTarget& target, const sf::Transform& parentTransform) const
{
    //NOOP if this Node is not visible
    if(m_visible)
    {
        // combine the parent transform with the node's one
        sf::Transform combinedTransform = parentTransform * m_transform;

        // let the node draw itself
        onDraw(target, combinedTransform);

        // draw its children
        for (std::size_t i = 0; i < m_children.size(); ++i)
            m_children[i]->draw(target, combinedTransform);
    }
}
void Node::Tick(float seconds)
{
    for (std::size_t i = 0; i < m_children.size(); ++i)
        m_children[i]->Tick(seconds);
}

void Node::onDraw(sf::RenderTarget& target, const sf::Transform& transform) const
{
    //NOOP
}
void Node::setBoundingBox(sf::FloatRect bb)
{
    m_boundingBox = bb;
}

sf::FloatRect Node::getBoundingBox()
{
    return m_boundingBox;
}

sf::FloatRect Node::getGlobalBoundingBox()
{
    sf::Transform tf = getGlobalTransform();
    return tf.transformRect(getBoundingBox());
}

sf::Transform Node::getGlobalTransform()
{
    if(m_parent == 0)
    {
        return getTransform();
    }
    else
    {
        return m_parent->getGlobalTransform() * getTransform();
    }
}

void Node::moveNode(float x, float y)
{
    m_transform.translate(x,y);
}

void Node::setPosition(float x, float y)
{
    //Set translation values of Transform
    sf::Vector2f atmPos = m_transform.transformPoint(0.0f, 0.0f);
    m_transform.translate(x - atmPos.x, y - atmPos.y);
}

sf::Transform Node::getTransform()
{
    return m_transform;
}

void Node::setTransform(sf::Transform newTransform)
{
    m_transform = newTransform;
}
void Node::setParent(Node* parent)
{
    if(m_parent != nullptr)
    {
        m_parent->removeChild(this);
    }
    m_parent = parent;
}

Node* Node::GetParent()
{
    return m_parent;
}

bool Node::IsVisible()
{
    return m_visible;
}


void Node::addChild(Node* child)
{
    child->setParent(this);
    m_children.push_back(child);
}

void Node::addChild(Node* child, int position)
{
    child->setParent(this);
    m_children.insert(m_children.begin() + position, child);
}

void Node::removeChild(Node* child)
{
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child));
}
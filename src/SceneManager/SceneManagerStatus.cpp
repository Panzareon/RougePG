#include "SceneManagerStatus.h"
#include "Controller/GameController.h"
#include "Controller/Configuration.h"
#include "SceneGraph/DrawableNode.h"
#include "Controller/Localization.h"
#include "Party/CharacterClass.h"

namespace MenuFunctions
{
    void DeselectSkills(SceneManagerStatus* sm)
    {
        sm->DeselectSkills();
    }
    void DeselectPassiveEffects(SceneManagerStatus* sm)
    {
        sm->DeselectPassiveEffects();
    }
    void SetDescription(SceneManagerStatus* sm, std::string str)
    {
        sm->SetDescription(str);
    }
}

SceneManagerStatus::SceneManagerStatus()
{
    //ctor
    m_finished = false;
    m_skillsActive = false;
    m_passiveEffectsActive = false;
    m_expWidth = 100.0f;
    m_expHeight = 15.0f;

    GameController* controller = GameController::getInstance();
    Party* party = controller->getParty();
    m_partyMember = party->GetAllPartyMembers();
    PartyMember* member = m_partyMember->at(0);
    m_selectedMember = 0;

    sf::Sprite* backgroundSprite = new sf::Sprite(*TextureList::getTexture(TextureList::StatusMenu));
    Node* background = new DrawableNode(backgroundSprite);
    int x = GameController::getInstance()->GetWindowWidth() - backgroundSprite->getLocalBounds().width;
    x /= 2;
    int y = GameController::getInstance()->GetWindowHeight() - backgroundSprite->getLocalBounds().height;
    y /= 2;
    background->moveNode(x,y);
    m_gui->addChild(background);

    m_name = new TextNode();
    m_name->SetColor(sf::Color::Black);
    m_name->moveNode(5.0f, 5.0f);
    background->addChild(m_name);

    m_level = new TextNode();
    m_level->SetColor(sf::Color::Black);
    m_level->moveNode(60.0f, 140.0f);
    m_level->SetFontSize(20);
    background->addChild(m_level);

    m_class = new TextNode();
    m_class->SetColor(sf::Color::Black);
    m_class->moveNode(60.0f, 110.0f);
    m_class->SetFontSize(20);
    background->addChild(m_class);


    sf::RectangleShape* expBackground = new sf::RectangleShape(sf::Vector2f(m_expWidth,m_expHeight));
    expBackground->setFillColor(sf::Color(128,128,128));
    expBackground->setOutlineColor(sf::Color::Black);
    expBackground->setOutlineThickness(1.0f);
    Node* expBackgroundNode = new DrawableNode(expBackground);
    expBackgroundNode->moveNode(60.0f, 170.0f);
    background->addChild(expBackgroundNode);

    m_exp = new sf::RectangleShape(sf::Vector2f(m_expWidth - 2.0f,m_expHeight - 2.0f));
    m_exp->setFillColor(sf::Color(244, 208, 63));
    Node* expNode = new DrawableNode(m_exp);
    expNode->moveNode(1.0f,1.0f);
    expBackgroundNode->addChild(expNode);


    m_manaAndHealth = new EntityBarsNode(member);
    m_manaAndHealth->moveNode(10.0f, 50.0f);

    background->addChild(m_manaAndHealth);

    Node* nextMember = GetAttributeNode(member, 0);
    nextMember->moveNode(0.0f, 210.0f);
    background->addChild(nextMember);

    m_battleSprite = new AnimatedNode(member->GetBattleSprite(), member->GetNumberSprites());
    m_battleSprite->moveNode(10.0f, 130.0f);
    background->addChild(m_battleSprite);

    int fontSize = 15;
    int height = 25;
    m_skills = new MenuNode(150);
    m_skills->moveNode(325.0f, 5.0f);
    m_skills->PreviousAvailable(true);
    m_skills->CallOnCancel(std::function<void()>(std::bind(&MenuFunctions::DeselectSkills,this)));
    m_skills->CancelAvailable(true);

    //Set Menu looks
    m_skills->SetMaxShownOptions(17);
    m_skills->SetPadding(2,0);
    m_skills->SetBackgroundColor(sf::Color::Transparent);
    m_skills->SetForegroundColorDisabled(sf::Color::Black);
    m_skills->SetForegroundColor(sf::Color::Black);
    m_skills->SetOutlineColor(sf::Color::Transparent);
    m_skills->SetSelectedColor(sf::Color::Blue);
    m_skills->SetFontSize(fontSize);
    m_skills->SetSpacing(height - fontSize);
    m_skills->ShowSelected(false);
    background->addChild(m_skills);

    m_passiveEffects = new MenuNode(150);
    m_passiveEffects->moveNode(485.0f, 5.0f);
    m_passiveEffects->PreviousAvailable(true);
    m_passiveEffects->CallOnCancel(std::function<void()>(std::bind(&MenuFunctions::DeselectPassiveEffects,this)));
    m_passiveEffects->CancelAvailable(true);

    //Set Menu looks
    m_passiveEffects->SetMaxShownOptions(17);
    m_passiveEffects->SetPadding(2,0);
    m_passiveEffects->SetBackgroundColor(sf::Color::Transparent);
    m_passiveEffects->SetForegroundColorDisabled(sf::Color::Black);
    m_passiveEffects->SetForegroundColor(sf::Color::Black);
    m_passiveEffects->SetOutlineColor(sf::Color::Transparent);
    m_passiveEffects->SetSelectedColor(sf::Color::Blue);
    m_passiveEffects->SetFontSize(fontSize);
    m_passiveEffects->SetSpacing(height - fontSize);
    m_passiveEffects->ShowSelected(false);
    background->addChild(m_passiveEffects);


    m_description = new TextNode();
    m_description->SetColor(sf::Color::Black);
    background->addChild(m_description);
    m_description->moveNode(5, 316);
    m_description->SetFontSize(15);


    ShowForEntity(member);
}

SceneManagerStatus::~SceneManagerStatus()
{
    //dtor
}

void SceneManagerStatus::Tick()
{
    GameController* controller = GameController::getInstance();
    if(m_skillsActive)
    {
        m_skills->CheckKeyboardInput();
    }
    else if(m_passiveEffectsActive)
    {
        m_passiveEffects->CheckKeyboardInput();
    }
    //Check menu Key
    if(controller->IsKeyPressed(Configuration::Accept) || controller->IsKeyPressed(Configuration::MoveRight))
    {
        //Select Skill menu
        if(!m_skillsActive && !m_passiveEffectsActive && m_skills->GetNrOptions() > 0)
        {
            SelectSkills();
        }
        else if(!m_passiveEffectsActive && m_passiveEffects->GetNrOptions() > 0)
        {
            DeselectSkills();
            m_passiveEffectsActive = true;
            m_passiveEffects->ShowSelected(true);
        }
    }
    else if(controller->IsKeyPressed(Configuration::Cancel))
    {
        //Close Menu
        m_finished = true;
    }
    else if(controller->IsKeyPressed(Configuration::MoveUp))
    {
        if(m_selectedMember > 0)
        {
            m_selectedMember--;
            ShowForEntity(m_partyMember->at(m_selectedMember));
        }
    }
    else if(controller->IsKeyPressed(Configuration::MoveDown))
    {
        if(m_selectedMember < m_partyMember->size() - 1)
        {
            m_selectedMember++;
            ShowForEntity(m_partyMember->at(m_selectedMember));
        }
    }
}

bool SceneManagerStatus::IsFinished()
{
    return m_finished;
}

bool SceneManagerStatus::IsTransparent()
{
    return true;
}

bool SceneManagerStatus::PausesSceneManagerBelow()
{
    return true;
}

void SceneManagerStatus::ShowForEntity(PartyMember* partyMember)
{
    Localization* localization = Localization::GetInstance();
    m_name->SetText(partyMember->GetName());
    UpdateAttributeNode(partyMember, 0);
    m_manaAndHealth->SetEntity(partyMember);
    m_level->SetText(localization->GetLocalization("menu.status.level") + std::to_string(partyMember->GetLevel()));
    m_exp->setSize(sf::Vector2f(partyMember->GetExpPercent()* (m_expWidth - 2.0f), m_expHeight - 2.0f));
    m_battleSprite->SetSprite(partyMember->GetBattleSprite(), partyMember->GetNumberSprites());
    m_class->SetText(localization->GetLocalization(partyMember->GetClass()->GetName()));

    m_skills->ResetOptions();
    std::vector<std::shared_ptr<Skill>>* skills = partyMember->GetSkillList();
    for(int i = 0; i < skills->size(); i++)
    {
        m_skills->AddDisabledOption(localization->GetLocalization(skills->at(i)->GetName()), std::function<void()>(std::bind(&MenuFunctions::SetDescription,this, skills->at(i)->GetLocalizedDescription())));
        m_skills->AddValueToOption(i, std::to_string(skills->at(i)->GetManaUse()));
    }

    m_passiveEffects->ResetOptions();
    std::multimap<int, IPassiveEffect*>* passiveEffects = partyMember->GetPassiveEffects();
    for(auto it = passiveEffects->begin(); it != passiveEffects->end();it++)
    {
        if(!it->second->IsEquipment())
        {
            m_passiveEffects->AddDisabledOption(localization->GetLocalization(it->second->GetName()), std::function<void()>(std::bind(&MenuFunctions::SetDescription,this, it->second->GetLocalizedDescription())));
        }
    }
}

void SceneManagerStatus::DeselectSkills()
{
    SetDescription("");
    m_skillsActive = false;
    m_skills->ShowSelected(false);
    m_skills->setVisibility(true);
}

void SceneManagerStatus::SelectSkills()
{
    m_skillsActive = true;
    m_skills->ShowSelected(true);
}

void SceneManagerStatus::DeselectPassiveEffects()
{
    SetDescription("");
    m_passiveEffectsActive = false;
    m_passiveEffects->ShowSelected(false);
    m_passiveEffects->setVisibility(true);
    if(m_skills->GetNrOptions() > 0)
    {
        SelectSkills();
    }
}

void SceneManagerStatus::SetDescription(std::string str)
{
    m_description->SetText(str);
}
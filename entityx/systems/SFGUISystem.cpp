#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include "SFGUISystem.h"
#include "entityx/events.h"
#include "entityx/components.h"

SFGUISystem::SFGUISystem(sf::RenderWindow& rw, ex::EntityManager& entities, ex::EventManager& events)
    : window(rw)
    , entities(entities)
    , events(events)
{
    createTheGUI();
}

void SFGUISystem::update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta dt)
{
    //Obligatory to draw with SFGUI
    window.resetGLStates();

    //The top-level event polling for the render window
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed: {
            window.close();
            break;
        }
        case sf::Event::MouseButtonPressed: {
            clickEvent(event.mouseButton);
            break;
        }
        default:
            break;
        }

        //Pass on the event to other listeners
        events.emit<sf::Event>(event);
    }

    //Because sfg::Scale doesn't have good events
    checkSliderEvents();

    gui_window->HandleEvent(event);
    gui_window->Update(dt);
    gui.Display(window);
}

void SFGUISystem::clickEvent(sf::Event::MouseButtonEvent click)
{
    //Do nothing if we have clicked inside the GUI window
    if(gui_window->GetAllocation().contains(click.x, click.y))
        return;

    /* On a left or right click, we want to spawn a new Box2D object, either
     * a box or a circle */
    if(click.button == sf::Mouse::Button::Left || click.button == sf::Mouse::Button::Right) {
        auto body_type = (click.button == sf::Mouse::Button::Left) ?
            SpawnComponent::BOX : SpawnComponent::CIRCLE;
        ex::Entity e = entities.create();
        e.assign<SpawnComponent>(click.x, click.y, body_type);
    }

    /* On a middle click, we want to remove the closest entity to the click position. This
     * is queried using the Box2D body information to distance to the click point */
    else if(click.button == sf::Mouse::Button::Middle) {
        b2Vec2 request(click.x, click.y);
        auto candidates = entities.entities_with_components<Box2DComponent>();
        ex::Entity e = *std::min_element(candidates.begin(), candidates.end(),
            [request](ex::Entity a, ex::Entity b) {
                b2Vec2 pos_a = a.component<const Box2DComponent>()->body->GetPosition();
                b2Vec2 pos_b = b.component<const Box2DComponent>()->body->GetPosition();
                return b2Distance(pos_a, request) < b2Distance(pos_b, request);
            });
        if(e.valid())
            entities.destroy(e.id());
    }
}

void SFGUISystem::checkSliderEvents()
{
    /* Check for gravity updates from sliders the zero gravity button sets these
     * sliders to 0, which causes this to occur as well */
    sf::Vector2f newGrav { gravx->GetValue(), gravy->GetValue() };
    if(newGrav != storedGrav) {
        storedGrav = newGrav;
        events.emit<GravityChangeEvent>(storedGrav.x, storedGrav.y);
    }

    //Checks for changes in the RGB light-color sliders
    sf::Color newColor { (sf::Uint8)colorr->GetValue(),
                         (sf::Uint8)colorg->GetValue(),
                         (sf::Uint8)colorb->GetValue() };
    if(newColor != storedColor) {
        storedColor = newColor;
        events.emit<LightColorEvent>(newColor);
    }
}

void SFGUISystem::createTheGUI()
{
    gui_window = sfg::Window::Create();
    gui_window->SetTitle("Control Window");
    gui_window->SetRequisition(sf::Vector2f(200,100));
    gui_window->SetPosition(sf::Vector2f(200,200));

    auto notebook = sfg::Notebook::Create();

    //The Box2D setting widgets
    auto Box2DWidget = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    {
        auto gravBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
        {
            //Create X and Y gravity sliders
            sfg::Scale::Ptr sliders[2];
            std::string names[2] = {"Gravity X", "Gravity Y"};
            for(int i = 0; i != 2; ++i) {
                auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
                auto label = sfg::Label::Create(names[i]);
                sliders[i] = sfg::Scale::Create(-50, 50, 1);
                sliders[i]->SetRequisition( sf::Vector2f( 80.f, 20.f ) );
                box->Pack(label);
                box->Pack(sliders[i]);
                gravBox->Pack(box);
            }
            gravx = std::move(sliders[0]);
            gravy = std::move(sliders[1]);

            //Zero gravity button, set to reset our gravity sliders.
            auto zeroButton = sfg::Button::Create("Zero Gravity");
            zeroButton->GetSignal(sfg::Button::OnMouseLeftRelease).Connect(
                [&]( ){ gravx->SetValue(0); gravy->SetValue(0); });

            Box2DWidget->Pack(gravBox);
            Box2DWidget->Pack(zeroButton);
        }
    }

    //Let There Be Light settings widgets
    auto LTBLWidget = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    {
        //Creates three horizontal sliders to set RGB of mouse light
        sfg::Scale::Ptr sliders[3];
        std::string names[] = {"R", "G", "B"};
        auto colorFrame = sfg::Frame::Create("Light Color");
        auto sliderBox = sfg::Box::Create();
        for(int i = 0; i != 3; ++i) {
            auto labelbox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
            sliders[i] = sfg::Scale::Create(0, 255, 1);
            sliders[i]->SetRequisition( sf::Vector2f( 80.f, 20.f ) );
            labelbox->Pack(sfg::Label::Create(names[i]));
            labelbox->Pack(sliders[i]);
            sliderBox->Pack(labelbox);
        }
        colorr = std::move(sliders[0]);
        colorg = std::move(sliders[1]);
        colorb = std::move(sliders[2]);
        colorFrame->Add(sliderBox);

        //Misc buttons to control light, packed horizontally
        auto miscBox = sfg::Box::Create();
        auto enableButton = sfg::CheckButton::Create("Enable");
        auto mouseButton  = sfg::CheckButton::Create("Mouse light");
        enableButton->SetActive(true);
        mouseButton->SetActive(true);
        miscBox->Pack(enableButton);
        miscBox->Pack(mouseButton);

        //Reload light button; reloads textures and such
        auto reloadButton = sfg::Button::Create("Reload Light");
        reloadButton->GetSignal(sfg::Button::OnMouseLeftRelease)
            .Connect(std::bind(&SFGUISystem::lightReloadEvent, this));

        //Put everyhing in the notebook widget
        LTBLWidget->SetSpacing(8);
        LTBLWidget->Pack(colorFrame);
        LTBLWidget->Pack(miscBox);
        LTBLWidget->Pack(reloadButton);
    }

    //Frame to control graphics settings
    auto graphicsFrame = sfg::Frame::Create("Graphics");
    {
        // The `rect` parameter tells the table how to place the widget.
        // It is a 4-tuple (ignore the fact that it is a rect) containing
        // in order: column index, row index, column span, row span.
        // ( 0, 0, 1, 1 ) would mean 0th column, 0th row, occupy 1 column and 1 row.
        graphics = {
            {GraphicsEvent::ImageRender,    {sfg::CheckButton::Create("Image Render"),    {0, 0, 1, 1}}},
            {GraphicsEvent::WindowCollision,{sfg::CheckButton::Create("Window Collision"),{0, 1, 1, 1}}},
            {GraphicsEvent::ShowPositions,  {sfg::CheckButton::Create("Show Positions"),  {1, 1, 1, 1}}},
            {GraphicsEvent::ShowAAABs,      {sfg::CheckButton::Create("Show AABBs"),      {0, 2, 1, 1}}},
            {GraphicsEvent::RandomTextures, {sfg::CheckButton::Create("Random Textures"), {1, 2, 1, 1}}}
        };

        //Put all button in table, and set up event when clicked
        auto table = sfg::Table::Create();
        for(const auto& entry : graphics) {
            const auto& placement = entry.second;
            table->Attach(placement.first, placement.second);
            placement.first->
                GetSignal(sfg::CheckButton::OnToggle).Connect(std::bind(&SFGUISystem::graphicsEvent, this, entry));
        }

        //Turn on checkboxes that are initially on
        graphics[GraphicsEvent::RandomTextures].first->SetActive(true);

        graphicsFrame->Add(table);
    }

    //Add the two trees to the notebook
    notebook->AppendPage(Box2DWidget, sfg::Label::Create("Box2D"));
    notebook->AppendPage(LTBLWidget,  sfg::Label::Create("LTBL2"));

    //And a "clear" button not in any notebook
    auto clearButton = sfg::Button::Create("Clear Bodies");
    clearButton->GetSignal(sfg::Button::OnMouseLeftRelease).Connect(std::bind(&SFGUISystem::destroyAllEntities, this));

    //Pack the notbook above the clear button and graphics boxes, and add to window
    auto final_box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    final_box->SetSpacing(8);
    final_box->Pack(notebook);
    final_box->Pack(clearButton);
    final_box->Pack(graphicsFrame);
    gui_window->Add(final_box);
}

void SFGUISystem::lightReloadEvent()
{
    events.emit<LightReloadEvent>();
}

void SFGUISystem::destroyAllEntities()
{
    for(ex::Entity e : entities.entities_with_components<SpawnComponent>())
        entities.destroy(e.id());
}

void SFGUISystem::graphicsEvent(const GraphicsEntry& entry)
{
    events.emit<GraphicsEvent>(entry.first, entry.second.first->IsActive());
}

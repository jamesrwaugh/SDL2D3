#ifndef SDL2D3_SFGUI_SYSTEM_H
#define SDL2D3_SFGUI_SYSTEM_H

#include <SFGUI/Widgets.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include "entityx/events.h"
namespace ex = entityx;

/* The SFGUI system creates the GUI window, and emits all events to EntityX
 * as the user interacts with the entire game window. SFML events are polled
 * here, and the passed along to the rest of the system */

class SFGUISystem : public ex::System<SFGUISystem>
{
public:
    SFGUISystem(sf::RenderWindow& rw, ex::EntityManager& entities, ex::EventManager& events);

public:
    /** EntityX Interfaces **/
    //Draw the GUI and etc, and emit events if needed
    void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta dt) override;

private:
    //General GUI components
    void createTheGUI();
    sf::RenderWindow& window;     //Window to draw to
    sfg::SFGUI gui;               //Obligatory
    sfg::Window::Ptr gui_window;  //The single GUI window

    /* Slider section and data, because the sfg::Scale doesn't have slider
     * dragged/dropped events, we poll the sliders each update() with this data */
    void checkSliderEvents();
    sfg::Scale::Ptr gravx, gravy, colorr, colorg, colorb;
    sf::Vector2f storedGrav;
    sf::Color storedColor;

    /* For the "Graphics" checkboxes, this is a map of the event type to the button
     * handle in SFGUI, and the placement in the table the buttons are packed in */
    std::map<GraphicsEvent::TYPE, std::pair<sfg::CheckButton::Ptr, sf::Rect<sf::Uint32>>> graphics;
    typedef decltype(graphics)::value_type GraphicsEntry;

private:
    //GUI callbacks and event handlers
    void onMouseClick(sf::Event::MouseButtonEvent);
    void onKeyPressed(sf::Event::KeyEvent);
    void onMouseWheelScrolled(sf::Event::MouseWheelScrollEvent);
    void onWindowPosSizeChage();
    void updateWindowView();
    void resetWindowView();
    void lightReloadEvent();
    void graphicsEvent(const GraphicsEntry& entry);
    void destroyAllEntities();

private:
    ex::EntityManager& entities;    //EntityX convience items
    ex::EventManager& events;
};

#endif

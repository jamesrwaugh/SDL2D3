#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include "utility/keyvalues.h"

//Entity X systems
#include "sdl2d3/systems/Box2DSystem.h"
#include "sdl2d3/systems/SFGUISystem.h"
#include "sdl2d3/systems/LTBLSystem.h"
#include "sdl2d3/systems/TextureSystem.h"

class SDL2D3 : public entityx::EntityX
{
public:
    SDL2D3(int argc, char** argv);
    void update(entityx::TimeDelta dt);
    void run();

private:
    KeyValue keys;              //Interface to keys file
    sf::RenderWindow window;    //Render window created here
};

SDL2D3::SDL2D3(int argc, char** argv)
{
    //Load a key=value config file
    std::string path = (argc > 1) ? argv[1] : "config.ini";
    keys.LoadFromFile(path);

    //Initialize our SFML window
    int width  = keys.GetInt("WIDTH");
    int height = keys.GetInt("HEIGHT");
    if(width == 0 || height == 0) {
        std::cerr << "Invalid Window width and height" << std::endl;
        std::exit(1);
    }
    auto style = sf::Style::Titlebar | sf::Style::Close;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    window.create(sf::VideoMode(width, height), "SDL2D3", style, settings);
    window.setFramerateLimit(60);

    //Initialize systems
    systems.add<Box2DSystem>(window);
    systems.add<SFGUISystem>(window, entities, events);
    systems.add<LTBLSystem>(window, entities, keys);
    systems.add<TextureSystem>(window,entities, keys);
    systems.configure();
}

void SDL2D3::update(entityx::TimeDelta dt)
{
    systems.update<Box2DSystem>(dt);
    systems.update<TextureSystem>(dt);
    systems.update<LTBLSystem>(dt);
    systems.update<SFGUISystem>(dt);
}

void SDL2D3::run()
{
    sf::Clock clock;

    /* window.pollEvent et al is handled in the SFGUISystem update()
     * the reason is to more cleanly filter events and pass to other systems */
    while (window.isOpen())
    {
        window.clear({100,100,100});
        update(clock.restart().asSeconds());
        window.display();
    }
}


/***************************************************************/

int main(int argc, char** argv)
{
    SDL2D3 D3(argc, argv);
    D3.run();
    return EXIT_SUCCESS;
}

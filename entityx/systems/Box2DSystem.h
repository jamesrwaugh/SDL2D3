#ifndef SDL2D3_BOX2D_SYSTEM_H
#define SDL2D3_BOX2D_SYSTEM_H
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include "box2d/SFMLDebugDraw.h"
#include "entityx/events.h"
#include "entityx/components.h"
#include "keyvalues.h"
namespace ex = entityx;

/* The Box2D System is to manage the Box2D world and receive events from the GUI
 * about changing gravity and whatever else. This is the entire Box2D part of the code
 */

class Box2DSystem : public entityx::System<Box2DSystem>, public entityx::Receiver<Box2DSystem>
{
public:
    //Dimentions of spawned entities
    static constexpr float box_halfwidth = 20.f;  //Halfwidth of spwaned boxed
    static constexpr float circle_radius = 30.f;  //Radius of spawned circles

    //Conversion factors for pixels to MKS
    static constexpr float ppm = 1.f;             //Pixels-Per-Meter
    static constexpr float mpp = 1.f/ppm;         //Meters-Per-Pixel
    static constexpr float wallsz = 10.f * ppm;   //Wall size in pixels

public:
    //Initizlize with a RenderWindow so we can create walls around it
    Box2DSystem(sf::RenderWindow& rw);

    //Steps the Box2D world and draws shapes
    void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta dt) override;

    //EntityX event listeners
    void configure(ex::EventManager& events) override;
    void receive(const GravityChangeEvent& ge);
    void receive(const entityx::ComponentAddedEvent<SpawnComponent>& e);
    void receive(const entityx::EntityDestroyedEvent& e);
    void receive(const GraphicsEvent& e);

private:
    //Utility functions to create b2 bodies
    void addToWorld(ex::Entity e);
    b2Body* createStaticBox(int x, int y, int halfwidth, int halfheight);
    b2Body* createDynamicBox(int x, int y, int halfwidth, int halfheight);
    b2Body* createDynamicCircle(int x, int y, int radius);
    b2Body* createSpawnComponentBody(int x, int y, SpawnComponent::TYPE type, b2BodyType btype);
    b2Body* createBody(int x, int y, float wx, float wy, SpawnComponent::TYPE type, b2BodyType btype);

    //World information
    std::list<ex::Entity> unspawned;
    std::unique_ptr<b2World> world;
    SFMLDebugDraw drawer;  
    sf::RenderWindow& window;

    //State data. Just used to render Box2D debug or not
    bool debugEnabled;
};

#endif

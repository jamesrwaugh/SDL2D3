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
    b2Body* createStaticBox(float x, float y, float halfwidth, float halfheight);
    b2Body* createDynamicBox(float x, float y, float halfwidth, float halfheight);
    b2Body* createDynamicCircle(float x, float y, int radius);
    b2Body* createSpawnComponentBody(float x, float y, SpawnComponent::TYPE type, b2BodyType btype);
    b2Body* createBody(float x, float y, float wx, float wy, SpawnComponent::TYPE type, b2BodyType btype);

    //World information
    std::list<ex::Entity> unspawned;
    std::unique_ptr<b2World> world;
    SFMLDebugDraw drawer;  
    sf::RenderWindow& window;

    //State data. Just used to render Box2D debug or not
    bool debugEnabled;
};

#endif

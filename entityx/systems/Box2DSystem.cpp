#include <memory>
#include "utility.h"
#include "entityx/components.h"
#include "Box2DSystem.h"

Box2DSystem::Box2DSystem(sf::RenderWindow& rw)
    : window(rw)
    , debugEnabled(true)
{
    //Create world, initially 0 gravity
    world = std::make_unique<b2World>(b2Vec2(0,0));

    //Add static boxes to world to create walls around screen
    float width  = meters(window.getSize().x);
    float height = meters(window.getSize().y);
    float halfwidth  = width  / 2;
    float halfheight = height / 2;
    const float wallsz = 15px;
    createStaticBox(halfwidth, wallsz, halfwidth, wallsz);        //Top wall
    createStaticBox(wallsz, halfheight, wallsz, halfheight);      //Left wall
    createStaticBox(width-wallsz, halfheight, wallsz, halfheight);//Right wall
    createStaticBox(halfwidth, height-wallsz, halfwidth, wallsz); //Bottom wall

    //Setup Debug draw and link to world
    drawer.setWindow(window);
    drawer.SetFlags(b2Draw::e_shapeBit);
    world->SetDebugDraw(&drawer);
}

void Box2DSystem::update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta dt)
{
    //If we have unspawned entites, create bodies in the world for them each
    for(ex::Entity e : unspawned)
        addToWorld(e);
    unspawned.clear();

    //Step the world
    const int32 velocityIterations = 8;
    const int32 positionIterations = 5;
    world->Step(dt, velocityIterations, positionIterations);
    if(debugEnabled) {
        world->DrawDebugData();
    }
}

void Box2DSystem::addToWorld(ex::Entity e)
{
    //Get the spawn info and add an actual b2Body to the world
    auto spawn = e.component<SpawnComponent>();
    b2Body* body = createSpawnComponentBody(spawn->x, spawn->y, spawn->type, b2_dynamicBody);

    //Store it in the EntityX system
    e.assign<Box2DComponent>(body);
}

void Box2DSystem::configure(ex::EventManager& events)
{
    events.subscribe<GravityChangeEvent>(*this);
    events.subscribe<GraphicsEvent>(*this);
    events.subscribe<ex::ComponentAddedEvent<SpawnComponent>>(*this);
    events.subscribe<ex::EntityDestroyedEvent>(*this);
}

void Box2DSystem::receive(const GravityChangeEvent& ge)
{
    world->SetGravity(b2Vec2(ge.x, ge.y));
}

void Box2DSystem::receive(const entityx::ComponentAddedEvent<SpawnComponent>& e)
{
    /* Event listener to add a Box2D component when an entity is spawned */
    unspawned.push_back(e.entity);
}

void Box2DSystem::receive(const ex::EntityDestroyedEvent& e)
{
    /* We only care if a Box2DComponent ent has been removed.
     * If one has, we remove it from the b2World. */
    if(e.entity.has_component<Box2DComponent>())
        world->DestroyBody(e.entity.component<const Box2DComponent>()->body);
}

void Box2DSystem::receive(const GraphicsEvent& e)
{
    switch(e.type)
    {
    case GraphicsEvent::ImageRender:
        debugEnabled = !e.value;
        break;
    case GraphicsEvent::ShowAAABs:
        drawer.SetFlags(drawer.GetFlags() ^ b2Draw::e_aabbBit);
        break;
    default:
        break;
    }
}

b2Body* Box2DSystem::createStaticBox(float x, float y, float halfwidth, float halfheight)
{
    return createBody(x, y, halfwidth, halfheight, SpawnComponent::BOX, b2_staticBody);
}

b2Body* Box2DSystem::createDynamicBox(float x, float y, float halfwidth, float halfheight)
{
    return createBody(x, y, halfwidth, halfheight, SpawnComponent::BOX, b2_dynamicBody);
}

b2Body* Box2DSystem::createSpawnComponentBody(float x, float y, SpawnComponent::TYPE type, b2BodyType btype)
{
    float width = (type == SpawnComponent::BOX) ? conf::box_halfwidth : conf::circle_radius;
    return createBody(x, y, width, width, type, btype);
}

b2Body* Box2DSystem::createBody(float x, float y, float wx, float wy, SpawnComponent::TYPE type, b2BodyType btype)
{
    b2Body*   body;
    b2BodyDef bodyDef;
    b2FixtureDef fix;
    b2PolygonShape poly;    //Two shapes for fixtures. Must remain in scope until CreateFixture
    b2CircleShape circle;    
    bodyDef.type = btype;
    bodyDef.position.Set(x,y);
    fix.density = (btype == b2_dynamicBody) ? 1.0 : 0.0;
    fix.restitution = 0.3;
    body = world->CreateBody(&bodyDef);

    if(type == SpawnComponent::BOX) {
        poly.SetAsBox(wx, wy);
        fix.shape = &poly;
    }
    else if(type == SpawnComponent::CIRCLE) {
        circle.m_radius = wx;
        fix.restitution = 1;
        fix.shape = &circle;
    }
    else {
        throw std::runtime_error("This type is not implemented");
    }

    body->CreateFixture(&fix);
    body->SetSleepingAllowed(false);

    return body;
}

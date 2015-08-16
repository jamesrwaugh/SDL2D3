#include <array>
#include "LTBLSystem.h"
#include "Box2DSystem.h"
#include "entityx/components.h"

LTBLSystem::LTBLSystem(sf::RenderWindow& rw, entityx::EntityManager& entities, KeyValue& keys)
    : lighingEnabled(true)
    , lightingMouseEnabled(true)
    , window(rw)
    , entities(entities)
    , keys(keys)
{
    loadSetupLightSystem();
}

void LTBLSystem::loadSetupLightSystem()
{
    //Loads textures and shaders
    loadTextures();

    //Initialize the light system
    ls = std::make_unique<ltbl::LightSystem>();
    ls->create({0,0,9999,9999}, window.getSize(), penumbraTexture, unshadowShader, lightOverShapeShader);

    /* Create a point light, remove if already made, then set
     * origin to half that of the sprite and set other parameters */
    ls->removeLight(mouselight);
    sf::Vector2u texsize { pointLightTexture.getSize() };
    mouselight = std::make_shared<ltbl::LightPointEmission>();
    mouselight->_emissionSprite.setOrigin((float)texsize.x * 0.5, (float)texsize.y * 0.5);
    mouselight->_emissionSprite.setTexture(pointLightTexture);
    ls->addLight(mouselight);
}

void LTBLSystem::loadTextures()
{
    //Load shaders and create objects
    std::string    ushadowPath = keys.GetString("LIGHT_UNSHADOW_SHADER");
    std::string overShaderPath = keys.GetString("LIGHT_OVER_SHADER");
    unshadowShader.loadFromFile(ushadowPath + ".vert", ushadowPath + ".frag");
    lightOverShapeShader.loadFromFile(overShaderPath + ".vert", overShaderPath + ".frag");

    //Load and create the penumbra and point light textures
    std::string prenumbraPath  = keys.GetString("LIGHT_PRENUMBRA_TEXTURE");
    std::string pointLightPath = keys.GetString("LIGHT_POINT_TEXTURE");
    penumbraTexture.loadFromFile(prenumbraPath);
    pointLightTexture.loadFromFile(pointLightPath);
    penumbraTexture.setSmooth(true);
    pointLightTexture.setSmooth(true);
}

void LTBLSystem::update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta)
{
    //If we have entities to place in the system, do it
    for(ex::Entity e : unspawned)
        addToWorld(e);
    unspawned.clear();

    if(lighingEnabled) {
        //Take all Box2D components, and update the LTBL components
        ex::ComponentHandle<Box2DComponent> box;
        ex::ComponentHandle<LTBLComponent> light;
        for(ex::Entity e : entities.entities_with_components(box, light)) {
            (void)e;
            sf::ConvexShape& s = light->light->_shape;
            b2Vec2 position = box->body->GetPosition();
            s.setPosition(window.mapPixelToCoords({(int)position.x, (int)position.y}));
            s.setRotation(box->body->GetAngle() * (180.0 / M_PI));
        }
        //Update the mouse light's position
        if(lightingMouseEnabled) {
            sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            mouselight->_emissionSprite.setPosition(window.mapPixelToCoords({(int)mouse.x,(int)mouse.y}));
        }
        //RENDER THE LIGHTS
        ls->render(window.getView(), unshadowShader, lightOverShapeShader);
        sf::Sprite lighting(ls->getLightingTexture());
        window.draw(lighting, sf::BlendMultiply);
    }
}

void LTBLSystem::addToWorld(ex::Entity e)
{
    //Create an initial light shape...
    std::shared_ptr<ltbl::LightShape> lightShape = std::make_shared<ltbl::LightShape>();

    //Use the SpawnComponent to create a light with the right body and x/y points
    auto spawn = e.component<SpawnComponent>();
    if(spawn->type == SpawnComponent::BOX) {
        float w = Box2DSystem::box_halfwidth * 2;
        sf::ConvexShape& light = lightShape->_shape;
        light.setPointCount(4);
        light.setPoint(0, {0, 0});
        light.setPoint(1, {0, w});
        light.setPoint(2, {w, w});
        light.setPoint(3, {w, 0});
        light.setOrigin({w/2, w/2});
    }
    else {
        //Circle requested; create circle shape and copy points out.
        float radius = Box2DSystem::circle_radius;
        sf::CircleShape circle(radius, 10);
        int nPoints = circle.getPointCount();
        lightShape->_shape.setPointCount(nPoints);
        for(int i = 0; i != nPoints; ++i)
            lightShape->_shape.setPoint(i, circle.getPoint(i));
        lightShape->_shape.setOrigin(radius, radius);
    }

    lightShape->_shape.setPosition(spawn->x, spawn->y);

    //Add a LTBL component to the entity, and add to light system
    e.assign<LTBLComponent>(lightShape);
    ls->addShape(lightShape);
}

void LTBLSystem::configure(ex::EventManager& events)
{
    events.subscribe<ex::ComponentAddedEvent<SpawnComponent>>(*this);
    events.subscribe<ex::EntityDestroyedEvent>(*this);
    events.subscribe<sf::Event>(*this);
    events.subscribe<LightReloadEvent>(*this);
    events.subscribe<LightColorEvent>(*this);
}

void LTBLSystem::receive(const ex::ComponentAddedEvent<SpawnComponent>& e)
{
    unspawned.push_back(e.entity);
}

void LTBLSystem::receive(const ex::EntityDestroyedEvent& e)
{
    if(e.entity.has_component<LTBLComponent>())
        ls->removeShape(e.entity.component<const LTBLComponent>()->light);
}

void LTBLSystem::receive(const LightColorEvent &e)
{
    mouselight->_emissionSprite.setColor(e.color);
}

void LTBLSystem::receive(const LightReloadEvent&)
{
    loadSetupLightSystem();
    for(ex::Entity e : entities.entities_with_components<Box2DComponent>()) {
        addToWorld(e);
    }
}

void LTBLSystem::receive(const GraphicsEvent& e)
{
    switch(e.type)
    {
    case GraphicsEvent::LightEnabled:
        lighingEnabled = e.value;
        break;
    case GraphicsEvent::LightMouseEnabled:
        if(e.value) {
            ls->addLight(mouselight);
        } else {
            ls->removeLight(mouselight);
        }
        break;
    default:
        break;
    }
}

void LTBLSystem::receive(const sf::Event &e)
{
    switch(e.type)
    {
    //Change size of light when scrolled
    case sf::Event::MouseWheelScrolled: {
        sf::Vector2f scale = mouselight->_emissionSprite.getScale();
        float delta = e.mouseWheelScroll.delta;
        mouselight->_emissionSprite.setScale(scale + sf::Vector2f(delta,delta));
        mouselight->_sourceRadius += delta;
        break;
    }
    default:
        break;
    }
}

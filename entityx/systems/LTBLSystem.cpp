#include <array>
#include "utility.h"
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
    ls->_directionEmissionRange = 200;
    ls->_directionEmissionRadiusMultiplier = 0.3;
    ls->_ambientColor = {100,100,100};

    //Create and setup the mouse light
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
            sf::Vector2f adjusted = {pixels(position.x), pixels(position.y)};
            s.setPosition(window.mapPixelToCoords({(int)adjusted.x, (int)adjusted.y}));
            s.setRotation(box->body->GetAngle() * (180.0 / M_PI));
        }
        //Update the mouse light's position
        if(lightingMouseEnabled) {
            sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            mouselight->_emissionSprite.setPosition(window.mapPixelToCoords({(int)mouse.x,(int)mouse.y}));
        }
        //Render the lights
        ls->render(window.getView(), unshadowShader, lightOverShapeShader);
        sf::Sprite lighting(ls->getLightingTexture());
        window.draw(lighting, sf::BlendMultiply);
    }
}

void LTBLSystem::configure(ex::EventManager& events)
{
    events.subscribe<ex::ComponentAddedEvent<SpawnComponent>>(*this);
    events.subscribe<ex::EntityDestroyedEvent>(*this);
    events.subscribe<sf::Event>(*this);
    events.subscribe<LightEvent>(*this);
    events.subscribe<GraphicsEvent>(*this);
}

void LTBLSystem::receive(const LightEvent& e)
{
    switch(e.type)
    {
    case LightEvent::Color:
        mouselight->_emissionSprite.setColor(e.color);
        break;
    case LightEvent::Enabled:
        lighingEnabled = e.value;
        break;
    case LightEvent::MouseEnabled:
        if(e.value) {
            ls->addLight(mouselight);
        } else {
            ls->removeLight(mouselight);
        }
        break;
    case LightEvent::Reload:
        loadSetupLightSystem();
        for(ex::Entity e : entities.entities_with_components<Box2DComponent>())
            addToWorld(e);
        break;
    default:
        break;
    }
}

void LTBLSystem::receive(const GraphicsEvent& e)
{
    switch(e.type)
    {
    case GraphicsEvent::WindowZoomed: {
        float direction = std::copysign(0.1, -e.delta);
        scaleAllEntities(direction, false);
        break;
    }
    case GraphicsEvent::WindowZoomReset: {
        scaleAllEntities(1, true);
        break;
    }
    default:
        break;
    }
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

void LTBLSystem::receive(const sf::Event &e)
{
    switch(e.type)
    {
    case sf::Event::MouseWheelScrolled: {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            //Change size of light when scrolled and CTRL
            sf::Vector2f scale = mouselight->_emissionSprite.getScale();
            float delta = e.mouseWheelScroll.delta;
            mouselight->_emissionSprite.setScale(scale + sf::Vector2f(delta,delta));
            mouselight->_sourceRadius += delta;
        }
        break;
    }
    default:
        break;
    }
}

void LTBLSystem::addToWorld(ex::Entity e)
{
    //Create an initial light shape...
    std::shared_ptr<ltbl::LightShape> lightShape = std::make_shared<ltbl::LightShape>();

    //Use the SpawnComponent to create a light with the right body and x/y points
    auto spawn = e.component<SpawnComponent>();

    if(spawn->type == SpawnComponent::BOX) {
        float w = pixels(conf::box_halfwidth * 2);
        sf::ConvexShape& light = lightShape->_shape;
        light.setPointCount(4);
        light.setPoint(0, {0, 0});
        light.setPoint(1, {0, w});
        light.setPoint(2, {w, w});
        light.setPoint(3, {w, 0});
        light.setOrigin({w/2, w/2});
    }
    else if(spawn->type == SpawnComponent::CIRCLE) {
        //Circle requested; create SFML circle shape and copy points out.
        float radius = pixels(conf::circle_radius);
        sf::CircleShape circle(radius, 15);
        int nPoints = circle.getPointCount();
        lightShape->_shape.setPointCount(nPoints);
        for(int i = 0; i != nPoints; ++i)
            lightShape->_shape.setPoint(i, circle.getPoint(i));
        lightShape->_shape.setOrigin(radius, radius);
    }
    else {
        throw std::runtime_error("Spawn type not recognized");
    }

    lightShape->_shape.setPosition(spawn->x, spawn->y);

    //Accounts for window view. The light shape must be scaled to fit the Box2D size
    sf::View nowView = window.getView();
    sf::View defView = window.getDefaultView();
    float zoom = nowView.getSize().x / defView.getSize().x;
    lightShape->_shape.scale(zoom, zoom);

    //Add a LTBL component to the entity, and add to light system
    e.assign<LTBLComponent>(lightShape);
    ls->addShape(lightShape);
}

void LTBLSystem::scaleAllEntities(float delta, bool absolute)
{
    ex::ComponentHandle<LTBLComponent> light;
    for(ex::Entity e : entities.entities_with_components(light)) {
        (void)e;
        if(absolute) {
             light->light->_shape.setScale(delta, delta);
        } else {
             light->light->_shape.scale(1.0 + delta, 1.0 + delta);
        }
    }
}

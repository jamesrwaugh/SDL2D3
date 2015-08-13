#include "Box2DSystem.h"
#include "TextureSystem.h"

TextureSystem::TextureSystem(sf::RenderWindow& rw, KeyValue& keys, entityx::EntityManager& entities)
    : window(rw)
    , imageRenderEnabled(false)
    , randomTexturesEnabled(true)
    , entities(entities)
{
    /* This doesn't change. It maps the type shape to the vector of textures aviliable
     * under random texturing, or we use the first for no random textures */
    texturemap = {
        {    SpawnComponent::BOX, {&boxTextures,  Box2DSystem::box_halfwidth*2}},
        { SpawnComponent::CIRCLE, {&ballTextures, Box2DSystem::circle_radius*2}}
    };

    /* Textures for physics objects
     * `loadTextures` reads a key from an .ini consisting of colon-delimited
     * textures, and loads them into a vector */
    loadTextures(boxTextures,  keys.GetString("BOX_TEXTURES"));
    loadTextures(ballTextures, keys.GetString("BALL_TEXTURES"));

    //Load background texture and make it repeating
    bgTexture.loadFromFile(keys.GetString("BACKGROUND_TEXTURE"));
    bgTexture.setRepeated(true);
    bgSprite.setTexture(bgTexture);
    auto windsz = rw.getSize();
    bgSprite.setTextureRect(sf::IntRect(0, 0, windsz.x*2, windsz.y*2));
}

//General-porpose string split function
static std::vector<std::string> strSplit(const std::string& target, const std::string& delim)
{
    std::vector<std::string> result;
    size_t startPos = 0, it = 0;
    do {
        it = target.find(delim, startPos);
        result.push_back(target.substr(startPos, it - startPos));
        startPos = it + delim.length();
    }
    while(it != std::string::npos);
    return result;
}

void TextureSystem::loadTextures(std::vector<sf::Texture>& dest, const std::string& colonpaths)
{
    std::vector<std::string> paths = strSplit(colonpaths, ":");
    for(const std::string& s : paths) {
        sf::Texture t;
        t.loadFromFile(s);
        dest.push_back(std::move(t));
    }
}

void TextureSystem::update(ex::EntityManager& entities, ex::EventManager& events, ex::TimeDelta dt)
{
    (void)entities;
    (void)events;
    (void)dt;

    //Untextured entities, deal with them
    for(ex::Entity e : unspawned)
        addToWorld(e);
    unspawned.clear();

    if(imageRenderEnabled) {
        //Draw backgound
        window.draw(bgSprite);
        //For each Box2D component, draw a texture over it
        ex::ComponentHandle<Box2DComponent> box;
        ex::ComponentHandle<TextureComponent> tex;
        for(ex::Entity e : entities.entities_with_components(box, tex))
        {
            (void)e;
            sf::Sprite& sprite = tex->sprite;
            b2Body* body = box->body;
            b2Vec2  position = body->GetPosition();
            sprite.setPosition(position.x, position.y);
            sprite.setRotation(body->GetAngle() * (180 / M_PI));
            window.draw(sprite);
        }
    }
}

void TextureSystem::addToWorld(entityx::Entity e)
{
    retexture(e);
}

void TextureSystem::retexture(entityx::Entity e)
{
    //Add a texture component if it is not there, first time.
    auto textureComponent = e.component<TextureComponent>();
    if(!textureComponent) {
        e.assign<TextureComponent>(sf::Sprite());
    }

    /* Use the texturemap on the type the ent was spawned with to choose a random texure
     * then the new texture needs to be scaled to the Box2D component */
    sf::Sprite& s = e.component<TextureComponent>()->sprite;
    auto type = e.component<SpawnComponent>()->type;
    auto& textureBank = texturemap.at(type).first;
    s.setTexture(textureBank->at(rand() % (randomTexturesEnabled ? textureBank->size() : 1)), true);
    scaleTexture(e);
}

void TextureSystem::scaleTexture(entityx::Entity e)
{
    sf::Sprite& s = e.component<TextureComponent>()->sprite;
    auto type = e.component<SpawnComponent>()->type;
    float scalar = texturemap[type].second;
    auto texsize = s.getTexture()->getSize();
    s.setOrigin(texsize.x/2, texsize.y/2);
    s.setScale(1,1);
    s.scale(scalar / texsize.x, scalar / texsize.y);
}

void TextureSystem::configure(entityx::EventManager& events)
{
    events.subscribe<GraphicsEvent>(*this);
    events.subscribe<ex::ComponentAddedEvent<SpawnComponent>>(*this);
}

void TextureSystem::receive(const GraphicsEvent& e)
{
    switch(e.type) {
    case GraphicsEvent::ImageRender: {
        imageRenderEnabled = e.value;
    }
    case GraphicsEvent::RandomTextures: {
        randomTexturesEnabled = e.value;
        for(ex::Entity e : entities.entities_with_components<TextureComponent>())
            retexture(e);
    }
    default: {
        break;
    }
    }
}

void TextureSystem::receive(const ex::ComponentAddedEvent<SpawnComponent>& e)
{
    unspawned.push_back(e.entity);
}

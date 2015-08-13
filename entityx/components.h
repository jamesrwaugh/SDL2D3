#ifndef SDL2D3_COMPONENTS_H
#define SDL2D3_COMPONENTS_H
#include <Box2D/Box2D.h>
#include <ltbl/lighting/LightSystem.h>

/* EntityX components. These are properties given to an entity
 * corrisponding to each library the entity is used in. */

//Component given from GUI as where to spawn the entity
struct SpawnComponent
{
    enum TYPE { CIRCLE=0, BOX=1 } type;  //Type of shape
    int x, y;   //Initial spawn position

    SpawnComponent(float x, float y, TYPE t) : type(t), x(x), y(y) { }
};

//Handle to a body in the Box2D physics engine
struct Box2DComponent
{
    Box2DComponent(b2Body* body) : body(body) { }
    b2Body* body;
};

//Handle to a light occulder in the LTBL system
struct LTBLComponent
{
    LTBLComponent(std::shared_ptr<ltbl::LightShape> light) : light(light) { }
    std::shared_ptr<ltbl::LightShape> light;
};

//Handle to an image texture to draw with SFML over the entity
struct TextureComponent
{
    TextureComponent(sf::Sprite sprite) : sprite(sprite) { }
    sf::Sprite sprite;
};

#endif // SDL2D3_COMPONENTS_H

#ifndef SDL2D3_EVENTS_H
#define SDL2D3_EVENTS_H
#include <SFML/Graphics.hpp>
#include <Box2D/Common/b2Math.h>

struct PhysicsEvent
{
    enum TYPE {
        WindowCollision, //!<Emitted on "Window Collision" checkbox change
        GravityChange,   //!<Emitted on a gravity slider chagned
        EntityRemoveReq  //!<Emiited on a middle click
    } type ;
    union {
        bool  value; //!<For WindowCollision
        b2Vec2 grav; //!<For GravityChange
        b2Vec2 pos;  //!<For EntityRemoveReq
    };
    PhysicsEvent(TYPE type)
        : type(type)
        { }
};

/* Any type of relevant graphical change, from Window scaling to any of
 * the "Graphics" checkboxes checked */
struct GraphicsEvent
{
    enum TYPE {
        ImageRender,
        RandomTextures,
        ShowAAABs,
        ShowPositions,
        WindowZoomed,
        WindowZoomReset,
        GuiWindowChange,
    } type;
    union {
        bool value;    //!<For checkboxes
        float delta;   //!<For window zooming
        sf::FloatRect alloc; //!<For GUI window allocation change
    };
    GraphicsEvent(TYPE type, bool value = false)
        : type(type), value(value)
        { }
};

struct LightEvent {
    enum TYPE {
        Color,
        Enabled,
        MouseEnabled,
        Reload
    } type;
    union {
        bool value;
        sf::Color color;
    };
    LightEvent(TYPE type)
        : type(type)
        { }
};

#endif

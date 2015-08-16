#ifndef SDL2D3_LTBL_SYSTEM_H
#define SDL2D3_LTBL_SYSTEM_H

#include <entityx/entityx.h>
#include <ltbl/lighting/LightSystem.h>
#include "keyvalues.h"
#include "entityx/events.h"
#include "entityx/components.h"
namespace ex = entityx;

/* The Let There Be Light system creates a light system and updates it
 * for all entities with light components.
 * Recieves events from GUI when creating objects
 */

class LTBLSystem : public ex::System<LTBLSystem>, public ex::Receiver<LTBLSystem>
{
public:
    //Creates light system; Renderwindow and keyValue to load shaders and textures
    LTBLSystem(sf::RenderWindow& rw, ex::EntityManager& entities, KeyValue& keys);

    //Updates and draws the light system
    void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

    /*Event subscription and receiving. We care when an object is added or removed because
     *we must remove them from the light system */
    void configure(ex::EventManager& events) override;
    void receive(const ex::ComponentAddedEvent<SpawnComponent>&);
    void receive(const ex::EntityDestroyedEvent&);
    void receive(const LightColorEvent&);
    void receive(const LightReloadEvent&);
    void receive(const GraphicsEvent&);
    void receive(const sf::Event &e);

private:
    //Setup the entire light system and load textures
    void loadSetupLightSystem();
    void loadTextures();

    //Add an entity to the light system (assuming a SpawnComponent is present)
    void addToWorld(ex::Entity e);

private:
    //Light textures
    sf::Shader  unshadowShader, lightOverShapeShader;
    sf::Texture penumbraTexture, pointLightTexture;

    //The mouse light and the light system
    std::shared_ptr<ltbl::LightPointEmission> mouselight;
    std::unique_ptr<ltbl::LightSystem> ls;
    std::list<ex::Entity> unspawned;
    bool lighingEnabled;
    bool lightingMouseEnabled;

    //I/O devices (keys for textures, window for drawing)
    sf::RenderWindow& window;
    ex::EntityManager& entities;
    KeyValue& keys;
};

#endif

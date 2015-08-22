TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++14 -Wno-extra -Wno-sign-compare
QMAKE_INCDIR += lbtl/
unix|win32: LIBS += -lsfml-system -lsfml-window -lsfml-graphics -lsfgui -lBox2D -lGL -lentityx


SOURCES += \
    main.cpp \
    keyvalues.cpp \
    box2d/SFMLDebugDraw.cpp \
    entityx/systems/Box2DSystem.cpp \
    entityx/systems/SFGUISystem.cpp \
    entityx/systems/LTBLSystem.cpp \
    ltbl/quadtree/StaticQuadtree.cpp \
    ltbl/quadtree/QuadtreeOccupant.cpp \
    ltbl/quadtree/QuadtreeNode.cpp \
    ltbl/quadtree/Quadtree.cpp \
    ltbl/quadtree/DynamicQuadtree.cpp \
    ltbl/lighting/LightSystem.cpp \
    ltbl/lighting/LightShape.cpp \
    ltbl/lighting/LightPointEmission.cpp \
    ltbl/lighting/LightDirectionEmission.cpp \
    ltbl/Math.cpp \
    entityx/systems/TextureSystem.cpp \
    utility.cpp

HEADERS += \
    keyvalues.h \
    box2d/SFMLDebugDraw.h \
    entityx/systems/Box2DSystem.h \
    entityx/events.h \
    entityx/systems/SFGUISystem.h \
    entityx/systems/LTBLSystem.h \
    entityx/components.h \
    ltbl/quadtree/StaticQuadtree.h \
    ltbl/quadtree/QuadtreeOccupant.h \
    ltbl/quadtree/QuadtreeNode.h \
    ltbl/quadtree/Quadtree.h \
    ltbl/quadtree/DynamicQuadtree.h \
    ltbl/lighting/LightSystem.h \
    ltbl/lighting/LightShape.h \
    ltbl/lighting/LightPointEmission.h \
    ltbl/lighting/LightDirectionEmission.h \
    ltbl/Math.h \
    entityx/systems/TextureSystem.h \
    utility.h

OTHER_FILES += \
    config.ini


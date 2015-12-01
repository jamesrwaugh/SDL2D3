# SDL2D3
A project combining SFML Box2D LTBL2 SFGUI and EntityX

![Alt Text](sdl2d3_0.jpg)
==================
![Alt Text](sdl2d3_1.jpg)

##Requirements
- SFML 2.2+ (http://www.sfml-dev.org/download.php)
- EntityX (https://github.com/alecthomas/entityx)
- Box2D (https://github.com/erincatto/Box2D)
- SFGUI 0.30 (http://sfgui.sfml-dev.de/download/)

Currently LTBL2 in included in the sources.

##Building and Running
Building requires GCC 4.9 or later or any compiler with C++14 support. A QtCreater project file is also provided in /src. `data` and `config.ini` (or .ini specified on ``argv[1]``) should be in the same directory as the built executable
```
cd SDL2D3/
cmake .
make
```

##Controls
Control | Action
----------| ---------
Arrow Keys / WASD | Pan screen
Mouse wheel | Zoom screen 
Mouse wheel + CTRL |  Change light size
Left click  | Place box
Right click | Place circle
Middle click| Remove body at cursor


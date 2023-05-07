# space-blaster

Good old asteroids with 100% vector graphics!  
No bitmaps whatsoever - everything you see is computed at runtime. Does this make sense? Not really, but coding this was a lot of fun.

## How to build?

- Install dependencies: `libsdl2-dev libsdl2-ttf-dev libsdl2-net-dev`
- Create a build directory: `mkdir build && cd build`
- build: `cmake .. && make`

## What's missing?

A lot of things! Objects are not reasonably culled when out of view, resulting
in a performance drop with a large number of asteroids due to increasingly
complex collision detection. But hey, at least we have *pixel perfect collisions*.


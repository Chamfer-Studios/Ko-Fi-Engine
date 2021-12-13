# Ko-Fi Engine
Ko-Fi Engine is a 3D game engine based on SDL and OpenGL, developed by Álex Ávila and Bosco Barber for the "Game Engines" subject in the Bachelor's degree in Video Game Design and Development at the CITM-UPC center, Barcelona.

# Assignment 2

## Overview
For our second assignment the goal is to remove our dependency from FBX for running our games,
organize resources coherently and apply the minimal optimizations of a graphics engine (frustum culling).

## Content
- A scene with some houses, a floor and a camera is automatically loaded at the start.
- The user is able to use the inspector to modify a GameObject:
    - Hierarchy: delete, reparent, create empty and create children.
    - Transform: translate, rotate and scale Game Objects.
    - Mesh: select or drop any imported mesh.
    - Texture: select or drop any imported texture.
    - Camera is a component with settings that can be modified.
- GameObjects can be picked from the world using the mouse.
- All meshes use a bounding volume (AABB) and can be discarded using Frustum Culling. This process is visualized in the editor (debug raycast and boxes).
- Models, meshes and textures are saved to our own format (.sugar) under “Library” folder.
- Scene can be serialized to a file that can be loaded.
- The user can Start / Pause / Stop the simulation and recover its original state.
- Resource Management:
    - There is an “Assets” window that shows all user assets.
        - *as a simple tree viewer
    - The user can drop/import new files. Assets window reacts properly.
    - Asset files can be deleted through the window and /Library entries are removed.
    - Upon start, all resources not managed inside Assets are generated in Library.
        - */Library folder is regenerated from /Assets + meta content
    - All resources use reference counting (e.g. a texture / mesh is only once in memory regardless of how many gameobject use it). This process is visualized in the editor.

## Controls
- WASDQE = Move around the 3D space
- Shift + Move: Speed increase
- Right click: Camera Rotation
- Alt + Left click: Rotate around the object selected
- Mouse Wheel: Zoom
- F: Focus Object
- ESC: Exit engine

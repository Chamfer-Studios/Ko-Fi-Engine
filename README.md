# Ko-Fi Engine
Ko-Fi Engine is a 3D game engine based on SDL and OpenGL, developed by Álex Ávila and Bosco Barber for the "Game Engines" subject in the Bachelor's degree in Video Game Design and Development at the CITM-UPC center, Barcelona.

# Brand-New Shaders Pipeline
From Version 3.0 fordward Ko-Fi Engine uses Shaders to render our game objects geometry into the scene.
##Features
- You can double click a shader in the assets window to open the in-engine text editor. You can write or modify your own shaders and save them.
- You can modify the shader uniform values through the inspector under the material menu item.
- You can change, delete or add textures to the material component and then use them on your shader.
- Editing a shader in-engine automatically compiles and applies the changes
- We have created a water shader for you to test and a default shader with a albedo texture and albedo tint.
- Compile error lines and messages are shown in the editor.

## Content
- A scene with some houses, a floor and lake is automatically loaded at the start.
- The user is able to use the inspector to modify a GameObject:
    - Hierarchy: delete, reparent, create empty and create children.
    - Transform: translate, rotate and scale Game Objects.
    - Mesh: select or drop any imported mesh.
    - Texture: select or drop any imported texture.
    - Camera is a component with settings that can be modified.
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
# Known Issues 🚧🚧
- Primitives do not work at the moment
- Frustum culling does not work after the shaders implementation due to projection rework.
- Text editor sometimes goes crazy and keeps the delete action active. Text editor has problems with ctrl commands.
- Importing objects from the assets directly can cause hierarchy selection problems

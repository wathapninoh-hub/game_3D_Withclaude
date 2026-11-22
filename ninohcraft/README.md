# NinohCraft 3D

Prototype C++ voxel sandbox game, inspired by block-building games. This is a separate project inside the repository; the existing Bus Simulator Madagascar 3D files remain untouched.

## Features in this first prototype
- First-person 3D camera and WASD movement
- Procedurally generated block terrain, hills, trees and leaves
- Break blocks with the left mouse button
- Place blocks with the right mouse button
- Select Grass, Dirt or Stone with keys 1, 2 and 3
- Raylib renderer with resizable window

## Build on a computer

Requirements: CMake 3.20+, a C++17 compiler, Git, and an internet connection for the first Raylib download.

\`\`\`sh
cmake -S . -B build
cmake --build build --config Release
\`\`\`

Run the generated \`ninohcraft\` executable (on Windows it will be \`ninohcraft.exe\`).

## Controls
- WASD: move
- Mouse: look around
- Space: jump
- Left mouse: break a block
- Right mouse: place the selected block
- 1 / 2 / 3: select Grass / Dirt / Stone
- Escape: quit

## Important
This is an early playable foundation, not yet a finished realistic Minecraft-scale game. It currently uses colored cubes rather than detailed textures, animated models, crafting, saving, or multiplayer. The C++ desktop build does not automatically create an Android APK; Android packaging needs an Android NDK build setup and device testing.

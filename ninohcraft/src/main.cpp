#include "raylib.h"
#include "raymath.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

enum class BlockType : std::uint8_t { Grass, Dirt, Stone, Wood, Leaves };

struct Block {
    int x, y, z;
    BlockType type;
    bool active = true;
};

static constexpr int WORLD_X = 48;
static constexpr int WORLD_Z = 48;
static constexpr int WORLD_Y = 24;

Color BlockColor(BlockType type) {
    switch (type) {
        case BlockType::Grass:  return Color{92, 157, 72, 255};
        case BlockType::Dirt:   return Color{126, 83, 52, 255};
        case BlockType::Stone:  return Color{117, 121, 126, 255};
        case BlockType::Wood:   return Color{116, 78, 43, 255};
        case BlockType::Leaves: return Color{48, 112, 55, 255};
    }
    return WHITE;
}

int TerrainHeight(int x, int z) {
    const float hills =
        2.2f * std::sin(static_cast<float>(x) * 0.19f) +
        1.8f * std::cos(static_cast<float>(z) * 0.16f) +
        1.1f * std::sin(static_cast<float>(x + z) * 0.11f);
    return std::clamp(5 + static_cast<int>(std::round(hills)), 2, 10);
}

bool HasBlockAt(const std::vector<Block>& blocks, int x, int y, int z) {
    for (const Block& b : blocks) {
        if (b.active && b.x == x && b.y == y && b.z == z) return true;
    }
    return false;
}

void AddTree(std::vector<Block>& blocks, int x, int y, int z) {
    for (int i = 0; i < 4; ++i) blocks.push_back({x, y + i, z, BlockType::Wood});
    for (int dy = 2; dy <= 5; ++dy) {
        const int radius = (dy == 5) ? 1 : 2;
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dz = -radius; dz <= radius; ++dz) {
                if (std::abs(dx) + std::abs(dz) <= radius + 1) {
                    const int lx = x + dx, ly = y + dy, lz = z + dz;
                    if (!HasBlockAt(blocks, lx, ly, lz))
                        blocks.push_back({lx, ly, lz, BlockType::Leaves});
                }
            }
        }
    }
}

void GenerateWorld(std::vector<Block>& blocks) {
    blocks.reserve(WORLD_X * WORLD_Z * 8);
    for (int x = 0; x < WORLD_X; ++x) {
        for (int z = 0; z < WORLD_Z; ++z) {
            const int height = TerrainHeight(x, z);
            for (int y = 0; y <= height; ++y) {
                BlockType type = BlockType::Stone;
                if (y == height) type = BlockType::Grass;
                else if (y >= height - 3) type = BlockType::Dirt;
                blocks.push_back({x, y, z, type});
            }
        }
    }

    // Trees are spread out so the starting area stays open.
    for (int x = 5; x < WORLD_X - 4; x += 7) {
        for (int z = 5; z < WORLD_Z - 4; z += 8) {
            if ((x * 13 + z * 7) % 5 != 0) {
                const int y = TerrainHeight(x, z) + 1;
                AddTree(blocks, x, y, z);
            }
        }
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "NinohCraft 3D - C++ Voxel World");
    SetTargetFPS(60);

    std::vector<Block> blocks;
    GenerateWorld(blocks);

    Camera3D camera{};
    camera.position = Vector3{24.0f, static_cast<float>(TerrainHeight(24, 24) + 3), 30.0f};
    camera.target = Vector3{24.0f, camera.position.y, 29.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 75.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor();
    BlockType selected = BlockType::Grass;
    const char* selectedName = "Grass";

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        if (IsKeyPressed(KEY_ONE)) { selected = BlockType::Grass; selectedName = "Grass"; }
        if (IsKeyPressed(KEY_TWO)) { selected = BlockType::Dirt; selectedName = "Dirt"; }
        if (IsKeyPressed(KEY_THREE)) { selected = BlockType::Stone; selectedName = "Stone"; }
        if (IsKeyPressed(KEY_F)) EnableCursor();
        if (IsKeyPressed(KEY_G)) DisableCursor();

        const Ray ray = GetMouseRay(GetMousePosition(), camera);
        int hitIndex = -1;
        RayCollision nearestHit{};
        nearestHit.distance = 1000000.0f;

        for (int i = 0; i < static_cast<int>(blocks.size()); ++i) {
            const Block& b = blocks[i];
            if (!b.active) continue;
            const BoundingBox box{
                Vector3{static_cast<float>(b.x) - 0.5f, static_cast<float>(b.y) - 0.5f, static_cast<float>(b.z) - 0.5f},
                Vector3{static_cast<float>(b.x) + 0.5f, static_cast<float>(b.y) + 0.5f, static_cast<float>(b.z) + 0.5f}
            };
            const RayCollision hit = GetRayCollisionBox(ray, box);
            if (hit.hit && hit.distance < nearestHit.distance) {
                nearestHit = hit;
                hitIndex = i;
            }
        }

        if (hitIndex >= 0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            blocks[hitIndex].active = false;
        }
        if (hitIndex >= 0 && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            const Block& target = blocks[hitIndex];
            const int nx = target.x + static_cast<int>(std::round(nearestHit.normal.x));
            const int ny = target.y + static_cast<int>(std::round(nearestHit.normal.y));
            const int nz = target.z + static_cast<int>(std::round(nearestHit.normal.z));
            if (ny >= 0 && ny < WORLD_Y && !HasBlockAt(blocks, nx, ny, nz)) {
                blocks.push_back({nx, ny, nz, selected});
            }
        }

        BeginDrawing();
        ClearBackground(Color{135, 190, 235, 255});
        BeginMode3D(camera);

        DrawPlane(Vector3{WORLD_X / 2.0f, -0.56f, WORLD_Z / 2.0f},
                  Vector2{180.0f, 180.0f}, Color{75, 145, 68, 255});

        for (const Block& b : blocks) {
            if (!b.active) continue;
            const Vector3 pos{static_cast<float>(b.x), static_cast<float>(b.y), static_cast<float>(b.z)};
            DrawCube(pos, 1.0f, 1.0f, 1.0f, BlockColor(b.type));
            DrawCubeWires(pos, 1.002f, 1.002f, 1.002f, Color{0, 0, 0, 24});
        }

        if (hitIndex >= 0) {
            const Block& b = blocks[hitIndex];
            DrawCubeWires(Vector3{static_cast<float>(b.x), static_cast<float>(b.y), static_cast<float>(b.z)},
                          1.035f, 1.035f, 1.035f, YELLOW);
        }

        EndMode3D();

        DrawRectangle(12, 12, 360, 92, Fade(BLACK, 0.58f));
        DrawText("NINOHCRAFT 3D  |  C++ VOXEL SANDBOX", 24, 22, 18, RAYWHITE);
        DrawText("WASD: move   SPACE: jump   Mouse: look", 24, 49, 14, RAYWHITE);
        DrawText("Left click: break   Right click: place", 24, 70, 14, RAYWHITE);

        DrawRectangle(12, GetScreenHeight() - 48, 280, 34, Fade(BLACK, 0.58f));
        DrawText(TextFormat("BLOCK: %s   [1] Grass [2] Dirt [3] Stone", selectedName),
                 22, GetScreenHeight() - 39, 13, RAYWHITE);
        DrawLine(GetScreenWidth() / 2 - 8, GetScreenHeight() / 2,
                 GetScreenWidth() / 2 + 8, GetScreenHeight() / 2, WHITE);
        DrawLine(GetScreenWidth() / 2, GetScreenHeight() / 2 - 8,
                 GetScreenWidth() / 2, GetScreenHeight() / 2 + 8, WHITE);
        DrawFPS(GetScreenWidth() - 90, 14);

        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}

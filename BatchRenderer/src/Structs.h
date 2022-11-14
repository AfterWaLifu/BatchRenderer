#pragma once

struct Vec2 {
    float x, y;
};
struct Vec3 {
    float x, y, z;
};
struct Vec4 {
    float x, y, z, w;
};

struct Vertex {
    Vec3 Position;
    Vec4 Color;
    Vec2 TexCoords;
    float TexID;
};
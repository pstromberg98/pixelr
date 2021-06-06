#include "raylib.h"
#include "raymath.h"
#include "rlights.h"
#include "scene.h"

struct Pixelizer
{
    float pixelIntensity;
    Shader *pixelizerShader;
    RenderTexture2D *target;
};

Pixelizer *InitPixelizer(Scene *scene);
void UpdatePixelizer();
void DrawPixelizer(Pixelizer *pixelizer, Scene *scene);
void DrawPixelizerGui(Pixelizer *pixelizer, Scene *scene);
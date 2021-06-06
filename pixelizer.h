#include "raylib.h"
#include "raymath.h"
#include "rlights.h"

struct Pixelizer
{
    Model *model;
    ModelAnimation *animations;
    int animationCount;
    int selectedAnimation;
};

Pixelizer *InitPixelizer();
void SetupModel(char *file);
void UpdatePixelizer();
void DrawPixelizer(RenderTexture2D target, Camera camera);
void DrawPixelizerGui(int screenWidth, int screenHeight);
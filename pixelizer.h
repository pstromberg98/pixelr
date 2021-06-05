#include "raylib.h"
#include "raymath.h"
#include "rlights.h"

void InitPixelizer(Model *targetModel);
void UpdatePixelizer();
void DrawPixelizer(RenderTexture2D target, Camera camera, float pixelPercent);

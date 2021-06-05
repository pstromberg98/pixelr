#include "raylib.h"
#include "raymath.h"
#include "rlights.h"
#include <iostream>
using namespace std;

#define GLSL_VERSION 330

Shader pixelizerShader;
Shader lightingShader;
Model *model;

float currentPixelPercent = 0.0f;

Light lights[MAX_LIGHTS] = {0};

void InitPixelizer(Model *targetModel)
{
    model = targetModel;
    pixelizerShader = LoadShader(0, TextFormat("/home/parker/Documents/playground/pixelr/shaders/glsl%i/pixelizer.fs", GLSL_VERSION));

    lightingShader = LoadShader(TextFormat("/home/parker/Documents/playground/pixelr/shaders/glsl%i/lighting.vs", GLSL_VERSION),
                                TextFormat("/home/parker/Documents/playground/pixelr/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    lightingShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(lightingShader, "matModel");
    lightingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightingShader, "viewPos");

    int ambientLoc = GetShaderLocation(lightingShader, "ambient");
    const float a[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    SetShaderValue(lightingShader, ambientLoc, a, SHADER_UNIFORM_VEC4);

    lights[0] = CreateLight(LIGHT_POINT, (Vector3){10, 2, 10}, Vector3Zero(), WHITE, lightingShader);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){10, 2, 10}, Vector3Zero(), RED, lightingShader);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){0, 7, 10}, Vector3Zero(), GREEN, lightingShader);
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){0, 7, 10}, Vector3Zero(), BLUE, lightingShader);

    model->materials[0].shader = lightingShader;
}

void UpdatePixelizer()
{
    UpdateLightValues(lightingShader, lights[0]);
    UpdateLightValues(lightingShader, lights[1]);
    UpdateLightValues(lightingShader, lights[2]);
    UpdateLightValues(lightingShader, lights[3]);
}

void DrawPixelizer(RenderTexture2D target, Camera camera, float pixelPercent)
{
    // int pixelWidthLocation = GetShaderLocation(pixelizerShader, "pixelWidth");
    // int pixelHeightLocation = GetShaderLocation(pixelizerShader, "pixelHeight");
    // cout << pixelWidthLocation << endl;

    BeginTextureMode(target);

    BeginMode3D(camera);

    ClearBackground(RAYWHITE);
    DrawModel(*model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

    EndMode3D();

    EndTextureMode();

    BeginShaderMode(pixelizerShader);

    if (currentPixelPercent != pixelPercent)
    {
        float pixel = 10.0f * pixelPercent;

        int pixelWidthLocation = GetShaderLocation(pixelizerShader, "pixelWidth");
        int pixelHeightLocation = GetShaderLocation(pixelizerShader, "pixelHeight");

        const float pixelSize = 10.0f * pixelPercent;

        SetShaderValue(pixelizerShader, pixelWidthLocation, &pixelSize, SHADER_UNIFORM_FLOAT);
        SetShaderValue(pixelizerShader, pixelHeightLocation, &pixelSize, SHADER_UNIFORM_FLOAT);
    }

    currentPixelPercent = pixelPercent;

    DrawTextureRec(
        target.texture,
        (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height},
        (Vector2){0, 0},
        WHITE);

    EndShaderMode();
}
#include "raylib.h"
#include "raymath.h"
#include "pixelizer.h"
#include "raygui.h"
#include "scene.h"
#include <cstring>
#include <iostream>
using namespace std;

#define GLSL_VERSION 330

// int animFrame = 0;

// Light lights[MAX_LIGHTS] = {0};

RenderTexture2D GenerateTexture(Pixelizer *pixelizer, Scene *scene);
void UpdateScreen(Pixelizer *pixelizer, int screenWidth, int screenHeight);
bool ExportPixelizer(Pixelizer *pixelizer, Scene *scene, char *file);
void UpdatePixelIntensity(Pixelizer *pixelizer, float pixelIntensity);

Pixelizer *InitPixelizer(Scene *scene)
{

    Pixelizer *pixelizer = (Pixelizer *)malloc(sizeof(Pixelizer));
    *pixelizer = {pixelIntensity : 0.0f, target : (RenderTexture2D *)malloc(sizeof(RenderTexture2D))};

    pixelizer->pixelizerShader = (Shader *)malloc(sizeof(Shader));
    *(pixelizer->pixelizerShader) = LoadShader(0, TextFormat("/home/parker/Documents/playground/pixelr/shaders/glsl330/pixelizer.fs", GLSL_VERSION));

    *(pixelizer->target) = LoadRenderTexture(scene->screenWidth, scene->screenHeight);

    UpdatePixelIntensity(pixelizer, 0.5f);
    UpdateScreen(pixelizer, scene->screenWidth, scene->screenHeight);

    return pixelizer;
}

void UpdatePixelizer(Pixelizer *pixelizer, Scene *scene)
{
    RenderTexture2D *target = pixelizer->target;
    if (target != NULL)
    {
        int targetWidth = target->texture.width;
        int targetHeight = target->texture.height;

        if (targetWidth != scene->screenWidth || targetHeight != scene->screenHeight)
        {
            UnloadTexture(pixelizer->target->texture);
            free(pixelizer->target);

            pixelizer->target = (RenderTexture2D *)malloc(sizeof(RenderTexture2D));
            *(pixelizer->target) = LoadRenderTexture(scene->screenWidth, scene->screenHeight);
        }
    }
}

void DrawPixelizer(Pixelizer *pixelizer, Scene *scene)
{
    RenderTexture2D target = GenerateTexture(pixelizer, scene);
    BeginShaderMode(*(pixelizer->pixelizerShader));

    DrawTextureRec(
        target.texture,
        (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height},
        (Vector2){0, 0},
        BLANK);

    EndShaderMode();
}

bool ExportPixelizer(Pixelizer *pixelizer, Scene *scene, char *file)
{
    RenderTexture2D sceneTexture = GenerateTexture(pixelizer, scene);
    RenderTexture2D imageTexture = LoadRenderTexture(sceneTexture.texture.width, sceneTexture.texture.height);

    BeginTextureMode(imageTexture);

    BeginShaderMode(*(pixelizer->pixelizerShader));

    DrawTextureRec(
        sceneTexture.texture,
        (Rectangle){0, 0, (float)sceneTexture.texture.width, (float)sceneTexture.texture.height},
        (Vector2){0, 0},
        WHITE);

    EndShaderMode();

    EndTextureMode();

    Image textureImage = GetTextureData(imageTexture.texture);
    return ExportImage(textureImage, file);
}

RenderTexture2D GenerateTexture(Pixelizer *pixelizer, Scene *scene)
{
    RenderTexture2D target = *(pixelizer->target);
    BeginTextureMode(target);

    BeginMode3D(*(scene->camera));

    ClearBackground(BLANK);

    Model *model = scene->model;

    DrawModel(*model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

    Vector3 largestMax = (Vector3){0.0f, 0.0f, 0.0f};
    Vector3 largestMin = (Vector3){0.0f, 0.0f, 0.0f};
    BoundingBox *largestBox = NULL;
    for (int i = 0; i < model->meshCount; i++)
    {
        BoundingBox box = MeshBoundingBox(model->meshes[0]);

        float maxDistance = Vector3Distance(box.max, (Vector3){0.0f, 0.0f, 0.0f});
        float largestMaxDistance = Vector3Distance(largestMax, (Vector3){0.0f, 0.0f, 0.0f});

        if (maxDistance > largestMaxDistance)
        {
            largestMax = box.max;
        }

        float minDistance = Vector3Distance(box.min, (Vector3){});
        float largestMinDistance = Vector3Distance(largestMin, (Vector3){});
        if (minDistance > largestMinDistance)
        {
            largestMin = box.min;
        }
    }

    // DrawBoundingBox((BoundingBox){largestMin, largestMax}, RED);

    EndMode3D();

    EndTextureMode();

    return target;
}

void DrawPixelizerGui(Pixelizer *pixelizer, Scene *scene)
{
    float width = 120;
    float height = 20;
    float xPadding = 40;
    float yPadding = 10;
    float x = scene->screenWidth - width - xPadding;
    float y = scene->screenHeight - height - yPadding;

    float pixelIntensity = GuiSlider((Rectangle){x, y, width, height}, "0", "100", pixelizer->pixelIntensity, 0, 1);

    if (pixelIntensity != pixelizer->pixelIntensity)
    {
        UpdatePixelIntensity(pixelizer, pixelIntensity);
        pixelizer->pixelIntensity = pixelIntensity;
    }

    bool pressed = GuiLabelButton((Rectangle){x, y - 20, 20, 20}, "Export");
    if (pressed)
    {
        ExportPixelizer(pixelizer, scene, "/home/parker/Documents/playground/pixelr/output/test.png");
    }
}

void UpdatePixelIntensity(Pixelizer *pixelizer, float pixelIntensity)
{
    Shader pixelizerShader = *(pixelizer->pixelizerShader);

    int pixelWidthLocation = GetShaderLocation(pixelizerShader, "pixelWidth");
    int pixelHeightLocation = GetShaderLocation(pixelizerShader, "pixelHeight");

    float pixelSize = 10.0f * pixelIntensity;

    SetShaderValue(pixelizerShader, pixelWidthLocation, &pixelSize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(pixelizerShader, pixelHeightLocation, &pixelSize, SHADER_UNIFORM_FLOAT);
}

void UpdateScreen(Pixelizer *pixelizer, int screenWidth, int screenHeight)
{
    Shader pixelizerShader = *(pixelizer->pixelizerShader);

    int renderWidthLocation = GetShaderLocation(pixelizerShader, "renderWidth");
    int renderHeightLocation = GetShaderLocation(pixelizerShader, "renderHeight");

    SetShaderValue(pixelizerShader, renderWidthLocation, &screenWidth, SHADER_UNIFORM_INT);
    SetShaderValue(pixelizerShader, renderHeightLocation, &screenHeight, SHADER_UNIFORM_INT);
}

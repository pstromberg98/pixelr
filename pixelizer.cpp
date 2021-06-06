#include "raylib.h"
#include "raymath.h"
#include "rlights.h"
#include "pixelizer.h"
#include "raygui.h"
#include <cstring>
#include <iostream>
using namespace std;

#define GLSL_VERSION 330

Shader pixelizerShader;
Shader lightingShader;

int animFrame = 0;

Pixelizer *pixelizer;

float currentPixelPercent = 0.0f;
float pixelPercent = 0.5f;

Light lights[MAX_LIGHTS] = {0};

Pixelizer *InitPixelizer()
{
    pixelizer = (Pixelizer *)malloc(sizeof(Pixelizer));
    *pixelizer = {animationCount : 0, selectedAnimation : -1};

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

    return pixelizer;
}

void UpdatePixelizer()
{
    UpdateLightValues(lightingShader, lights[0]);
    UpdateLightValues(lightingShader, lights[1]);
    UpdateLightValues(lightingShader, lights[2]);
    UpdateLightValues(lightingShader, lights[3]);

    if (pixelizer->selectedAnimation != -1)
    {
        ModelAnimation selectedAnimation = pixelizer->animations[pixelizer->selectedAnimation];
        animFrame++;
        UpdateModelAnimation(*(pixelizer->model), selectedAnimation, animFrame);
        if (animFrame >= selectedAnimation.frameCount)
        {
            animFrame = 0;
        }
    }
}

void DrawPixelizer(RenderTexture2D target, Camera camera)
{
    // int pixelWidthLocation = GetShaderLocation(pixelizerShader, "pixelWidth");
    // int pixelHeightLocation = GetShaderLocation(pixelizerShader, "pixelHeight");
    // cout << pixelWidthLocation << endl;

    BeginTextureMode(target);

    BeginMode3D(camera);

    ClearBackground(BLACK);

    Model *model = pixelizer->model;

    DrawModel(*model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

    // cout << "Mesh Count: " << model->meshCount << endl;

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

    DrawBoundingBox((BoundingBox){largestMin, largestMax}, RED);

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

void SetupModel(char *file)
{
    Model model = LoadModel(file);

    int animsCount = 0;
    ModelAnimation *animations = LoadModelAnimations(file, &animsCount);

    for (int i = 0; i < animsCount; i++)
    {
        cout << "Frame Count: " << animations[0].frameCount << endl;
    }

    if (pixelizer->model != NULL)
    {
        UnloadModel((*pixelizer->model));
        free(pixelizer->model);
    }

    pixelizer->model = (Model *)malloc(sizeof(Model));
    *(pixelizer->model) = model;
    pixelizer->animations = animations;
    pixelizer->animationCount = animsCount;
    // pixelizer->model->materials[MATERIAL_MAP_DIFFUSE].shader = lightingShader;
}

void DrawPixelizerGui(int screenWidth, int screenHeight)
{
    float width = 120;
    float height = 20;
    float xPadding = 40;
    float yPadding = 10;
    float x = screenWidth - width - xPadding;
    float y = screenHeight - height - yPadding;
    pixelPercent = GuiSlider((Rectangle){x, y, width, height}, "0", "100", pixelPercent, 0, 1);

    int wh = 20;
    int currentY = 0;
    for (int i = 0; i < pixelizer->animationCount; i++)
    {
        ModelAnimation animation = pixelizer->animations[i];
        std::string name = std::string("Animation: ") += std::to_string(i + 1);
        char *cname = strcpy(new char[name.length() + 1], name.c_str());

        bool checked = i == pixelizer->selectedAnimation;

        bool pressed = GuiCheckBox((Rectangle){0, currentY, wh, wh}, cname, checked);

        if (pressed && !checked)
        {
            bool valid = IsModelAnimationValid(*(pixelizer->model), pixelizer->animations[pixelizer->selectedAnimation]);
            cout << "Valid: " << valid << endl;
            pixelizer->selectedAnimation = i;
        }

        currentY += wh + 5;
    }

    bool clearAnim = GuiLabelButton((Rectangle){0, currentY, wh, wh}, "Clear Animation");

    if (clearAnim && pixelizer->selectedAnimation != -1)
    {
        UpdateModelAnimation(*(pixelizer->model), pixelizer->animations[pixelizer->selectedAnimation], 0);
        pixelizer->selectedAnimation = -1;
    }
}

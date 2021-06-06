#include "raygui.h"
#include "raylib.h"
#include "scene.h"
#include "raymath.h"
#include <cstring>
#include <stdlib.h>
#include <iostream>

#define GLSL_VERSION 330

Scene *InitScene()
{
    Camera *camera = (Camera *)malloc(sizeof(Camera));
    *camera = (Camera){0};
    camera->position = (Vector3){0.0f, 0.0f, 0.0f};
    camera->target = (Vector3){0.0f, 0.0f, 0.0f};
    camera->up = (Vector3){0.0f, 1.6f, 0.0f};
    camera->fovy = 45.0f;
    camera->projection = CAMERA_PERSPECTIVE;

    Scene *scene = (Scene *)malloc(sizeof(Scene));
    *scene = {
        camera : camera,
        screenWidth : 900,
        screenHeight : 600,
        animationIndex : -1,
        animationCount : 0,
        lights : {0},
        enableLighting : false
    };

    InitWindow(scene->screenWidth, scene->screenHeight, "Pixelr");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(60);

    SetCameraMode(*(scene->camera), CAMERA_FREE);

    Shader *lightingShader = (Shader *)malloc(sizeof(Shader));

    *lightingShader = LoadShader(TextFormat("/home/parker/Documents/playground/pixelr/shaders/glsl%i/lighting.vs", GLSL_VERSION),
                                 TextFormat("/home/parker/Documents/playground/pixelr/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    scene->lightingShader = lightingShader;

    lightingShader->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(*lightingShader, "matModel");
    lightingShader->locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(*lightingShader, "viewPos");

    int ambientLoc = GetShaderLocation(*lightingShader, "ambient");
    const float a[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    SetShaderValue(*lightingShader, ambientLoc, a, SHADER_UNIFORM_VEC4);

    Light *lights = scene->lights;

    lights[0] = CreateLight(LIGHT_DIRECTIONAL, (Vector3){25, 0, 25}, Vector3Zero(), RED, *(scene->lightingShader));
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){10, 2, 10}, Vector3Zero(), RED, *(scene->lightingShader));
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){0, 7, 10}, Vector3Zero(), GREEN, *(scene->lightingShader));
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){0, 7, 10}, Vector3Zero(), BLUE, *(scene->lightingShader));

    lights[1].enabled = false;
    lights[2].enabled = false;
    lights[3].enabled = false;

    UpdateLightValues(*(scene->lightingShader), lights[1]);
    UpdateLightValues(*(scene->lightingShader), lights[2]);
    UpdateLightValues(*(scene->lightingShader), lights[3]);

    return scene;
}

void LoadModelFile(Scene *scene, char *file)
{
    Model model = LoadModel(file);

    int animsCount = 0;
    ModelAnimation *animations = LoadModelAnimations(file, &animsCount);

    for (int i = 0; i < animsCount; i++)
    {
        std::cout << "Frame Count: " << animations[0].frameCount << std::endl;
    }

    if (scene->model != NULL)
    {
        UnloadModel((*scene->model));
        free(scene->model);
    }

    scene->model = (Model *)malloc(sizeof(Model));
    *(scene->model) = model;
    scene->animations = animations;
    scene->animationCount = animsCount;
    scene->animationIndex = -1;
}

void UpdateScene(Scene *scene)
{
    UpdateCamera(scene->camera);

    if (IsWindowResized())
    {
        scene->screenHeight = GetScreenHeight();
        scene->screenWidth = GetScreenWidth();
    }

    if (scene->animationIndex != -1)
    {
        ModelAnimation selectedAnimation = scene->animations[scene->animationIndex];
        scene->animationFrame++;
        UpdateModelAnimation(*(scene->model), selectedAnimation, scene->animationFrame);
        if (scene->animationFrame >= selectedAnimation.frameCount)
        {
            scene->animationFrame = 0;
        }
    }
}

void DrawSceneGui(Scene *scene)
{
    int wh = 20;
    int currentY = 0;
    for (int i = 0; i < scene->animationCount; i++)
    {
        ModelAnimation animation = scene->animations[i];
        std::string name = std::string("Animation: ") += std::to_string(i + 1);
        char *cname = strcpy(new char[name.length() + 1], name.c_str());

        bool checked = i == scene->animationIndex;

        bool pressed = GuiCheckBox((Rectangle){0, currentY, wh, wh}, cname, checked);

        if (pressed && !checked)
        {
            bool valid = IsModelAnimationValid(*(scene->model), scene->animations[scene->animationIndex]);
            // cout << "Valid: " << valid << endl;
            scene->animationIndex = i;
        }

        currentY += wh + 5;
    }

    bool enableLighting = GuiCheckBox((Rectangle){0, currentY, wh, wh}, "Enable Lighting", scene->enableLighting);

    if (enableLighting != scene->enableLighting)
    {
        scene->enableLighting = enableLighting;
        if (enableLighting)
        {
            scene->model->materials[0].shader = *(scene->lightingShader);
        }
        else
        {
            scene->model->materials[0].shader = LoadMaterialDefault().shader;
        }
    }

    currentY += wh + 5;

    bool clearAnim = GuiLabelButton((Rectangle){0, currentY, wh, wh}, "Clear Animation");

    if (clearAnim && scene->animationIndex != -1)
    {
        UpdateModelAnimation(*(scene->model), scene->animations[scene->animationIndex], 0);
        scene->animationIndex = -1;
    }

    currentY += wh + 5;

    // bool pressed = GuiLabelButton((Rectangle){0, currentY, wh, wh}, "Export");
    // if (pressed)
    // {
    //     ExportPixelizer();
    // }
}

void DisposeScene(Scene *scene)
{
    UnloadShader(*(scene->lightingShader));
    free(scene->lightingShader);

    free(scene->camera);
    free(scene->animations);

    UnloadModel(*(scene->model));
    free(scene->model);

    free(scene);
}

#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "rlights.h"
#include <stdlib.h>

struct Scene
{
    Camera *camera;
    int screenWidth;
    int screenHeight;

    Model *model;
    ModelAnimation *animations;
    int animationIndex;
    int animationCount;
    int animationFrame;

    Shader *lightingShader;

    Light lights[MAX_LIGHTS];
    bool enableLighting;
};

Scene *InitScene();
void LoadModelFile(Scene *scene, char *file);
void UpdateScene(Scene *scene);
void DrawSceneGui(Scene *scene);
void DisposeScene(Scene *scene);

#endif
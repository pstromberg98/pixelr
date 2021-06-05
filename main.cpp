#define RAYGUI_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

#define GLSL_VERSION 330

#include "raylib.h"
#include "raymath.h"
#include "pixelizer.h"
#include "raygui.h"
#include <iostream>
using namespace std;

Model *currentModel = NULL;

float rotation = 0.0f;

const int screenWidth = 800;
const int screenHeight = 450;

float pixelPercent = 0.5f;

void DrawGui(float screenWidth, float screenHeight);
void DrawScene(RenderTexture2D target);

int main(void)
{

    InitWindow(screenWidth, screenHeight, "Ray Window");

    currentModel = (Model *)malloc(sizeof(Model));
    *currentModel = LoadModel("/home/parker/Downloads/castle.obj");
    currentModel->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("/home/parker/Downloads/castle.png");

    Camera camera = {0};
    camera.position = (Vector3){40.0f, 20.0f, 20.0f};
    camera.target = (Vector3){0.0f, 8.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.6f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);
    SetCameraMode(camera, CAMERA_FREE);

    const char *helperText = "Drag 3d model into window";
    const int helperTextSize = 20;

    int helperTextWidth = MeasureText(helperText, helperTextSize);

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    InitPixelizer(currentModel);

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera);

        // if(IsFileDropped()) {
        //     cout << "File dropped!" << endl;
        //     int count = 0;
        //     char** files = GetDroppedFiles(&count);
        //     cout << "File Count: " << count << endl;

        //     if (count == 1) {
        //         cout << files[0] << endl;
        //         currentModel = LoadModel(files[0]);
        //         hasModel = true;
        //     }

        //     ClearDroppedFiles();
        // }

        UpdatePixelizer();

        BeginDrawing();

        if (currentModel != NULL)
        {
            DrawPixelizer(target, camera, pixelPercent);
        }
        else
        {
            ClearBackground(RAYWHITE);
            DrawText(helperText, (screenWidth / 2) - (helperTextWidth / 2), screenHeight / 2, helperTextSize, LIGHTGRAY);
        }

        DrawGui(screenWidth, screenHeight);

        EndDrawing();

        rotation += 0.01f;
        currentModel->transform = MatrixRotateXYZ((Vector3){0.0f, rotation, 0.0f});
    }

    CloseWindow();
}

void DrawGui(float screenWidth, float screenHeight)
{
    float width = 120;
    float height = 20;
    float xPadding = 40;
    float yPadding = 10;
    float x = screenWidth - width - xPadding;
    float y = screenHeight - height - yPadding;
    pixelPercent = GuiSlider((Rectangle){x, y, width, height}, "0", "100", pixelPercent, 0, 1);
    // GuiWindowBox((Rectangle){0, 0, screenWidth, screenHeight}, "Pixler");
}

#define RAYGUI_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

#define GLSL_VERSION 330

#include "raylib.h"
#include "raymath.h"
#include "pixelizer.h"
#include "raygui.h"
#include <iostream>
#include "scene.h"
using namespace std;

float rotation = 0.0f;

bool IsModel(char *charArr);
void DrawGui(float screenWidth, float screenHeight);
void DrawScene(RenderTexture2D target);

int main(void)
{
    Scene *scene = InitScene();
    Pixelizer *pixelizer = InitPixelizer(scene);

    const char *helperText = "Drag 3d model into window";
    const int helperTextSize = 20;

    int helperTextWidth = MeasureText(helperText, helperTextSize);

    while (!WindowShouldClose())
    {
        if (IsFileDropped())
        {
            cout << "File dropped!" << endl;
            int count = 0;
            char **files = GetDroppedFiles(&count);
            cout << "File Count: " << count << endl;

            if (count == 1)
            {
                if (IsModel(files[0]))
                {
                    cout << files[0] << endl;
                    LoadModelFile(scene, files[0]);
                }
            }

            ClearDroppedFiles();
        }

        if (IsKeyDown(KEY_D) && scene->model != NULL)
        {
            rotation += 0.01f;
            scene->model->transform = MatrixRotateXYZ((Vector3){0.0f, 0.0f, rotation});
        }

        if (IsKeyDown(KEY_A) && scene->model != NULL)
        {
            rotation -= 0.01f;
            scene->model->transform = MatrixRotateXYZ((Vector3){0.0f, 0.0f, rotation});
        }

        UpdateScene(scene);
        UpdatePixelizer(pixelizer, scene);

        BeginDrawing();

        if (scene->model != NULL)
        {
            DrawPixelizer(pixelizer, scene);
            DrawPixelizerGui(pixelizer, scene);
            DrawSceneGui(scene);
        }
        else
        {
            ClearBackground(RAYWHITE);
            DrawText(helperText, (scene->screenWidth / 2) - (helperTextWidth / 2), scene->screenHeight / 2, helperTextSize, LIGHTGRAY);
        }

        EndDrawing();

        // rotation += 0.01f;
        // currentModel->transform = MatrixRotateXYZ((Vector3){0.0f, rotation, 0.0f});
    }

    DisposeScene(scene);

    CloseWindow();
}

bool IsModel(char *charArr)
{
    std::string str = std::string(charArr);
    if (str.length() >= 3)
    {
        std::string extension = str.substr(str.length() - 3, 3);
        cout << extension << endl;
        if (extension == "obj" || extension == "ltf")
        {
            return true;
        }
    }

    return false;
}

void ResetCamera(Camera camera)
{
    camera.position = (Vector3){0.0f, 0.0f, 0.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.6f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

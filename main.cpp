#define RAYGUI_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

#define GLSL_VERSION 330

#include "raylib.h"
#include "raymath.h"
#include "pixelizer.h"
#include "raygui.h"
#include <iostream>
using namespace std;

float rotation = 0.0f;

const int screenWidth = 800;
const int screenHeight = 450;

bool IsModel(char *charArr);
void DrawGui(float screenWidth, float screenHeight);
void DrawScene(RenderTexture2D target);

int main(void)
{

    InitWindow(screenWidth, screenHeight, "Ray Window");

    Camera camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 0.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.6f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);
    SetCameraMode(camera, CAMERA_FREE);

    const char *helperText = "Drag 3d model into window";
    const int helperTextSize = 20;

    int helperTextWidth = MeasureText(helperText, helperTextSize);

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    Pixelizer *pixelizer = InitPixelizer();

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera);

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

                    SetupModel(files[0]);
                }
            }

            ClearDroppedFiles();
        }

        UpdatePixelizer();

        BeginDrawing();

        if (pixelizer->model != NULL)
        {
            DrawPixelizer(target, camera);
        }
        else
        {
            ClearBackground(RAYWHITE);
            DrawText(helperText, (screenWidth / 2) - (helperTextWidth / 2), screenHeight / 2, helperTextSize, LIGHTGRAY);
        }

        DrawPixelizerGui(screenWidth, screenHeight);

        EndDrawing();

        // rotation += 0.01f;
        // currentModel->transform = MatrixRotateXYZ((Vector3){0.0f, rotation, 0.0f});
    }

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

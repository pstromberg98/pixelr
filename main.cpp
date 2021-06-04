#define RAYGUI_IMPLEMENTATION

#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include "raygui.h"
using namespace std;

Model currentModel;
bool hasModel = false;

float rotation = 0.0f;

const int screenWidth = 800;
const int screenHeight = 450;

void BuildGui();

int main(void) {

    InitWindow(screenWidth, screenHeight, "Ray Window");

    SetTargetFPS(60);
    Camera camera = { 0 };
    camera.position = (Vector3){ 20.0f, 8.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 8.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.6f, 0.0f };
    camera.fovy = 45.0f;                              
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FREE);

    const char* helperText = "Drag 3d model into window";
    const int helperTextSize = 20;

    int helperTextWidth = MeasureText(helperText, helperTextSize);

    while(!WindowShouldClose()) {
        UpdateCamera(&camera);
        
        if(IsFileDropped()) {
            cout << "File dropped!" << endl;
            int count = 0;
            char** files = GetDroppedFiles(&count);   
            cout << "File Count: " << count << endl;

            if (count == 1) {
                cout << files[0] << endl;
                currentModel = LoadModel(files[0]);
                hasModel = true;
            }

            ClearDroppedFiles();
        }

        BuildGui();

        BeginDrawing();

        if (hasModel) {
            BeginMode3D(camera);
            ClearBackground(BLACK);
            DrawModel(currentModel, (Vector3) {0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
            EndMode3D();
        } else {
            ClearBackground(RAYWHITE);
            DrawText(helperText, (screenWidth / 2) - (helperTextWidth / 2), screenHeight / 2, helperTextSize, LIGHTGRAY);
        }

        EndDrawing();

        rotation += 0.01f;
        currentModel.transform = MatrixRotateXYZ((Vector3){ 0.0f, rotation, 0.0f });
    }

    CloseWindow();
}

void BuildGui() {
    // GuiWindowBox((Rectangle){0, 0, screenWidth, screenHeight}, "Pixler");
}

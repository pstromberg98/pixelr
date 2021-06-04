#define RAYGUI_IMPLEMENTATION

#define GLSL_VERSION            330

#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include "raygui.h"
using namespace std;

Model* currentModel = NULL;

float rotation = 0.0f;

const int screenWidth = 800;
const int screenHeight = 450;

Camera camera = { 0 };

void BuildGui();
void DrawPixelatedTexture(RenderTexture2D target, Shader shader);
void DrawSceneToTexture(RenderTexture2D target);

int main(void) {

    InitWindow(screenWidth, screenHeight, "Ray Window");

    currentModel = (Model*)malloc(sizeof(Model));
    *currentModel = LoadModel("/Users/parkerstromberg/Downloads/castle.obj");
    // currentModel.materials[0].maps[MATERIAL_MAP_EMISSION].texture = LoadTexture("/Users/parkerstromberg/Downloads/emission.jpeg");
    currentModel->materials[0].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("/Users/parkerstromberg/Downloads/castle.png");
    // currentModel.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadTexture("/Users/parkerstromberg/Downloads/normal.png");
    // SetMaterialTexture(&currentModel.materials[0], MAP_DIFFUSE, texture);

    camera.position = (Vector3){ 40.0f, 20.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 8.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.6f, 0.0f };
    camera.fovy = 45.0f;                              
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);
    SetCameraMode(camera, CAMERA_FREE);

    const char* helperText = "Drag 3d model into window";
    const int helperTextSize = 20;

    int helperTextWidth = MeasureText(helperText, helperTextSize);

    Shader shader = LoadShader(0, TextFormat("/Users/parkerstromberg/Documents/playground/ray/shaders/glsl%i/pixelizer.fs", GLSL_VERSION));

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    while(!WindowShouldClose()) {
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

        BeginDrawing();

        if (currentModel != NULL) {
            DrawSceneToTexture(target);
            DrawPixelatedTexture(target, shader);
        } else {
            ClearBackground(RAYWHITE);
            DrawText(helperText, (screenWidth / 2) - (helperTextWidth / 2), screenHeight / 2, helperTextSize, LIGHTGRAY);
        }

        // BuildGui();

        EndDrawing();

        rotation += 0.01f;
        currentModel->transform = MatrixRotateXYZ((Vector3){ 0.0f, rotation, 0.0f });
    }

    CloseWindow();
}

void DrawPixelatedTexture(RenderTexture2D target, Shader shader) {
    BeginShaderMode(shader);

        DrawTextureRec(
            target.texture, 
            (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, 
            (Vector2){ 0, 0 }, 
            WHITE
        );

    EndShaderMode();
}

void DrawSceneToTexture(RenderTexture2D target) {
    BeginTextureMode(target);

        BeginMode3D(camera);

            ClearBackground(BLACK);
            DrawModel(*currentModel, (Vector3) {0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

        EndMode3D();

    EndTextureMode();
}

void BuildGui() {
    // GuiWindowBox((Rectangle){0, 0, screenWidth, screenHeight}, "Pixler");
}

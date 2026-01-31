#include <stdio.h>
#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "cJSON.h"



Rectangle MyRectangle(int x,int y,int width,int height);
void LoadLua();
void DraggablePanel(Rectangle* rect, const char* title);
void CreateLualib();
int TestWindow();
int close = -1;
lua_State *L = NULL;

static const luaL_Reg tab_funcs[] = {
    {"testwindow", TestWindow},
    {NULL, NULL}
};
int luaopen_window (lua_State *L);

int main(void)
{
    InitWindow(1280, 720, "MyGame");
    L = luaL_newstate();
    LoadLua();
    // Texture2D texture = LoadTexture("assets/test.png");

    // 带数组对象的json数据解析方式：
    // char *jsonText = LoadFileText("config.json");
    // cJSON *root = cJSON_Parse(jsonText);
    //
    // cJSON *window = cJSON_GetObjectItem(root, "window");
    // cJSON *content = cJSON_GetObjectItem(window, "content");
    //
    // int count = cJSON_GetArraySize(content);
    //
    // for (int i = 0; i < count; i++)
    // {
    //     cJSON *item = cJSON_GetArrayItem(content, i);
    //
    //     int id = cJSON_GetObjectItem(item, "id")->valueint;
    //     const char *name =
    //         cJSON_GetObjectItem(item, "name")->valuestring;
    //
    //     printf("content[%d]: id=%d, name=%s\n", i, id, name);
    // }
    //
    // cJSON_Delete(root);
    // UnloadFileText(jsonText);
    // 读取json文件，不带数组对象
    // char *jsonText = LoadFileText("assets/config.json");
    // cJSON *root = cJSON_Parse(jsonText);
    // if (root)
    // {
    //     cJSON *window = cJSON_GetObjectItem(root, "window");
    //
    //     int width  = cJSON_GetObjectItem(window, "width")->valueint;
    //     int height = cJSON_GetObjectItem(window, "height")->valueint;
    //     const char *title = cJSON_GetObjectItem(window, "title")->valuestring;
    //     printf("window title %s\n",title);
    // }
    Rectangle panelRect = MyRectangle(0,0,200,200);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        // DrawTexture(texture,0,0,WHITE);
        if (close == 0) {
            DraggablePanel(&panelRect, "MySkill");
        }

        EndDrawing();
    }
    // UnloadTexture(texture);
    // UnloadFileText(jsonText);
    CloseWindow();
    return 0;
}
int TestWindow() {
    // printf("%s\n","testwindow:success!!");
    close = 0;
    return 0;
}
void DraggablePanel(Rectangle* rect, const char* title)
{
    static bool dragging = false;
    static Vector2 dragOffset = { 0 };

    Rectangle titleBar = {
        rect->x,
        rect->y,
        rect->width,
        24
    };

    // 标题栏点击
    if (CheckCollisionPointRec(GetMousePosition(), titleBar))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            dragging = true;
            dragOffset.x = GetMouseX() - rect->x;
            dragOffset.y = GetMouseY() - rect->y;
        }
    }

    if (dragging)
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            rect->x = GetMouseX() - dragOffset.x;
            rect->y = GetMouseY() - dragOffset.y;
        }
        else
        {
            dragging = false;
        }
    }
    close = GuiWindowBox(*rect, title);
    int btnX = 50;
    int btnY = 50;
    Rectangle btn_rect = {rect->x + btnX,rect->y + btnY,80,30};
    GuiButton(btn_rect,"OpenSkill");
}

void CreateLualib() {
    luaL_requiref(L, "gamewindow", luaopen_window, 1);
    lua_pop(L, 1);  /* remove lib */
}
int luaopen_window (lua_State *L) {
    luaL_newlib(L, tab_funcs);
    return 1;
}

void LoadLua() {

    luaL_openlibs(L);
    CreateLualib();
    // luaL_setmetatable(L,"main_texture");
    luaL_dofile(L,"main.lua");
    lua_getglobal(L,"onLoad");
    int bb = lua_pcall(L,0,1,0);
    const char* cc = lua_tostring(L,bb);

    printf("%s\n",cc);

    lua_close(L);
}
Rectangle MyRectangle(int x,int y,int width,int height) {
    Rectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    return rect;
}
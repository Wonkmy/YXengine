#include <stdio.h>
#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "cJSON.h"

#include "lua_global_func_register.h"

Rectangle MyRectangle(int x,int y,int width,int height);
void LoadLua(lua_State *L);
void DraggablePanel(Rectangle* rect,int index, const char* title);
int close = -1;


int lastRectIndex = 0;
static int draggingIndex = -1;
static Vector2 dragOffset = {0};


int main(void)
{
    InitWindow(1280, 720, "MyGame");

    WindowList_Init(&windows);


    lua_State* L = luaL_newstate();
    LoadLua(L);


    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < windows.count; i++) {
            if (windows.data[i].visible == 0) {
                DraggablePanel(&windows.data[i].rect,i,"MySkill");
            }
        }
        EndDrawing();
    }
    free(windows.data);
    CloseWindow();
    return 0;
}

void DraggablePanel(Rectangle* rect,int index, const char* title)
{
    Rectangle titleBar = {
        rect->x,
        rect->y,
        rect->width,
        24
    };

    // 标题栏点击
    if (CheckCollisionPointRec(GetMousePosition(), titleBar))
    {
        lastRectIndex = index;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            WindowList_BringToFront(&windows, index);

            draggingIndex = windows.count - 1; // 新索引
            dragOffset.x = GetMouseX() - rect->x;
            dragOffset.y = GetMouseY() - rect->y;
        }
    }

    if (draggingIndex == index)
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            rect->x = GetMouseX() - dragOffset.x;
            rect->y = GetMouseY() - dragOffset.y;
        }
        else
        {
            draggingIndex = -1;
        }
    }

    windows.data[index].visible = GuiWindowBox(*rect, title);
    int btnX = 50;
    int btnY = 50;
    Rectangle btn_rect = {rect->x + btnX,rect->y + btnY,80,30};
    GuiButton(btn_rect,"OpenSkill");
}




void LoadLua(lua_State *L) {
    luaL_openlibs(L);// 加载官方lua库
    CreateCustomLualibs(L);// 加载自定义lua库
    luaL_dofile(L,"../game1/main.lua");// 执行main.lua文件
    lua_getglobal(L,"onLoad");// 获取全局lua函数 "onLoad"，并压栈。写在main.lua中的，目前没有限定必须在main.lua中，理论上可以在任何地方，后面实现功能
    lua_pcall(L,0,0,0);// 执行栈顶的onLoad函数，这个方法：lua_pcall会执行栈顶的函数
    lua_close(L);
}

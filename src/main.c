#include <stdio.h>
#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "cJSON.h"

typedef struct {
    Rectangle rect;
    int visible;
}WindowInfo;

typedef struct {
    WindowInfo* data;
    int count;
    int capacity;
} WindowList;

Rectangle MyRectangle(int x,int y,int width,int height);
void LoadLua(lua_State *L);
void DraggablePanel(Rectangle* rect,int index, const char* title);
void CreateLualib();
int close = -1;

WindowList windows;
int lastRectIndex = 0;
static int draggingIndex = -1;
static Vector2 dragOffset = {0};
static int openWindow(lua_State *L) {
    // printf("%s\n","testwindow:success!!");
    close = 0;
    return 0;
}

static void WindowList_Init(WindowList* list)
{
    list->count = 0;
    list->capacity = 4;
    list->data = malloc(sizeof(WindowInfo) * list->capacity);
}
static void WindowList_Add(WindowList* list, WindowInfo w)
{
    if (list->count >= list->capacity)
    {
        list->capacity *= 2;
        list->data = realloc(list->data,
            sizeof(WindowInfo) * list->capacity);
    }

    list->data[list->count++] = w;
}
void WindowList_BringToFront(WindowList* list, int index)
{
    if (index < 0 || index >= list->count) return;

    WindowInfo temp = list->data[index];

    for (int i = index; i < list->count - 1; i++)
    {
        list->data[i] = list->data[i + 1];
    }

    list->data[list->count - 1] = temp;
}

static int createWindow(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int visible = luaL_checkinteger(L, 5);
    Rectangle panelRect = MyRectangle(x,y,w,h);

    WindowList_Add(&windows, (WindowInfo){
        .rect = panelRect,
        .visible = visible
    });
    return 0;
}


static const luaL_Reg tab_funcs[] = {
    {"createwindow", createWindow},
    {"openwindow", openWindow},
    {NULL, NULL}
};
static int luaopen_window (lua_State *L) {
    luaL_newlib(L, tab_funcs);
    return 1;
}

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

void CreateLualib(lua_State *L) {
    luaL_requiref(L, "gamewindow", luaopen_window, 1);// gamewindow代表库名: 即lua代码中的：require "xxx"的时候用的
    lua_pop(L, 1);  /* remove lib */
}


void LoadLua(lua_State *L) {
    luaL_openlibs(L);// 加载官方lua库
    CreateLualib(L);// 加载自定义lua库
    luaL_dofile(L,"main.lua");// 执行main.lua文件
    lua_getglobal(L,"onLoad");// 获取全局lua函数 "onLoad"，并压栈。写在main.lua中的，目前没有限定必须在main.lua中，理论上可以在任何地方，后面实现功能
    lua_pcall(L,0,0,0);// 执行栈顶的onLoad函数，这个方法：lua_pcall会执行栈顶的函数
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
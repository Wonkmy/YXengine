//
// Created by 你好 on 2026/2/2.
//

#include "lua.h"
#include "lauxlib.h"
#include "raylib.h"
#include  "stdlib.h"

void CreateCustomLualibs(lua_State *L);

typedef struct {
    Rectangle rect;
    int visible;
}WindowInfo;

typedef struct {
    WindowInfo* data;
    int count;
    int capacity;
} WindowList;
WindowList windows;
Rectangle MyRectangle(int x,int y,int width,int height) {
    Rectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    return rect;
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

static int luaB_print (lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    for (i = 1; i <= n; i++) {  /* for each argument */
        size_t l;
        const char *s = luaL_tolstring(L, i, &l);  /* convert it to string */
        if (i > 1)  /* not the first element? */
            lua_writestring("\t", 1);  /* add a tab before it */
        lua_writestring(s, l);  /* print it */
        lua_pop(L, 1);  /* pop result */
    }
    lua_writeline();
    return 0;
}

/*
 * 在这里注册一些全局的函数给lua端调用，而不用污染原lua源代码。也不用去实现额外的包
 */
static void register_lua_global_funcs(lua_State* L)
{
    lua_register(L, "lloog", luaB_print);
}

static void LoadAllCustomLibs(lua_State* L)
{
    CreateCustomLualibs(L);
}

// =====================================================下面是注册并实现一个lua包的逻辑（例如实现自己的在lua代码那边的require xxx的功能）=================================================
// =========luaopen_gamewindow库的实际函数的实现============
static  int setGameWindow(lua_State *L)
{
    int w = luaL_checkinteger(L, 1);
    int h = luaL_checkinteger(L, 2);
    SetWindowSize(w,h);
    return 0;
}
static int createPanel(lua_State *L) {
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
    return 1;
}
// =========luaopen_graphics库的实际函数的实现============
static  int createImage(lua_State *L)
{
    const char* filename = luaL_checkstring(L, 1);
    Texture2D texture = LoadTexture(filename);
    if(texture.id == 0) {
        return luaL_error(L, "Failed to load texture: %s", filename);
    }

    Texture2D* tex = (Texture2D*)lua_newuserdata(L, sizeof(Texture2D));
    *tex = texture;

    luaL_getmetatable(L, "myTexture");
    lua_setmetatable(L, -2);
    return 1;
}

static int drawImage(lua_State* L) {
    // 获取传入的Texture userdata
    Texture2D* tex = (Texture2D*)luaL_checkudata(L, 1, "myTexture");
    Texture2D texture = *tex;
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    DrawTexture(texture, x, y, WHITE);
    return 0;
}

static  int drawText(lua_State *L)
{
    const char* textContent = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int fontSize = luaL_checkinteger(L, 4);
    DrawText(textContent,x,y,fontSize,WHITE);
    return  0;
}

// 游戏窗口库相关的函数数组定义
static const luaL_Reg tab_funcs_gamewindow[] = {
    {"createpanel", createPanel},//创建一个游戏内面板
    {"setGameWindow",setGameWindow},// 设置游戏窗口宽高
    {NULL, NULL}
};

// 游戏窗口库相关的函数注册数组
static int luaopen_gamewindow (lua_State *L) {
    luaL_newlib(L, tab_funcs_gamewindow);
    return 1;
}

// 游戏绘图库相关的函数数组定义
static const luaL_Reg tab_funcs_gamegraphics[] = {
    {"createImage", createImage},//创建一张Image
{"drawimage", drawImage},//绘制一张Image
    {"drawtext",drawText},
    {NULL, NULL}
};
// 游戏绘图库相关的函数注册数组
static  int luaopen_graphics(lua_State *L)
{
    luaL_newlib(L, tab_funcs_gamegraphics);
    return 1;
}

// 这里是自定义的包的数组定义
static const luaL_Reg loadedlibs[] = {
    {"window", luaopen_gamewindow},//游戏窗口相关库window
    {"graphics",luaopen_graphics},// 游戏图形库graphics
    {NULL, NULL}
};

// 这里是加载所有自定义包
void CreateCustomLualibs(lua_State *L) {
    const luaL_Reg *lib;
    for (lib = loadedlibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }
    register_lua_global_funcs(L);
}
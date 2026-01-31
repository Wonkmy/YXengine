# YXengine
A Simple GameEngine Use Clion

一些常用的代码逻辑写法，由于不想放在main.c中，但是又不想删除，所以临时放在这里，
直接粘贴到对应的地方就能使用

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
// }~~~~
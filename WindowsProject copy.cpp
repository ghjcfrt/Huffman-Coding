#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <string>
#include "Huffman coding.h"
#include <filesystem>

using namespace std;

#define FilePatchShow 1002
#define ReadFile 1003
#define SaveFile 1004
#define CompressData 1007
#define DecompressData 1008

int Operation_Record = 0;
string fileName;
string fileExtension;
string content;
string key;
filesystem::path parentPath;
filesystem::path outputPath;
string decompressedData;
string compressedData;
Node *root;

HWND hEdit; // 全局变量声明
HWND hEdit2;

// 函数原型
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// WinMain函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 注册窗口类
    const char CLASS_NAME[] = "哈弗曼编码";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // 创建窗口
    HWND hwnd = CreateWindowEx(
        0,                   // 可选窗口样式
        CLASS_NAME,          // 窗口类名
        "My Window",         // 窗口文本
        WS_OVERLAPPEDWINDOW, // 窗口样式

        // 大小和位置
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 350,

        NULL,      // 父窗口
        NULL,      // 菜单
        hInstance, // 实例句柄
        NULL       // 附加应用程序数据
    );
    // 创建编辑框来显示文件路径
    hEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,                                   // 扩展样式
        "EDIT",                                             // 类名
        "",                                                 // 初始文本为空
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER, // 样式为编辑框
        100, 50, 200, 20,                                   // 位置和大小
        hwnd,                                               // 父窗口句柄
        (HMENU)FilePatchShow,                               // 控件标识符
        hInstance,                                          // 实例句柄
        NULL                                                // 附加参数
    );
    // 创建编辑框来显示哈夫曼树文件路径
    hEdit2 = CreateWindowEx(
        WS_EX_CLIENTEDGE,                                   // 扩展样式
        "EDIT",                                             // 类名
        "",                                                 // 初始文本为空
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER, // 样式为编辑框
        100, 80, 200, 20,                                   // 位置和大小
        hwnd,                                               // 父窗口句柄
        (HMENU)FilePatchShow,                               // 控件标识符
        hInstance,                                          // 实例句柄
        NULL                                                // 附加参数
    );
    // 文件读取按钮
    HWND btnReadFile = CreateWindow(
        "BUTTON",                                              // 类名
        "文件读取",                                            // 按钮文本
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // 按钮样式
        60, 120, 100, 30,                                      // 位置和大小
        hwnd,                                                  // 父窗口句柄
        (HMENU)ReadFile,                                       // 菜单句柄
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),     // 实例句柄
        NULL                                                   // 附加数据
    );
    // 压缩数据按钮
    HWND btnCompressData = CreateWindow(
        "BUTTON",                                              // 类名
        "压缩数据",                                            // 按钮文本
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // 按钮样式
        230, 180, 100, 30,                                     // 位置和大小
        hwnd,                                                  // 父窗口句柄
        (HMENU)CompressData,                                   // 菜单句柄
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),     // 实例句柄
        NULL                                                   // 附加数据
    );
    // 解码数据按钮
    HWND btnDecompressData = CreateWindow(
        "BUTTON",                                              // 类名
        "解码数据",                                            // 按钮文本
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // 按钮样式
        230, 240, 100, 30,                                     // 位置和大小
        hwnd,                                                  // 父窗口句柄
        (HMENU)DecompressData,                                 // 菜单句柄
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),     // 实例句柄
        NULL                                                   // 附加数据
    );
    // 文件保存按钮
    HWND btnSaveFile = CreateWindow(
        "BUTTON",                                              // 类名
        "文件保存",                                            // 按钮文本
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // 按钮样式
        230, 120, 100, 30,                                     // 位置和大小
        hwnd,                                                  // 父窗口句柄
        (HMENU)SaveFile,                                       // 菜单句柄
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),     // 实例句柄
        NULL                                                   // 附加数据
    );
    if (hwnd == NULL)
    {
        return 0;
    }

    // 显示窗口
    ShowWindow(hwnd, nCmdShow);

    // 运行消息循环
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        // 处理按钮点击事件
        if (LOWORD(wParam) == ReadFile) // 当点击文件读取按钮时
        {
            // 获取文件路径
            char filePath[MAX_PATH]; // 文件路径
            string jsonFilePath="";     // json文件路径
            GetWindowText(hEdit, filePath, MAX_PATH);
            // 获取父路径
            parentPath = filesystem::path(filePath).parent_path();
            // 在父路径下查找同名json文件
            jsonFilePath = (parentPath / (filesystem::path(filePath).stem().string() + ".json")).string();
            if (filesystem::exists(jsonFilePath))
            {
                // 找到同名json文件
                SetWindowText(hEdit2, jsonFilePath.c_str());
            }
            else
            {
                jsonFilePath ="";
            }
            // 读取文件
            ifstream file(filePath);
            BOOL bIsEmpty = GetWindowTextLength(hEdit2);
            if (file.is_open())
            {
                Operation_Record = 0;
                readFile(filePath, content);                                     // 读取文件内容
                fileName = filesystem::path(filePath).stem().string();           // 保存文件名
                fileExtension = filesystem::path(filePath).extension().string(); // 保存文件扩展名
                MessageBox(hwnd, "文件已读取。", "提示", MB_OK | MB_ICONINFORMATION);
                /*if (keyfile.is_open())
                {

                    // 读取json文件内容
                    string jsonContent;
                    getline(keyfile, jsonContent);

                    // 从JSON文件构建哈夫曼树
                    root = buildHuffmanTreeFromJson(jsonContent);

                    // 关闭key文件
                    keyfile.close();
                    MessageBox(hwnd, "哈夫曼树已从JSON文件读取。", "提示", MB_OK | MB_ICONINFORMATION);
                }
                else if (jsonFilePath != "")
                {
                    // 弹出JSON文件打开失败的错误消息框
                    MessageBox(hwnd, "JSON文件无法打开。", "错误", MB_OK | MB_ICONERROR);
                }*/
            }
            else
            {
                // 弹出文件打开失败的错误消息框
                MessageBox(hwnd, "无法打开文件。", "错误", MB_OK | MB_ICONERROR);
            }
        }
        else if (LOWORD(wParam) == SaveFile) // 当点击文件保存按钮时
        {
            // 获取文件路径
            char filePath[MAX_PATH];
            GetWindowText(hEdit, filePath, MAX_PATH);

            // 保存文件
            ofstream file(filePath);
            if (file.is_open())
            {
                if (Operation_Record == 0)
                {
                    MessageBox(hwnd, "尚未进行操作，无法保存！", "错误", MB_OK | MB_ICONERROR);
                }
                else if (Operation_Record == 1007)
                {
                    // 压缩文件路径
                    string compressedFilename;
                    // 在父路径下创建Out文件夹
                    outputPath = parentPath / "Out";
                    filesystem::create_directory(outputPath);
                    // 如果文件已经存在，则添加序号
                    int index = 1;
                    while (ifstream(compressedFilename).good())
                    {
                        compressedFilename = (outputPath / (fileName + to_string(index) + fileExtension)).string();
                        index++;
                    }
                    saveToFile(compressedFilename, compressedData); // 保存压缩数据到文件
                    MessageBox(hwnd, "数据压缩保存成功！", "提示", MB_OK | MB_ICONINFORMATION);
                    string huffmanTreeJson = saveHuffmanTreeToJson(root);                                         // 保存哈夫曼树到json
                    string huffmanTreeFilename = (outputPath / (fileName + to_string(index) + ".json")).string(); // 哈夫曼树文件路径
                    saveToFile(huffmanTreeFilename, huffmanTreeJson);                                             // 保存哈夫曼树到文件
                }
                else if (Operation_Record == 1008)
                {
                    int index = 1;
                    // 解压缩文件路径
                    string decompressedFilename;
                    // 如果文件已经存在，则添加序号
                    index = 1;
                    while (ifstream(decompressedFilename).good())
                    {
                        decompressedFilename = (outputPath / (fileName + to_string(index) + fileExtension)).string();
                        index++;
                    }
                    saveToFile(decompressedFilename, decompressedData); // 保存解压数据到文件

                    MessageBox(hwnd, "数据解压保存成功！", "提示", MB_OK | MB_ICONINFORMATION);
                }
                file.close();
            }
            else
            {
                // 弹出文件创建失败的错误消息框
                MessageBox(hwnd, "无法创建文件。", "错误", MB_OK | MB_ICONERROR);
            }
        }
        else if (LOWORD(wParam) == CompressData) // 点击压缩数据按钮
        {
            unordered_map<char, int> frequencies = calculateFrequencies(content); // 计算字符频率
            MessageBox(hwnd, "频率分析成功！", "提示", MB_OK | MB_ICONINFORMATION);
            Node *root = buildHuffmanTree(frequencies);    // 构建哈夫曼树
            unordered_map<char, string> codes;             // 生成哈夫曼编码
            generateCodes(root, "", codes);                // 生成哈夫曼编码
            compressedData = compressData(content, codes); // 压缩数据
            Operation_Record = 1007;
            MessageBox(hwnd, "数据压缩成功！", "提示", MB_OK | MB_ICONINFORMATION);
        }
        else if (LOWORD(wParam) == DecompressData) // 点击解压数据按钮
        {
            decompressedData = decompressData(content, root); // 解压缩数据
            Operation_Record = 1008;
            MessageBox(hwnd, "数据解压成功！", "提示", MB_OK | MB_ICONINFORMATION);
        }
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

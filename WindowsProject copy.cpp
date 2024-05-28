#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>
#include "Huffman coding.h"

using namespace std;

#define FilePatchShow 1002
#define ReadFile 1003
#define SaveFile 1004
#define FrequencyAnalysis 1005
#define HuffmanEncoding 1006
#define CompressData 1007
#define DecompressData 1008

int Operation_Record = 0;

HWND hEdit; // 全局变量声明

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
    // 频率分析按钮
    HWND btnFrequencyAnalysis = CreateWindow(
        "BUTTON",
        "频率分析",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        60, 180, 100, 30,
        hwnd,
        (HMENU)FrequencyAnalysis,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);
    // 哈夫曼编码按钮
    HWND btnHuffmanEncoding = CreateWindow(
        "BUTTON",
        "哈夫曼编码",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        60, 240, 100, 30,
        hwnd,
        (HMENU)HuffmanEncoding,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);
    // 压缩数据按钮
    HWND btnCompressData = CreateWindow(
        "BUTTON",
        "压缩数据",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        230, 180, 100, 30,
        hwnd,
        (HMENU)CompressData,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);
    // 解码数据按钮
    HWND btnDecompressData = CreateWindow(
        "BUTTON",
        "解码数据",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        230, 240, 100, 30,
        hwnd,
        (HMENU)DecompressData,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);
    // 文件保存按钮
    HWND btnSaveFile = CreateWindow(
        "BUTTON",
        "文件保存",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        230, 120, 100, 30,
        hwnd,
        (HMENU)SaveFile,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);
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
            char filePath[MAX_PATH];
            GetWindowText(hEdit, filePath, MAX_PATH);

            // 读取文件
            ifstream file(filePath);
            if (file.is_open())
            {
                Operation_Record = 0;
                MessageBox(hwnd, "文件已读取。", "提示", MB_OK | MB_ICONINFORMATION);
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
                else if (Operation_Record == 1005)
                {
                    MessageBox(hwnd, "已进行频率分析，无法保存！", "错误", MB_OK | MB_ICONERROR);
                }
                else if (Operation_Record == 1006)
                {
                    MessageBox(hwnd, "已进行哈夫曼编码，无法保存！", "错误", MB_OK | MB_ICONERROR);
                }
                else if (Operation_Record == 1007)
                {
                    // 压缩文件路径
                    string compressedFilename = "D:\\code\\1.5\\compressed.txt";
                    // 如果文件已经存在，则添加序号
                    int index = 1;
                    while (ifstream(compressedFilename).good())
                    {
                        compressedFilename = "D:\\code\\1.5\\compressed" + to_string(index) + ".txt";
                        index++;
                    }
                    saveToFile(compressedFilename, compressedData); // 保存压缩数据到文件
                    MessageBox(hwnd, "数据压缩保存成功！", "提示", MB_OK | MB_ICONINFORMATION);
                }
                else if (Operation_Record == 1008)
                {
                    int index = 1;
                    // 解压缩文件路径
                    string decompressedFilename = "D:\\code\\1.5\\decompressed.txt";
                    // 如果文件已经存在，则添加序号
                    index = 1;
                    while (ifstream(decompressedFilename).good())
                    {
                        decompressedFilename = "D:\\code\\1.5\\decompressed" + to_string(index) + ".txt";
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

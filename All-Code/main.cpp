#include <windows.h>
#include <iostream>
#include <string>
#include "Huffman coding.h"
#include "window_utils.h"
#include <filesystem>

using namespace std;

#define FilePatchShow 1002
#define ReadFile 1003
#define SaveFile 1004
#define CompressData 1007
#define DecompressData 1008

int Operation_Record = 0;
int key_exists = 0;
string fileName;                     // 文件名
string fileExtension;                // 文件扩展名
string content;                      // 文件内容
filesystem::path parentPath;         // 父路径
filesystem::path outputPath;         // 输出路径
string decompressedData;             // 解压数据
string compressedData;               // 压缩数据
Node *root = nullptr;                // 哈夫曼树的根节点
unordered_map<string, string> codes; // 生成哈夫曼编码

HWND hEdit;  //  文件路径编辑框
HWND hEdit2; //  json文件路径编辑框

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~封装代码~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// 处理文件读取按钮点击事件
void OnReadFile(HWND hwnd)
{
    key_exists = 0;
    char filePath[MAX_PATH];  // 文件路径
    string jsonFilePath = ""; // json文件路径
    GetWindowText(hEdit, filePath, MAX_PATH);
    // 获取父路径
    parentPath = filesystem::path(filePath).parent_path();
    // 在父路径下查找同名json文件
    jsonFilePath = (parentPath / (filesystem::path(filePath).stem().string() + ".json")).string();
    if (filesystem::exists(jsonFilePath))
    {
        // 找到同名json文件
        key_exists = 1;
        SetWindowText(hEdit2, jsonFilePath.c_str());
    }
    else
    {
        jsonFilePath = "";
    }
    // 读取文件
    ifstream file(filePath);
    BOOL bIsEmpty = GetWindowTextLength(hEdit2);
    if (file.is_open())
    {
        Operation_Record = 0;
        readFile(filePath, content); // 读取文件内容
        // 重置哈夫曼树为空
        root = nullptr;
        // 判断文件内容是否为空
        if (file.peek() == ifstream::traits_type::eof())
        {
            MessageBox(hwnd, "文件内容为空。", "警告", MB_OK | MB_ICONERROR);
            return;
        }
        fileName = filesystem::path(filePath).stem().string();           // 保存文件名
        fileExtension = filesystem::path(filePath).extension().string(); // 保存文件扩展名
        MessageBox(hwnd, "文件已读取。", "提示", MB_OK | MB_ICONINFORMATION);
        // 如果找到同名json文件，则读取哈夫曼编码
        if (key_exists == 1)
        {
            ifstream keyfile(jsonFilePath);
            if (keyfile.is_open())
            {
                codes = loadHuffmanCodesFromJson(keyfile); // 从json文件读取哈夫曼编码
                keyfile.close();
                MessageBox(hwnd, "哈夫曼编码已从JSON文件读取。", "提示", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                // 弹出文件打开失败的错误消息框
                MessageBox(hwnd, "无法打开json。", "错误", MB_OK | MB_ICONERROR);
            }
        }
    }
    else
    {
        // 弹出文件打开失败的错误消息框
        MessageBox(hwnd, "无法打开文件。", "错误", MB_OK | MB_ICONERROR);
    }
}

// 处理文件保存按钮点击事件
void OnSaveFile(HWND hwnd)
{
    char filePath[MAX_PATH];
    GetWindowText(hEdit, filePath, MAX_PATH);
    // 保存文件
    ifstream file(filePath);
    if (file.is_open())
    {
        // 在父路径下创建Out文件夹
        outputPath = parentPath / "Out";
        // 创建Out文件夹
        filesystem::create_directory(outputPath);
        if (Operation_Record == 0) // 未进行操作
        {
            MessageBox(hwnd, "尚未进行操作，无法保存！", "错误", MB_OK | MB_ICONERROR);
            return;
        }
        else if (Operation_Record == 1007) // 压缩
        {
            // 压缩文件路径
            string compressedFilename = (outputPath / (fileName + fileExtension)).string();
            // 哈夫曼编码文件路径
            string huffmanCodesFilename = (outputPath / (fileName + ".json")).string();
            // 如果文件已经存在，则添加序号
            int file_index = 1;
            while (ifstream(compressedFilename).good()) // 判断压缩文件是否存在
            {
                compressedFilename = (outputPath / (fileName + to_string(file_index) + fileExtension)).string();
                file_index++;
            }
            int key_index = 1;
            while (ifstream(huffmanCodesFilename).good()) // 判断key文件是否存在
            {
                huffmanCodesFilename = (outputPath / (fileName + to_string(key_index) + ".json")).string();
                key_index++;
            }
            saveToFile(compressedFilename, compressedData); // 保存压缩数据到文件
            MessageBox(hwnd, "数据压缩保存成功！", "提示", MB_OK | MB_ICONINFORMATION);

            saveHuffmanCodesToJson(codes, huffmanCodesFilename); // 保存哈夫曼编码到文件
        }
        else if (Operation_Record == 1008) // 解压
        {
            int index = 1;
            // 解压缩文件路径
            string decompressedFilename = (outputPath / (fileName + fileExtension)).string();
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
// 处理压缩数据按钮点击事件
void OnbtnCompressData(HWND hwnd)
{
    unordered_map<char, int> frequencies = calculateFrequencies(content); // 计算字符频率
    MessageBox(hwnd, "频率分析成功！", "提示", MB_OK | MB_ICONINFORMATION);
    root = buildHuffmanTree(frequencies); // 构建哈夫曼树

    generateCodes(root, "", codes);                // 生成哈夫曼编码
    compressedData = compressData(content, codes); // 压缩数据
    Operation_Record = 1007;
    MessageBox(hwnd, "数据压缩成功！", "提示", MB_OK | MB_ICONINFORMATION);
}
// 处理解压数据按钮点击事件
void OnbtnDecompressData(HWND hwnd)
{
    decompressedData = decompressData(compressedData, codes); // 解压缩数据
    Operation_Record = 1008;
    MessageBox(hwnd, "数据解压成功！", "提示", MB_OK | MB_ICONINFORMATION);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~封装代码~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
                                         *
                                         *
                                        隔离
                                         *
                                         *
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~代码主体~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
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
    // wc.hbrBackground = CreateSolidBrush(RGB(163, 188, 216)); // 设置窗口背景颜色

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
    hEdit = CreateEdit(hwnd, 100, 50, FilePatchShow, hInstance);
    CreateIcon(hwnd, 80, 50, hInstance, TEXT("%SystemRoot%\\System32\\SHELL32.dll"), 0);
    hEdit2 = CreateEdit(hwnd, 100, 90, FilePatchShow, hInstance);
    CreateIcon(hwnd, 84, 90, hInstance, TEXT("%SystemRoot%\\System32\\mstsc.exe"), 13);
    HWND btnReadFile = CreateButton(hwnd, 60, 150, ReadFile, "文件读取", hInstance);
    HWND btnCompressData = CreateButton(hwnd, 60, 210, CompressData, "压缩数据", hInstance);
    HWND btnDecompressData = CreateButton(hwnd, 230, 210, DecompressData, "解码数据", hInstance);
    HWND btnSaveFile = CreateButton(hwnd, 230, 150, SaveFile, "文件保存", hInstance);
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

// 处理窗口消息
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ReadFile: // 当点击文件读取按钮时
            OnReadFile(hwnd);
            break;
        case SaveFile: // 当点击文件保存按钮时
            OnSaveFile(hwnd);
            break;
        case CompressData: // 点击压缩数据按钮
            OnbtnCompressData(hwnd);
            break;
        case DecompressData: // 点击解压数据按钮
            if (key_exists == 0)
            {
                MessageBox(hwnd, "未找到哈夫曼编码文件！", "错误", MB_OK | MB_ICONERROR);
                break;
            }
            else
            {
                OnbtnDecompressData(hwnd);
            }
            break;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

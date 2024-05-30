// 主函数
#include <windows.h>
#include <iostream>
#include <string>
#include "Huffman coding.h" //哈夫曼编码头文件
#include "window_utils.h"   //控件创建函数
#include <filesystem>

using namespace std;

#define FilePatchShow 1002  // 文件路径显示框ID
#define ReadFile 1003       // 读取ID
#define SaveFile 1004       // 保存ID
#define CompressData 1007   // 压缩ID
#define DecompressData 1008 // 解压ID

int Operation_Record = 0;            // 操作记录
int key_exists = 0;                  // key文件是否存在
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
    key_exists = 0; // 重置key文件

    // 文件路径
    char filePath[MAX_PATH] = "";             // 文件路径
    string jsonFilePath = "";                 // json文件路径
    GetWindowText(hEdit, filePath, MAX_PATH); // 获取文件路径
    // 获取父路径
    parentPath = filesystem::path(filePath).parent_path(); // 父路径

    // 在父路径下查找同名json文件
    jsonFilePath = (parentPath / (filesystem::path(filePath).stem().string() + ".json")).string(); // json文件路径
    if (filesystem::exists(jsonFilePath))                                                          // 如果json文件存在
    {
        // 找到同名json文件
        key_exists = 1;                              // 找到key文件
        SetWindowText(hEdit2, jsonFilePath.c_str()); // 显示json文件路径
    }
    else
    {
        jsonFilePath = ""; // json文件路径为空
    }

    Operation_Record = 0; // 重置操作记录

    // 读取文件内容
    // 判断文件路径是否为空
    if (filePath[0] == '\0') // 如果文件路径为空
    {
        MessageBox(hwnd, "文件路径为空。", "警告", MB_OK | MB_ICONERROR);
        return;
    }
    else
    {
        readFile(filePath, content); // 读取文件内容
    }

    // 判断文件内容是否为空
    if (content.empty()) // 如果文件内容为空
    {
        MessageBox(hwnd, "文件内容为空。", "警告", MB_OK | MB_ICONERROR);
        return;
    }

    fileName = filesystem::path(filePath).stem().string();           // 保存文件名
    fileExtension = filesystem::path(filePath).extension().string(); // 保存文件扩展名
    Operation_Record = 1003;                                         // 文件已读取

    // 如果找到同名json文件，则读取哈夫曼编码
    if (key_exists == 1) // 如果key文件存在
    {
        // 读取json文件
        ifstream keyfile;                        // key文件
        if (openJsonFile(jsonFilePath, keyfile)) // 打开json文件
        {
            codes = loadHuffmanCodesFromJson(keyfile); // 从json文件读取哈夫曼编码
            keyfile.close();                           // 关闭文件
            MessageBox(hwnd, "哈夫曼编码已从JSON文件读取。", "提示", MB_OK | MB_ICONINFORMATION);
        }
    }
}

// 处理文件保存按钮点击事件
void OnSaveFile(HWND hwnd)
{
    // 保存文件路径
    char filePath[MAX_PATH];                  // 文件路径
    GetWindowText(hEdit, filePath, MAX_PATH); // 获取文件路径

    // 保存文件
    ifstream file(filePath); // 打开文件
    if (file.is_open())      // 文件打开成功
    {
        // 父路径下的Out文件夹
        outputPath = parentPath / "Out";          // 在父路径下创建Out文件夹
        filesystem::create_directory(outputPath); // 创建Out文件夹

        if (Operation_Record == 0) // 未进行操作
        {
            MessageBox(hwnd, "尚未进行操作，无法保存！", "错误", MB_OK | MB_ICONERROR);
            return;
        }
        else if (Operation_Record == 1007) // 压缩
        {
            // 路径
            string compressedFilename = (outputPath / (fileName + fileExtension)).string(); // 压缩文件路径
            string huffmanCodesFilename = (outputPath / (fileName + ".json")).string();     // 哈夫曼编码文件路径

            // 如果文件已经存在，则添加序号
            int file_index = 1;
            while (ifstream(compressedFilename).good()) // 判断压缩文件是否存在
            {
                compressedFilename = (outputPath / (fileName + to_string(file_index) + fileExtension)).string();
                file_index++;
            }

            int key_index = 1;                            // 文件序号
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
            int index = 1; // 文件序号

            // 解压文件路径
            string decompressedFilename = (outputPath / (fileName + fileExtension)).string();

            // 如果文件已经存在，则添加序号
            while (ifstream(decompressedFilename).good()) // 判断解压文件是否存在
            {
                decompressedFilename = (outputPath / (fileName + to_string(index) + fileExtension)).string();
                index++;
            }

            saveToFile(decompressedFilename, decompressedData); // 保存解压数据到文件
            MessageBox(hwnd, "数据解压保存成功！", "提示", MB_OK | MB_ICONINFORMATION);
        }
        file.close(); // 关闭文件
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
    if (Operation_Record != 1003) // 未读取文件
    {
        MessageBox(hwnd, "请先读取文件！", "错误", MB_OK | MB_ICONERROR);
        return;
    }

    // 哈夫曼操作
    unordered_map<char, int> frequencies = calculateFrequencies(content); // 计算字符频率
    MessageBox(hwnd, "频率分析成功！", "提示", MB_OK | MB_ICONINFORMATION);

    root = buildHuffmanTree(frequencies); // 构建哈夫曼树
    MessageBox(hwnd, "哈夫曼树构建成功！", "提示", MB_OK | MB_ICONINFORMATION);

    generateCodes(root, "", codes); // 生成哈夫曼编码
    MessageBox(hwnd, "哈夫曼编码生成成功！", "提示", MB_OK | MB_ICONINFORMATION);

    // 压缩数据
    compressedData = compressData(content, codes); // 压缩数据
    Operation_Record = 1007;                       // 进行了压缩操作
    MessageBox(hwnd, "数据压缩成功！", "提示", MB_OK | MB_ICONINFORMATION);
}
// 处理解压数据按钮点击事件
void OnbtnDecompressData(HWND hwnd)
{
    if (key_exists == 0) // 未找到key文件
    {
        MessageBox(hwnd, "未找到哈夫曼编码文件！", "错误", MB_OK | MB_ICONERROR);
        return;
    }
    // 解压缩数据
    decompressedData = decompressData(compressedData, codes); // 解压缩数据
    Operation_Record = 1008;                                  // 进行了解压操作
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
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // 处理窗口消息

// WinMain函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) // 程序入口
{
    // 注册窗口类
    const char CLASS_NAME[] = "哈弗曼编码";

    WNDCLASS wc = {}; // 窗口类

    wc.lpfnWndProc = WindowProc;   // 窗口处理函数
    wc.hInstance = hInstance;      // 实例句柄
    wc.lpszClassName = CLASS_NAME; // 类名

    RegisterClass(&wc); // 注册窗口类

    // 创建窗口
    HWND hwnd = CreateWindowEx(
        0,                                      // 可选窗口样式
        CLASS_NAME,                             // 窗口类名
        "哈弗曼编解码程序",                     // 窗口文本
        WS_OVERLAPPEDWINDOW,                    // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 350, // 大小和位置
        NULL,                                   // 父窗口
        NULL,                                   // 菜单
        hInstance,                              // 实例句柄
        NULL                                    // 附加应用程序数据
    );
    // 创建窗口控件
    hEdit = CreateEdit(hwnd, 100, 50, FilePatchShow, hInstance);                                  // 创建文件路径编辑框
    CreateIcon(hwnd, 80, 50, hInstance, TEXT("%SystemRoot%\\System32\\SHELL32.dll"), 0);          // 创建文件路径图标
    hEdit2 = CreateEdit(hwnd, 100, 90, FilePatchShow, hInstance);                                 // 创建json文件路径编辑框
    CreateIcon(hwnd, 84, 90, hInstance, TEXT("%SystemRoot%\\System32\\mstsc.exe"), 13);           // 创建json文件路径图标
    HWND btnReadFile = CreateButton(hwnd, 60, 150, ReadFile, "文件读取", hInstance);              // 创建文件读取按钮
    HWND btnCompressData = CreateButton(hwnd, 60, 210, CompressData, "压缩数据", hInstance);      // 创建压缩数据按钮
    HWND btnDecompressData = CreateButton(hwnd, 230, 210, DecompressData, "解码数据", hInstance); // 创建解码数据按钮
    HWND btnSaveFile = CreateButton(hwnd, 230, 150, SaveFile, "文件保存", hInstance);             // 创建文件保存按钮

    // 创建窗口失败
    if (hwnd == NULL)
    {
        return 0;
    }

    // 显示窗口
    ShowWindow(hwnd, nCmdShow);

    // 运行消息循环
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) // 获取消息
    {
        TranslateMessage(&msg); // 翻译消息
        DispatchMessage(&msg);  // 分发消息
    }

    return 0;
}

// 处理窗口消息
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) // 消息类型
    {
    case WM_COMMAND: // 当点击按钮时
    {
        switch (LOWORD(wParam)) // 按钮ID
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
            OnbtnDecompressData(hwnd);
            break;
        }
        break;
    }
    case WM_DESTROY:        // 当窗口关闭时
        PostQuitMessage(0); // 退出消息循环
        return 0;
    default:                                              // 默认情况
        return DefWindowProc(hwnd, uMsg, wParam, lParam); // 默认窗口处理函数
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); // 默认窗口处理函数
}

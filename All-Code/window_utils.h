#include <windows.h>
#include <commctrl.h>

// 定义常量
const int EDIT_WIDTH = 200;   // 编辑框的宽度
const int EDIT_HEIGHT = 20;   // 编辑框的高度
const int BUTTON_WIDTH = 100; // 按钮的宽度
const int BUTTON_HEIGHT = 30; // 按钮的高度
const int ICON_WIDTH = 20;    // 图标的宽度

// 创建编辑框
HWND CreateEdit(HWND hwndParent, int x, int y, int id, HINSTANCE hInst)
{
    return CreateWindowEx(
        WS_EX_CLIENTEDGE,                                   // 给编辑框添加边框
        "EDIT",                                             // 编辑框的类名
        "",                                                 // 编辑框的内容
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER, // 编辑框的风格
        x, y,                                               // 编辑框的位置
        EDIT_WIDTH, EDIT_HEIGHT,                            // 编辑框的大小
        hwndParent,                                         // 父窗口句柄
        (HMENU)id,                                          // 编辑框的ID
        hInst,                                              // 实例句柄
        NULL                                                // 附加数据
    );
}

// 创建按钮
HWND CreateButton(HWND hwndParent, int x, int y, int id, const char *text, HINSTANCE hInst)
{
    return CreateWindow(
        "BUTTON",                                              // 按钮的类名
        text,                                                  // 按钮的文本
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // 按钮的风格
        x, y,                                                  // 按钮的位置
        BUTTON_WIDTH, BUTTON_HEIGHT,                           // 按钮的大小
        hwndParent,                                            // 父窗口句柄
        (HMENU)id,                                             // 按钮的ID
        hInst,                                                 // 实例句柄
        NULL                                                   // 附加数据
    );
}

// 创建图标的函数
HWND CreateIcon(HWND hwndParent, int x, int y, HINSTANCE hInst, const char *filePath, int iconIndex)
{
    HICON hIcon = ExtractIcon(hInst, filePath, iconIndex); // 从文件中提取图标

    // 等比缩放图标
    int iconHeight = 20;                                                                                 // 图标的高度
    int iconWidth = MulDiv(GetSystemMetrics(SM_CXICON), iconHeight, GetSystemMetrics(SM_CYICON));        // 计算图标的宽度
    HICON hScaledIcon = (HICON)CopyImage(hIcon, IMAGE_ICON, iconWidth, iconHeight, LR_COPYFROMRESOURCE); // 缩放图标

    // 创建静态控件
    HWND hWnd = CreateWindowEx(
        0,                                                        // 扩展风格
        WC_STATIC,                                                // 静态控件的类名
        NULL, WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZEIMAGE, // 风格
        x, y,                                                     // 位置
        iconWidth, iconHeight,                                    // 大小
        hwndParent,                                               // 父窗口句柄
        NULL,                                                     // 菜单句柄
        hInst,                                                    // 实例句柄
        NULL                                                      // 附加数据
    );
    
    SendMessage(hWnd, STM_SETICON, (WPARAM)hScaledIcon, 0);// 设置图标
    DestroyIcon(hIcon);// 销毁图标
    return hWnd;
}

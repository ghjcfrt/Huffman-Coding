#include <windows.h>
#include <commctrl.h>
// 定义常量
const int EDIT_WIDTH = 200;
const int EDIT_HEIGHT = 20;
const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 30;
const int ICON_WIDTH = 20;

// 创建编辑框
HWND CreateEdit(HWND hwndParent, int x, int y, int id, HINSTANCE hInst)
{
    return CreateWindowEx(
        WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
        x, y, EDIT_WIDTH, EDIT_HEIGHT, hwndParent, (HMENU)id, hInst, NULL);
}

// 创建按钮
HWND CreateButton(HWND hwndParent, int x, int y, int id, const char *text, HINSTANCE hInst)
{
    return CreateWindow(
        "BUTTON", text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y, BUTTON_WIDTH, BUTTON_HEIGHT, hwndParent, (HMENU)id, hInst, NULL);
}

// 创建图标的函数
HWND CreateIcon(HWND hwndParent, int x, int y, HINSTANCE hInst, const char *filePath, int iconIndex)
{
    HICON hIcon = ExtractIcon(hInst, filePath, iconIndex);                                               // 从文件中提取图标
    int iconHeight = 20;                                                                                 // 图标的高度
    int iconWidth = MulDiv(GetSystemMetrics(SM_CXICON), iconHeight, GetSystemMetrics(SM_CYICON));        // 计算图标的宽度
    HICON hScaledIcon = (HICON)CopyImage(hIcon, IMAGE_ICON, iconWidth, iconHeight, LR_COPYFROMRESOURCE); // 缩放图标
    // 等比缩放图标
    // HICON hScaledIcon = (HICON)CopyImage(hIcon, IMAGE_ICON, iconWidth, iconHeight, LR_COPYFROMRESOURCE); // 等比例缩放图标

    HWND hWnd = CreateWindowEx(
        0,
        WC_STATIC,
        NULL, WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZEIMAGE,
        x, y, iconWidth, iconHeight, hwndParent, NULL, hInst, NULL);
    SendMessage(hWnd, STM_SETICON, (WPARAM)hScaledIcon, 0);
    DestroyIcon(hIcon);
    return hWnd;
}

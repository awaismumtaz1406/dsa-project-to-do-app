#define _WIN32_WINNT 0x0601  // Windows 7 or higher
#define _WIN32_IE 0x0500     // Needed for InitCommonControlsEx

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <sstream>
#include "Database.h"
#include "TaskQueue.h"

#pragma comment(lib, "comctl32.lib")

using namespace std;

// Global variables
HINSTANCE hInst;
HWND hwndMain;
HWND hwndLogin;
Database db;
TaskQueue taskQueue;
int currentUserId = -1;
string currentUsername;

// Control IDs
#define IDC_LOGIN_USER 1001
#define IDC_LOGIN_PASS 1002
#define IDC_BTN_LOGIN 1003
#define IDC_BTN_REGISTER 1004
#define IDC_TASK_LIST 2001
#define IDC_TASK_TITLE 2002
#define IDC_TASK_DESC 2003
#define IDC_BTN_ADD 2004
#define IDC_BTN_COMPLETE 2005
#define IDC_BTN_DELETE 2006
#define IDC_BTN_REFRESH 2007
#define IDC_STATUS_BAR 2008
#define IDC_BTN_STATS 2009

// Function declarations
LRESULT CALLBACK LoginWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
void CreateLoginWindow(HINSTANCE hInstance);
void CreateMainWindow(HINSTANCE hInstance);
void RefreshTaskList();
void LoadTasksIntoQueue();
void UpdateStatusBar();

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Connect to database
    if (!db.connect("todoapp.db")) {
        MessageBox(NULL, "Failed to connect to database!", "Error", MB_ICONERROR);
        return 0;
    }

    // Create login window
    CreateLoginWindow(hInstance);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    db.close();
    return (int)msg.wParam;
}

// ==================== Create Login Window ====================
void CreateLoginWindow(HINSTANCE hInstance) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = LoginWndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "LoginWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    hwndLogin = CreateWindowEx(
        0,
        "LoginWindowClass",
        "TODO App - Login",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, hInstance, NULL
    );

    CreateWindow("STATIC", "TODO App - Task Manager",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        50, 20, 300, 30,
        hwndLogin, NULL, hInstance, NULL);

    CreateWindow("STATIC", "Username:",
        WS_CHILD | WS_VISIBLE,
        50, 70, 80, 20,
        hwndLogin, NULL, hInstance, NULL);

    CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        140, 68, 200, 24,
        hwndLogin, (HMENU)IDC_LOGIN_USER, hInstance, NULL);

    CreateWindow("STATIC", "Password:",
        WS_CHILD | WS_VISIBLE,
        50, 110, 80, 20,
        hwndLogin, NULL, hInstance, NULL);

    CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD,
        140, 108, 200, 24,
        hwndLogin, (HMENU)IDC_LOGIN_PASS, hInstance, NULL);

    CreateWindow("BUTTON", "Login",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        80, 160, 100, 35,
        hwndLogin, (HMENU)IDC_BTN_LOGIN, hInstance, NULL);

    CreateWindow("BUTTON", "Register",
        WS_CHILD | WS_VISIBLE,
        210, 160, 100, 35,
        hwndLogin, (HMENU)IDC_BTN_REGISTER, hInstance, NULL);

    ShowWindow(hwndLogin, SW_SHOW);
    UpdateWindow(hwndLogin);
}

// ==================== Create Main Window ====================
void CreateMainWindow(HINSTANCE hInstance) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "MainWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    hwndMain = CreateWindowEx(
        0,
        "MainWindowClass",
        "TODO App - Task Manager (DSA Project)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL, NULL, hInstance, NULL
    );

    string title = "Welcome, " + currentUsername + "!";
    CreateWindow("STATIC", title.c_str(),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, 10, 860, 25,
        hwndMain, NULL, hInstance, NULL);

    HWND hwndList = CreateWindowEx(
        0, WC_LISTVIEW, "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
        20, 50, 650, 400,
        hwndMain, (HMENU)IDC_TASK_LIST, hInstance, NULL);

    // Setup columns
    LVCOLUMN lvc = {0};
lvc.mask = LVCF_TEXT | LVCF_WIDTH;

lvc.pszText = (LPSTR)"ID"; lvc.cx = 50; ListView_InsertColumn(hwndList, 0, &lvc);
lvc.pszText = (LPSTR)"Title"; lvc.cx = 200; ListView_InsertColumn(hwndList, 1, &lvc);
lvc.pszText = (LPSTR)"Description"; lvc.cx = 250; ListView_InsertColumn(hwndList, 2, &lvc);
lvc.pszText = (LPSTR)"Status"; lvc.cx = 100; ListView_InsertColumn(hwndList, 3, &lvc);


    // Task input fields and buttons
    CreateWindow("STATIC", "Task Title:", WS_CHILD | WS_VISIBLE, 690, 50, 180, 20, hwndMain, NULL, hInstance, NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        690, 75, 180, 24, hwndMain, (HMENU)IDC_TASK_TITLE, hInstance, NULL);

    CreateWindow("STATIC", "Description:", WS_CHILD | WS_VISIBLE, 690, 110, 180, 20, hwndMain, NULL, hInstance, NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        690, 135, 180, 80, hwndMain, (HMENU)IDC_TASK_DESC, hInstance, NULL);

    CreateWindow("BUTTON", "Add Task\n(ENQUEUE)", WS_CHILD | WS_VISIBLE | BS_MULTILINE,
        690, 230, 180, 45, hwndMain, (HMENU)IDC_BTN_ADD, hInstance, NULL);

    CreateWindow("BUTTON", "Mark Complete", WS_CHILD | WS_VISIBLE,
        690, 285, 180, 35, hwndMain, (HMENU)IDC_BTN_COMPLETE, hInstance, NULL);

    CreateWindow("BUTTON", "Delete Task\n(REMOVE)", WS_CHILD | WS_VISIBLE | BS_MULTILINE,
        690, 330, 180, 45, hwndMain, (HMENU)IDC_BTN_DELETE, hInstance, NULL);

    CreateWindow("BUTTON", "Refresh List\n(TRAVERSE)", WS_CHILD | WS_VISIBLE | BS_MULTILINE,
        690, 385, 180, 45, hwndMain, (HMENU)IDC_BTN_REFRESH, hInstance, NULL);

    CreateWindow("BUTTON", "Queue Statistics", WS_CHILD | WS_VISIBLE,
        690, 440, 180, 35, hwndMain, (HMENU)IDC_BTN_STATS, hInstance, NULL);

    CreateWindowEx(0, STATUSCLASSNAME, "", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0, hwndMain, (HMENU)IDC_STATUS_BAR, hInstance, NULL);

    LoadTasksIntoQueue();
    RefreshTaskList();
    UpdateStatusBar();

    ShowWindow(hwndMain, SW_SHOW);
    UpdateWindow(hwndMain);
}

// ==================== Refresh Task List ====================
void RefreshTaskList() {
    HWND hwndList = GetDlgItem(hwndMain, IDC_TASK_LIST);
    ListView_DeleteAllItems(hwndList);

    vector<Task> tasks = taskQueue.getAllTasks();

    vector<string> ids(tasks.size()), titles(tasks.size()), descs(tasks.size()), statuses(tasks.size());

    for (size_t i = 0; i < tasks.size(); i++) {
        LVITEM lvi = {0};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;

        ids[i] = to_string(tasks[i].getId());
        lvi.pszText = &ids[i][0]; lvi.iSubItem = 0; ListView_InsertItem(hwndList, &lvi);

        titles[i] = tasks[i].getTitle(); lvi.pszText = &titles[i][0]; lvi.iSubItem = 1; ListView_SetItem(hwndList, &lvi);
        descs[i] = tasks[i].getDescription(); lvi.pszText = &descs[i][0]; lvi.iSubItem = 2; ListView_SetItem(hwndList, &lvi);
        statuses[i] = tasks[i].isCompleted() ? "Completed" : "Pending"; lvi.pszText = &statuses[i][0]; lvi.iSubItem = 3; ListView_SetItem(hwndList, &lvi);
    }

    UpdateStatusBar();
}

// ==================== Load Tasks into Queue ====================
void LoadTasksIntoQueue() {
    taskQueue.clear();
    vector<Task> tasks = db.getTasks(currentUserId);
    for (const auto& task : tasks) taskQueue.enqueue(task);
}

// ==================== Update Status Bar ====================
void UpdateStatusBar() {
    HWND hwndStatus = GetDlgItem(hwndMain, IDC_STATUS_BAR);
    stringstream ss;
    ss << "Queue Size: " << taskQueue.getTotalCount()
       << " | Pending: " << taskQueue.getPendingCount()
       << " | Completed: " << taskQueue.getCompletedCount()
       << " | DSA: Queue (Linked List)";
    SetWindowText(hwndStatus, ss.str().c_str());
}

// ==================== Login Window Procedure ====================
LRESULT CALLBACK LoginWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDC_BTN_LOGIN || LOWORD(wParam) == IDC_BTN_REGISTER) {
                char username[256], password[256];
                GetDlgItemText(hwnd, IDC_LOGIN_USER, username, 256);
                GetDlgItemText(hwnd, IDC_LOGIN_PASS, password, 256);

                if(strlen(username) == 0 || strlen(password) == 0) {
                    MessageBox(hwnd, "Enter username and password!", "Error", MB_ICONERROR);
                    break;
                }

                if(LOWORD(wParam) == IDC_BTN_REGISTER) {
                    if(db.registerUser(username, password))
                        MessageBox(hwnd, "Registration successful!", "Success", MB_ICONINFORMATION);
                    else
                        MessageBox(hwnd, "Registration failed!", "Error", MB_ICONERROR);
                } else {
                    if(db.loginUser(username, password)) {
                        currentUserId = db.getUserId(username);
                        currentUsername = username;
                        ShowWindow(hwnd, SW_HIDE);
                        CreateMainWindow(hInst);
                    } else {
                        MessageBox(hwnd, "Invalid username or password!", "Error", MB_ICONERROR);
                    }
                }
            }
            break;
        }
        case WM_CLOSE: DestroyWindow(hwnd); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ==================== Main Window Procedure ====================
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_COMMAND: {
            HWND hwndList = GetDlgItem(hwnd, IDC_TASK_LIST);
            switch(LOWORD(wParam)) {
                case IDC_BTN_ADD: {
                    char title[256], desc[512];
                    GetDlgItemText(hwnd, IDC_TASK_TITLE, title, 256);
                    GetDlgItemText(hwnd, IDC_TASK_DESC, desc, 512);
                    if(strlen(title) == 0) { MessageBox(hwnd,"Enter task title","Error",MB_ICONERROR); break; }
                    Task t(title, desc, 0);
                    if(db.addTask(currentUserId, t)) { LoadTasksIntoQueue(); RefreshTaskList(); SetDlgItemText(hwnd, IDC_TASK_TITLE,""); SetDlgItemText(hwnd,IDC_TASK_DESC,""); MessageBox(hwnd,"Task added!","Success",MB_ICONINFORMATION); }
                    break;
                }
                case IDC_BTN_COMPLETE: {
                    int sel = ListView_GetNextItem(hwndList,-1,LVNI_SELECTED);
                    if(sel==-1){ MessageBox(hwnd,"Select a task","Error",MB_ICONERROR); break; }
                    char id[32]; ListView_GetItemText(hwndList, sel,0,id,32);
                    if(db.completeTask(atoi(id))){ taskQueue.markComplete(atoi(id)); RefreshTaskList(); MessageBox(hwnd,"Marked complete","Success",MB_ICONINFORMATION); }
                    break;
                }
                case IDC_BTN_DELETE: {
                    int sel = ListView_GetNextItem(hwndList,-1,LVNI_SELECTED);
                    if(sel==-1){ MessageBox(hwnd,"Select a task","Error",MB_ICONERROR); break; }
                    char id[32]; ListView_GetItemText(hwndList, sel,0,id,32);
                    if(MessageBox(hwnd,"Delete this task?","Confirm",MB_YESNO|MB_ICONQUESTION)==IDYES){
                        if(db.deleteTask(atoi(id))){ taskQueue.removeById(atoi(id)); RefreshTaskList(); MessageBox(hwnd,"Deleted","Success",MB_ICONINFORMATION); }
                    }
                    break;
                }
                case IDC_BTN_REFRESH: { LoadTasksIntoQueue(); RefreshTaskList(); MessageBox(hwnd,"Refreshed","Success",MB_ICONINFORMATION); break; }
                case IDC_BTN_STATS: {
                    stringstream ss; ss<<"Queue Stats\nTotal: "<<taskQueue.getTotalCount()<<" | Pending: "<<taskQueue.getPendingCount()<<" | Completed: "<<taskQueue.getCompletedCount();
                    MessageBox(hwnd,ss.str().c_str(),"Queue Stats",MB_ICONINFORMATION);
                    break;
                }
            }
            break;
        }
        case WM_SIZE: { HWND hwndStatus = GetDlgItem(hwnd, IDC_STATUS_BAR); SendMessage(hwndStatus, WM_SIZE, 0,0); break; }
        case WM_CLOSE: DestroyWindow(hwnd); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProc(hwnd,msg,wParam,lParam);
    }
    return 0;
}

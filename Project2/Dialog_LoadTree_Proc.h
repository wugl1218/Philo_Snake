﻿#pragma once

#include "Common.h"
#include "Direct2D.h"
#include "mian.h"
#include "resource.h"
//#include <afx.h>
#pragma warning(disable : 4996)  //std::wstring_convert C++17中不推薦使用,並以報錯方式提醒, 以此忽略此報錯


//防呆
bool Dialog_LoadTree_is_open = false;
bool Dialog_Input_is_open = false;

// Dialog相關
std::vector<POINT> fruitpoint;
POINT clickPoint = { 0 }; //主視窗點擊座標
POINT treePoint = { 0 }; //Dialog樹木生成座標
POINT treeclickPoint = { 0 }; //Dialog點擊座標
POINT movePoint = { 0 }; //Dialog點擊座標
json saveData; 
//判斷是否重繪

//圖檔繪製大小 樹木圖檔為正方形,長寬為fixed_px, 
int fixed_px = 200;
//圖檔繪製大小 水果圖檔為正方形,長寬為fixed_px_fruit, 
int fixed_px_fruit = 30;
RECT TreeRect ;

//Dialog繪圖
bool Dialog_do_Clear = 1;
bool Dialog_do_TreeClear = 1;
bool Dialog_is_Save = 0;
bool Dialog_isfruit;
ID2D1Bitmap* Tree_Bitmap;
ID2D1Bitmap* Fruit_Bitmap;
ID2D1HwndRenderTarget* Tree_RenderTarget;

//主視窗繪圖
Tree* drawTree;
FruitTree* drawFruitTree;
ID2D1Factory* pD2DFactory;
// C++ 17
std::filesystem::path currentPath = std::filesystem::current_path();


std::wstring  Load_File_Path;
std::wstring  Save_File_Path;
std::string Tree_File_Name;
std::string Fruit_File_Name;
std::string Save_Name;

// 原本自己寫Dialog用以存放檔案 ,但發現使用Windows自帶的IShellItem 則更為輕便
INT_PTR CALLBACK Dialog_Input_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DWORD dwID = wParam;

    switch (uMsg) {
    case WM_INITDIALOG:
    {
        Dialog_Input_is_open = 1;

        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            // 使用者按下了確定按鈕
        case IDOK:
        {
            // 取得玩家名稱
            int textLength = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_EDIT1));
            if (textLength == 0)
            {
                // 輸入為空
                MessageBox(hwndDlg, L"未輸入檔案名稱", L"錯誤", MB_OK);
                break;

            }
            else
            {
                wchar_t buffer[100]; // 要存放資料的緩衝區
                // C++14
                GetDlgItemText(hwndDlg, IDC_EDIT1, buffer, 100);
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                Save_Name = converter.to_bytes(buffer);
                // C++17
                //GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT1), buffer, 100);
                //int bufferSize = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
                //Save_Name.resize(bufferSize);
                //WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &Save_Name[0], bufferSize, nullptr, nullptr);
                //OutputDebugString(buffer);
                //OutputDebugString(L"保存\n");

                //std::wstring wideSaveName(Save_Name.begin(), Save_Name.end());
                //OutputDebugString(wideSaveName.c_str());

                //OutputDebugString(L"保存\n");

                //TODO: 寫入存檔路徑以及快捷記錄檔

            }
            EndDialog(hwndDlg, IDOK);
        }
        break;
        // 使用者按下了取消按鈕
        case IDCANCEL:
        {
            EndDialog(hwndDlg, IDCANCEL);
        }
        break;
        }
        break;
    case WM_DESTROY:
    {
        Dialog_Input_is_open = 0;
    }
    break;
    }
    return FALSE;
}

//用以還原控制項
void Dialog_clear()
{
    if (!fruitpoint.empty())
        fruitpoint.clear();
    Dialog_do_Clear = 1;
    Dialog_do_TreeClear = 1;
    treeclickPoint = { 0 };
}
INT_PTR CALLBACK Dialog_LoadTree_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DWORD dwID = wParam;
    switch (uMsg) {
    case WM_INITDIALOG:
    {

        Dialog_LoadTree_is_open = 1;
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
        if (SUCCEEDED(hr))
        {

            OutputDebugString(L"Dialog初始化\n");
            treePoint.x = DIALOG_TREELOAD_POINT_X;
            treePoint.y = DIALOG_TREELOAD_POINT_Y;
            TreeRect = {
            static_cast<LONG>(treePoint.x),
            static_cast<LONG>(treePoint.y),
            static_cast<LONG>(treePoint.x + fixed_px),
            static_cast<LONG>(treePoint.y + fixed_px)
            };
            Dialog_is_Save = 0;
            Tree_Bitmap = NULL;
            Fruit_Bitmap = NULL;
            Common::InitD2D(hwndDlg, pD2DFactory, &Tree_RenderTarget);

            
        }
        if (Dialog_isfruit)
        {
            // 顯示水果圖片按鈕
            ShowWindow(GetDlgItem(hwndDlg, ID_LOADFRUIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwndDlg, ID_DATASAVE), SW_SHOW);
        }
        else
        {
            // 隱藏水果圖片按鈕
            ShowWindow(GetDlgItem(hwndDlg, ID_LOADFRUIT), SW_HIDE);
            ShowWindow(GetDlgItem(hwndDlg, ID_DATASAVE), SW_HIDE);
        }

        std::string path = currentPath.string() + "/Images/map_data.json";
        std::ifstream inFile(path);
        if (!inFile.is_open()) {
            //std::cerr << "無法打開 JSON 文件" << std::endl;
            //OutputDebugString(L"JSON開啟成功\n");
            MessageBox(hWnd, L"存檔讀取失敗", L"錯誤", MB_OK);

            return 1;
        }
        //else
            //OutputDebugString(L"JSON開啟成功\n");
        json mapData;
        inFile >> mapData;
        inFile.close();
        saveData = mapData;
        std::vector<std::string> items;

        for (auto it = mapData.begin(); it != mapData.end(); ++it) 
        {
            items.push_back(it.key());
        }
        //OutputDebugString(L"items完成\n");

        CComboBox pComboBox;
        pComboBox.Attach(GetDlgItem(hwndDlg, IDC_COMBO1));
        CFont font;
        font.CreatePointFont(120, _T("Verdana"));  // 120 是字體大小，"Verdana" 是字體名稱
        pComboBox.SetFont(&font);

        for (const auto& item : items) {
            //將string轉換到wstring //Cstring對中文支援不好
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring wideSaveName = converter.from_bytes(item);
            pComboBox.AddString(wideSaveName.c_str());
            //OutputDebugString(wideSaveName.c_str());
            //OutputDebugString(L"\n");

        }

        pComboBox.Detach();
        //OutputDebugString(L"JSON讀取成功\n");


    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            case IDOK:
                // 使用者按下了確定按鈕
            {
                

                //OutputDebugString(L"儲存圖片\n");
                //if (!Dialog_isfruit)//先處理一般樹
                //{
                //    // 自動帶入一般樹圖片
                //    OutputDebugString(Load_File_Path.c_str());
                //    IWICImagingFactory* pIWICFactory = NULL;
                //    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pIWICFactory);
                //    Common::LoadBitmapFromFile(engine->m_pRenderTarget, pIWICFactory, Load_File_Path, 0, 0, &drawTree->treeBitmap);
                //    pIWICFactory->Release();
                //}
                //else
                //{
                //    if(!Dialog_is_Save)
                //    {
                //        drawFruitTree->Release();
                //        Save_File_Path = L"./tree_save/auto_save.png";
                //        Common::SaveWindowToImage(hwndDlg, Save_File_Path.c_str(), treePoint, fixed_px, fixed_px);
                //    }
                //    // 自動帶入水果樹圖片      
                //    IWICImagingFactory* pIWICFactory = NULL;
                //    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pIWICFactory);
                //    Common::LoadBitmapFromFile(engine->m_pRenderTarget, pIWICFactory, Save_File_Path, 0, 0, &drawFruitTree->treeBitmap);
                //    pIWICFactory->Release();
                //}
                EndDialog(hwndDlg, IDOK);
            }
            break;

            case IDCANCEL:
            {
                // 使用者按下了取消按鈕
                Dialog_do_Clear = 1;
                Dialog_do_TreeClear = 1;

                EndDialog(hwndDlg, IDCANCEL);
            }
            break;
            case ID_LOADTREE: // 選擇樹木
            {
                std::wstring filename;
                Common::OpenFile(hwndDlg, Tree_RenderTarget, &Tree_Bitmap, Load_File_Path , filename);
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                Tree_File_Name = converter.to_bytes(filename);

                //OutputDebugString(Load_File_Path.c_str());
                Dialog_clear();
                InvalidateRect(hwndDlg, NULL, TRUE);

            }
            break;

            case ID_LOADFRUIT: // 選擇水果
            {
                if (!Tree_Bitmap)
                {
                    MessageBox(hwndDlg, L"請先選擇樹木", L"錯誤", MB_OK);
                    break;
                }
                std::wstring filename;
                Common::OpenFile(hwndDlg, Tree_RenderTarget, &Fruit_Bitmap, Load_File_Path , filename);
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                Fruit_File_Name = converter.to_bytes(filename);

                Dialog_clear();
                InvalidateRect(hwndDlg, NULL, TRUE);
            }
            break;
            case ID_DATASAVE: // 儲存檔案
            {
                if (fruitpoint.empty() || !Fruit_Bitmap)
                {
                    MessageBox(hwndDlg, L"無進行任何繪圖", L"錯誤", MB_OK);
                    break;
                }
                Dialog_is_Save = 1;

                // SAVE
                json save_tree;
                json fruit;
                // 創建一個 JSON array，用於存放 coordinates
                json coordinatesArray;
                // 將 fruitpoint 中的每個 POINT 轉換為 JSON object 並添加到 array 中
                for (const POINT& point : fruitpoint) {
                    json pointObject = {
                        {"X", point.x},
                        {"Y", point.y}
                    };

                    coordinatesArray.push_back(pointObject);
                }

                // 將 coordinatesArray 存入 JSON 中的 "coordinates"
                fruit["coordinates"] = coordinatesArray;
                fruit["image"] = Fruit_File_Name;

                save_tree["coordinates"] = { {"X", 0}, {"Y", 0} };
                save_tree["image"] = Tree_File_Name;
                save_tree["Fruit"] = fruit;

                int result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INPUT), NULL, Dialog_Input_Proc);
                if (result == -1)
                {
                    // 對話框創建失敗
                    MessageBox(NULL, L"對話框創建失敗", L"錯誤", MB_OK | MB_ICONERROR);
                }
                else
                {
                    if (result == IDCANCEL)
                    {
                        break;
                    }
                }
                //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                //std::wstring wideName = converter.from_bytes(Save_Name);
                OutputDebugString(L"讀取保存名稱\n");

                std::wstring wideSaveName(Save_Name.begin(), Save_Name.end());
                OutputDebugString(wideSaveName.c_str());

                saveData[Save_Name] = save_tree;
                // TREE_1
                //json tree1;
                //tree1["coordinates"] = { {"X", 1}, {"Y", 2} };
                //tree1["image"] = "tree_1.png";
                //mapData["TREE_1"] = tree1;

                // 將 JSON 對象轉換為字串
                std::string jsonString = saveData.dump(4);

                // 將 JSON 字串保存到文件
                std::string path = currentPath.string() + "/Images/saveData.json";
                std::ofstream outFile(path);
                if (outFile.is_open()) 
                {
                    outFile << jsonString;
                    outFile.close();
                    OutputDebugString(L"JSON 已保存到 map_data.json\n");

                    CComboBox pComboBox;
                    pComboBox.Attach(GetDlgItem(hwndDlg, IDC_COMBO1));

                    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                    std::wstring wideSaveName = converter.from_bytes(Save_Name);
                    pComboBox.AddString(wideSaveName.c_str());


                    pComboBox.Detach();
                }
                else 
                {
                    OutputDebugString(L"無法打開文件以保存 JSON\n");
                }



            }
            break;
        }
        break;
    case WM_LBUTTONDOWN:
        // 處理滑鼠左鍵
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        if (Fruit_Bitmap 
            && xPos >= treePoint.x 
            && yPos >= treePoint.y 
            && xPos <= treePoint.x + fixed_px 
            && yPos <= treePoint.y + fixed_px)
        {
            treeclickPoint.x = static_cast<FLOAT>(xPos);
            treeclickPoint.y = static_cast<FLOAT>(yPos);
            fruitpoint.push_back(treeclickPoint);
            InvalidateRect(hwndDlg, NULL, FALSE);
        }
    }
    break;
    case WM_MOUSEMOVE:
        // 在滑鼠指向的位置渲染圖片 TODO:
    {
        //int xPos = GET_X_LPARAM(lParam);
        //int yPos = GET_Y_LPARAM(lParam);
        //if (xPos >= treePoint.x && yPos >= treePoint.y && xPos <= treePoint.x + fixed_px && yPos <= treePoint.y + fixed_px)
        //{
        //    movePoint.x = static_cast<FLOAT>(xPos);
        //    movePoint.y = static_cast<FLOAT>(yPos);
        //    InvalidateRect(hwndDlg, NULL, FALSE);
        //}
    }    
    break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwndDlg, &ps);

            Tree_RenderTarget->BeginDraw();
            Tree_RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            //不重複清除是因為要保留繪製結果 //使用點擊陣列後即可每次清除
            if (Dialog_do_Clear)
            {
                Tree_RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
                Dialog_do_Clear = 0;
            }

            //有讀取樹木圖片
            if (Tree_Bitmap && Dialog_do_TreeClear)
            {
                D2D1_SIZE_U size = Tree_Bitmap->GetPixelSize();
                UINT width = size.width;
                UINT height = size.height;
                Tree_RenderTarget->DrawBitmap(Tree_Bitmap, D2D1::RectF(treePoint.x, treePoint.y, treePoint.x + fixed_px, treePoint.y + fixed_px));
                Dialog_do_TreeClear = 0;
            }
            //有讀取水果圖片並點擊在樹上
            //if (Fruit_Bitmap && treeclickPoint.x != 0 && treeclickPoint.y != 0)
            //{
            //    D2D1_SIZE_U size = Fruit_Bitmap->GetPixelSize();
            //    UINT width = size.width;
            //    UINT height = size.height;
            //    Tree_RenderTarget->DrawBitmap(Fruit_Bitmap, D2D1::RectF(treeclickPoint.x, treeclickPoint.y, treeclickPoint.x + fixed_px_fruit, treeclickPoint.y + fixed_px_fruit));
            //}
            
            // 讀取點擊陣列並繪圖水果
            for (const POINT& fruitPoint : fruitpoint)
            {
                if (Fruit_Bitmap)
                {
                    D2D1_SIZE_U size = Fruit_Bitmap->GetPixelSize();
                    UINT width = size.width;
                    UINT height = size.height;
                    Tree_RenderTarget->DrawBitmap(Fruit_Bitmap, D2D1::RectF(fruitPoint.x, fruitPoint.y, fruitPoint.x + fixed_px_fruit, fruitPoint.y + fixed_px_fruit));
                }
            }
            Tree_RenderTarget->EndDraw();

            EndPaint(hwndDlg, &ps);
        }
        break;
    case WM_DESTROY:
        {
            if (Tree_Bitmap)
                Tree_Bitmap->Release();
            if (Fruit_Bitmap)
                Fruit_Bitmap->Release();
            if (!fruitpoint.empty())
                fruitpoint.clear();
            Dialog_do_Clear = 1;
            Dialog_do_TreeClear = 1;
            treeclickPoint = { 0 };
            Tree_RenderTarget->Release();
            Dialog_LoadTree_is_open = 0;
        }
    break;
    case WM_ERASEBKGND:
    break;
    }

    return FALSE;
}


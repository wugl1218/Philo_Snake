﻿#pragma once

#include "framework.h"
#include "Common.h"
#include "Direct2D.h"
#include "mian.h"
#include "resource.h"

// Dialog相關
POINT clickPoint = { 0 }; //主視窗點擊座標
POINT treePoint = { 0 }; //Dialog樹木生成座標
POINT treeclickPoint = { 0 }; //Dialog點擊座標
//判斷是否重繪
bool Dialog_do_Clear = 1; 
bool Dialog_do_TreeClear = 1;
bool Dialog_is_Save = 0;
//圖檔繪製大小 樹木圖檔為正方形,長寬為fixed_px, 
int fixed_px = 200;
//圖檔繪製大小 水果圖檔為正方形,長寬為fixed_px_fruit, 
int fixed_px_fruit = 30;

//dialog繪圖
bool dialog_isfruit;
ID2D1Bitmap* Tree_Bitmap;
ID2D1Bitmap* Fruit_Bitmap;
ID2D1HwndRenderTarget* Tree_RenderTarget;

//主視窗繪圖
Tree* drawTree;
FruitTree* drawFruitTree;
ID2D1Bitmap* Map_Bitmap;

std::wstring  Load_File_Path = L"./種樹得樹.png";
std::wstring  Save_File_Path = L"./種樹得樹.png";

std::string Save_name;

INT_PTR CALLBACK Dialog_Input_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DWORD dwID = wParam;

    switch (uMsg) {
    case WM_INITDIALOG:
    {


        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            // 使用者按下了確定按鈕
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
                GetDlgItemText(hwndDlg, IDC_EDIT1, buffer, 100);
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                Save_name = converter.to_bytes(buffer);

                //TODO: 寫入存檔路徑以及快捷記錄檔

                //wchar_t buffer[100]; // 要存放資料的緩衝區
                //GetDlgItemText(hwndDlg, IDC_EDIT1, buffer, 100);
                //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                //std::string newName = converter.to_bytes(buffer);

                //int newScore = 0;
                //int newDifficulty = 0;

                //std::ifstream file("./Ranklist.json", std::ifstream::binary);
                //json jsonData;
                //file >> jsonData;
                //file.close();

                //json newEntry = {
                //    {"name", newName},
                //    {"score", newScore},
                //    {"difficulty", newDifficulty}
                //};
                //jsonData["Ranklist"].push_back(newEntry);
                //std::ofstream outFile("./Ranklist.json");
                //outFile << jsonData.dump(4);  // 4 是縮排的數量
                //outFile.close();

            }
            EndDialog(hwndDlg, IDOK);
        }
        break;

        case IDCANCEL:
            // 使用者按下了取消按鈕
            EndDialog(hwndDlg, IDCANCEL);
            break;
        }
        break;
    }
    return FALSE;
}
INT_PTR CALLBACK Dialog_LoadTree_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DWORD dwID = wParam;
    switch (uMsg) {
    case WM_INITDIALOG:
    {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
        if (SUCCEEDED(hr))
        {
            OutputDebugString(L"Dialog初始化\n");
            treePoint.x = 130;
            treePoint.y = 10;
            Dialog_is_Save = 0;
            Tree_Bitmap = NULL;
            Fruit_Bitmap = NULL;

            RECT rc;
            GetClientRect(hwndDlg, &rc);
            // 創建 D2D 渲染目標
            hr = pD2DFactory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(hwndDlg, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
                &Tree_RenderTarget
            );
        }
        if (dialog_isfruit) 
        {
            // 顯示水果圖片按鈕
            ShowWindow(GetDlgItem(hwndDlg, ID_LOADFRUIT), SW_SHOW);
            ShowWindow(GetDlgItem(hwndDlg, ID_SAVEFILE), SW_SHOW);
        }
        else 
        {
            // 隱藏水果圖片按鈕
            ShowWindow(GetDlgItem(hwndDlg, ID_LOADFRUIT), SW_HIDE);
            ShowWindow(GetDlgItem(hwndDlg, ID_SAVEFILE), SW_HIDE);
        }
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            case IDOK:
                // 使用者按下了確定按鈕
            {
                

                //OutputDebugString(L"儲存圖片\n");
                if (!dialog_isfruit)//先處理一般樹
                {
                    // 自動帶入一般樹圖片
                    OutputDebugString(Load_File_Path.c_str());
                    IWICImagingFactory* pIWICFactory = NULL;
                    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pIWICFactory);
                    Common::LoadBitmapFromFile(engine->m_pRenderTarget, pIWICFactory, Load_File_Path, 0, 0, &drawTree->treeBitmap);
                    pIWICFactory->Release();
                }
                else
                {
                    if(!Dialog_is_Save)
                    {
                        drawFruitTree->Release();
                        Save_File_Path = L"./auto_save.png";

                        Common::SaveWindowToImage(hwndDlg, Save_File_Path.c_str(), treePoint, fixed_px, fixed_px);
                    }
                    // 自動帶入水果樹圖片      
                    IWICImagingFactory* pIWICFactory = NULL;
                    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pIWICFactory);
                    Common::LoadBitmapFromFile(engine->m_pRenderTarget, pIWICFactory, Save_File_Path, 0, 0, &drawFruitTree->treeBitmap);
                    pIWICFactory->Release();
                }
                EndDialog(hwndDlg, IDOK);
            }
            break;

            case IDCANCEL:
                // 使用者按下了取消按鈕
                Dialog_do_Clear = 1;
                Dialog_do_TreeClear = 1;

                EndDialog(hwndDlg, IDCANCEL);
                break;
            case ID_LOADTREE: // 選擇樹木
            {
                Common::OpenFile(hwndDlg, Tree_RenderTarget, &Tree_Bitmap, Load_File_Path);
                Dialog_do_Clear = 1;
                Dialog_do_TreeClear = 1;
                treeclickPoint = { 0 };
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
                Common::OpenFile(hwndDlg, Tree_RenderTarget, &Fruit_Bitmap, Load_File_Path);
                Dialog_do_Clear = 1;
                Dialog_do_TreeClear = 1;
                treeclickPoint = { 0 };
                InvalidateRect(hwndDlg, NULL, TRUE);
            }
            break;
            case ID_SAVEFILE: // 儲存檔案
            {
                if (!Tree_Bitmap)
                {
                    MessageBox(hwndDlg, L"請先選擇樹木", L"錯誤", MB_OK);
                    break;
                }
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
                Dialog_is_Save = 1;
                if (!dialog_isfruit)
                    drawTree->Release();
                else
                    drawFruitTree->Release();

                //固定儲存地點
                //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                //std::wstring wideName = converter.from_bytes(Save_name);
                //// 拼接字串
                //std::wstring filePath = L"./" + wideName + L".png";
                //Save_File_Path = filePath;

                std::wstring Path;
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::wstring wideName = converter.from_bytes(Save_name);
                Common::OpenFolder(hWnd, Path);
                // 拼接字串
                std::wstring filePath = Path + L"/" + wideName + L".png";

                Common::SaveWindowToImage(hwndDlg, Save_File_Path.c_str(), treePoint, fixed_px , fixed_px);
            }
            break;
        }
        break;
    case WM_LBUTTONDOWN:
        // 處理滑鼠左鍵
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            if (xPos >= treePoint.x && yPos >= treePoint.y && xPos <= treePoint.x + fixed_px && yPos <= treePoint.y + fixed_px)
            {
                treeclickPoint.x = static_cast<FLOAT>(xPos);
                treeclickPoint.y = static_cast<FLOAT>(yPos);
                InvalidateRect(hwndDlg, NULL, TRUE);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwndDlg, &ps);

            Tree_RenderTarget->BeginDraw();
            Tree_RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            //不重複清除是因為要保留繪製結果
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
            if (Fruit_Bitmap && treeclickPoint.x != 0 && treeclickPoint.y != 0)
            {
                D2D1_SIZE_U size = Fruit_Bitmap->GetPixelSize();
                UINT width = size.width;
                UINT height = size.height;
                Tree_RenderTarget->DrawBitmap(Fruit_Bitmap, D2D1::RectF(treeclickPoint.x, treeclickPoint.y, treeclickPoint.x + fixed_px_fruit, treeclickPoint.y + fixed_px_fruit));
            }
            //if (Tree_Bitmap)
            //    Tree_RenderTarget->DrawBitmap(Fruit_Bitmap, D2D1::RectF(treeclickPoint.x, treeclickPoint.y, treeclickPoint.x + width, treeclickPoint.y + height));

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
        Dialog_do_Clear = 1;
        Dialog_do_TreeClear = 1;
        treeclickPoint = { 0 };
        }
        break;

    }
    return FALSE;
}
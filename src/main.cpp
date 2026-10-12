/* ============================================================
   main.cpp — StudentRegistration v3 (C++)
   ბილდში მხოლოდ ეს ფაილი შედის. დანარჩენი #include-ით ემატება.
   ============================================================ */

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>                     /* „შენახვის“ ფანჯარა (ექსპორტი) */
#include <string>
#include <array>
#include <algorithm>
#include <optional>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <format>

/* SQLite C-ზეა დაწერილი; sqlite3.h-ში extern "C" უკვე წერია,
   ამიტომ C++-იდან პირდაპირ შეიძლება. ბიბლიოთეკა — sqlite3.dll (lib-ში) */
#include "sqlite3.h"

/* ბიბლიოთეკები და მანიფესტი პირდაპირ კოდიდან */
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "lib\\sqlite3.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

constexpr int IDI_APP = 1;              /* აიქონი app.rc-დან */

#include "cvladebi.cpp"                 /* ID-ები, ზომები, ცვლადები */
#include "teqsti.cpp"                   /* ტექსტები ორ ენაზე */
#include "ui.cpp"                       /* კონტროლების ცხრილი, შექმნა, განლაგება */
#include "ena.cpp"                      /* ენის გადართვა */
#include "sql.cpp"                      /* SQLite-ის კლასები: Database, Statement */
#include "db.cpp"                       /* ბაზების პოვნა და გახსნა */
#include "parametrebi.cpp"              /* ენის და ფანჯრის დამახსოვრება (ახალი) */
#include "geo.cpp"                      /* ქვეყნები, რეგიონები, ქალაქები */
#include "registracia.cpp"              /* რეგისტრაცია, გასუფთავება, ასაკი, რეჟიმები */
#include "shemowmeba.cpp"               /* ველების შემოწმება */
#include "sortireba.cpp"                /* სორტირება სვეტით (ახალი) */
#include "dzebna.cpp"                   /* ძებნა: პირადი ნომრით და გვარით */
#include "chatvirtva.cpp"               /* სტუდენტის ჩატვირთვა ფორმაში */
#include "redaqtireba.cpp"              /* შენახვა, წაშლა, მენიუ */
#include "eksporti.cpp"                 /* CSV ექსპორტი */
#include "click.cpp"                    /* ღილაკები და სიები */

/* ------------------------------------------------------------
   WndProc — მთავარი ფანჯრის შეტყობინებები
   ------------------------------------------------------------ */
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        g_hMain = hWnd;
        create_controls(hWnd);
        apply_language();
        qveynebis_shevseba();
        regionebis_shevseba();          /* საქართველოს რეგიონები და +995 */
        asakis_gamotvla();
        rejimis_dayeneba();             /* ახალი სტუდენტის რეჟიმი („წაშლა“ დამალულია) */
        return 0;

    case WM_NOTIFY:
    {
        auto *nm = reinterpret_cast<NMHDR *>(lParam);
        if (nm->idFrom == IDC_DAB)              /* დაბადების თარიღი შეიცვალა -> ასაკი */
            asakis_gamotvla();
        else if (nm->idFrom == IDC_SIA)         /* შედეგების ცხრილი */
        {
            switch (nm->code)
            {
            case NM_DBLCLK:                     /* ორჯერ დაწკაპუნება -> ფორმაში */
                archeulis_chatvirtva();
                break;
            case NM_RCLICK:                     /* მარჯვენა ღილაკი -> მენიუ */
                konteqsturi_menu();
                break;
            case LVN_ITEMCHANGED:               /* მონიშვნა შეიცვალა */
                ghilakebis_ganaxleba();
                break;
            case LVN_COLUMNCLICK:               /* სათაურზე დაჭერა -> სორტირება */
                sortireba(reinterpret_cast<NMLISTVIEW *>(lParam)->iSubItem);
                break;
            case LVN_KEYDOWN:                   /* Delete კლავიში -> წაშლა */
                if (reinterpret_cast<NMLVKEYDOWN *>(lParam)->wVKey == VK_DELETE)
                    archeulis_washla();
                break;
            }
        }
        return 0;
    }

    case WM_SIZE:
        on_size(LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_GETMINMAXINFO:              /* ფანჯარა ამაზე პატარა ვერ გახდება */
    {
        auto *mmi = reinterpret_cast<MINMAXINFO *>(lParam);
        mmi->ptMinTrackSize = { g_min_sigane, g_min_simagle };
        return 0;
    }

    case WM_COMMAND:
        on_command(LOWORD(wParam), HIWORD(wParam));
        return 0;

    case WM_CLOSE:                      /* დახურვამდე: ენა და ფანჯრის ადგილი */
        parametrebis_shenaxva();
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

/* ------------------------------------------------------------
   wWinMain — მომზადება, ფანჯრის შექმნა, შეტყობინებების ციკლი
   ------------------------------------------------------------ */
int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nShow)
{
    g_hInst = hInst;

    /* ეკრანის მასშტაბი */
    SetProcessDPIAware();
    HDC hdc = GetDC(nullptr);
    g_dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(nullptr, hdc);

    INITCOMMONCONTROLSEX icc{ sizeof(icc),
        ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES | ICC_DATE_CLASSES };
    InitCommonControlsEx(&icc);

    /* ბაზები */
    if (!geo_gaxsna())
    {
        MessageBoxW(nullptr, teqsti(T_SHECDOMA_GEO), nullptr, MB_ICONERROR);
        return 0;
    }
    if (!students_gaxsna())
    {
        MessageBoxW(nullptr, teqsti(T_SHECDOMA_STUD), nullptr, MB_ICONERROR);
        return 0;
    }

    /* შენახული ენა და ფანჯრის ადგილი (ფანჯრის შექმნამდე, რომ წარწერები სწორ ენაზე შეიქმნას) */
    parametrebis_wakitxva();

    /* ფანჯრის ზომები: მინიმალური და საწყისი */
    SIZE minimumi = fanjris_zoma(MIN_SIGANE, MIN_SIMAGLE);
    g_min_sigane  = minimumi.cx;
    g_min_simagle = minimumi.cy;
    SIZE sawyisi  = fanjris_zoma(SAWYISI_SIGANE, SAWYISI_SIMAGLE);

    /* ფანჯრის კლასი */
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hIcon         = LoadIconW(hInst, MAKEINTRESOURCEW(IDI_APP));
    wc.hIconSm       = static_cast<HICON>(LoadImageW(hInst, MAKEINTRESOURCEW(IDI_APP), IMAGE_ICON,
                                         GetSystemMetrics(SM_CXSMICON),
                                         GetSystemMetrics(SM_CYSMICON), 0));
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(static_cast<INT_PTR>(COLOR_BTNFACE + 1));
    wc.lpszClassName = L"StudentRegWnd";
    RegisterClassExW(&wc);

    CreateWindowExW(0, wc.lpszClassName, nullptr, WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, sawyisi.cx, sawyisi.cy,
                    nullptr, nullptr, hInst, nullptr);
    /* შენახული ადგილი და ზომა, თუ არის; თუ არა — ჩვეულებრივად */
    if (g_aqvs_shenaxuli)
        SetWindowPlacement(g_hMain, &g_shenaxuli);
    else
        ShowWindow(g_hMain, nShow);
    UpdateWindow(g_hMain);

    /* შეტყობინებების ციკლი; IsDialogMessage = Tab-ით ველებს შორის გადასვლა */
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        if (IsDialogMessageW(g_hMain, &msg))
            continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    /* ბაზები პროგრამის დასრულებისას თავისით დაიხურება (დესტრუქტორები) */
    return static_cast<int>(msg.wParam);
}

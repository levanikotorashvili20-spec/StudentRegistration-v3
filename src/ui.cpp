/* ============================================================
   ui.cpp — ფანჯრის კონტროლები: ცხრილი, შექმნა, განლაგება

   ყველა კონტროლი აღწერილია ერთ ცხრილში (kontrolebi), როგორც v1/v2-ში.
   კოორდინატები 96 DPI-ზეა (100%); masshtabi მათ ეკრანის მასშტაბზე ამრავლებს.
   ============================================================ */

/* ---------- ცხრილის ერთი ხაზი ---------- */
struct CTL
{
    const wchar_t *klasi;               /* კლასის სახელი */
    DWORD          stili;               /* WS_... */
    DWORD          exstili;             /* WS_EX_... */
    int            x, y, w, h;          /* 96 DPI-ზე */
    int            id;                  /* კონტროლის ID */
    int            teqsti_id;           /* T_... ან NO_TEXT */
};

constexpr int NO_TEXT = -1;

/* ---------- განლაგება: ყველაფერი ამ რიცხვებზეა მიბმული ---------- */
constexpr int LX    = 16;               /* წარწერების x */
constexpr int LW    = 180;              /* წარწერების სიგანე */
constexpr int IX    = LX + LW + 6;      /* ველების x */
constexpr int IW    = 270;              /* ველების სიგანე */
constexpr int PX    = IX + IW + 20;     /* მარჯვენა პანელის x */
constexpr int SIA_Y = 158;              /* შედეგების ცხრილის y */

/* ---------- სტილები ---------- */
constexpr DWORD ST_LABEL         = WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE;
constexpr DWORD ST_CENTER        = WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE;
constexpr DWORD ST_EDIT          = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL;
constexpr DWORD ST_EDITNUM       = ST_EDIT | ES_NUMBER;
constexpr DWORD ST_LIST          = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST;
constexpr DWORD ST_COMBO         = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWN | CBS_AUTOHSCROLL;
constexpr DWORD ST_DATE          = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
constexpr DWORD ST_DATE_NONE     = ST_DATE | DTS_SHOWNONE;
constexpr DWORD ST_BTN           = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON;
constexpr DWORD ST_BTN_DAMALULI  = WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON;     /* თავიდან დამალული */
constexpr DWORD ST_BTN_GATISHULI = ST_BTN | WS_DISABLED;                      /* თავიდან გამორთული */
constexpr DWORD ST_GROUP         = WS_CHILD | WS_VISIBLE | BS_GROUPBOX;
constexpr DWORD ST_LV            = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
constexpr DWORD EX_EDGE          = WS_EX_CLIENTEDGE;

static constexpr CTL kontrolebi[] =
{
    /* კლასი          სტილი             ex       x        y    w        h    ID               წარწერა */
    /* ---------- რეგისტრაციის ფორმა (მარცხნივ) ---------- */
    { L"STATIC",   ST_LABEL,         0,       LX,      16,  LW,      26,  201,             T_SAXELI },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,      16,  IW,      26,  IDC_SAXELI,      NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,      48,  LW,      26,  202,             T_GVARI },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,      48,  IW,      26,  IDC_GVARI,       NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,      80,  LW,      26,  203,             T_MAMIS_SAXELI },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,      80,  IW,      26,  IDC_MAMIS,       NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     112,  LW,      26,  204,             T_DAB_TARIGHI },
    { DATETIMEPICK_CLASSW, ST_DATE,  0,       IX,     112,  140,     26,  IDC_DAB,         NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       IX+150, 112,  56,      26,  205,             T_ASAKI },
    { L"STATIC",   ST_LABEL,         0,       IX+208, 112,  IW-208,  26,  IDC_ASAKI,       NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     144,  LW,      26,  206,             T_MOQALAQEOBA },
    { L"COMBOBOX", ST_LIST,          0,       IX,     144,  IW,      300, IDC_MOQ,         NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     176,  LW,      26,  207,             T_PIRADI },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,     176,  IW,      26,  IDC_PIRADI,      NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     208,  LW,      26,  208,             T_QVEYANA },
    { L"COMBOBOX", ST_LIST,          0,       IX,     208,  IW,      300, IDC_QVEYANA,     NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     240,  LW,      26,  209,             T_REGIONI },
    { L"COMBOBOX", ST_LIST,          0,       IX,     240,  IW,      300, IDC_REGIONI,     NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     272,  LW,      26,  210,             T_QALAQI },
    { L"COMBOBOX", ST_COMBO,         0,       IX,     272,  IW,      300, IDC_QALAQI,      NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     304,  LW,      26,  211,             T_QUCHA },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,     304,  IW,      26,  IDC_QUCHA,       NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     336,  LW,      26,  212,             T_SAXLI_BINA },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,     336,  127,     26,  IDC_SAXLI,       NO_TEXT },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX+143, 336,  127,     26,  IDC_BINA,        NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     368,  LW,      26,  213,             T_INDEKSI },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,     368,  127,     26,  IDC_INDEKSI,     NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     400,  LW,      26,  214,             T_MOBILURI },
    { L"STATIC",   ST_LABEL,         0,       IX,     400,  50,      26,  IDC_TELKODI,     NO_TEXT },
    { L"EDIT",     ST_EDITNUM,       EX_EDGE, IX+54,  400,  IW-54,   26,  IDC_MOB,         NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     432,  LW,      26,  215,             T_EMAIL },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,     432,  IW,      26,  IDC_EMAIL,       NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     464,  LW,      26,  216,             T_GPA },
    { L"EDIT",     ST_EDIT,          EX_EDGE, IX,     464,  90,      26,  IDC_GPA,         NO_TEXT },
    { L"STATIC",   ST_CENTER,        0,       IX+94,  464,  16,      26,  IDC_SLASH,       NO_TEXT },
    { L"COMBOBOX", ST_LIST,          0,       IX+114, 464,  90,      300, IDC_GPA_SKALA,   NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     496,  LW,      26,  217,             T_SWAVLA_START },
    { DATETIMEPICK_CLASSW, ST_DATE,  0,       IX,     496,  140,     26,  IDC_START,       NO_TEXT },
    { L"STATIC",   ST_LABEL,         0,       LX,     528,  LW,      26,  218,             T_SWAVLA_END },
    { DATETIMEPICK_CLASSW, ST_DATE_NONE, 0,   IX,     528,  140,     26,  IDC_END,         NO_TEXT },
    { L"BUTTON",   ST_BTN,           0,       IX,     572,  130,     32,  IDC_REGISTRACIA, T_REGISTRACIA },
    { L"BUTTON",   ST_BTN,           0,       IX+140, 572,  130,     32,  IDC_GASUFTAVEBA, T_GASUFTAVEBA },
    { L"BUTTON",   ST_BTN_DAMALULI,  0,       LX,     572,  130,     32,  IDC_WASHLA,      T_WASHLA },

    /* ---------- ძებნა (მარჯვნივ) ---------- */
    { L"BUTTON",   ST_GROUP,         0,       PX,       8,  436,     140, IDC_DZ_JGUFI,    T_DZEBNA },
    { L"STATIC",   ST_LABEL,         0,       PX+16,   34,  150,     26,  219,             T_MOQALAQEOBA },
    { L"COMBOBOX", ST_LIST,          0,       PX+170,  34,  150,     300, IDC_DZ_MOQ,      NO_TEXT },
    { L"BUTTON",   ST_BTN_GATISHULI, 0,       PX+330,  32,  90,      30,  IDC_DZ_WASHLA,   T_WASHLA },
    { L"STATIC",   ST_LABEL,         0,       PX+16,   68,  150,     26,  220,             T_PIRADI },
    { L"EDIT",     ST_EDIT,          EX_EDGE, PX+170,  68,  150,     26,  IDC_DZ_PIRADI,   NO_TEXT },
    { L"BUTTON",   ST_BTN,           0,       PX+330,  66,  90,      30,  IDC_DZ_GHILAKI,  T_DZEBNA },
    { L"STATIC",   ST_LABEL,         0,       PX+16,  102,  150,     26,  221,             T_GVARI },
    { L"EDIT",     ST_EDIT,          EX_EDGE, PX+170, 102,  150,     26,  IDC_DZ_GVARI,    NO_TEXT },
    { L"BUTTON",   ST_BTN_GATISHULI, 0,       PX+330, 100,  90,      30,  IDC_DZ_EKSPORTI, T_EKSPORTI },
    { WC_LISTVIEWW, ST_LV,           EX_EDGE, PX,  SIA_Y,   600,     400, IDC_SIA,         NO_TEXT },
    { L"BUTTON",   ST_BTN,           0,  SAWYISI_SIGANE-116, 14, 100, 30, IDC_LANG,        T_ENA_GHILAKI },
};

/* ---------- შედეგების ცხრილის სვეტები ---------- */
constexpr int SV_RAOD = 5;
static constexpr std::array<int, SV_RAOD> sv_siganeebi = { 160, 110, 130, 140, 70 };
static constexpr std::array<int, SV_RAOD> sv_teqstebi  = { T_PIRADI, T_SAXELI, T_GVARI, T_QALAQI, T_GPA };

/* ------------------------------------------------------------
   masshtabi — 96 DPI-ის მნიშვნელობა -> ეკრანის რეალური პიქსელები
   ------------------------------------------------------------ */
static int masshtabi(int v)
{
    return MulDiv(v, g_dpi, 96);
}

/* ------------------------------------------------------------
   fanjris_zoma — კლიენტის ზომიდან (96 DPI) ფანჯრის სრული ზომა
   ------------------------------------------------------------ */
static SIZE fanjris_zoma(int kw, int kh)
{
    RECT rc{ 0, 0, masshtabi(kw), masshtabi(kh) };
    AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, 0);
    return { rc.right - rc.left, rc.bottom - rc.top };     /* C++: სტრუქტურის დაბრუნება {}-ით */
}

/* ------------------------------------------------------------
   create_controls — ცხრილიდან ყველა კონტროლის შექმნა
   ------------------------------------------------------------ */
static void create_controls(HWND hWnd)
{
    /* შრიფტი: Segoe UI, ზომა მასშტაბის მიხედვით */
    g_hFont = CreateFontW(-masshtabi(16), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                          CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    /* range-based for: ცხრილის ყოველი ხაზი (ინდექსი i აღარ გვჭირდება) */
    for (const CTL &k : kontrolebi)
    {
        HWND h = CreateWindowExW(k.exstili, k.klasi, nullptr, k.stili,
                                 masshtabi(k.x), masshtabi(k.y),
                                 masshtabi(k.w), masshtabi(k.h),
                                 hWnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(k.id)),
                                 g_hInst, nullptr);
        SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);
    }

    /* შედეგების ცხრილი: მთლიანი სტრიქონის მონიშვნა, ბადე */
    HWND hSia = GetDlgItem(hWnd, IDC_SIA);
    ListView_SetExtendedListViewStyle(hSia, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    /* სვეტები: ფორმატით და ცარიელი ტექსტით (სათაურებს apply_language წერს) */
    LVCOLUMNW lvc{};
    lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt     = LVCFMT_LEFT;
    lvc.pszText = const_cast<LPWSTR>(L"");      /* C++-ში ტექსტური ლიტერალი const-ია */
    for (int i = 0; i < SV_RAOD; i++)
    {
        lvc.cx       = masshtabi(sv_siganeebi[i]);
        lvc.iSubItem = i;
        ListView_InsertColumn(hSia, i, &lvc);
    }

    /* GPA-ს სკალები */
    for (const wchar_t *s : { L"4", L"5", L"10", L"20", L"100" })
        SendDlgItemMessageW(hWnd, IDC_GPA_SKALA, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s));
    SendDlgItemMessageW(hWnd, IDC_GPA_SKALA, CB_SETCURSEL, 0, 0);

    /* ენაზე დამოუკიდებელი წარწერები */
    SetDlgItemTextW(hWnd, IDC_TELKODI, L"+995");
    SetDlgItemTextW(hWnd, IDC_SLASH, L"/");
}

/* ------------------------------------------------------------
   on_size — WM_SIZE: ფორმა ადგილზე რჩება, იცვლება მხოლოდ
   ენის ღილაკი (მარჯვენა კიდე) და შედეგების ცხრილი (დარჩენილი ადგილი)
   ------------------------------------------------------------ */
static void on_size(int sigane, int simagle)
{
    if (sigane == 0)                    /* ჩაკეცილი ფანჯარა */
        return;

    MoveWindow(GetDlgItem(g_hMain, IDC_LANG),
               sigane - masshtabi(116), masshtabi(14), masshtabi(100), masshtabi(30), TRUE);

    MoveWindow(GetDlgItem(g_hMain, IDC_SIA),
               masshtabi(PX), masshtabi(SIA_Y),
               sigane  - masshtabi(PX) - masshtabi(16),
               simagle - masshtabi(SIA_Y) - masshtabi(16), TRUE);
}

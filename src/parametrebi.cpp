/* ============================================================
   parametrebi.cpp — პარამეტრების დამახსოვრება (v3-ში ახალი)
     ენა, ფანჯრის ზომა და ადგილი, გაშლილია თუ არა
   ინახება data\parametrebi.ini-ში (პროგრამასთან ერთად — პორტატულად).
   INI ფაილს Windows-ის ფუნქციები კითხულობს და წერს:
       [Ena]
       ena=1
       [Fanjara]
       x=120
       ...
   ============================================================ */

static std::wstring    g_param_faili;                   /* data\parametrebi.ini */
static WINDOWPLACEMENT g_shenaxuli{ sizeof(WINDOWPLACEMENT) };
static bool            g_aqvs_shenaxuli = false;        /* ფანჯრის ადგილი ნაპოვნია */

/* რიცხვის წაკითხვა INI-დან (თუ არ არის — nagulisxmevi).
   GetPrivateProfileIntW უარყოფით რიცხვებს ვერ კითხულობს, მეორე მონიტორზე კი x შეიძლება
   უარყოფითი იყოს (მარცხნივ მდგარი მონიტორი), ამიტომ ტექსტად ვკითხულობთ */
static int ini_wakitxva(const wchar_t *seqcia, const wchar_t *gasaghebi, int nagulisxmevi)
{
    wchar_t buf[32];
    GetPrivateProfileStringW(seqcia, gasaghebi, L"", buf, 32, g_param_faili.c_str());
    return buf[0] ? _wtoi(buf) : nagulisxmevi;
}

static void ini_chawera(const wchar_t *seqcia, const wchar_t *gasaghebi, int mnishvneloba)
{
    WritePrivateProfileStringW(seqcia, gasaghebi, std::to_wstring(mnishvneloba).c_str(),
                               g_param_faili.c_str());
}

/* ------------------------------------------------------------
   parametrebis_wakitxva — გაშვებისას, ფანჯრის შექმნამდე
   ------------------------------------------------------------ */
static void parametrebis_wakitxva()
{
    g_param_faili = (g_data_dir / L"parametrebi.ini").wstring();

    g_ena = (ini_wakitxva(L"Ena", L"ena", 0) == 1) ? 1 : 0;

    int x = ini_wakitxva(L"Fanjara", L"x", 0);
    int y = ini_wakitxva(L"Fanjara", L"y", 0);
    int w = ini_wakitxva(L"Fanjara", L"w", 0);
    int h = ini_wakitxva(L"Fanjara", L"h", 0);
    if (w <= 0 || h <= 0)
        return;                                         /* პირველი გაშვება: ფაილი ჯერ არ არის */

    RECT rc{ x, y, x + w, y + h };
    if (MonitorFromRect(&rc, MONITOR_DEFAULTTONULL) == nullptr)
        return;                                         /* ის მონიტორი აღარ არის — ჩვეულებრივ ადგილზე */

    g_shenaxuli.rcNormalPosition = rc;
    g_shenaxuli.showCmd = (ini_wakitxva(L"Fanjara", L"gashlili", 0) == 1) ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
    g_aqvs_shenaxuli = true;
}

/* ------------------------------------------------------------
   parametrebis_shenaxva — დახურვისას (WM_CLOSE)
   ------------------------------------------------------------ */
static void parametrebis_shenaxva()
{
    WINDOWPLACEMENT wp{ sizeof(wp) };
    if (GetWindowPlacement(g_hMain, &wp))
    {
        /* rcNormalPosition = ჩვეულებრივი ზომა, მაშინაც კი, თუ ახლა გაშლილია */
        const RECT &rc = wp.rcNormalPosition;
        ini_chawera(L"Fanjara", L"x", rc.left);
        ini_chawera(L"Fanjara", L"y", rc.top);
        ini_chawera(L"Fanjara", L"w", rc.right - rc.left);
        ini_chawera(L"Fanjara", L"h", rc.bottom - rc.top);
        ini_chawera(L"Fanjara", L"gashlili", wp.showCmd == SW_SHOWMAXIMIZED ? 1 : 0);
    }
    ini_chawera(L"Ena", L"ena", g_ena);
}

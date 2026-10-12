/* ============================================================
   sortireba.cpp — შედეგების დალაგება სვეტის სათაურზე დაჭერით (v3-ში ახალი)
     პირველი დაჭერა: ზრდადობით (▲), მეორე: კლებადობით (▼)
   ============================================================ */

static int  g_sort_sveti  = -1;         /* რომელი სვეტით ლაგდება (-1 = არცერთით) */
static bool g_sort_zrdadi = true;       /* true = ზრდადობით, false = კლებადობით */

/* ------------------------------------------------------------
   sortis_isrebi — სვეტის სათაურზე ისარი (▲/▼) მხოლოდ დალაგებულ სვეტზე
   ------------------------------------------------------------ */
static void sortis_isrebi()
{
    HWND hHeader = ListView_GetHeader(GetDlgItem(g_hMain, IDC_SIA));
    for (int i = 0; i < SV_RAOD; i++)
    {
        HDITEMW hdi{};
        hdi.mask = HDI_FORMAT;
        Header_GetItem(hHeader, i, &hdi);
        hdi.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);            /* ძველი ისარი ქრება */
        if (i == g_sort_sveti)
            hdi.fmt |= g_sort_zrdadi ? HDF_SORTUP : HDF_SORTDOWN;
        Header_SetItem(hHeader, i, &hdi);
    }
}

/* ახალი ძებნის შემდეგ: დალაგება აღარ არის */
static void sortis_gauqmeba()
{
    g_sort_sveti = -1;
    sortis_isrebi();
}

/* ------------------------------------------------------------
   shedareba — ორი სტრიქონის შედარება (ცხრილი ამას თავად იძახებს)
   < 0: a წინაა, > 0: b წინაა, 0: ტოლია
   ------------------------------------------------------------ */
static int CALLBACK shedareba(LPARAM a, LPARAM b, LPARAM)
{
    HWND hSia = GetDlgItem(g_hMain, IDC_SIA);
    wchar_t ta[256], tb[256];
    ListView_GetItemText(hSia, static_cast<int>(a), g_sort_sveti, ta, 256);
    ListView_GetItemText(hSia, static_cast<int>(b), g_sort_sveti, tb, 256);

    int shedegi;
    if (sv_teqstebi[g_sort_sveti] == T_GPA)
    {
        /* GPA რიცხვად: ტექსტად "10" < "9" იქნებოდა. ცარიელი — ყველაზე პატარა */
        double da = ta[0] ? _wtof(ta) : -1.0;
        double db = tb[0] ? _wtof(tb) : -1.0;
        shedegi = (da < db) ? -1 : (da > db) ? 1 : 0;
    }
    else
    {
        /* ტექსტი Windows-ის ენის წესებით (ქართული ანბანიც სწორად ლაგდება).
           CompareStringEx აბრუნებს 1, 2 ან 3 -> -1, 0 ან 1 */
        shedegi = CompareStringEx(LOCALE_NAME_USER_DEFAULT, NORM_IGNORECASE,
                                  ta, -1, tb, -1, nullptr, nullptr, 0) - CSTR_EQUAL;
    }
    return g_sort_zrdadi ? shedegi : -shedegi;
}

/* ------------------------------------------------------------
   sortireba — სათაურზე დაჭერა (LVN_COLUMNCLICK)
   ------------------------------------------------------------ */
static void sortireba(int sveti)
{
    if (sveti == g_sort_sveti)
        g_sort_zrdadi = !g_sort_zrdadi;         /* იგივე სვეტი -> მიმართულება იცვლება */
    else
    {
        g_sort_sveti  = sveti;                  /* ახალი სვეტი -> ზრდადობით */
        g_sort_zrdadi = true;
    }
    ListView_SortItemsEx(GetDlgItem(g_hMain, IDC_SIA), shedareba, 0);
    sortis_isrebi();
}

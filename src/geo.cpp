/* ============================================================
   geo.cpp — ქვეყნების, რეგიონებისა და ქალაქების სიები
   ============================================================ */

/* "GE" UTF-16-ში 4 ბაიტია = ერთი DWORD (იგივე ხერხი, რაც v1/v2-ში) */
constexpr DWORD ISO_GE = 0x00450047;    /* 'G' = 0047h, 'E' = 0045h */

static DWORD iso_dword(const std::wstring &iso)
{
    DWORD d = 0;
    if (iso.size() >= 2)
        std::memcpy(&d, iso.data(), 4);     /* ორი სიმბოლო -> ერთი DWORD */
    return d;
}

/* ------------------------------------------------------------
   archeuli_monacemi — სიაში არჩეული ელემენტის მიბმული მნიშვნელობა
   (ქვეყნისთვის ISO კოდი, რეგიონისთვის id). CB_ERR, თუ არაფერია არჩეული
   ------------------------------------------------------------ */
static LRESULT archeuli_monacemi(int id)
{
    LRESULT idx = SendDlgItemMessageW(g_hMain, id, CB_GETCURSEL, 0, 0);
    if (idx == CB_ERR)
        return CB_ERR;
    return SendDlgItemMessageW(g_hMain, id, CB_GETITEMDATA, idx, 0);
}

/* ------------------------------------------------------------
   qveynebis_shevseba — სამივე ქვეყნის სია; ნაგულისხმევად საქართველო
   ------------------------------------------------------------ */
static void qveynebis_shevseba()
{
    constexpr int siebi[] = { IDC_MOQ, IDC_QVEYANA, IDC_DZ_MOQ };
    LRESULT ge_idx = 0;

    /* st ფუნქციის ბოლოს თავისით დაიხურება — finalize აღარ გვჭირდება */
    sql::Statement st(g_geo, "SELECT iso2, name FROM countries ORDER BY name");
    while (st.next())
    {
        DWORD        kodi   = iso_dword(st.column_text(0));
        std::wstring saxeli = st.column_text(1);
        LRESULT      idx    = 0;

        /* სამივე სიაში ერთნაირი რიგით -> ინდექსიც ერთნაირია */
        for (int id : siebi)
        {
            idx = SendDlgItemMessageW(g_hMain, id, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(saxeli.c_str()));
            SendDlgItemMessageW(g_hMain, id, CB_SETITEMDATA, idx, kodi);
        }
        if (kodi == ISO_GE)
            ge_idx = idx;
    }

    for (int id : siebi)
        SendDlgItemMessageW(g_hMain, id, CB_SETCURSEL, ge_idx, 0);
}

/* ------------------------------------------------------------
   regionebis_shevseba — ქვეყანა შეიცვალა:
   რეგიონების სია, ქალაქების გასუფთავება, სატელეფონო კოდი
   ------------------------------------------------------------ */
static void regionebis_shevseba()
{
    HWND hReg = GetDlgItem(g_hMain, IDC_REGIONI);
    HWND hQal = GetDlgItem(g_hMain, IDC_QALAQI);

    SendMessageW(hReg, CB_RESETCONTENT, 0, 0);
    SendMessageW(hQal, CB_RESETCONTENT, 0, 0);
    SetDlgItemTextW(g_hMain, IDC_TELKODI, L"");

    LRESULT archeuli = archeuli_monacemi(IDC_QVEYANA);
    if (archeuli == CB_ERR)
        return;
    DWORD iso = static_cast<DWORD>(archeuli);

    /* ---- რეგიონები ---- */
    {
        sql::Statement st(g_geo, "SELECT id, name FROM states WHERE country_iso = ?1 ORDER BY name");
        st.bind_iso(1, iso);
        while (st.next())
        {
            std::wstring saxeli = st.column_text(1);
            LRESULT idx = SendMessageW(hReg, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(saxeli.c_str()));
            SendMessageW(hReg, CB_SETITEMDATA, idx, st.column_int(0));
        }
    }   /* <- აქ st იხურება (ბლოკის ბოლო) */

    /* ---- სატელეფონო კოდი ---- */
    sql::Statement st(g_geo, "SELECT phonecode FROM countries WHERE iso2 = ?1");
    st.bind_iso(1, iso);
    if (st.next())
    {
        std::wstring kodi = st.column_text(0);
        if (!kodi.empty())
        {
            if (kodi[0] != L'+')            /* ბაზაში შეიძლება "+"-ით ეწეროს ან მის გარეშე */
                kodi.insert(0, L"+");
            SetDlgItemTextW(g_hMain, IDC_TELKODI, kodi.c_str());
        }
    }
}

/* ------------------------------------------------------------
   qalaqebis_shevseba — რეგიონი შეიცვალა: ქალაქების სია
   ------------------------------------------------------------ */
static void qalaqebis_shevseba()
{
    HWND hQal = GetDlgItem(g_hMain, IDC_QALAQI);
    SendMessageW(hQal, CB_RESETCONTENT, 0, 0);

    LRESULT regioni = archeuli_monacemi(IDC_REGIONI);
    if (regioni == CB_ERR)
        return;

    sql::Statement st(g_geo, "SELECT name FROM cities WHERE state_id = ?1 ORDER BY name");
    st.bind(1, static_cast<int>(regioni));
    while (st.next())
    {
        std::wstring saxeli = st.column_text(0);
        SendMessageW(hQal, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(saxeli.c_str()));
    }
}

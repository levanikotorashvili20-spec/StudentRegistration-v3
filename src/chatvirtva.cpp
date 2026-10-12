/* ============================================================
   chatvirtva.cpp — სტუდენტის სრული მონაცემები -> ფორმა
   (შედეგების ცხრილში ორჯერ დაწკაპუნებით)

   იყენებს reg_velebi ცხრილს (registracia.cpp) საპირისპირო მიმართულებით:
     რეგისტრაცია: ფორმა -> ბაზა
     ჩატვირთვა:   ბაზა  -> ფორმა
   ============================================================ */

/* სვეტების რიგი = reg_velebi-ს რიგი!
   gpa_skala მთელ რიცხვად: REAL 4.0 ტექსტად "4.0" იქნებოდა, სიაში კი "4" წერია */
static constexpr const char *SQL_CHATVIRTVA =
    "SELECT saxeli, gvari, mamis_saxeli, dab_tarighi, moqalaqeoba, piradi_nomeri, "
    "qveyana, regioni, qalaqi, qucha, saxlis_n, bina, indeksi, mob_nomeri, email, "
    "gpa, CAST(gpa_skala AS INTEGER), swavla_start, swavla_end FROM students WHERE id = ?1";

/* "YYYY-MM-DD" -> თარიღის ამრჩევი; ცარიელი -> გამორთული (სწავლის დასრულება) */
static void tarighis_dayeneba(int id, const std::wstring &t)
{
    HWND h = GetDlgItem(g_hMain, id);
    if (t.empty())
    {
        DateTime_SetSystemtime(h, GDT_NONE, nullptr);
        return;
    }
    SYSTEMTIME dro{};
    swscanf_s(t.c_str(), L"%hu-%hu-%hu", &dro.wYear, &dro.wMonth, &dro.wDay);
    DateTime_SetSystemtime(h, GDT_VALID, &dro);
}

/* ქვეყნის სიაში იმ ქვეყნის არჩევა, რომლის ISO კოდიც iso-ა */
static void iso_archeva(int id, DWORD iso)
{
    LRESULT raod = SendDlgItemMessageW(g_hMain, id, CB_GETCOUNT, 0, 0);
    for (LRESULT i = 0; i < raod; i++)
    {
        if (static_cast<DWORD>(SendDlgItemMessageW(g_hMain, id, CB_GETITEMDATA, i, 0)) == iso)
        {
            SendDlgItemMessageW(g_hMain, id, CB_SETCURSEL, i, 0);
            return;
        }
    }
    SendDlgItemMessageW(g_hMain, id, CB_SETCURSEL, static_cast<WPARAM>(-1), 0);
}

/* სიაში ზუსტად ამ ტექსტის არჩევა (რეგიონი, GPA-ს სკალა) */
static void teqstit_archeva(int id, const std::wstring &t)
{
    LRESULT idx = CB_ERR;
    if (!t.empty())
        idx = SendDlgItemMessageW(g_hMain, id, CB_FINDSTRINGEXACT, static_cast<WPARAM>(-1),
                                  reinterpret_cast<LPARAM>(t.c_str()));
    SendDlgItemMessageW(g_hMain, id, CB_SETCURSEL, idx, 0);
}

/* "+995555123456" -> მობილურის ველი: თუ ნომერი მიმდინარე კოდით იწყება,
   ველში მხოლოდ დარჩენილი ნაწილი ჩაიწერება.
   starts_with — C++20 (v2-ში: wcsncmp + wcslen) */
static void telefonis_dayeneba(const std::wstring &t)
{
    std::wstring kodi = velis_teqsti(IDC_TELKODI);
    std::wstring nomeri = t.starts_with(kodi) ? t.substr(kodi.size()) : t;
    SetDlgItemTextW(g_hMain, IDC_MOB, nomeri.c_str());
}

/* ------------------------------------------------------------
   studentis_chatvirtva — სტუდენტის (sid) ყველა ველი ფორმაში
   ------------------------------------------------------------ */
static void studentis_chatvirtva(int sid)
{
    sql::Statement st(g_stud, SQL_CHATVIRTVA);
    st.bind(1, sid);
    if (!st.next())
        return;

    /* ქვეყანა რეგიონამდე ჩაიტვირთება, რეგიონი — ქალაქამდე (reg_velebi-ს რიგით) */
    int sveti = 0;
    for (const VELI &v : reg_velebi)
    {
        std::wstring t = st.column_text(sveti++);       /* NULL -> "" */

        switch (v.tipi)
        {
        case Wyaro::Teqsti:
            SetDlgItemTextW(g_hMain, v.id, t.c_str());
            break;
        case Wyaro::Sia:
            teqstit_archeva(v.id, t);
            if (v.id == IDC_REGIONI)
                qalaqebis_shevseba();                   /* ამ რეგიონის ქალაქები */
            break;
        case Wyaro::Iso:
            if (!t.empty())
                iso_archeva(v.id, iso_dword(t));
            if (v.id == IDC_QVEYANA)
                regionebis_shevseba();                  /* ამ ქვეყნის რეგიონები და კოდი */
            break;
        case Wyaro::Tarighi:
            tarighis_dayeneba(v.id, t);
            break;
        case Wyaro::Telefoni:
            telefonis_dayeneba(t);
            break;
        }
    }

    g_mimdinare_id = sid;                               /* ფორმაში ახლა ეს სტუდენტია */
    rejimis_dayeneba();                                 /* შენახვა / ახალი / წაშლა */
    asakis_gamotvla();
}

/* შედეგების ცხრილში მონიშნული სტრიქონის სტუდენტის id; -1, თუ არაფერია მონიშნული */
static int archeulis_id()
{
    HWND hSia = GetDlgItem(g_hMain, IDC_SIA);
    int rigi = ListView_GetNextItem(hSia, -1, LVNI_SELECTED);
    if (rigi < 0)
        return -1;

    LVITEMW item{};
    item.mask  = LVIF_PARAM;
    item.iItem = rigi;
    ListView_GetItem(hSia, &item);
    return static_cast<int>(item.lParam);
}

/* მონიშნული სტუდენტი -> ფორმა */
static void archeulis_chatvirtva()
{
    int sid = archeulis_id();
    if (sid >= 0)
        studentis_chatvirtva(sid);
}

/* ============================================================
   dzebna.cpp — სტუდენტის ძებნა:
     პირადი ნომრით — ზუსტად, მოქალაქეობის გათვალისწინებით
     გვარით        — ყველა, ვისი გვარიც ასე იწყება (პირველი 100)
   ყოველ სტრიქონს სტუდენტის id ებმის (ეკრანზე არ ჩანს)
   ============================================================ */

/* 0 = id, 1..5 = ცხრილის სვეტები (sv_teqstebi-ს რიგით, ui.cpp) */
static constexpr const char *SQL_DZEBNA_PIRADI =
    "SELECT id, piradi_nomeri, saxeli, gvari, qalaqi, gpa FROM students "
    "WHERE moqalaqeoba = ?1 AND piradi_nomeri = ?2";

/* gvari >= 'ბერ' AND gvari < 'ბეს' = „იწყება ბერ-ით“ — ინდექსი (gvari, saxeli) მუშაობს.
   LIMIT 101: 100 ვაჩვენებთ, 101-ე მხოლოდ იმის გასაგებად, რომ მეტია */
static constexpr const char *SQL_DZEBNA_GVARI =
    "SELECT id, piradi_nomeri, saxeli, gvari, qalaqi, gpa FROM students "
    "WHERE gvari >= ?1 AND gvari < ?2 ORDER BY gvari, saxeli LIMIT 101";

constexpr int MAX_SHEDEGI = 100;

/* ------------------------------------------------------------
   ghilakebis_ganaxleba — ძებნის პანელის ღილაკები:
     „წაშლა“    აქტიურია, როცა ცხრილში სტრიქონია მონიშნული
     „ექსპორტი“ აქტიურია, როცა ცხრილში ერთი შედეგი მაინც არის
   ------------------------------------------------------------ */
static void ghilakebis_ganaxleba()
{
    HWND hSia = GetDlgItem(g_hMain, IDC_SIA);
    EnableWindow(GetDlgItem(g_hMain, IDC_DZ_WASHLA),   ListView_GetSelectedCount(hSia) > 0);
    EnableWindow(GetDlgItem(g_hMain, IDC_DZ_EKSPORTI), ListView_GetItemCount(hSia) > 0);
}

/* ბაზის ერთი სტრიქონი -> შედეგების ცხრილი */
static void rigis_damateba(HWND hSia, sql::Statement &st, int rigi)
{
    /* პირველი სვეტი: ახალი სტრიქონი + მიბმული id */
    std::wstring pirveli = st.column_text(1);
    LVITEMW item{};
    item.mask    = LVIF_TEXT | LVIF_PARAM;
    item.iItem   = rigi;
    item.lParam  = st.column_int(0);
    item.pszText = pirveli.data();
    int idx = ListView_InsertItem(hSia, &item);

    /* დანარჩენი სვეტები (ბაზის სვეტი = ცხრილის სვეტი + 1) */
    for (int sv = 1; sv < SV_RAOD; sv++)
    {
        std::wstring t = st.column_text(sv + 1);
        ListView_SetItemText(hSia, idx, sv, t.data());
    }
}

/* ------------------------------------------------------------
   dzebna — „ძებნა“ ღილაკი (ან Enter ძებნის ველში)
   ------------------------------------------------------------ */
static void dzebna()
{
    HWND hSia = GetDlgItem(g_hMain, IDC_SIA);
    ListView_DeleteAllItems(hSia);
    sortis_gauqmeba();                      /* ახალი შედეგები ჯერ დაულაგებელია */

    teqstis_gasuftaveba(IDC_DZ_PIRADI);
    teqstis_gasuftaveba(IDC_DZ_GVARI);
    std::wstring piradi = velis_teqsti(IDC_DZ_PIRADI);
    std::wstring gvari  = velis_teqsti(IDC_DZ_GVARI);

    if (piradi.empty() && gvari.empty())
    {
        shetyobineba(T_SHEIYVANE_DZEBNA, MB_ICONWARNING);
        ghilakebis_ganaxleba();
        return;
    }

    /* პირადი ნომერი უპირატესია; თუ ცარიელია — გვარით */
    sql::Statement st(g_stud, piradi.empty() ? SQL_DZEBNA_GVARI : SQL_DZEBNA_PIRADI);
    if (!piradi.empty())
    {
        st.bind_iso(1, static_cast<DWORD>(archeuli_monacemi(IDC_DZ_MOQ)));
        st.bind(2, piradi);
    }
    else
    {
        std::wstring zeda = gvari;
        zeda.back()++;                      /* ბოლო ასო ერთით წინ: "ბერ" -> "ბეს" */
        st.bind(1, gvari);
        st.bind(2, zeda);
    }

    int  napovni = 0;
    bool metia   = false;
    while (st.next())
    {
        if (napovni == MAX_SHEDEGI)         /* 101-ე სტრიქონი: შედეგი 100-ზე მეტია */
        {
            metia = true;
            break;
        }
        rigis_damateba(hSia, st, napovni++);
    }

    if (napovni == 0)
        shetyobineba(T_VER_MOIDZEBNA, MB_ICONINFORMATION);
    else if (metia)
        shetyobineba(T_PIRVELI_100, MB_ICONINFORMATION);

    ghilakebis_ganaxleba();
}

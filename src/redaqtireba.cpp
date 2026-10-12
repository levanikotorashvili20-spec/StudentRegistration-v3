/* ============================================================
   redaqtireba.cpp — ჩატვირთული სტუდენტის შენახვა და წაშლა,
   მარჯვენა ღილაკის მენიუ
   ============================================================ */

/* ?1..?19 = reg_velebi-ს რიგი (იგივე, რაც INSERT-ში), ?20 = id */
static constexpr const char *SQL_SHENAXVA =
    "UPDATE students SET saxeli = ?1, gvari = ?2, mamis_saxeli = ?3, dab_tarighi = ?4, "
    "moqalaqeoba = ?5, piradi_nomeri = ?6, qveyana = ?7, regioni = ?8, qalaqi = ?9, "
    "qucha = ?10, saxlis_n = ?11, bina = ?12, indeksi = ?13, mob_nomeri = ?14, "
    "email = ?15, gpa = ?16, gpa_skala = ?17, swavla_start = ?18, swavla_end = ?19 "
    "WHERE id = ?20";

static constexpr const char *SQL_WASHLA = "DELETE FROM students WHERE id = ?1";

/* შედეგების ცხრილის სვეტები -> ფორმის ველები (sv_teqstebi-ს რიგით) */
static constexpr std::array<int, SV_RAOD> sv_kontrolebi = { IDC_PIRADI, IDC_SAXELI, IDC_GVARI, IDC_QALAQI, IDC_GPA };

/* შედეგების ცხრილში სტრიქონი, რომელსაც ეს id (sid) აქვს მიბმული; -1, თუ არ არის */
static int rigis_povna(int sid)
{
    LVFINDINFOW fi{};
    fi.flags  = LVFI_PARAM;
    fi.lParam = sid;
    return ListView_FindItem(GetDlgItem(g_hMain, IDC_SIA), -1, &fi);
}

/* შენახვის შემდეგ: ცხრილის სტრიქონი ფორმის ახალი მნიშვნელობებით */
static void siis_ganaxleba()
{
    int rigi = rigis_povna(g_mimdinare_id);
    if (rigi < 0)
        return;

    HWND hSia = GetDlgItem(g_hMain, IDC_SIA);
    for (int sv = 0; sv < SV_RAOD; sv++)
    {
        std::wstring t = velis_teqsti(sv_kontrolebi[sv]);
        ListView_SetItemText(hSia, rigi, sv, t.data());
    }
}

/* ------------------------------------------------------------
   shenaxva — „შენახვა“: შემოწმება -> UPDATE
   ------------------------------------------------------------ */
static void shenaxva()
{
    if (!shemowmeba())
        return;

    sql::Statement st(g_stud, SQL_SHENAXVA);
    if (!st)
    {
        MessageBoxW(g_hMain, g_stud.errmsg(), nullptr, MB_ICONERROR);
        return;
    }
    velebis_mibma(st);                      /* ?1..?19 ფორმიდან */
    st.bind(20, g_mimdinare_id);            /* ?20 = რომელი სტუდენტი */
    if (shesruleba(st))
    {
        siis_ganaxleba();
        shetyobineba(T_SHENAXULIA, MB_ICONINFORMATION);
    }
}

/* ------------------------------------------------------------
   studentis_washla — სტუდენტის (sid) წაშლა დადასტურებით:
   ბაზიდან, ცხრილიდან, და თუ ფორმაში იყო — ფორმიდანაც
   ------------------------------------------------------------ */
static void studentis_washla(int sid)
{
    if (shetyobineba(T_DADASTURE, MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    sql::Statement st(g_stud, SQL_WASHLA);
    st.bind(1, sid);
    if (!shesruleba(st))
        return;

    int rigi = rigis_povna(sid);
    if (rigi >= 0)
        ListView_DeleteItem(GetDlgItem(g_hMain, IDC_SIA), rigi);
    ghilakebis_ganaxleba();

    if (sid == g_mimdinare_id)              /* ეს სტუდენტი ფორმაში იყო */
        formis_gasuftaveba();

    shetyobineba(T_WASHLILIA, MB_ICONINFORMATION);
}

/* ფორმის „წაშლა“ ღილაკი: ჩატვირთული სტუდენტი */
static void washla()
{
    if (g_mimdinare_id != 0)
        studentis_washla(g_mimdinare_id);
}

/* ცხრილში მონიშნული სტუდენტი (ძებნის „წაშლა“, Delete კლავიში, მენიუ) */
static void archeulis_washla()
{
    int sid = archeulis_id();
    if (sid >= 0)
        studentis_washla(sid);
}

/* ------------------------------------------------------------
   konteqsturi_menu — მარჯვენა ღილაკი შედეგზე: „გახსნა“ / „წაშლა“
   ------------------------------------------------------------ */
enum : int { MENU_GAXSNA = 1001, MENU_WASHLA };

static void konteqsturi_menu()
{
    if (archeulis_id() < 0)                 /* ცარიელ ადგილზე დაწკაპუნება */
        return;

    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, MENU_GAXSNA, teqsti(T_GAXSNA));
    AppendMenuW(hMenu, MF_STRING, MENU_WASHLA, teqsti(T_WASHLA));

    /* მენიუ კურსორის ადგილას; TPM_RETURNCMD: არჩეული პუნქტი პირდაპირ ბრუნდება */
    POINT kursori{};
    GetCursorPos(&kursori);
    int archeuli = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                  kursori.x, kursori.y, 0, g_hMain, nullptr);
    DestroyMenu(hMenu);

    if (archeuli == MENU_GAXSNA)
        archeulis_chatvirtva();
    else if (archeuli == MENU_WASHLA)
        archeulis_washla();
}

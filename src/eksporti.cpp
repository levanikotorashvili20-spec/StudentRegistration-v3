/* ============================================================
   eksporti.cpp — ნაპოვნი სტუდენტების ექსპორტი CSV ფაილში (Excel)

   Excel-ის სამი „ხაფანგი“ (იგივე, რაც v2-ში):
     1. ქართული ასოები: ფაილი UTF-8-შია, თავში BOM ნიშნით (EF BB BF)
     2. გამყოფი: Windows-ის რეგიონული პარამეტრებიდან
     3. წინა ნულები: ="01999000001" სახით

   v2-ში ფაილს MSVC-ის სპეციალური რეჟიმი (ccs=UTF-8) წერდა.
   აქ ყოველ სტრიქონს std::wstring-ად ვაწყობთ, UTF-8-ში ვთარგმნით
   და სტანდარტული std::ofstream-ით ვწერთ.
   ============================================================ */

enum class Formati
{
    Teqsti,                         /* ჩვეულებრივი ტექსტი */
    TeqstiRicxvi,                   /* ციფრები, რომლებიც ტექსტად უნდა დარჩეს */
    Atwiladi                        /* ათწილადი: წერტილი -> სისტემის ნიშანი */
};

struct EksportisSveti
{
    int     t_id;                   /* სათაური (T_...) */
    Formati formati;
};

/* რიგი = SQL_EKSPORTI-ს სვეტების რიგი */
static constexpr EksportisSveti eksportis_svetebi[] =
{
    { T_PIRADI,          Formati::TeqstiRicxvi },
    { T_SAXELI,          Formati::Teqsti },
    { T_GVARI,           Formati::Teqsti },
    { T_MAMIS_SAXELI,    Formati::Teqsti },
    { T_DAB_TARIGHI,     Formati::Teqsti },
    { T_MOQALAQEOBA,     Formati::Teqsti },
    { T_QVEYANA,         Formati::Teqsti },
    { T_REGIONI,         Formati::Teqsti },
    { T_QALAQI,          Formati::Teqsti },
    { T_QUCHA,           Formati::Teqsti },
    { T_SAXLI,           Formati::Teqsti },
    { T_BINA,            Formati::Teqsti },
    { T_INDEKSI,         Formati::TeqstiRicxvi },
    { T_MOBILURI,        Formati::TeqstiRicxvi },
    { T_EMAIL,           Formati::Teqsti },
    { T_GPA,             Formati::Atwiladi },
    { T_GPA_SKALA,       Formati::Teqsti },
    { T_SWAVLA_START,    Formati::Teqsti },
    { T_SWAVLA_END,      Formati::Teqsti },
    { T_REGISTR_TARIGHI, Formati::Teqsti },
};

static constexpr const char *SQL_EKSPORTI =
    "SELECT piradi_nomeri, saxeli, gvari, mamis_saxeli, dab_tarighi, moqalaqeoba, "
    "qveyana, regioni, qalaqi, qucha, saxlis_n, bina, indeksi, mob_nomeri, email, "
    "gpa, CAST(gpa_skala AS INTEGER), swavla_start, swavla_end, registr_tarighi "
    "FROM students WHERE id = ?1";

/* UTF-16 (std::wstring) -> UTF-8 (std::string) */
static std::string utf8(const std::wstring &w)
{
    if (w.empty())
        return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()),
                                nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()),
                        s.data(), n, nullptr, nullptr);
    return s;
}

/* ------------------------------------------------------------
   csv_veli — ერთი ველი სტრიქონის ბოლოს (გამყოფს გამომძახებელი წერს)
   ------------------------------------------------------------ */
static void csv_veli(std::wstring &xazi, const std::wstring &t, Formati formati,
                     wchar_t gamyofi, wchar_t atwiladi)
{
    if (t.empty())                                  /* ცარიელი ველი */
        return;

    if (formati == Formati::TeqstiRicxvi)
    {
        xazi += L"=\"" + t + L"\"";                 /* ="01999000001" */
        return;
    }

    if (formati == Formati::Atwiladi && atwiladi != gamyofi)
    {
        std::wstring r = t;
        std::replace(r.begin(), r.end(), L'.', atwiladi);   /* 3.75 -> 3,75 */
        xazi += r;
        return;
    }

    /* თუ შიგნით გამყოფი, ბრჭყალი ან ახალი ხაზია — ბრჭყალებში, " -> "" */
    if (t.find_first_of(std::wstring{ gamyofi, L'"', L'\n', L'\r' }) != std::wstring::npos)
    {
        xazi += L'"';
        for (wchar_t c : t)
        {
            if (c == L'"')
                xazi += L'"';
            xazi += c;
        }
        xazi += L'"';
    }
    else
        xazi += t;
}

/* „შენახვის“ ფანჯარა: არჩეული ფაილის გზა, ან std::nullopt (გაუქმდა) */
static std::optional<fs::path> eksportis_failis_archeva()
{
    /* ფილტრი: "CSV ფაილი (Excel) (*.csv)\0*.csv\0" (+ ბოლო \0 c_str()-ისგან) */
    std::wstring filtri = std::wstring(teqsti(T_CSV_FAILI)) + L" (*.csv)" + L'\0' + L"*.csv" + L'\0';

    wchar_t gza[MAX_PATH] = L"students.csv";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = g_hMain;
    ofn.lpstrFilter = filtri.c_str();
    ofn.lpstrFile   = gza;
    ofn.nMaxFile    = MAX_PATH;
    ofn.lpstrDefExt = L"csv";
    ofn.Flags       = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (!GetSaveFileNameW(&ofn))
        return std::nullopt;
    return fs::path(gza);
}

/* ------------------------------------------------------------
   eksporti — „ექსპორტი“ ღილაკი
   ------------------------------------------------------------ */
static void eksporti()
{
    HWND hSia = GetDlgItem(g_hMain, IDC_SIA);
    int  raod = ListView_GetItemCount(hSia);
    if (raod == 0)
        return;

    auto gza = eksportis_failis_archeva();
    if (!gza)
        return;                                     /* მომხმარებელმა გააუქმა */

    std::ofstream f(*gza, std::ios::binary);
    if (!f)
    {
        shetyobineba(T_EKSPORTI_SHECDOMA, MB_ICONERROR);
        return;
    }
    f << "\xEF\xBB\xBF";                            /* UTF-8 BOM: Excel ქართულს სწორად წაიკითხავს */

    /* გამყოფი და ათწილადის ნიშანი Windows-ის რეგიონული პარამეტრებიდან */
    wchar_t gamyofi = L',', atwiladi = L'.', buf[8];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SLIST, buf, 8) > 0)
        gamyofi = buf[0];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, buf, 8) > 0)
        atwiladi = buf[0];

    /* ---- სათაურები მიმდინარე ენაზე ---- */
    std::wstring xazi;
    for (size_t i = 0; i < std::size(eksportis_svetebi); i++)
    {
        if (i > 0)
            xazi += gamyofi;
        csv_veli(xazi, teqsti(eksportis_svetebi[i].t_id), Formati::Teqsti, gamyofi, atwiladi);
    }
    f << utf8(xazi) << "\r\n";

    /* ---- ცხრილის ყოველი სტუდენტი: id -> სრული ჩანაწერი ---- */
    sql::Statement st(g_stud, SQL_EKSPORTI);
    for (int rigi = 0; rigi < raod; rigi++)
    {
        LVITEMW item{};
        item.mask  = LVIF_PARAM;
        item.iItem = rigi;
        ListView_GetItem(hSia, &item);

        st.bind(1, static_cast<int>(item.lParam));
        if (st.next())
        {
            xazi.clear();
            for (size_t i = 0; i < std::size(eksportis_svetebi); i++)
            {
                if (i > 0)
                    xazi += gamyofi;
                csv_veli(xazi, st.column_text(static_cast<int>(i)),
                         eksportis_svetebi[i].formati, gamyofi, atwiladi);
            }
            f << utf8(xazi) << "\r\n";
        }
        st.reset();                                 /* იგივე მოთხოვნა შემდეგი id-ით */
    }

    bool kargia = f.good();
    f.close();
    shetyobineba(kargia ? T_EKSPORTI_SHENAXULIA : T_EKSPORTI_SHECDOMA,
                 kargia ? MB_ICONINFORMATION : MB_ICONERROR);
}

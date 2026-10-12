/* ============================================================
   registracia.cpp — სტუდენტის რეგისტრაცია:
     ველების წაკითხვა და ბაზაში ჩაწერა, ფორმის გასუფთავება,
     ასაკის გამოთვლა, ფორმის რეჟიმები
   ============================================================ */

static int g_mimdinare_id = 0;      /* ფორმაში ჩატვირთული სტუდენტის id (0 = ახალი სტუდენტი) */

static bool shemowmeba();           /* პროტოტიპი: shemowmeba.cpp-შია (ამ ფაილის შემდეგ შედის) */

/* ---------- ველის წყაროს ტიპი ----------
   enum class („მკაცრი“ ჩამონათვალი): მნიშვნელობები Wyaro::Teqsti სახით იწერება
   და შემთხვევით რიცხვში ან სხვა ჩამონათვალში ვერ აირევა */
enum class Wyaro
{
    Teqsti,                         /* ტექსტი კონტროლიდან (ცარიელი -> NULL) */
    Sia,                            /* ჩამოსაშლელი სია: ინახება ტექსტი */
    Iso,                            /* ქვეყნის ISO კოდი სიიდან */
    Tarighi,                        /* თარიღი (გამორთული -> NULL) */
    Telefoni                        /* სატელეფონო კოდი + ნომერი */
};

struct VELI
{
    Wyaro tipi;
    int   id;                       /* კონტროლის ID */
};

/* ერთი ხაზი = ერთი სვეტი ბაზაში; რიგი = ?1..?19 (იგივე, რაც v1/v2-ში) */
static constexpr VELI reg_velebi[] =
{
    { Wyaro::Teqsti,   IDC_SAXELI    },     /* ?1  saxeli */
    { Wyaro::Teqsti,   IDC_GVARI     },     /* ?2  gvari */
    { Wyaro::Teqsti,   IDC_MAMIS     },     /* ?3  mamis_saxeli */
    { Wyaro::Tarighi,  IDC_DAB       },     /* ?4  dab_tarighi */
    { Wyaro::Iso,      IDC_MOQ       },     /* ?5  moqalaqeoba */
    { Wyaro::Teqsti,   IDC_PIRADI    },     /* ?6  piradi_nomeri */
    { Wyaro::Iso,      IDC_QVEYANA   },     /* ?7  qveyana */
    { Wyaro::Sia,      IDC_REGIONI   },     /* ?8  regioni */
    { Wyaro::Teqsti,   IDC_QALAQI    },     /* ?9  qalaqi */
    { Wyaro::Teqsti,   IDC_QUCHA     },     /* ?10 qucha */
    { Wyaro::Teqsti,   IDC_SAXLI     },     /* ?11 saxlis_n */
    { Wyaro::Teqsti,   IDC_BINA      },     /* ?12 bina */
    { Wyaro::Teqsti,   IDC_INDEKSI   },     /* ?13 indeksi */
    { Wyaro::Telefoni, IDC_MOB       },     /* ?14 mob_nomeri */
    { Wyaro::Teqsti,   IDC_EMAIL     },     /* ?15 email */
    { Wyaro::Teqsti,   IDC_GPA       },     /* ?16 gpa */
    { Wyaro::Sia,      IDC_GPA_SKALA },     /* ?17 gpa_skala */
    { Wyaro::Tarighi,  IDC_START     },     /* ?18 swavla_start */
    { Wyaro::Tarighi,  IDC_END       },     /* ?19 swavla_end */
};

static constexpr const char *SQL_CHAWERA =
    "INSERT INTO students (saxeli, gvari, mamis_saxeli, dab_tarighi, moqalaqeoba, "
    "piradi_nomeri, qveyana, regioni, qalaqi, qucha, saxlis_n, bina, indeksi, "
    "mob_nomeri, email, gpa, gpa_skala, swavla_start, swavla_end) VALUES "
    "(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19)";

/* ------------------------------------------------------------
   velis_teqsti — კონტროლის ტექსტი std::wstring-ად
   (v2-ში: wchar_t buf[256] — გრძელი ტექსტი მოიჭრებოდა; აქ ზომა ზუსტად ერგება)
   ------------------------------------------------------------ */
static std::wstring velis_teqsti(int id)
{
    HWND h = GetDlgItem(g_hMain, id);
    int sigrdze = GetWindowTextLengthW(h);
    std::wstring t(sigrdze, L'\0');
    if (sigrdze > 0)
        GetWindowTextW(h, t.data(), sigrdze + 1);
    return t;
}

/* შეტყობინება მიმდინარე ენაზე */
static int shetyobineba(int t_id, UINT ikona)
{
    return MessageBoxW(g_hMain, teqsti(t_id), teqsti(T_SATAURI), ikona);
}

/* ------------------------------------------------------------
   asakis_datvla — დაბადების თარიღიდან ასაკი
   (უარყოფითი, თუ თარიღი მომავალშია)
   ------------------------------------------------------------ */
static int asakis_datvla()
{
    SYSTEMTIME dab{}, dghes{};
    DateTime_GetSystemtime(GetDlgItem(g_hMain, IDC_DAB), &dab);
    GetLocalTime(&dghes);

    int asaki = dghes.wYear - dab.wYear;
    if (dghes.wMonth < dab.wMonth ||
        (dghes.wMonth == dab.wMonth && dghes.wDay < dab.wDay))
        asaki--;                        /* დაბადების დღე წელს ჯერ არ დამდგარა */
    return asaki;
}

/* ასაკი -> „ასაკი: 22“ */
static void asakis_gamotvla()
{
    int asaki = asakis_datvla();
    std::wstring teqsti_asaki = (asaki >= 0) ? std::to_wstring(asaki) : L"";
    SetDlgItemTextW(g_hMain, IDC_ASAKI, teqsti_asaki.c_str());
}

/* ------------------------------------------------------------
   tarighi_teqstad — თარიღის ამრჩევიდან "YYYY-MM-DD"
   std::optional: ან ტექსტი, ან „არაფერი“ (თარიღი გამორთულია).
   v2-ში ამისთვის ცალკე 0/1 დაბრუნება და ბუფერი გვჭირდებოდა
   ------------------------------------------------------------ */
static std::optional<std::wstring> tarighi_teqstad(int id)
{
    SYSTEMTIME dro{};
    if (DateTime_GetSystemtime(GetDlgItem(g_hMain, id), &dro) != GDT_VALID)
        return std::nullopt;
    return std::format(L"{:04}-{:02}-{:02}", dro.wYear, dro.wMonth, dro.wDay);
}

/* ------------------------------------------------------------
   rejimis_dayeneba — ღილაკები ფორმის რეჟიმის მიხედვით:
     g_mimdinare_id = 0 -> „რეგისტრაცია“, „გასუფთავება“
     g_mimdinare_id > 0 -> „შენახვა“, „ახალი“, „წაშლა“
   ------------------------------------------------------------ */
static void rejimis_dayeneba()
{
    bool axali = (g_mimdinare_id == 0);
    SetDlgItemTextW(g_hMain, IDC_REGISTRACIA, teqsti(axali ? T_REGISTRACIA : T_SHENAXVA));
    SetDlgItemTextW(g_hMain, IDC_GASUFTAVEBA, teqsti(axali ? T_GASUFTAVEBA : T_AXALI));
    ShowWindow(GetDlgItem(g_hMain, IDC_WASHLA), axali ? SW_HIDE : SW_SHOW);
}

/* ------------------------------------------------------------
   formis_gasuftaveba — ფორმა საწყის მდგომარეობაში
   (ქვეყანა და მოქალაქეობა რჩება: ხშირად ერთი ქვეყნის სტუდენტებს ვარეგისტრირებთ)
   ------------------------------------------------------------ */
static void formis_gasuftaveba()
{
    for (int id : { IDC_SAXELI, IDC_GVARI, IDC_MAMIS, IDC_PIRADI, IDC_QUCHA, IDC_SAXLI,
                    IDC_BINA, IDC_INDEKSI, IDC_MOB, IDC_EMAIL, IDC_GPA, IDC_QALAQI })
        SetDlgItemTextW(g_hMain, id, L"");

    /* რეგიონის არჩევანი ეხსნება, ქალაქების სია ცარიელდება */
    SendDlgItemMessageW(g_hMain, IDC_REGIONI, CB_SETCURSEL, static_cast<WPARAM>(-1), 0);
    SendDlgItemMessageW(g_hMain, IDC_QALAQI, CB_RESETCONTENT, 0, 0);

    /* თარიღები: დღევანდელი */
    SYSTEMTIME dghes{};
    GetLocalTime(&dghes);
    for (int id : { IDC_DAB, IDC_START, IDC_END })
        DateTime_SetSystemtime(GetDlgItem(g_hMain, id), GDT_VALID, &dghes);
    asakis_gamotvla();

    g_mimdinare_id = 0;             /* ფორმა ისევ ახალი სტუდენტისთვისაა */
    rejimis_dayeneba();
    SetFocus(GetDlgItem(g_hMain, IDC_SAXELI));
}

/* ------------------------------------------------------------
   velebis_mibma — ფორმის ყველა ველი -> ?1..?19
   (რეგისტრაციაც და შენახვაც ამას იყენებს)
   ------------------------------------------------------------ */
static void velebis_mibma(sql::Statement &st)
{
    int param = 1;
    for (const VELI &v : reg_velebi)
    {
        switch (v.tipi)
        {
        case Wyaro::Teqsti:
        case Wyaro::Sia:
            st.bind(param, velis_teqsti(v.id));         /* ცარიელი -> NULL (Statement::bind) */
            break;

        case Wyaro::Iso:
        {
            LRESULT r = archeuli_monacemi(v.id);
            if (r == CB_ERR)
                st.bind_null(param);
            else
                st.bind_iso(param, static_cast<DWORD>(r));
            break;
        }

        case Wyaro::Tarighi:
            if (auto t = tarighi_teqstad(v.id))         /* optional: თუ თარიღი არის */
                st.bind(param, *t);
            else
                st.bind_null(param);
            break;

        case Wyaro::Telefoni:
        {
            std::wstring nomeri = velis_teqsti(v.id);
            if (nomeri.empty())
                st.bind_null(param);
            else
                st.bind(param, velis_teqsti(IDC_TELKODI) + nomeri);   /* "+995" + "555123456" */
            break;
        }
        }
        param++;
    }
}

/* ------------------------------------------------------------
   shesruleba — INSERT/UPDATE/DELETE-ის შესრულება
   true = წარმატება, false = შეცდომა (შეტყობინება უკვე ნაჩვენებია).
   finalize აღარ გვჭირდება: Statement თავისით დაიხურება
   ------------------------------------------------------------ */
static bool shesruleba(sql::Statement &st)
{
    if (st.run())
        return true;

    switch (g_stud.extended_errcode())
    {
    case SQLITE_CONSTRAINT_UNIQUE:          /* ასეთი პირადი ნომერი უკვე არის */
        shetyobineba(T_UKVE_ARSEBOBS, MB_ICONWARNING);
        break;
    case SQLITE_CONSTRAINT_NOTNULL:         /* სავალდებულო ველი ცარიელია */
        shetyobineba(T_SHEAVSE_VELEBI, MB_ICONWARNING);
        break;
    default:
        MessageBoxW(g_hMain, g_stud.errmsg(), nullptr, MB_ICONERROR);
    }
    return false;
}

/* ------------------------------------------------------------
   registracia — ახალი სტუდენტი: შემოწმება -> INSERT
   ------------------------------------------------------------ */
static void registracia()
{
    if (!shemowmeba())
        return;

    sql::Statement st(g_stud, SQL_CHAWERA);
    if (!st)
    {
        MessageBoxW(g_hMain, g_stud.errmsg(), nullptr, MB_ICONERROR);
        return;
    }
    velebis_mibma(st);
    if (shesruleba(st))
    {
        shetyobineba(T_DAREGISTRIRDA, MB_ICONINFORMATION);
        formis_gasuftaveba();
    }
}

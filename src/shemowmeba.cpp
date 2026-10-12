/* ============================================================
   shemowmeba.cpp — ველების შემოწმება ბაზაში ჩაწერამდე
   ყოველი შეცდომისას: შეტყობინება მიმდინარე ენაზე
   და კურსორი იმ ველზე, რომელიც უნდა გასწორდეს
   ============================================================ */

/* შეტყობინება + კურსორი ველზე. ყოველთვის false: return shecdoma(...); */
static bool shecdoma(int t_id, int ctl_id)
{
    shetyobineba(t_id, MB_ICONWARNING);
    SetFocus(GetDlgItem(g_hMain, ctl_id));
    return false;
}

/* ველის ტექსტს თავში და ბოლოში ჰარებს აჭრის
   (v1: rep movsw; v2: ინდექსების ციკლები; აქ: find_first_not_of / find_last_not_of) */
static void teqstis_gasuftaveba(int id)
{
    std::wstring t = velis_teqsti(id);
    size_t tavi = t.find_first_not_of(L' ');
    std::wstring gasuftavebuli = (tavi == std::wstring::npos)
        ? L""
        : t.substr(tavi, t.find_last_not_of(L' ') - tavi + 1);

    if (gasuftavebuli != t)                         /* რამე მოიჭრა -> ველში ვწერთ */
        SetDlgItemTextW(g_hMain, id, gasuftavebuli.c_str());
}

/* true, თუ ტექსტში მხოლოდ ციფრებია.
   std::all_of ყოველ სიმბოლოს ამოწმებს ლამბდა-ფუნქციით: [](wchar_t c) { ... } */
static bool mxolod_cifrebi(const std::wstring &t)
{
    return std::all_of(t.begin(), t.end(),
                       [](wchar_t c) { return c >= L'0' && c <= L'9'; });
}

/* ელ-ფოსტა: ზუსტად ერთი '@', მის წინ რამე, მის შემდეგ "რამე.რამე" */
static bool emailis_shemowmeba(const std::wstring &s)
{
    size_t et = s.find(L'@');
    if (et == std::wstring::npos || et == 0 || s.find(L'@', et + 1) != std::wstring::npos)
        return false;

    std::wstring domeni = s.substr(et + 1);
    if (domeni.empty() || domeni[0] == L'.')
        return false;

    size_t wertili = domeni.rfind(L'.');            /* ბოლო წერტილი */
    return wertili != std::wstring::npos && wertili + 1 < domeni.size();
}

/* "3.75" -> 375, "4" -> 400 (მეასედებში); std::nullopt, თუ ფორმატი არასწორია.
   დასაშვებია: 1-3 ციფრი, შემდეგ არასავალდებულო '.' და 1-2 ციფრი */
static std::optional<int> gpas_gardaqmna(const std::wstring &s)
{
    /* ლამბდა: არის თუ არა k-ე სიმბოლო ციფრი ([&] — ხედავს s-ს) */
    auto cifri = [&](size_t k) { return k < s.size() && s[k] >= L'0' && s[k] <= L'9'; };

    size_t i = 0;
    int mteli = 0;
    while (cifri(i))
    {
        mteli = mteli * 10 + (s[i] - L'0');
        if (++i > 3)
            return std::nullopt;                    /* 3 ციფრზე მეტი */
    }
    if (i == 0)
        return std::nullopt;

    int shedegi = mteli * 100;
    if (i == s.size())
        return shedegi;                             /* "4" */
    if (s[i] != L'.' || !cifri(i + 1))
        return std::nullopt;                        /* წერტილის შემდეგ ციფრი აუცილებელია */
    shedegi += (s[i + 1] - L'0') * 10;
    if (i + 2 == s.size())
        return shedegi;                             /* "3.7" */
    if (!cifri(i + 2) || i + 3 != s.size())
        return std::nullopt;                        /* მესამე ციფრი აღარ შეიძლება */
    return shedegi + (s[i + 2] - L'0');             /* "3.75" */
}

/* GPA: მძიმე -> წერტილი, რიცხვი 0-დან სკალამდე. ცარიელი დასაშვებია */
static bool gpas_shemowmeba()
{
    std::wstring gpa = velis_teqsti(IDC_GPA);
    if (gpa.empty())
        return true;

    if (gpa.find(L',') != std::wstring::npos)       /* "3,75" -> "3.75" */
    {
        std::replace(gpa.begin(), gpa.end(), L',', L'.');
        SetDlgItemTextW(g_hMain, IDC_GPA, gpa.c_str());
    }

    auto mnishvneloba = gpas_gardaqmna(gpa);
    auto maximumi     = gpas_gardaqmna(velis_teqsti(IDC_GPA_SKALA));   /* "4" -> 400 */
    return mnishvneloba && maximumi && *mnishvneloba <= *maximumi;
}

/* თარიღი -> YYYYMMDD რიცხვი (შესადარებლად); 0, თუ გამორთულია */
static int tarighi_ricxvad(int id)
{
    SYSTEMTIME dro{};
    if (DateTime_GetSystemtime(GetDlgItem(g_hMain, id), &dro) != GDT_VALID)
        return 0;
    return dro.wYear * 10000 + dro.wMonth * 100 + dro.wDay;
}

/* ------------------------------------------------------------
   shemowmeba — ყველა ველი ფორმის რიგით (ზემოდან ქვემოთ)
   true = ყველაფერი სწორია, false = შეცდომაა (შეტყობინება უკვე ნაჩვენებია)
   ------------------------------------------------------------ */
static bool shemowmeba()
{
    /* 0. ჰარების მოჭრა */
    for (int id : { IDC_SAXELI, IDC_GVARI, IDC_MAMIS, IDC_PIRADI, IDC_QALAQI,
                    IDC_QUCHA, IDC_SAXLI, IDC_BINA, IDC_INDEKSI, IDC_EMAIL, IDC_GPA })
        teqstis_gasuftaveba(id);

    /* 1. სახელი, გვარი */
    if (velis_teqsti(IDC_SAXELI).empty())
        return shecdoma(T_V_SAXELI, IDC_SAXELI);
    if (velis_teqsti(IDC_GVARI).empty())
        return shecdoma(T_V_GVARI, IDC_GVARI);

    /* 2. დაბადების თარიღი: ასაკი 14-დან 100-მდე */
    int asaki = asakis_datvla();
    if (asaki < 14 || asaki > 100)
        return shecdoma(T_V_DAB, IDC_DAB);

    /* 3. მოქალაქეობა */
    LRESULT moq = archeuli_monacemi(IDC_MOQ);
    if (moq == CB_ERR)
        return shecdoma(T_V_MOQ, IDC_MOQ);

    /* 4. პირადი ნომერი (საქართველო: ზუსტად 11 ციფრი) */
    std::wstring piradi = velis_teqsti(IDC_PIRADI);
    if (piradi.empty())
        return shecdoma(T_V_PIRADI, IDC_PIRADI);
    if (static_cast<DWORD>(moq) == ISO_GE && (piradi.size() != 11 || !mxolod_cifrebi(piradi)))
        return shecdoma(T_V_PIRADI_GE, IDC_PIRADI);

    /* 5. ქვეყანა */
    if (archeuli_monacemi(IDC_QVEYANA) == CB_ERR)
        return shecdoma(T_V_QVEYANA, IDC_QVEYANA);

    /* 6. ქალაქი */
    if (velis_teqsti(IDC_QALAQI).empty())
        return shecdoma(T_V_QALAQI, IDC_QALAQI);

    /* 7. მობილური: 6-დან 15 ციფრამდე */
    size_t mob = velis_teqsti(IDC_MOB).size();
    if (mob < 6 || mob > 15)
        return shecdoma(T_V_MOB, IDC_MOB);

    /* 8. ელ-ფოსტა (თუ შეყვანილია) */
    std::wstring email = velis_teqsti(IDC_EMAIL);
    if (!email.empty() && !emailis_shemowmeba(email))
        return shecdoma(T_V_EMAIL, IDC_EMAIL);

    /* 9. GPA (თუ შეყვანილია) */
    if (!gpas_shemowmeba())
        return shecdoma(T_V_GPA, IDC_GPA);

    /* 10. სწავლის დასრულება დაწყებამდე ვერ იქნება */
    int dasruleba = tarighi_ricxvad(IDC_END);
    if (dasruleba != 0 && dasruleba < tarighi_ricxvad(IDC_START))
        return shecdoma(T_V_DASRULEBA, IDC_END);

    return true;
}

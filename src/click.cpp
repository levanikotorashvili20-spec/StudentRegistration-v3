/* ============================================================
   click.cpp — WM_COMMAND: ღილაკები და სიები
   ============================================================ */

static void on_command(int id, int kodi)
{
    if (id == IDC_LANG && kodi == BN_CLICKED)
    {
        toggle_language();
        rejimis_dayeneba();             /* რეჟიმის ღილაკები ახალ ენაზე */
        sortis_isrebi();                /* სათაურების თარგმნისას ისარი იშლება — ვაბრუნებთ */
    }
    else if (id == IDC_QVEYANA && kodi == CBN_SELCHANGE)
        regionebis_shevseba();          /* ქვეყანა შეიცვალა */

    else if (id == IDC_REGIONI && kodi == CBN_SELCHANGE)
        qalaqebis_shevseba();           /* რეგიონი შეიცვალა */

    else if (id == IDC_REGISTRACIA && kodi == BN_CLICKED)
    {
        if (g_mimdinare_id == 0)
            registracia();              /* „რეგისტრაცია“: ახალი სტუდენტი */
        else
            shenaxva();                 /* „შენახვა“: ჩატვირთულის ცვლილებები */
    }
    else if (id == IDC_GASUFTAVEBA && kodi == BN_CLICKED)
        formis_gasuftaveba();           /* „გასუფთავება“ / „ახალი“ */

    else if (id == IDC_WASHLA && kodi == BN_CLICKED)
        washla();                       /* ფორმის „წაშლა“ */

    else if (id == IDC_DZ_GHILAKI && kodi == BN_CLICKED)
        dzebna();

    else if (id == IDC_DZ_WASHLA && kodi == BN_CLICKED)
        archeulis_washla();             /* ძებნის „წაშლა“: მონიშნული სტრიქონი */

    else if (id == IDC_DZ_EKSPORTI && kodi == BN_CLICKED)
        eksporti();                     /* ნაპოვნი სტუდენტები -> CSV */

    else if (id == IDOK)
    {
        /* Enter: თუ კურსორი ძებნის ველშია — ვეძებთ */
        HWND fokusi = GetFocus();
        if (fokusi == GetDlgItem(g_hMain, IDC_DZ_PIRADI) ||
            fokusi == GetDlgItem(g_hMain, IDC_DZ_GVARI))
            dzebna();
    }
}

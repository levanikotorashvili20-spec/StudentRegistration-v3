/* ============================================================
   ena.cpp — ენის გადართვა
   ============================================================ */

/* ------------------------------------------------------------
   apply_language — ყველა წარწერას მიმდინარე ენაზე ცვლის
   ------------------------------------------------------------ */
static void apply_language()
{
    SetWindowTextW(g_hMain, teqsti(T_SATAURI));

    /* ცხრილის ყველა კონტროლი, რომელსაც წარწერა აქვს */
    for (const CTL &k : kontrolebi)
        if (k.teqsti_id != NO_TEXT)
            SetDlgItemTextW(g_hMain, k.id, teqsti(k.teqsti_id));

    /* შედეგების ცხრილის სვეტების სათაურები */
    HWND hSia = GetDlgItem(g_hMain, IDC_SIA);
    LVCOLUMNW lvc{};
    lvc.mask = LVCF_FMT | LVCF_TEXT;
    lvc.fmt  = LVCFMT_LEFT;
    for (int i = 0; i < SV_RAOD; i++)
    {
        lvc.pszText = const_cast<LPWSTR>(teqsti(sv_teqstebi[i]));
        ListView_SetColumn(hSia, i, &lvc);
    }
}

/* ------------------------------------------------------------
   toggle_language — ქართული <-> English
   ------------------------------------------------------------ */
static void toggle_language()
{
    g_ena ^= 1;
    apply_language();
}

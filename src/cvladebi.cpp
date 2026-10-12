/* ============================================================
   cvladebi.cpp — კონტროლების ID-ები, ზომები, გლობალური ცვლადები
   ============================================================ */

/* ---------- ფანჯრის ზომები (96 DPI-ზე, მასშტაბირდება) ----------
   #define-ის ნაცვლად constexpr: ტიპი აქვს და კომპილერი ამოწმებს */
constexpr int SAWYISI_SIGANE  = 1180;   /* საწყისი ზომა */
constexpr int SAWYISI_SIMAGLE = 660;
constexpr int MIN_SIGANE      = 1060;   /* ამაზე პატარა ვერ გახდება */
constexpr int MIN_SIMAGLE     = 620;

/* ---------- კონტროლების ID-ები ----------
   ჩვეულებრივი enum (და არა enum class): Windows-ის ფუნქციები ID-ს
   int-ად ელოდება, ამიტომ ასე გარდაქმნა არ გვჭირდება */
enum : int
{
    IDC_LANG = 100,                     /* ენის გადამრთველი */

    /* რეგისტრაციის ფორმა */
    IDC_SAXELI = 101, IDC_GVARI, IDC_MAMIS, IDC_DAB, IDC_ASAKI, IDC_MOQ, IDC_PIRADI,
    IDC_QVEYANA, IDC_REGIONI, IDC_QALAQI, IDC_QUCHA, IDC_SAXLI, IDC_BINA, IDC_INDEKSI,
    IDC_TELKODI, IDC_MOB, IDC_EMAIL, IDC_GPA, IDC_SLASH, IDC_GPA_SKALA, IDC_START, IDC_END,
    IDC_REGISTRACIA = 130, IDC_GASUFTAVEBA, IDC_WASHLA,

    /* ძებნის პანელი */
    IDC_DZ_JGUFI = 140, IDC_DZ_MOQ, IDC_DZ_PIRADI, IDC_DZ_GVARI,
    IDC_DZ_GHILAKI, IDC_DZ_WASHLA, IDC_DZ_EKSPORTI,
    IDC_SIA = 150                       /* შედეგების ცხრილი */
};

/* ---------- გლობალური ცვლადები ---------- */
static HINSTANCE g_hInst   = nullptr;
static HWND      g_hMain   = nullptr;
static HFONT     g_hFont   = nullptr;
static int       g_dpi     = 96;        /* ეკრანის მასშტაბი (96 = 100%) */
static int       g_min_sigane  = 0;     /* ფანჯრის მინიმალური ზომა (პიქსელებში) */
static int       g_min_simagle = 0;
static int       g_ena     = 0;         /* 0 = ქართული, 1 = English */

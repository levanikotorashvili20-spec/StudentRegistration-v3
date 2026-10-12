/* ============================================================
   db.cpp — ბაზების პოვნა და გახსნა

   data საქაღალდეს exe-დან ზემოთ ვეძებთ, მაქსიმუმ ორი დონე:
     <exe>\data\            მზა პროგრამა (Release zip)
     <exe>\..\..\data\      Visual Studio-დან (exe x64\Debug\-შია)
   ============================================================ */

namespace fs = std::filesystem;

static fs::path      g_data_dir;            /* data საქაღალდე */
static sql::Database g_geo;                 /* geo.db */
static sql::Database g_stud;                /* students.db */

/* სტუდენტების ცხრილი — იგივე, რაც v1-სა და v2-ში (R"(...)" — ტექსტი ხაზებით, ბრჭყალების გარეშე) */
static constexpr const char *SQL_SQEMA = R"(
    CREATE TABLE IF NOT EXISTS students (
        id INTEGER PRIMARY KEY, saxeli TEXT NOT NULL, gvari TEXT NOT NULL,
        mamis_saxeli TEXT, dab_tarighi TEXT NOT NULL, moqalaqeoba TEXT NOT NULL,
        piradi_nomeri TEXT NOT NULL, qveyana TEXT NOT NULL, regioni TEXT,
        qalaqi TEXT NOT NULL, qucha TEXT, saxlis_n TEXT, bina TEXT, indeksi TEXT,
        mob_nomeri TEXT NOT NULL, email TEXT, gpa REAL, gpa_skala REAL,
        registr_tarighi TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
        swavla_start TEXT NOT NULL, swavla_end TEXT,
        UNIQUE (moqalaqeoba, piradi_nomeri));
    CREATE INDEX IF NOT EXISTS idx_students_saxeli ON students (gvari, saxeli);
)";

/* data საქაღალდის პოვნა: იქ, სადაც geo.db დევს */
static bool data_dir_povna()
{
    wchar_t exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, MAX_PATH);

    /* v2-ში: wcsrchr + swprintf_s. აქ: parent_path() და ოპერატორი "/" */
    fs::path dir = fs::path(exe).parent_path();
    for (int i = 0; i < 3; i++)
    {
        std::error_code ec;                 /* exists შეცდომისას გამონაკლისს არ ისვრის */
        if (fs::exists(dir / L"data" / L"geo.db", ec))
        {
            g_data_dir = dir / L"data";
            return true;
        }
        dir = dir.parent_path();            /* ერთი დონით ზემოთ */
    }
    return false;
}

/* geo.db-ის პოვნა და გახსნა */
static bool geo_gaxsna()
{
    return data_dir_povna() && g_geo.open(g_data_dir / L"geo.db");
}

/* students.db-ის გახსნა (თუ არ არსებობს, შეიქმნება) და ცხრილი */
static bool students_gaxsna()
{
    return g_stud.open(g_data_dir / L"students.db") && g_stud.exec(SQL_SQEMA);
}

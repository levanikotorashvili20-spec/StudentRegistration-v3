/* ============================================================
   sql.cpp — SQLite-ის C++ კლასები (RAII)

   v2-ში ყოველ გასასვლელთან sqlite3_finalize ხელით უნდა დაგვეწერა.
   აქ ამას დესტრუქტორი აკეთებს: როცა ობიექტი ფუნქციიდან გასვლისას ქრება,
   ის მოთხოვნას (ან ბაზას) თავად ხურავს — დავიწყება შეუძლებელია.
   ამ ხერხს RAII ჰქვია (Resource Acquisition Is Initialization).
   ============================================================ */

namespace sql
{
    /* ---------- ბაზა ---------- */
    class Database
    {
    public:
        Database() = default;
        ~Database() { if (db_) sqlite3_close(db_); }

        /* ასლი აკრძალულია: ორი ობიექტი ერთ ბაზას ორჯერ დახურავდა */
        Database(const Database &) = delete;
        Database &operator=(const Database &) = delete;

        bool open(const std::filesystem::path &gza)
        {
            return sqlite3_open16(gza.c_str(), &db_) == SQLITE_OK;
        }

        /* რამდენიმე SQL ბრძანება ერთად (მაგ. CREATE TABLE ...; CREATE INDEX ...) */
        bool exec(const char *sql)
        {
            return sqlite3_exec(db_, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
        }

        sqlite3 *get() const { return db_; }
        int extended_errcode() const { return sqlite3_extended_errcode(db_); }
        const wchar_t *errmsg() const { return static_cast<const wchar_t *>(sqlite3_errmsg16(db_)); }

    private:
        sqlite3 *db_ = nullptr;
    };

    /* ---------- მოთხოვნა ---------- */
    class Statement
    {
    public:
        Statement(const Database &db, const char *sql)
        {
            if (sqlite3_prepare_v2(db.get(), sql, -1, &st_, nullptr) != SQLITE_OK)
                st_ = nullptr;
        }
        ~Statement() { sqlite3_finalize(st_); }     /* nullptr-ზე უსაფრთხოა */

        Statement(const Statement &) = delete;
        Statement &operator=(const Statement &) = delete;

        /* if (st) — მომზადდა თუ არა მოთხოვნა */
        explicit operator bool() const { return st_ != nullptr; }

        /* შემდეგი სტრიქონი: true, სანამ სტრიქონები არის */
        bool next() { return sqlite3_step(st_) == SQLITE_ROW; }

        /* INSERT / UPDATE / DELETE: true = წარმატება */
        bool run() { return sqlite3_step(st_) == SQLITE_DONE; }

        void reset() { sqlite3_reset(st_); }

        /* ---- პარამეტრები ?1, ?2, ... ---- */
        void bind(int i, int v) { sqlite3_bind_int(st_, i, v); }
        void bind_null(int i)   { sqlite3_bind_null(st_, i); }

        /* ცარიელი ტექსტი -> NULL (როგორც v1/v2-ში) */
        void bind(int i, const std::wstring &t)
        {
            if (t.empty())
                sqlite3_bind_null(st_, i);
            else
                sqlite3_bind_text16(st_, i, t.c_str(), -1, SQLITE_TRANSIENT);
        }

        /* ISO კოდი: 4 ბაიტი პირდაპირ DWORD-იდან (იგივე ხერხი, რაც v1/v2-ში) */
        void bind_iso(int i, DWORD iso)
        {
            sqlite3_bind_text16(st_, i, &iso, 4, SQLITE_TRANSIENT);
        }

        /* ---- სვეტები ---- */
        int  column_int(int i)  { return sqlite3_column_int(st_, i); }
        bool column_null(int i) { return sqlite3_column_type(st_, i) == SQLITE_NULL; }

        /* ტექსტი std::wstring-ად; NULL -> ცარიელი */
        std::wstring column_text(int i)
        {
            auto p = static_cast<const wchar_t *>(sqlite3_column_text16(st_, i));
            return p ? p : L"";
        }

    private:
        sqlite3_stmt *st_ = nullptr;
    };
}

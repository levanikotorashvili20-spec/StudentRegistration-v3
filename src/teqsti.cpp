/* ============================================================
   teqsti.cpp — ინტერფეისის ტექსტები ორ ენაზე

   C-ში [T_SAXELI] = L"..." ჩანაწერი გვქონდა; C++ მასივებისთვის მას არ იცნობს.
   აქ ორივე ენა ერთ ხაზზეა (ქართული და ინგლისური ერთმანეთის გვერდით),
   და static_assert კომპილაციისას ამოწმებს, რომ რიგი T_ ნომრებს ემთხვევა:
   თუ ერთი ხაზი გამოგრჩა ან აირია, პროგრამა საერთოდ არ აიწყობა.
   ============================================================ */

enum : int
{
    T_SATAURI, T_ENA_GHILAKI, T_SAXELI, T_GVARI, T_MAMIS_SAXELI, T_DAB_TARIGHI,
    T_ASAKI, T_MOQALAQEOBA, T_PIRADI, T_QVEYANA, T_REGIONI, T_QALAQI, T_QUCHA,
    T_SAXLI_BINA, T_INDEKSI, T_MOBILURI, T_EMAIL, T_GPA, T_SWAVLA_START, T_SWAVLA_END,
    T_REGISTRACIA, T_GASUFTAVEBA, T_DZEBNA, T_SHENAXVA, T_AXALI, T_WASHLA, T_GAXSNA,
    T_EKSPORTI, T_SAXLI, T_BINA, T_GPA_SKALA, T_REGISTR_TARIGHI, T_SHECDOMA_GEO,
    T_SHECDOMA_STUD, T_DAREGISTRIRDA, T_UKVE_ARSEBOBS, T_SHEAVSE_VELEBI,
    T_SHEIYVANE_DZEBNA, T_VER_MOIDZEBNA, T_PIRVELI_100, T_SHENAXULIA, T_DADASTURE,
    T_WASHLILIA, T_EKSPORTI_SHENAXULIA, T_EKSPORTI_SHECDOMA, T_CSV_FAILI, T_V_SAXELI,
    T_V_GVARI, T_V_MOQ, T_V_PIRADI, T_V_PIRADI_GE, T_V_QVEYANA, T_V_QALAQI, T_V_MOB,
    T_V_EMAIL, T_V_GPA, T_V_DAB, T_V_DASRULEBA,
    T_RAODENOBA                     /* ტექსტების რაოდენობა (ყოველთვის ბოლოს) */
};

struct Teqsti
{
    int            id;              /* T_... */
    const wchar_t *ka;              /* ქართული */
    const wchar_t *en;              /* English */
};

static constexpr Teqsti teqstebi[] =
{
    /* ---------- წარწერები და ღილაკები ---------- */
    { T_SATAURI,             L"სტუდენტების რეგისტრაცია",
                             L"Student Registration" },
    { T_ENA_GHILAKI,         L"English",
                             L"ქართული" },
    { T_SAXELI,              L"სახელი",
                             L"First name" },
    { T_GVARI,               L"გვარი",
                             L"Last name" },
    { T_MAMIS_SAXELI,        L"მამის სახელი",
                             L"Father's name" },
    { T_DAB_TARIGHI,         L"დაბადების თარიღი",
                             L"Date of birth" },
    { T_ASAKI,               L"ასაკი:",
                             L"Age:" },
    { T_MOQALAQEOBA,         L"მოქალაქეობა",
                             L"Citizenship" },
    { T_PIRADI,              L"პირადი ნომერი",
                             L"Personal ID" },
    { T_QVEYANA,             L"ქვეყანა",
                             L"Country" },
    { T_REGIONI,             L"რეგიონი",
                             L"Region" },
    { T_QALAQI,              L"ქალაქი",
                             L"City" },
    { T_QUCHA,               L"ქუჩა",
                             L"Street" },
    { T_SAXLI_BINA,          L"სახლი / ბინა",
                             L"House / Apt." },
    { T_INDEKSI,             L"საფოსტო ინდექსი",
                             L"Postal code" },
    { T_MOBILURI,            L"მობილური",
                             L"Mobile" },
    { T_EMAIL,               L"ელ-ფოსტა",
                             L"E-mail" },
    { T_GPA,                 L"GPA",
                             L"GPA" },
    { T_SWAVLA_START,        L"სწავლის დაწყება",
                             L"Study start" },
    { T_SWAVLA_END,          L"სწავლის დასრულება",
                             L"Study end" },
    { T_REGISTRACIA,         L"რეგისტრაცია",
                             L"Register" },
    { T_GASUFTAVEBA,         L"გასუფთავება",
                             L"Clear" },
    { T_DZEBNA,              L"ძებნა",
                             L"Search" },
    { T_SHENAXVA,            L"შენახვა",
                             L"Save" },
    { T_AXALI,               L"ახალი",
                             L"New" },
    { T_WASHLA,              L"წაშლა",
                             L"Delete" },
    { T_GAXSNA,              L"გახსნა",
                             L"Open" },
    { T_EKSPORTI,            L"ექსპორტი",
                             L"Export" },
    { T_SAXLI,               L"სახლი",
                             L"House" },
    { T_BINA,                L"ბინა",
                             L"Apt." },
    { T_GPA_SKALA,           L"GPA-ს სკალა",
                             L"GPA scale" },
    { T_REGISTR_TARIGHI,     L"რეგისტრაციის თარიღი",
                             L"Registration date" },

    /* ---------- შეტყობინებები ---------- */
    { T_SHECDOMA_GEO,        L"ვერ მოიძებნა ქვეყნების ბაზა: data\\geo.db",
                             L"Countries database not found: data\\geo.db" },
    { T_SHECDOMA_STUD,       L"ვერ გაიხსნა სტუდენტების ბაზა: data\\students.db",
                             L"Could not open the students database: data\\students.db" },
    { T_DAREGISTRIRDA,       L"სტუდენტი წარმატებით დარეგისტრირდა.",
                             L"The student has been registered." },
    { T_UKVE_ARSEBOBS,       L"ამ ქვეყნის ამ პირადი ნომრით სტუდენტი უკვე რეგისტრირებულია.",
                             L"A student with this citizenship and personal ID is already registered." },
    { T_SHEAVSE_VELEBI,      L"შეავსე სავალდებულო ველები: სახელი, გვარი, პირადი ნომერი, ქალაქი, მობილური.",
                             L"Please fill in the required fields: first name, last name, personal ID, city, mobile." },
    { T_SHEIYVANE_DZEBNA,    L"შეიყვანე საძიებო პირადი ნომერი ან გვარი.",
                             L"Enter a personal ID or a last name to search for." },
    { T_VER_MOIDZEBNA,       L"სტუდენტი ვერ მოიძებნა.",
                             L"No student found." },
    { T_PIRVELI_100,         L"ნაპოვნია 100-ზე მეტი სტუდენტი. ნაჩვენებია პირველი 100 — დააზუსტე გვარი.",
                             L"More than 100 students found. Showing the first 100 — refine the last name." },
    { T_SHENAXULIA,          L"ცვლილებები შენახულია.",
                             L"Changes saved." },
    { T_DADASTURE,           L"ნამდვილად გინდა ამ სტუდენტის წაშლა? ამის გაუქმება შეუძლებელი იქნება.",
                             L"Do you really want to delete this student? This cannot be undone." },
    { T_WASHLILIA,           L"სტუდენტი წაიშალა.",
                             L"The student has been deleted." },
    { T_EKSPORTI_SHENAXULIA, L"შედეგები შენახულია ფაილში.",
                             L"The results have been saved to the file." },
    { T_EKSPORTI_SHECDOMA,   L"ფაილის შენახვა ვერ მოხერხდა.",
                             L"The file could not be saved." },
    { T_CSV_FAILI,           L"CSV ფაილი (Excel)",
                             L"CSV file (Excel)" },

    /* ---------- ველების შემოწმება ---------- */
    { T_V_SAXELI,            L"შეიყვანე სახელი.",
                             L"Enter the first name." },
    { T_V_GVARI,             L"შეიყვანე გვარი.",
                             L"Enter the last name." },
    { T_V_MOQ,               L"აირჩიე მოქალაქეობა.",
                             L"Select the citizenship." },
    { T_V_PIRADI,            L"შეიყვანე პირადი ნომერი.",
                             L"Enter the personal ID." },
    { T_V_PIRADI_GE,         L"საქართველოს მოქალაქის პირადი ნომერი ზუსტად 11 ციფრისგან შედგება.",
                             L"A Georgian personal ID consists of exactly 11 digits." },
    { T_V_QVEYANA,           L"აირჩიე ქვეყანა.",
                             L"Select the country." },
    { T_V_QALAQI,            L"აირჩიე ან ჩაწერე ქალაქი.",
                             L"Select or type the city." },
    { T_V_MOB,               L"შეიყვანე მობილურის ნომერი (6-დან 15 ციფრამდე).",
                             L"Enter a mobile number (6 to 15 digits)." },
    { T_V_EMAIL,             L"ელ-ფოსტის მისამართი არასწორია (მაგ. name@example.com).",
                             L"The e-mail address is not valid (e.g. name@example.com)." },
    { T_V_GPA,               L"GPA უნდა იყოს რიცხვი 0-დან არჩეულ სკალამდე, მაგ. 3.75",
                             L"GPA must be a number from 0 to the selected scale, e.g. 3.75" },
    { T_V_DAB,               L"დაბადების თარიღი არასწორია: ასაკი 14-დან 100 წლამდე უნდა იყოს.",
                             L"Invalid date of birth: age must be between 14 and 100." },
    { T_V_DASRULEBA,         L"სწავლის დასრულება დაწყებამდე ვერ იქნება.",
                             L"The study end date cannot be before the start date." },
};

/* კომპილაციის დროის შემოწმება: teqstebi[i].id == i ყველა ხაზისთვის */
constexpr bool teqstebis_rigi_sworia()
{
    if (std::size(teqstebi) != static_cast<size_t>(T_RAODENOBA))
        return false;
    for (size_t i = 0; i < std::size(teqstebi); i++)
        if (teqstebi[i].id != static_cast<int>(i))
            return false;
    return true;
}
static_assert(teqstebis_rigi_sworia(), "teqstebi: the order must match the T_ constants");

/* ------------------------------------------------------------
   teqsti — id ნომრის ტექსტი მიმდინარე ენაზე
   ------------------------------------------------------------ */
static const wchar_t *teqsti(int id)
{
    return g_ena == 0 ? teqstebi[id].ka : teqstebi[id].en;
}

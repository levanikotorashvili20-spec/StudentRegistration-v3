# Architecture

## Build model

Only `src\main.cpp` is compiled. All other source files are pulled in with `#include`, so the whole program is one translation unit, in dependency order:

| File | Purpose |
|---|---|
| `cvladebi.cpp` | Control IDs, window sizes, global variables |
| `teqsti.cpp` | All interface texts, Georgian and English side by side |
| `ui.cpp` | Control table, control creation, layout, DPI scaling, resizing |
| `ena.cpp` | Language switching |
| `sql.cpp` | C++ classes around SQLite: `sql::Database`, `sql::Statement` |
| `db.cpp` | Locating and opening the databases, the `students` table schema |
| `parametrebi.cpp` | Saving and restoring the language and the window placement |
| `geo.cpp` | Country / region / city lists |
| `registracia.cpp` | Reading the form, registration (INSERT), clearing, age, form modes |
| `shemowmeba.cpp` | Input validation |
| `sortireba.cpp` | Sorting the search results by column |
| `dzebna.cpp` | Search by personal ID or by last name |
| `chatvirtva.cpp` | Loading a student into the form |
| `redaqtireba.cpp` | Saving changes (UPDATE), deleting, right-click menu |
| `eksporti.cpp` | CSV export |
| `click.cpp` | `WM_COMMAND`: button and list events |

Why SQLite is a DLL here (in v2 it was compiled into the exe): SQLite is written in C and does not compile as C++, and the project compiles only `main.cpp`. So v3 links to the 64-bit `sqlite3.dll` through its import library, as v1 did with the 32-bit one.

## Key C++ ideas

### 1. RAII wrappers for SQLite

`sql::Statement` prepares a query in its constructor and calls `sqlite3_finalize` in its destructor; `sql::Database` closes the database the same way. A statement is closed automatically on every exit from a function, including early `return`s, so a forgotten `finalize` is impossible. Copying is deleted (`= delete`), because two copies would close the same handle twice. Inside a function, `{ }` blocks limit a statement's lifetime when a second query is needed.

### 2. Texts checked at compile time

C++ has no `[index] = value` initializers for arrays, so each text is one entry with both languages: `{ T_SAXELI, L"სახელი", L"First name" }`. A `constexpr` function verifies that entry *i* has id *i*, and `static_assert` stops the build if the order is wrong.

### 3. One field table, two directions

`reg_velebi` has 19 rows, one per database column, each with a source type of the scoped enum `Wyaro` (`Teqsti`, `Sia`, `Iso`, `Tarighi`, `Telefoni`). `velebis_mibma` binds the form to `?1`…`?19` for INSERT and UPDATE; `studentis_chatvirtva` uses the same table in reverse to fill the form.

### 4. Standard library instead of buffers

- `std::wstring` everywhere: `velis_teqsti` asks a control for its length first, so long text is never cut off.
- `std::optional` for values that may be missing: a disabled date (`tarighi_teqstad`), an invalid GPA (`gpas_gardaqmna`), a cancelled file dialog.
- `std::filesystem` for paths: `dir / L"data" / L"geo.db"`.
- `std::format` and `std::to_wstring` instead of `swprintf_s`.
- Algorithms and lambdas: `std::all_of` with a lambda checks that a personal ID has only digits; `std::replace` turns `3,75` into `3.75`.
- C++20 `starts_with` for the phone code.

### 5. Search by last name with an index

"Starts with ბერ" is the range `gvari >= 'ბერ' AND gvari < 'ბეს'` (last letter + 1), which can use the `(gvari, saxeli)` index, unlike `LIKE 'ბერ%'`. The query asks for 101 rows to know whether more than 100 exist.

### 6. Sorting

On `LVN_COLUMNCLICK` the list is sorted with `ListView_SortItemsEx`, which calls back `shedareba` for pairs of rows. Text is compared with `CompareStringEx` (the Windows language rules, so the Georgian alphabet is handled correctly), GPA numerically. The header shows the direction with `HDF_SORTUP` / `HDF_SORTDOWN`.

### 7. Remembered settings

On `WM_CLOSE`, `GetWindowPlacement` gives the normal window rectangle (even when maximized) and the show state; they are written with the language to `data\parametrebi.ini`. On start they are read back before the window is created and applied with `SetWindowPlacement`. `MonitorFromRect` checks that the saved position is still on a connected monitor. The settings live next to the data, so the program stays portable.

### 8. CSV export for Excel

Each line is built as `std::wstring`, converted to UTF-8 with `WideCharToMultiByte` and written with `std::ofstream` after a UTF-8 byte order mark. The separator and decimal sign come from the Windows regional settings; personal IDs, phone numbers and postal codes are written as `="0167"` so Excel keeps leading zeros.

## The same task in three languages

Closing a query:

```asm
; v1 (Assembly) — before every exit
invoke sqlite3_finalize, stmt
```
```c
/* v2 (C) — before every exit */
sqlite3_finalize(stmt);
```
```cpp
// v3 (C++) — nothing: the destructor of sql::Statement does it
```

Age from the date of birth:

| v1 | v2 | v3 |
|---|---|---|
| `cmp` / `jb` / `ja` chain, own digit conversion | `if` + `swprintf_s` into a buffer | `if` + `std::to_wstring` |

## Databases

**`students.db`**, table `students` (the same in all three versions):

- `UNIQUE (moqalaqeoba, piradi_nomeri)`: a personal ID is unique within a citizenship; this index also makes the search by personal ID instant.
- `INDEX (gvari, saxeli)`: for the search by last name.

**`geo.db`**, built by `tools\make_geo.sql`: `countries`, `states`, `cities`.

## Glossary

Identifiers are Georgian words written in Latin letters.

| Identifier | Meaning | Identifier | Meaning |
|---|---|---|---|
| `saxeli` | first name | `dzebna` | search |
| `gvari` | last name | `washla` | delete |
| `mamis_saxeli` | father's name | `shenaxva` | save |
| `dab_tarighi` | date of birth | `chatvirtva` | loading |
| `asaki` | age | `shemowmeba` | validation |
| `moqalaqeoba` | citizenship | `registracia` | registration |
| `piradi_nomeri` | personal ID | `sortireba` | sorting |
| `qveyana` | country | `parametrebi` | settings |
| `regioni` | region | `eksporti` | export |
| `qalaqi` | city | `teqsti` | text |
| `qucha` | street | `ena` | language |
| `saxlis_n` / `bina` | house / apartment | `wyaro` | source |
| `indeksi` | postal code | `ghilaki` | button |
| `mob_nomeri` | mobile number | `shecdoma` | error |
| `swavla_start` / `swavla_end` | study start / end | `mimdinare` | current |

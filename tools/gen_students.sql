-- gen_students.sql
-- ტესტ-მონაცემები: 1 000 000 გამოგონილი სტუდენტი
-- გაშვება tools საქაღალდიდან:
--   .\sqlite3.exe ..\data\students.db ".read gen_students.sql"
-- ყურადღება: students ცხრილს თავიდან ქმნის, არსებული მონაცემები წაიშლება!

.timer on

DROP TABLE IF EXISTS main.students;

CREATE TABLE main.students (
    id              INTEGER PRIMARY KEY,
    saxeli          TEXT NOT NULL,
    gvari           TEXT NOT NULL,
    mamis_saxeli    TEXT,
    dab_tarighi     TEXT NOT NULL,              -- 'YYYY-MM-DD'
    moqalaqeoba     TEXT NOT NULL,              -- ISO2: რომელი ქვეყნის პირადი ნომერია
    piradi_nomeri   TEXT NOT NULL,
    qveyana         TEXT NOT NULL,              -- ISO2: მისამართის ქვეყანა
    regioni         TEXT,
    qalaqi          TEXT NOT NULL,
    qucha           TEXT,
    saxlis_n        TEXT,
    bina            TEXT,
    indeksi         TEXT,
    mob_nomeri      TEXT NOT NULL,              -- '+995555123456'
    email           TEXT,
    gpa             REAL,
    gpa_skala       REAL,
    registr_tarighi TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
    swavla_start    TEXT NOT NULL,
    swavla_end      TEXT,
    UNIQUE (moqalaqeoba, piradi_nomeri)
);

CREATE INDEX main.idx_students_saxeli ON students (gvari, saxeli);

-- საქართველოს ქალაქები geo.db-დან, 0-დან დანომრილი
ATTACH DATABASE '../data/geo.db' AS geo;

CREATE TEMP TABLE ge_cities (
    rn      INTEGER PRIMARY KEY,
    qalaqi  TEXT NOT NULL,
    regioni TEXT NOT NULL
);

INSERT INTO ge_cities (rn, qalaqi, regioni)
SELECT row_number() OVER (ORDER BY c.id) - 1, c.name, s.name
FROM geo.cities c
JOIN geo.states s ON s.id = c.state_id
WHERE c.country_iso = 'GE';

WITH RECURSIVE
    seq(n) AS (
        SELECT 1
        UNION ALL
        SELECT n + 1 FROM seq WHERE n < 1000000
    ),
    L AS (
        SELECT '["გიორგი","დავითი","ლევანი","ნიკა","ლუკა","ალექსანდრე","ირაკლი","თორნიკე","საბა","დიმიტრი",
                 "ნინო","მარიამი","ანა","ელენე","თამარი","სალომე","ნათია","ეკატერინე","ქეთევანი","ლიკა"]' AS saxelebi,
               '["გიორგი","დავითი","ზურაბი","გელა","მამუკა","ნოდარი","ბესიკი","ვახტანგი","ლაშა","პაატა"]' AS mamebi,
               '["ბერიძე","კაპანაძე","გელაშვილი","მაისურაძე","გიორგაძე","ლომიძე","წიკლაური","ბოლქვაძე","ნოზაძე","აბაშიძე",
                 "ხუციშვილი","შენგელია","ჯაფარიძე","მჭედლიშვილი","ტაბატაძე","ჩხეიძე","კვარაცხელია","გოგოლაძე","ქავთარაძე","ლობჟანიძე"]' AS gvarebi,
               '["რუსთაველის გამზ.","ჭავჭავაძის გამზ.","აღმაშენებლის გამზ.","ბარათაშვილის ქ.","ნინოშვილის ქ.",
                 "წერეთლის გამზ.","კოსტავას ქ.","ვაჟა-ფშაველას გამზ.","გამსახურდიას ქ.","ტაბიძის ქ."]' AS quchebi
    ),
    -- MATERIALIZED: random() თითო სტრიქონზე ერთხელ გამოითვლება,
    -- რომ JOIN-მა ზუსტად ერთი ქალაქი იპოვოს
    r AS MATERIALIZED (
        SELECT n,
               abs(random()) % (SELECT COUNT(*) FROM ge_cities) AS ci,   -- რომელი ქალაქი
               abs(random()) % 12                               AS sy    -- სწავლის დაწყება: 2015 + sy
        FROM seq
    )
INSERT INTO main.students (
    saxeli, gvari, mamis_saxeli, dab_tarighi, moqalaqeoba, piradi_nomeri,
    qveyana, regioni, qalaqi, qucha, saxlis_n, bina, indeksi,
    mob_nomeri, email, gpa, gpa_skala, swavla_start, swavla_end)
SELECT
    json_extract(L.saxelebi, '$[' || (abs(random()) % json_array_length(L.saxelebi)) || ']'),
    json_extract(L.gvarebi,  '$[' || (abs(random()) % json_array_length(L.gvarebi))  || ']'),
    json_extract(L.mamebi,   '$[' || (abs(random()) % json_array_length(L.mamebi))   || ']'),
    date('1996-01-01', '+' || (abs(random()) % 4748) || ' days'),            -- 1996–2008
    CASE WHEN r.n % 50 = 0 THEN 'UA' ELSE 'GE' END,                          -- ყოველი 50-ე უცხოელია
    CASE WHEN r.n % 50 = 0 THEN printf('%010d', r.n)                         -- უკრაინული: 10 ციფრი
                           ELSE '01' || printf('%09d', r.n) END,             -- ქართული: 11 ციფრი
    'GE',
    g.regioni,
    g.qalaqi,
    json_extract(L.quchebi, '$[' || (abs(random()) % json_array_length(L.quchebi)) || ']'),
    CAST(1 + abs(random()) % 150 AS TEXT),
    CASE WHEN abs(random()) % 10 < 3 THEN CAST(1 + abs(random()) % 80 AS TEXT) END,
    printf('%04d', 100 + abs(random()) % 6900),
    '+9955' || printf('%08d', abs(random()) % 100000000),
    'student' || r.n || '@example.com',
    round((abs(random()) % 401) / 100.0, 2),                                 -- 0.00–4.00
    4.0,
    date('2015-09-15', '+' || r.sy || ' years'),
    CASE WHEN r.sy < 8 THEN date('2019-06-30', '+' || r.sy || ' years') END  -- 2022-ის ჩათვლით დაწყებულებმა დაამთავრეს
FROM r
JOIN ge_cities g ON g.rn = r.ci
CROSS JOIN L;

DETACH DATABASE geo;
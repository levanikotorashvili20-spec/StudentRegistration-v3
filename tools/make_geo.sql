-- make_geo.sql
-- world.sqlite3-დან (dr5hn, ODbL ლიცენზია) ქმნის პატარა geo.db-ს
-- გაშვება tools საქაღალდიდან:
--   .\sqlite3.exe ..\data\geo.db ".read make_geo.sql"

-- ძველის წაშლა (თავიდან გაშვებისთვის). ATTACH-მდე, რომ წყაროს არ შეეხოს
DROP TABLE IF EXISTS main.cities;
DROP TABLE IF EXISTS main.states;
DROP TABLE IF EXISTS main.countries;

ATTACH DATABASE '../data/world.sqlite3' AS src;

BEGIN;

-- ქვეყნები
CREATE TABLE main.countries (
    iso2         TEXT PRIMARY KEY,   -- 'GE'
    name         TEXT NOT NULL,      -- ინგლისური სახელი
    name_ka      TEXT,               -- ქართული სახელი (თუ არსებობს)
    phonecode    TEXT,               -- '995'
    postal_regex TEXT                -- საფოსტო ინდექსის შესამოწმებლად
);

INSERT INTO main.countries (iso2, name, name_ka, phonecode, postal_regex)
SELECT iso2,
       name,
       CASE WHEN json_valid(translations) THEN json_extract(translations, '$.ka') END,
       phonecode,
       postal_code_regex
FROM src.countries
WHERE iso2 IS NOT NULL;

-- მხარეები / შტატები
CREATE TABLE main.states (
    id          INTEGER PRIMARY KEY,
    country_iso TEXT NOT NULL,
    name        TEXT NOT NULL,
    name_ka     TEXT
);

-- მხოლოდ ის მხარეები, რომლებსაც ქალაქები აქვს,
-- რომ არჩევის შემდეგ ქალაქების სია ცარიელი არ დახვდეს
INSERT INTO main.states (id, country_iso, name, name_ka)
SELECT s.id, s.country_code, s.name,
       COALESCE(
           CASE WHEN json_valid(s.translations) THEN json_extract(s.translations, '$.ka') END,
           CASE WHEN s.country_code = 'GE' THEN s.native END)
FROM src.states s
WHERE s.id IN (SELECT DISTINCT state_id FROM src.cities);

-- ქალაქები
CREATE TABLE main.cities (
    id          INTEGER PRIMARY KEY,
    state_id    INTEGER NOT NULL,
    country_iso TEXT NOT NULL,
    name        TEXT NOT NULL,
    name_ka     TEXT
);

   -- ქალაქების native სვეტი არასანდოა (ბევრი არასწორი სახელია), ამიტომ არ ვიყენებთ
   INSERT INTO main.cities (id, state_id, country_iso, name, name_ka)
   SELECT c.id, c.state_id, c.country_code, c.name,
          CASE WHEN json_valid(c.translations) THEN json_extract(c.translations, '$.ka') END
   FROM src.cities c;

-- ინდექსები სწრაფი ძებნისთვის
CREATE INDEX main.idx_states_country ON states (country_iso, name);
CREATE INDEX main.idx_cities_state   ON cities (state_id, name);
CREATE INDEX main.idx_cities_country ON cities (country_iso, name);

COMMIT;

DETACH DATABASE src;
VACUUM;
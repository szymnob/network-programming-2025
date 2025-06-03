
echo "Tworzenie bazy danych"
rm -f osoby.sqlite

#sqlite3 osoby.sqlite < ./schema.sql

sqlite3 osoby.sqlite "CREATE TABLE osoby (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    imie VARCHAR,
    nazwisko TEXT,
    telefon TEXT,
    adres TEXT
);
INSERT INTO osoby VALUES (1, 'Anna', 'Nowak', '+48124569988',
    'Rynek Główny 2, 30-001 Kraków');
INSERT INTO osoby VALUES (3, 'Adam', 'Nowak', '+48123456789',
    'ul. Wawelska 5, 30-002 Kraków');
INSERT INTO osoby VALUES (2, 'Jan', 'Kowalski', '+48127770022',
    'ul. Podzamcze 1, 31-001 Kraków');


CREATE TABLE psy(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    imie VARCHAR,
    rasa VARCHAR,
    wlasciciel_id INTEGER,
    FOREIGN KEY (wlasciciel_id) REFERENCES osoby(id) ON DELETE SET NULL
);

INSERT INTO psy VALUES (1, 'Azor', 'Owczarek niemiecki', 1);
INSERT INTO psy VALUES (2, 'Burek', 'Labrador', 1);
INSERT INTO psy VALUES (3, 'Reksio', 'Beagle', 2);
"

sleep 1

env -i python3 -u rest_webapp.py > stdout.txt 2> stderr.txt &
server_pid=$!

printf ">>> Serwer uruchomiony w tle. PID: %d\n" "$server_pid"

wait "$server_pid"
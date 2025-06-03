#!/usr/bin/python3
# -*- coding: UTF-8 -*-

'''
Aplikacja WSGI implementująca najważniejsze części opisywanej na wykładzie
usługi REST dającej dostęp do bazy z danymi osób.

Uwaga: kod dydaktyczny bez pełnej obsługi błędów i sytuacji nadzwyczajnych.

Aplikacja nie potrafi sama stworzyć swojej bazy danych, trzeba to zrobić
przed jej uruchomieniem. Skrypt rest_webapp.sh pokazuje jak.
'''

plik_bazy = './osoby.sqlite'

import re, sqlite3

class OsobyApp:
    def __init__(self, environment, start_response):
        '''
Konstruktor wywoływany przez serwer WSGI. Jak każdy konstruktor tworzy nowy
obiekt, następnie zapamiętuje w jego polach przekazane przez serwer argumenty
i inicjuje pola na odpowiedź.
'''
        self.env = environment
        self.start_response = start_response
        self.status = '200 OK'
        self.headers = [ ('Content-Type', 'text/html; charset=UTF-8') ]
        self.content = b''

    def __iter__(self):
        '''
Metoda obsługująca proces iterowania po stworzonym obiekcie. Serwer WSGI
wymaga aby w środku była co najmniej jedna instrukcja "yield" zwracająca
ciąg bajtów do odesłania klientowi HTTP.
'''
        try:
            self.route()
        except sqlite3.Error as e:
            s = 'SQLite error: ' + str(e)
            self.failure('500 Internal Server Error', s)
        n = len(self.content)
        self.headers.append( ('Content-Length', str(n)) )
        self.start_response(self.status, self.headers)
        yield self.content

    def failure(self, status, detail = None):
        '''
Metoda wstawiająca do pól obiektu status błędu oraz dokument HTML
z komunikatem o jego wystąpieniu.
'''
        self.status = status
        s = '<html>\n<head>\n<title>' + status + '</title>\n</head>\n'
        s += '<body>\n<h1>' + status + '</h1>\n'
        if detail is not None:
            s += '<p>' + detail + '</p>\n'
        s += '</body>\n</html>\n'
        self.content = s.encode('UTF-8')

    def route(self):
        '''
Pierwszą rzeczą, którą aplikacja musi zrobić po odebraniu zapytania, jest
sprawdzenie nazwy metody HTTP oraz nazwy zasobu. Jest to konieczne aby się
zorientować o co klient prosi i wywołać odpowiedni fragment kodu realizujący
jego zlecenie. Jest to tzw. routing zapytania.

W niniejszej aplikacji routing jest realizowany częściowo w tej metodzie,
a częściowo w metodach handle_table() i handle_item().
'''
        if self.env['PATH_INFO'] == '/osoby':
            self.handle_table()
            return
        if self.env['PATH_INFO'] == '/osoby/search':
            self.handle_search_osoby()
            return
        m = re.search('^/osoby/(?P<id>[0-9]+)$', self.env['PATH_INFO'])
        if m is not None:
            self.handle_item(m.group('id'))
            return

        if self.env['PATH_INFO'] == '/psy':
            self.handle_dogs_table()
            return
        if self.env['PATH_INFO'] == '/psy/search':
            self.handle_search_psy()
            return
        m = re.search('^/psy/(?P<id>[0-9]+)$', self.env['PATH_INFO'])
        if m is not None:
            self.handle_dog_item(m.group('id'))
            return

        self.failure('404 Not Found')

    def handle_search_osoby(self):
        '''
        Obsługa wyszukiwania osób po imieniu i nazwisku.
        '''
        if self.env['REQUEST_METHOD'] == 'GET':
            qs = self.parse_query_string(self.env['QUERY_STRING'])
            imie = qs.get('imie')
            nazwisko = qs.get('nazwisko')
            colnames, rows = self.sql_select_osoby(imie=imie, nazwisko=nazwisko)
            self.send_rows(colnames, rows)

    def handle_search_psy(self):
        '''
        Obsługa wyszukiwania osób po imieniu i nazwisku.
        '''
        if self.env['REQUEST_METHOD'] == 'GET':
            qs = self.parse_query_string(self.env['QUERY_STRING'])
            print(qs)
            imie = qs.get('imie')
            rasa = qs.get('rasa')
            colnames, rows = self.sql_select_psy(imie=imie, rasa=rasa)
            self.send_rows(colnames, rows)

    def parse_query_string(self, qs_raw):
        '''
        Parsuje surowy ciąg zapytania (query string) z adresu URL na imie i nazwisko
        '''
        result = {}
        print(qs_raw)
        if qs_raw:
            pairs = qs_raw.split('&')
            for pair in pairs:
                if '=' in pair:
                    key, val = pair.split('=', 1)
                    result[key] = val
        return result

    def handle_dogs_table(self):
        '''
Obsługa zapytań odnoszących się do tabeli "psy" traktowanej jako całość.
Można ją pobrać, albo można dodać do niej nowy wiersz.
'''
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select_psy()
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'POST':
            colnames, vals = self.read_tsv()
            q = 'INSERT INTO psy (' + ', '.join(colnames) + ') VALUES (' + ', '.join(['?' for _ in vals]) + ')'
            dog_id = self.sql_modify(q, vals)
            colnames, rows = self.sql_select_psy(id=dog_id)
            self.send_rows(colnames, rows)
        else:
            self.failure('501 Not Implemented')

    def handle_table(self):
        '''
Obsługa zapytań odnoszących się do tabeli "osoby" traktowanej jako całość.
Można ją pobrać, albo można dodać do niej nowy wiersz.
'''
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select_osoby()
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'POST':
            colnames, vals = self.read_tsv()
            q = 'INSERT INTO osoby (' + ', '.join(colnames) + ') VALUES ('
            q += ', '.join(['?' for v in vals]) + ')'
            id = self.sql_modify(q, vals)
            colnames, rows = self.sql_select_osoby(id=id)
            self.send_rows(colnames, rows)
        else:
            self.failure('501 Not Implemented')

    def handle_dog_item(self, id):
        '''
        Obsluga zapytan wiersza w tabeli "psy". Moozna go pobrać, zmodyfikować lub usunąć.
        :param id:
        :return:
        '''
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select_psy(id=id)
            if len(rows) == 0:
                self.failure('404 Not Found')
            else:
                self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'PUT':
            colnames, vals = self.read_tsv()
            q = 'UPDATE psy SET ' + ', '.join([c + ' = ?' for c in colnames]) + ' WHERE id = ?'
            self.sql_modify(q, vals + [id])
            colnames, rows = self.sql_select_psy(id=id)
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'DELETE':
            q = 'DELETE FROM psy WHERE id = ?'
            self.sql_modify(q, [id])
        else:
            self.failure('501 Not Implemented')

    def handle_item(self, id):
        '''
Obsługa zapytań odnoszących się do konkretnego wiersza w tabeli "osoby".
Można go pobrać, zmodyfikować, albo usunąć.
'''
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select_osoby(id)
            if len(rows) == 0:
                self.failure('404 Not Found')
            else:
                self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'PUT':
            colnames, vals = self.read_tsv()
            q = 'UPDATE osoby SET '
            q += ', '.join([c + ' = ?' for c in colnames])
            q += ' WHERE id = ' + str(id)
            self.sql_modify(q, vals)
            colnames, rows = self.sql_select_osoby(id)
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'DELETE':
            q = 'DELETE FROM osoby WHERE id = ' + str(id)
            self.sql_modify(q)
        else:
            self.failure('501 Not Implemented')

    def read_tsv(self):
        f = self.env['wsgi.input']
        n = int(self.env['CONTENT_LENGTH'])
        raw_bytes = f.read(n)
        lines = raw_bytes.decode('UTF-8').splitlines()
        colnames = lines[0].split('\t')
        vals = lines[1].split('\t')
        return colnames, vals

    def send_rows(self, colnames, rows):
        s = '\t'.join(colnames) + '\n'
        for row in rows:
            s += '\t'.join([str(val) for val in row]) + '\n'
        self.content = s.encode('UTF-8')
        self.headers = [ ('Content-Type',
                'text/tab-separated-values; charset=UTF-8') ]

    def sql_select_psy(self, id=None, imie=None, rasa=None):
        query = 'SELECT * FROM psy'
        where_params = []
        values = []

        if id is not None:
            where_params.append('id = ?')
            values.append(id)
        if imie is not None:
            where_params.append('imie = ?')
            values.append(imie)
        if rasa is not None:
            where_params.append('rasa = ?')
            values.append(rasa)

        if where_params:
            query += ' WHERE ' + ' AND '.join(where_params)

        return self.sql_select(query, values)
    
    def sql_select_osoby(self, id = None, imie = None, nazwisko = None):
        query = 'SELECT * FROM osoby'
        where_params = []
        values = []

        if id is not None:
            where_params.append('id = ?')
            values.append(id)
        if imie is not None:
            where_params.append('imie = ?')
            values.append(imie)
        if nazwisko is not None:
            where_params.append('nazwisko = ?')
            values.append(nazwisko)

        if where_params:
            query += ' WHERE ' + ' AND '.join(where_params)

        return self.sql_select(query, values)
    
    def sql_select(self, query, values):
        conn = sqlite3.connect(plik_bazy)
        crsr = conn.cursor()


        crsr.execute(query, values)
        colnames = [ d[0] for d in crsr.description ]
        rows = crsr.fetchall()
        crsr.close()
        conn.close()
        return colnames, rows

    def sql_modify(self, query, params = None):
        conn = sqlite3.connect(plik_bazy)
        conn.execute('PRAGMA foreign_keys = ON')
        crsr = conn.cursor()
        if params is None:
            crsr.execute(query)
        else:
            crsr.execute(query, params)
        rowid = crsr.lastrowid   # id wiersza wstawionego przez INSERT
        crsr.close()
        conn.commit()
        conn.close()
        return rowid

if __name__ == '__main__':
    from wsgiref.simple_server import make_server
    port = 8000
    httpd = make_server('', port, OsobyApp)
    print('Listening on port %i, press ^C to stop.' % port)
    httpd.serve_forever()

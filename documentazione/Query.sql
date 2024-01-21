SELECT * FROM studenti WHERE Matricola = '<id studente>'

INSERT INTO appelli (CodiceAppello, CodiceEsame, dData) VALUES ('<id>', '<id>', '<datetime>')

INSERT INTO esami (CodiceEsame, CodiceCorso, Tipo, Modalita, Descrizione) VALUES ('<id>', '<id>', '<tipo>', '<modalita>', '<descrizione>')

INSERT INTO prenotazioni (CodiceAppello, Studente, NumeroProgressivo) VALUES ('<id>', '<id>', '<smallint>')

SELECT CodiceCorso AS Codice, Nome, CFU From corsi

SELECT CodiceEsame AS Codice, Tipo, Modalita, Descrizione FROM esami WHERE CodiceCorso = '<id corso>'

SELECT appelli.CodiceAppello AS Codice, appelli.Data, corsi.Nome
FROM appelli
    JOIN esami ON appelli.CodiceEsame = esami.CodiceEsame
    JOIN corsi ON esami.CodiceCorso = corsi.CodiceCorso
WHERE
	NOT EXISTS (
		SELECT prenotazioni.CodiceAppello
		FROM prenotazioni
		WHERE prenotazioni.Studente = '<id studente>'
			AND prenotazioni.CodiceAppello = appelli.CodiceAppello
	)
	AND corsi.CodiceCorso = '<id esame>'

SELECT prenotazioni.CodiceAppello AS Codice,appelli.Data,corsi.Nome,esami.Tipo,esami.Modalita,esami.Descrizione, Prenotazioni.NumeroProgressivo AS NPrenotazione
FROM prenotazioni 
    JOIN studenti ON prenotazioni.Studente = studenti.Matricola 
    JOIN appelli ON prenotazioni.CodiceAppello = appelli.CodiceAppello 
    JOIN esami ON appelli.CodiceEsame = esami.CodiceEsame
    JOIN corsi ON corsi.CodiceCorso = esami.CodiceCorso
WHERE prenotazioni.Studente = '<id studente>'

SELECT count(*) AS MaxNumero FROM prenotazioni
WHERE CodiceAppello = '<id appello>'
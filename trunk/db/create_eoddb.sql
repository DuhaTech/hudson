CREATE TABLE eod(
	symbol VARCHAR(16) NOT NULL,
	day_date DATE NOT NULL,
	open_price REAL,
	high_price REAL,
	low_price REAL,
	close_price REAL,
	adjclose_price REAL NOT NULL,
	volume INTEGER
);

CREATE UNIQUE INDEX eodIdx
ON eod (symbol, day_date);

CREATE TABLE desc(
	symbol VARCHAR(16) PRIMARY KEY,
	source VARCHAR(32) NOT NULL,
	description VARCHAR(64)
);
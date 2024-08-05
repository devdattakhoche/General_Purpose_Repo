SELECT UPPER(
    CONCAT(
        LEFT(ticker, REGEXP_INSTR(ticker, '[a-z]') - 1),
        '-',
        RIGHT(ticker, CHAR_LENGTH(ticker) - REGEXP_INSTR(ticker, '[a-z]') + 1)
    )
) AS ticker_with_hyphen

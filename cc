SELECT 
    ticker,
    REGEXP_INSTR(ticker, '[a-z]') AS first_lowercase_position
FROM
    (SELECT '123abCDef' AS ticker
     UNION ALL
     SELECT 'ABCD'
     UNION ALL
     SELECT 'a1B2C3') AS sample_table;

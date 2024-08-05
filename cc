SELECT UPPER(
    CASE
        WHEN LOCATE(
            LOWER(SUBSTRING(ticker, 1, 1)), 
            ticker
        ) > 0 THEN
            REPLACE(
                CONCAT(
                    REPEAT('-', LOCATE(
                        LOWER(SUBSTRING(ticker, 1, 1)), 
                        ticker
                    ) - 1),
                    SUBSTRING(ticker, LOCATE(
                        LOWER(SUBSTRING(ticker, 1, 1)), 
                        ticker
                    ))
                ),
                '-', ''
            )
        ELSE
            ticker
    END
) AS processed_ticker
FROM (SELECT 'ABC' AS ticker) AS temp;

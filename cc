SELECT UPPER(
    IF(
        LOCATE(LOWER(SUBSTRING('AC*', 1, 1)), 'AC*') > 0,
        CONCAT(
            REPEAT('-', LOCATE(LOWER(SUBSTRING('AC*', 1, 1)), 'AC*') - 1),
            SUBSTRING('AC*', LOCATE(LOWER(SUBSTRING('AC*', 1, 1)), 'AC*'))
        ),
        'AC*'
    )
) AS processed_ticker;

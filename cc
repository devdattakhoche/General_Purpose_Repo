USE tempdb;

DELIMITER $$

IF NOT EXISTS (
    SELECT 1 
    FROM INFORMATION_SCHEMA.TABLES 
    WHERE TABLE_SCHEMA = 'tempdb' 
      AND TABLE_NAME = 'tableName'
) THEN
    CREATE TABLE tableName (
        top_cass2 INT NULL,
        cass2 INT NULL,
        countryCode VARCHAR(10) NULL,
        longBookId VARCHAR(10) NULL,
        shortBookId VARCHAR(10) NULL,
        longEtfBookId VARCHAR(10) NULL,
        shortEtfBookId VARCHAR(10) NULL,
        washBookId VARCHAR(10) NULL,
        clientBookId VARCHAR(10) NULL,
        clientTradingUnit VARCHAR(32) NULL,
        clientAccountMnemonic VARCHAR(32) NULL,
        flowType VARCHAR(20) NULL,
        productType VARCHAR(20) NULL,
        exchangeIds VARCHAR(100) NULL
    );
END IF$$

DELIMITER ;

TRUNCATE TABLE tableName;

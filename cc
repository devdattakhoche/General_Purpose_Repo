

-- liquibase formatted sql
-- changeset R721572:sp_process_grips_data-20220453:73:26 stripComments:false endDelimiter://
-- comment Create SP sp_process_grips_data

USE reference//
DROP PROCEDURE IF EXISTS reference.sp_process_grips_data//

CREATE PROCEDURE reference.sp_process_grips_data(IN par_delete_missing BIT)
BEGIN
    DECLARE var_cass_id INT;
    DECLARE var_sub_cass_id INT;
    DECLARE var_countryCode VARCHAR(10);
    DECLARE var_longBookId VARCHAR(10);
    DECLARE var_shortBookId VARCHAR(10);
    DECLARE var_longEtfBookId VARCHAR(10);
    DECLARE var_shortEtfBookId VARCHAR(10);
    DECLARE var_washBookId VARCHAR(10);
    DECLARE var_clientBookId VARCHAR(10);
    DECLARE var_clientTradingUnit VARCHAR(32);
    DECLARE var_clientAccountMnemonic VARCHAR(256);
    DECLARE var_flowType VARCHAR(20);
    DECLARE var_productType VARCHAR(20);
    DECLARE var_gripsExchangeIds VARCHAR(100);
    DECLARE var_gripsExchangeId VARCHAR(20);
    DECLARE var_exchId VARCHAR(20);
    DECLARE var_pos INT;
    DECLARE var_iterations INT;
    DECLARE var_print_string VARCHAR(100);
    DECLARE done_outer INT DEFAULT 0;
    DECLARE done_inner INT DEFAULT 0;
    DECLARE var_count INT (10);
    DECLARE var_print_string VARCHAR (500);



    -- Declare cursors
    DECLARE get_grips_row CURSOR FOR
SELECT cass2, cass2, countryCode, longBookId, shortBookId, longEtfBookId, shortEtfBookId, washBookId, clientBookId, clientTradingUnit, clientAccountMnemonic, flowType, productType, exchangeIds
FROM reference.grips_data_raw
WHERE flowType IN ('DMA', 'ALL', 'HIGH_TOUCH', 'NO_TOUCH') AND productType IN ('SWAP','PNOTE');

DECLARE get_grips_exchangeid_row CURSOR FOR
SELECT exchangeId
FROM `#tempSplitExchangeIds`;

-- Handlers for cursor
DECLARE CONTINUE HANDLER FOR NOT FOUND SET done_outer = 1, done_inner = 1;

    -- Create temporary table
    CREATE TEMPORARY TABLE IF NOT EXISTS `#tempSplitExchangeIds` (
        exchangeId VARCHAR(20) NULL
    );

    -- Debug print statement
SELECT CONCAT_WS('', 'Delete mode: ', CAST(par_delete_missing AS CHAR)) INTO var_print_string;
-- SELECT var_print_string;
call reference.sp_log_message('sp_process_grips_data','Job Started');
-- Example of update based on parameter
IF (par_delete_missing = 1) THEN
   call reference.sp_log_message('sp_process_grips_data','Is deleted mode true');
UPDATE reference.client_preference
SET last_modified_by = CONCAT('x', last_modified_by)
WHERE exec_region = 'AP' AND pref_type IN ('&BB', 'IBB', '+LB', '+SB', 'ELB', 'ESB') AND last_modified_by IN ('iGRIPS', 'uGRIPS');
END IF;

    -- Open outer cursor
OPEN get_grips_row;

-- Outer loop
outer_loop: LOOP
        FETCH get_grips_row INTO var_cass_id, var_sub_cass_id, var_countryCode, var_longBookId, var_shortBookId, var_longEtfBookId, var_shortEtfBookId, var_washBookId, var_clientBookId, var_clientTradingUnit, var_clientAccountMnemonic, var_flowType, var_productType, var_gripsExchangeIds;
        IF done_outer THEN
            LEAVE outer_loop;
END IF;

select 0 into var_count;
SELECT
    count(*)
FROM reference.client
WHERE id_cass2 = var_cass_id AND is_migrated = 'Y' into var_count;
IF var_count=0 THEN
SELECT
    CONCAT_WS('', 'cass2 not setup, cass2=', CAST(var_cass_id AS CHAR (12)))
INTO var_print_string;
call reference.sp_log_message('sp_process_grips_data',var_print_string);
ITERATE outer_loop;
END IF;

SELECT CONCAT_WS('', 
'Outer Loop where cass_id = ', CASE WHEN var_cass_id IS NULL THEN 'NULL' ELSE CAST(var_cass_id AS CHAR(10)) END, 
' and country_code = "', var_countryCode, 
'" and long_book_id = "', var_longBookId, 
'" and short_book_id = "', var_shortBookId, 
'" and long_etf_book_id = "', var_longEtfBookId, 
'" and short_etf_book_id = "', var_shortEtfBookId, 
'" and wash_book_id = "', var_washBookId, 
'" and client_book_id = "', var_clientBookId, 
'" and client_trading_unit = "', var_clientTradingUnit, 
'" and client_account_mnemonic = "', var_clientAccountMnemonic, 
'" and flow_type = "', var_flowType, 
'" and product_type = "', var_productType, 
'" and grips_exchange_ids = "', var_gripsExchangeIds, 
'"'
	) INTO var_print_string;

CALL reference.sp_log_message('sp_process_grips_data', var_print_string);

        -- Process based on countryCode
        IF (var_countryCode = 'CN') THEN
            -- Example modification to var_gripsExchangeIds
            IF (LOCATE('[', var_gripsExchangeIds) > 0) THEN
SELECT SUBSTRING(var_gripsExchangeIds, LOCATE('[', var_gripsExchangeIds) + 1, LOCATE(']', var_gripsExchangeIds) - 2) INTO var_gripsExchangeIds;
END IF;

            -- Splitting var_gripsExchangeIds
SELECT LOCATE(',', var_gripsExchangeIds) INTO var_pos;
IF (var_pos = 0) THEN
                -- Only one exchange id
                INSERT INTO `#tempSplitExchangeIds` VALUES (var_gripsExchangeIds);
ELSE
                -- Multiple exchange ids, split and insert
                SET var_iterations = 0; -- Reset iteration count
                WHILE (var_pos <> 0 AND var_iterations < 20) DO
SELECT SUBSTRING(var_gripsExchangeIds, 1, var_pos - 1) INTO var_exchId;
INSERT INTO `#tempSplitExchangeIds` VALUES (var_exchId);
SET var_gripsExchangeIds = INSERT(var_gripsExchangeIds, 1, var_pos, NULL);
SELECT LOCATE(',', var_gripsExchangeIds) INTO var_pos;
SET var_iterations = var_iterations + 1;
END WHILE;
                -- Insert the last element
INSERT INTO `#tempSplitExchangeIds` VALUES (var_gripsExchangeIds);
END IF;
ELSE
            -- Insert a single row with NULL for non-CN countries
            INSERT INTO `#tempSplitExchangeIds` VALUES (NULL);
END IF;

        -- Modify exchangeId values in temp table
UPDATE `#tempSplitExchangeIds`
SET exchangeId = CASE exchangeId
                     WHEN '568913' THEN '620'
                     WHEN '838746' THEN '621'
                     WHEN '10001010' THEN '33'
                     WHEN '20001003' THEN '224'
                     ELSE exchangeId
    END;




-- Open inner cursor
OPEN get_grips_exchangeid_row;
set done_inner=0;
-- Inner loop
inner_loop: LOOP
            FETCH get_grips_exchangeid_row INTO var_gripsExchangeId;

            IF done_inner THEN
               set done_outer=0;
                LEAVE inner_loop;
END IF;
            -- Process based on flowType and productType
            IF ((var_flowType = 'NO_TOUCH') AND var_productType = 'SWAP') THEN
                -- Example procedure calls
                IF (LEFT(var_clientAccountMnemonic, 5) = 'JDMA_') THEN
                    SET var_clientAccountMnemonic = NULL;
END IF;

CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
END IF;
			
			            IF ((var_flowType = 'DMA' OR var_flowType = 'ALL') AND var_productType = 'SWAP') THEN
			                CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'DMS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'ALS', 0, var_clientAccountMnemonic, 'ECS', par_delete_missing, 0, @ReturnCode);
END IF;
			
			            IF ((var_flowType = 'HIGH_TOUCH' OR var_flowType = 'ALL') AND var_productType = 'SWAP') THEN
			                CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'SSS', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'PTS', 0, var_clientAccountMnemonic, 'PT', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'PTS', 0, var_clientAccountMnemonic, 'PT', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'PTS', 0, var_clientAccountMnemonic, 'PT', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'PTS', 0, var_clientAccountMnemonic, 'PT', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'PTS', 0, var_clientAccountMnemonic, 'PT', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'PTS', 0, var_clientAccountMnemonic, 'PT', par_delete_missing, 0, @ReturnCode);
END IF;
			
			            IF ((var_flowType = 'HIGH_TOUCH' OR var_flowType = 'ALL') AND var_productType = 'PNOTE') THEN
			                CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '&BB', var_clientBookId, 'PNT', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'IBB', var_washBookId, 'PNT', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+LB', var_longBookId, 'PNT', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, '+SB', var_shortBookId, 'PNT', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ELB', var_longEtfBookId, 'PNT', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
CALL sp_process_grips_record (var_cass_id, NULL, var_countryCode, var_gripsExchangeId, 'ESB', var_shortEtfBookId, 'PNT', 0, var_clientAccountMnemonic, 'CASH', par_delete_missing, 0, @ReturnCode);
END IF;


END LOOP; -- End of inner loop

        -- Close inner cursor
CLOSE get_grips_exchangeid_row;

END LOOP; -- End of outer loop

    -- Close outer cursor
CLOSE get_grips_row;

-- Clean up temporary table
TRUNCATE TABLE `#tempSplitExchangeIds`;
DROP TEMPORARY TABLE IF EXISTS `#tempSplitExchangeIds`;

    -- Example update if par_delete_missing is 1
    IF (par_delete_missing = 1) THEN
       call reference.sp_log_message('sp_process_grips_data','Inside last if before job completion');
UPDATE reference.client_preference
SET pref_enabled = 0, last_modified_by = 'dGRIPS', last_modified_date = NOW(6)
WHERE exec_region = 'AP' AND pref_type IN ('&BB', 'IBB', '+LB', '+SB', 'ELB', 'ESB') AND last_modified_by IN ('xiGRIPS', 'xuGRIPS');
END IF;
call reference.sp_log_message('sp_process_grips_data','Job Completed');
END;

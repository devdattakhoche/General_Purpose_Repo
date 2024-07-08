declare get_grips_row cursor for
    select cass2, cass2, countryCode, longBookId, shortBookId, longEtfBookId, shortEtfBookId, washBookId, clientBookId, clientTradingUnit, clientAccountMnemonic, flowType, productType, exchangeIds
    from tempdb..grips_data_raw
    where flowType in ('DMA', 'ALL', 'HIGH_TOUCH', 'NO_TOUCH') and productType in ('SWAP','PNOTE')
        for read only

use tempdb
go

IF NOT EXISTS (SELECT 1 FROM sysobjects WHERE name = 'tableName' AND type = 'U')
BEGIN
        Execute("Create Table tableName (
                top_cass2 int null,
                cass2 int null,
                countryCode varchar(10) null,
                longBookId varchar(10) null,
                shortBookId varchar(10) null,
                longEtfBookId varchar(10) null,
                shortEtfBookId varchar(10) null,
                washBookId varchar(10) null,
                clientBookId varchar(10) null,
                clientTradingUnit varchar(32) null,
                clientAccountMnemonic varchar(32) null,
                flowType varchar(20) null,
                productType varchar(20) null,
                exchangeIds varchar(100) null
                )")
END
GO

truncate table tableName

go

/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]
GO

/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS]
GO

/****** Objeto:  procedimiento almacenado dbo.SP_FECHAS_DE_FERIADO    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_FECHAS_DE_FERIADO]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_FECHAS_DE_FERIADO]
GO

/****** Objeto:  procedimiento almacenado dbo.SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main]
GO

/****** Objeto:  procedimiento almacenado dtn.SP_TEF_PASE_A_HISTORICO    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dtn].[SP_TEF_PASE_A_HISTORICO]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dtn].[SP_TEF_PASE_A_HISTORICO]
GO

/****** Objeto:  procedimiento almacenado dbo.SP_UPDATE_DN_PARAMETROS    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_UPDATE_DN_PARAMETROS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_UPDATE_DN_PARAMETROS]
GO

/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_MAESTRO_DE_CUENTAS    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_VIEW_MAESTRO_DE_CUENTAS]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_VIEW_MAESTRO_DE_CUENTAS]
GO

/****** Objeto:  procedimiento almacenado dbo.SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA]
GO

/****** Objeto:  procedimiento almacenado dbo.SP_VERIFICA_FERIADO    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[SP_VERIFICA_FERIADO]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[SP_VERIFICA_FERIADO]
GO

/****** Objeto: vista dbo.VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]') and OBJECTPROPERTY(id, N'IsView') = 1)
drop view [dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]
GO

/****** Objeto: tabla [dbo].[ACUERD]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ACUERD]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[ACUERD]
GO

/****** Objeto: tabla [dbo].[CLIENT]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CLIENT]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[CLIENT]
GO

/****** Objeto: tabla [dbo].[EXTCTA]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[EXTCTA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[EXTCTA]
GO

/****** Objeto: tabla [dtn].[EXTCTA_DISTINCT]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dtn].[EXTCTA_DISTINCT]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dtn].[EXTCTA_DISTINCT]
GO

/****** Objeto: tabla [dtn].[EXTCTA_DUP]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dtn].[EXTCTA_DUP]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dtn].[EXTCTA_DUP]
GO

/****** Objeto: tabla [dtn].[EXTCTA_HISTORICO]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dtn].[EXTCTA_HISTORICO]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dtn].[EXTCTA_HISTORICO]
GO

/****** Objeto: tabla [dtn].[EXTCTA_SINGLE]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dtn].[EXTCTA_SINGLE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dtn].[EXTCTA_SINGLE]
GO

/****** Objeto: tabla [dbo].[FERIAD]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FERIAD]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[FERIAD]
GO

/****** Objeto: tabla [dbo].[MAECUE]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MAECUE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MAECUE]
GO

/****** Objeto: tabla [dbo].[NOVEDA]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[NOVEDA]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[NOVEDA]
GO

/****** Objeto: tabla [dbo].[TABGEN]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[TABGEN]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[TABGEN]
GO

/****** Objeto: tabla [dbo].[dn_abonados_claves]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_abonados_claves]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_abonados_claves]
GO

/****** Objeto: tabla [dbo].[dn_abonados_cuentas]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_abonados_cuentas]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_abonados_cuentas]
GO

/****** Objeto: tabla [dbo].[dn_bancos_claves]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_bancos_claves]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_bancos_claves]
GO

/****** Objeto: tabla [dbo].[dn_comisiones]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_comisiones]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_comisiones]
GO

/****** Objeto: tabla [dbo].[dn_parametros]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_parametros]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_parametros]
GO

/****** Objeto: tabla [dbo].[dn_tef_enviado]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_tef_enviado]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_tef_enviado]
GO

/****** Objeto: tabla [dbo].[dn_tef_online]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_tef_online]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_tef_online]
GO

/****** Objeto: tabla [dtn].[dn_tef_online_historico]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dtn].[dn_tef_online_historico]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dtn].[dn_tef_online_historico]
GO

/****** Objeto: tabla [dbo].[dn_tef_online_historico]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[dn_tef_online_historico]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[dn_tef_online_historico]
GO

/****** Objeto: tabla [dtn].[extcta_20070416]    fecha de la secuencia de comandos: 24/04/2007 11:23:49 a.m. ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dtn].[extcta_20070416]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dtn].[extcta_20070416]
GO

/****** Objeto: inicio de sesión coa   fecha de la secuencia de comandos: 24/04/2007 11:23:42 a.m. ******/
if not exists (select * from master.dbo.syslogins where loginname = N'coa')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'FUEGODNET', @loginlang = N'Español'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'coa', null, @logindb, @loginlang
END
GO

/****** Objeto: inicio de sesión dbo   fecha de la secuencia de comandos: 24/04/2007 11:23:42 a.m. ******/
if not exists (select * from master.dbo.syslogins where loginname = N'dbo')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'master', @loginlang = N'Español'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'dbo', null, @logindb, @loginlang
END
GO

/****** Objeto: inicio de sesión dtn   fecha de la secuencia de comandos: 24/04/2007 11:23:42 a.m. ******/
if not exists (select * from master.dbo.syslogins where loginname = N'dtn')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'FUEGODNET', @loginlang = N'Español'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'dtn', null, @logindb, @loginlang
END
GO

/****** Objeto: inicio de sesión testdtn   fecha de la secuencia de comandos: 24/04/2007 11:23:42 a.m. ******/
if not exists (select * from master.dbo.syslogins where loginname = N'testdtn')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'FUEGODNET', @loginlang = N'Español'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'testdtn', null, @logindb, @loginlang
END
GO

/****** Objeto: inicio de sesión testlink   fecha de la secuencia de comandos: 24/04/2007 11:23:42 a.m. ******/
if not exists (select * from master.dbo.syslogins where loginname = N'testlink')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'FUEGODNET', @loginlang = N'Español'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'testlink', null, @logindb, @loginlang
END
GO

/****** Objeto: inicio de sesión UserH24   fecha de la secuencia de comandos: 24/04/2007 11:23:42 a.m. ******/
if not exists (select * from master.dbo.syslogins where loginname = N'UserH24')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'master', @loginlang = N'Español'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'UserH24', null, @logindb, @loginlang
END
GO

/****** Objeto: inicio de sesión BUILTIN\Administradores   fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addsrvrolemember N'BUILTIN\Administradores', sysadmin
GO

/****** Objeto: inicio de sesión testlink   fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addsrvrolemember N'testlink', dbcreator
GO

/****** Objeto: inicio de sesión testlink   fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addsrvrolemember N'testlink', bulkadmin
GO

/****** Objeto: usuario coa    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
if not exists (select * from dbo.sysusers where name = N'coa' and uid < 16382)
	EXEC sp_grantdbaccess N'coa', N'coa'
GO

/****** Objeto: usuario dbo    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
/****** Objeto: usuario dtn    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
if not exists (select * from dbo.sysusers where name = N'dtn' and uid < 16382)
	EXEC sp_grantdbaccess N'dtn', N'dtn'
GO

GRANT  CREATE FUNCTION ,  CREATE TABLE ,  CREATE VIEW ,  CREATE PROCEDURE ,  DUMP DATABASE ,  CREATE DEFAULT ,  DUMP TRANSACTION ,  CREATE RULE  TO [dtn]
GO

/****** Objeto: usuario testdtn    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
if not exists (select * from dbo.sysusers where name = N'testdtn' and uid < 16382)
	EXEC sp_grantdbaccess N'testdtn', N'testdtn'
GO

/****** Objeto: usuario testlink    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
if not exists (select * from dbo.sysusers where name = N'testlink' and uid < 16382)
	EXEC sp_grantdbaccess N'testlink', N'testlink'
GO

/****** Objeto: usuario UserH24    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
if not exists (select * from dbo.sysusers where name = N'UserH24' and uid < 16382)
	EXEC sp_grantdbaccess N'UserH24', N'UserH24'
GO

/****** Objeto: usuario testdtn    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_datareader', N'testdtn'
GO

/****** Objeto: usuario testlink    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_datareader', N'testlink'
GO

/****** Objeto: usuario UserH24    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_datareader', N'UserH24'
GO

/****** Objeto: usuario testdtn    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_datawriter', N'testdtn'
GO

/****** Objeto: usuario testlink    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_datawriter', N'testlink'
GO

/****** Objeto: usuario UserH24    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_datawriter', N'UserH24'
GO

/****** Objeto: usuario coa    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_owner', N'coa'
GO

/****** Objeto: usuario dtn    fecha de la secuencia de comandos: 24/04/2007 11:23:43 a.m. ******/
exec sp_addrolemember N'db_owner', N'dtn'
GO

/****** Objeto: tabla [dbo].[ACUERD]    fecha de la secuencia de comandos: 24/04/2007 11:23:51 a.m. ******/
CREATE TABLE [dbo].[ACUERD] (
	[AC_CODEMP] [numeric](5, 0) NOT NULL ,
	[AC_SUCURS] [numeric](5, 0) NOT NULL ,
	[AC_CODSIS] [smallint] NOT NULL ,
	[AC_TIPCTA] [numeric](5, 0) NOT NULL ,
	[AC_MONEDA] [numeric](5, 0) NOT NULL ,
	[AC_CUENTA] [numeric](15, 0) NOT NULL ,
	[AC_ORDEN] [numeric](15, 0) NOT NULL ,
	[AC_NROCLI] [numeric](15, 0) NOT NULL ,
	[AC_NROSOL] [numeric](15, 0) NOT NULL ,
	[AC_TIPACU] [smallint] NOT NULL ,
	[AC_CODLIN] [smallint] NOT NULL ,
	[AC_CODGAR] [smallint] NOT NULL ,
	[AC_PLATOT] [numeric](5, 0) NOT NULL ,
	[AC_FECALT] [datetime] NOT NULL ,
	[AC_FECVTO] [datetime] NOT NULL ,
	[AC_TASNOM] [numeric](12, 6) NOT NULL ,
	[AC_TASEFE] [numeric](12, 6) NOT NULL ,
	[AC_MONACU] [numeric](15, 2) NOT NULL ,
	[AC_TIPAPL] [smallint] NOT NULL ,
	[AC_AUTO01] [numeric](5, 0) NOT NULL ,
	[AC_AUTO02] [numeric](5, 0) NOT NULL ,
	[AC_AUTO03] [numeric](5, 0) NOT NULL ,
	[AC_FECPRO] [datetime] NOT NULL ,
	[AC_ESTADO] [smallint] NOT NULL ,
	[AC_FECCAN] [datetime] NOT NULL ,
	[AC_COME01] [char] (70) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[AC_COME02] [char] (70) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[AC_COME03] [char] (70) COLLATE Modern_Spanish_CI_AS NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[CLIENT]    fecha de la secuencia de comandos: 24/04/2007 11:23:53 a.m. ******/
CREATE TABLE [dbo].[CLIENT] (
	[CL_NROCLI] [numeric](15, 0) NOT NULL ,
	[CL_TIPPER] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_CATEGO] [smallint] NULL ,
	[CL_SECTOR] [smallint] NULL ,
	[CL_RESIDE] [smallint] NULL ,
	[CL_CODCON] [smallint] NULL ,
	[CL_NOMCLI] [char] (40) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[CL_DOMCLI] [char] (40) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_POSCLI] [smallint] NULL ,
	[CL_LOCCLI] [char] (30) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_DOMCOR] [char] (40) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_POSCOR] [smallint] NULL ,
	[CL_LOCCOR] [char] (30) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_TELEFO] [char] (40) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_FECALT] [datetime] NOT NULL ,
	[CL_FECBAJ] [datetime] NOT NULL ,
	[CL_MOTBAJ] [smallint] NULL ,
	[CL_SUCURS] [numeric](5, 0) NULL ,
	[CL_NROFUN] [numeric](5, 0) NULL ,
	[CL_TIPCLI] [smallint] NOT NULL ,
	[CL_CLACOD] [smallint] NULL ,
	[CL_CLAFEC] [datetime] NULL ,
	[CL_CLAPRO] [datetime] NULL ,
	[CL_CLAULT] [datetime] NULL ,
	[CL_CLACES] [smallint] NULL ,
	[CL_CLAVES] [datetime] NULL ,
	[CL_CODIVA] [smallint] NULL ,
	[CL_RETIVA] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_CODACT] [numeric](5, 0) NULL ,
	[CL_CODRAM] [numeric](5, 0) NULL ,
	[CL_GRUPRO] [numeric](5, 0) NULL ,
	[CL_GRUBCR] [numeric](5, 0) NULL ,
	[CL_TIPDOC] [smallint] NULL ,
	[CL_NRODOC] [numeric](15, 0) NULL ,
	[CL_NATJUR] [smallint] NULL ,
	[CL_CANEMP] [numeric](8, 0) NULL ,
	[CL_INSRPC] [char] (30) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_NOMPAD] [char] (40) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_NROLEG] [char] (25) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_CTABCR] [numeric](10, 0) NULL ,
	[CL_CODBCO_LIQ] [numeric](5, 0) NULL ,
	[CL_CUENTA_LIQ] [char] (15) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_CODBCO_USA] [numeric](5, 0) NULL ,
	[CL_CUENTA_USA] [char] (15) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_COSELL] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_COPUNI] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[CL_FECEST] [datetime] NOT NULL ,
	[CL_FECPRO] [datetime] NOT NULL ,
	[CL_NROPRO] [numeric](5, 0) NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[EXTCTA]    fecha de la secuencia de comandos: 24/04/2007 11:23:54 a.m. ******/
CREATE TABLE [dbo].[EXTCTA] (
	[EX_CODEMP] [numeric](5, 0) NOT NULL ,
	[EX_SUCURS] [numeric](5, 0) NOT NULL ,
	[EX_CODSIS] [smallint] NOT NULL ,
	[EX_TIPCTA] [numeric](5, 0) NOT NULL ,
	[EX_MONEDA] [numeric](5, 0) NOT NULL ,
	[EX_CUENTA] [numeric](15, 0) NOT NULL ,
	[EX_ORDEN] [numeric](15, 0) NOT NULL ,
	[EX_NROLOG] [numeric](12, 0) NOT NULL ,
	[EX_LOGCTA] [numeric](12, 0) NOT NULL ,
	[EX_NROCLI] [numeric](15, 0) NOT NULL ,
	[EX_NROCOM] [numeric](10, 0) NOT NULL ,
	[EX_NROCHE] [numeric](10, 0) NOT NULL ,
	[EX_CODBAN] [numeric](5, 0) NOT NULL ,
	[EX_CONCEP] [char] (40) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECOPE] [datetime] NOT NULL ,
	[EX_FECIMP] [datetime] NOT NULL ,
	[EX_CODTRA] [numeric](5, 0) NOT NULL ,
	[EX_TIPOPE] [smallint] NOT NULL ,
	[EX_IMPORT] [numeric](15, 2) NOT NULL ,
	[EX_CODCAM] [numeric](5, 0) NOT NULL ,
	[EX_PLACON] [smallint] NOT NULL ,
	[EX_FECCON] [datetime] NOT NULL ,
	[EX_HORCON] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_SUCORI] [numeric](5, 0) NOT NULL ,
	[EX_TIPMOV] [char] (2) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROTER] [numeric](5, 0) NOT NULL ,
	[EX_CODOPE] [numeric](5, 0) NOT NULL ,
	[EX_DATEOR] [datetime] NOT NULL ,
	[EX_TIMEOR] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROCAJ] [numeric](5, 0) NOT NULL ,
	[EX_CODCAJ] [numeric](5, 0) NOT NULL ,
	[EX_DATECA] [datetime] NOT NULL ,
	[EX_TIMECA] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_CODERR] [smallint] NOT NULL ,
	[EX_CODLIQ] [char] (1) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECCHE] [datetime] NOT NULL ,
	[EX_ASIIMP] [numeric](5, 0) NOT NULL ,
	[EX_SALACT] [numeric](15, 2) NOT NULL ,
	[EX_ESTADO] [smallint] NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dtn].[EXTCTA_DISTINCT]    fecha de la secuencia de comandos: 24/04/2007 11:23:54 a.m. ******/
CREATE TABLE [dtn].[EXTCTA_DISTINCT] (
	[EX_CODEMP] [numeric](5, 0) NOT NULL ,
	[EX_SUCURS] [numeric](5, 0) NOT NULL ,
	[EX_CODSIS] [smallint] NOT NULL ,
	[EX_TIPCTA] [numeric](5, 0) NOT NULL ,
	[EX_MONEDA] [numeric](5, 0) NOT NULL ,
	[EX_CUENTA] [numeric](15, 0) NOT NULL ,
	[EX_ORDEN] [numeric](15, 0) NOT NULL ,
	[EX_NROLOG] [numeric](12, 0) NOT NULL ,
	[EX_LOGCTA] [numeric](12, 0) NOT NULL ,
	[EX_NROCLI] [numeric](15, 0) NOT NULL ,
	[EX_NROCOM] [numeric](10, 0) NOT NULL ,
	[EX_NROCHE] [numeric](10, 0) NOT NULL ,
	[EX_CODBAN] [numeric](5, 0) NOT NULL ,
	[EX_CONCEP] [char] (40) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECOPE] [datetime] NOT NULL ,
	[EX_FECIMP] [datetime] NOT NULL ,
	[EX_CODTRA] [numeric](5, 0) NOT NULL ,
	[EX_TIPOPE] [smallint] NOT NULL ,
	[EX_IMPORT] [numeric](15, 2) NOT NULL ,
	[EX_CODCAM] [numeric](5, 0) NOT NULL ,
	[EX_PLACON] [smallint] NOT NULL ,
	[EX_FECCON] [datetime] NOT NULL ,
	[EX_HORCON] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_SUCORI] [numeric](5, 0) NOT NULL ,
	[EX_TIPMOV] [char] (2) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROTER] [numeric](5, 0) NOT NULL ,
	[EX_CODOPE] [numeric](5, 0) NOT NULL ,
	[EX_DATEOR] [datetime] NOT NULL ,
	[EX_TIMEOR] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROCAJ] [numeric](5, 0) NOT NULL ,
	[EX_CODCAJ] [numeric](5, 0) NOT NULL ,
	[EX_DATECA] [datetime] NOT NULL ,
	[EX_TIMECA] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_CODERR] [smallint] NOT NULL ,
	[EX_CODLIQ] [char] (1) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECCHE] [datetime] NOT NULL ,
	[EX_ASIIMP] [numeric](5, 0) NOT NULL ,
	[EX_SALACT] [numeric](15, 2) NOT NULL ,
	[EX_ESTADO] [smallint] NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dtn].[EXTCTA_DUP]    fecha de la secuencia de comandos: 24/04/2007 11:23:55 a.m. ******/
CREATE TABLE [dtn].[EXTCTA_DUP] (
	[Q] [int] NULL ,
	[EX_NROLOG] [numeric](12, 0) NOT NULL ,
	[EX_LOGCTA] [numeric](12, 0) NOT NULL ,
	[EX_IMPORT] [numeric](15, 2) NOT NULL ,
	[EX_TIPOPE] [smallint] NOT NULL ,
	[EX_FECCON] [datetime] NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dtn].[EXTCTA_HISTORICO]    fecha de la secuencia de comandos: 24/04/2007 11:23:55 a.m. ******/
CREATE TABLE [dtn].[EXTCTA_HISTORICO] (
	[EX_CODEMP] [numeric](5, 0) NOT NULL ,
	[EX_SUCURS] [numeric](5, 0) NOT NULL ,
	[EX_CODSIS] [smallint] NOT NULL ,
	[EX_TIPCTA] [numeric](5, 0) NOT NULL ,
	[EX_MONEDA] [numeric](5, 0) NOT NULL ,
	[EX_CUENTA] [numeric](15, 0) NOT NULL ,
	[EX_ORDEN] [numeric](15, 0) NOT NULL ,
	[EX_NROLOG] [numeric](12, 0) NOT NULL ,
	[EX_LOGCTA] [numeric](12, 0) NOT NULL ,
	[EX_NROCLI] [numeric](15, 0) NOT NULL ,
	[EX_NROCOM] [numeric](10, 0) NOT NULL ,
	[EX_NROCHE] [numeric](10, 0) NOT NULL ,
	[EX_CODBAN] [numeric](5, 0) NOT NULL ,
	[EX_CONCEP] [char] (40) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECOPE] [datetime] NOT NULL ,
	[EX_FECIMP] [datetime] NOT NULL ,
	[EX_CODTRA] [numeric](5, 0) NOT NULL ,
	[EX_TIPOPE] [smallint] NOT NULL ,
	[EX_IMPORT] [numeric](15, 2) NOT NULL ,
	[EX_CODCAM] [numeric](5, 0) NOT NULL ,
	[EX_PLACON] [smallint] NOT NULL ,
	[EX_FECCON] [datetime] NOT NULL ,
	[EX_HORCON] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_SUCORI] [numeric](5, 0) NOT NULL ,
	[EX_TIPMOV] [char] (2) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROTER] [numeric](5, 0) NOT NULL ,
	[EX_CODOPE] [numeric](5, 0) NOT NULL ,
	[EX_DATEOR] [datetime] NOT NULL ,
	[EX_TIMEOR] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROCAJ] [numeric](5, 0) NOT NULL ,
	[EX_CODCAJ] [numeric](5, 0) NOT NULL ,
	[EX_DATECA] [datetime] NOT NULL ,
	[EX_TIMECA] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_CODERR] [smallint] NOT NULL ,
	[EX_CODLIQ] [char] (1) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECCHE] [datetime] NOT NULL ,
	[EX_ASIIMP] [numeric](5, 0) NOT NULL ,
	[EX_SALACT] [numeric](15, 2) NOT NULL ,
	[EX_ESTADO] [smallint] NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dtn].[EXTCTA_SINGLE]    fecha de la secuencia de comandos: 24/04/2007 11:23:55 a.m. ******/
CREATE TABLE [dtn].[EXTCTA_SINGLE] (
	[Q] [int] NULL ,
	[EX_NROLOG] [numeric](12, 0) NOT NULL ,
	[EX_LOGCTA] [numeric](12, 0) NOT NULL ,
	[EX_IMPORT] [numeric](15, 2) NOT NULL ,
	[EX_TIPOPE] [smallint] NOT NULL ,
	[EX_FECCON] [datetime] NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[FERIAD]    fecha de la secuencia de comandos: 24/04/2007 11:23:56 a.m. ******/
CREATE TABLE [dbo].[FERIAD] (
	[FE_SUCURS] [numeric](5, 0) NOT NULL ,
	[FE_FECPRO] [datetime] NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[MAECUE]    fecha de la secuencia de comandos: 24/04/2007 11:23:56 a.m. ******/
CREATE TABLE [dbo].[MAECUE] (
	[MC_CODEMP] [numeric](5, 0) NOT NULL ,
	[MC_SUCURS] [numeric](5, 0) NOT NULL ,
	[MC_CODSIS] [smallint] NOT NULL ,
	[MC_TIPCTA] [numeric](5, 0) NOT NULL ,
	[MC_MONEDA] [numeric](5, 0) NOT NULL ,
	[MC_CUENTA] [numeric](15, 0) NOT NULL ,
	[MC_ORDEN] [numeric](15, 0) NOT NULL ,
	[MC_NROCLI] [numeric](15, 0) NOT NULL ,
	[MC_TIPORD] [smallint] NOT NULL ,
	[MC_CODTAS] [char] (1) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[MC_TASNOM] [numeric](12, 6) NOT NULL ,
	[MC_CODCAP] [smallint] NOT NULL ,
	[MC_COMMAN] [smallint] NOT NULL ,
	[MC_COMEXT] [smallint] NOT NULL ,
	[MC_FECALT] [datetime] NOT NULL ,
	[MC_FECMOV] [datetime] NOT NULL ,
	[MC_FECACT] [datetime] NOT NULL ,
	[MC_FECPRO] [datetime] NOT NULL ,
	[MC_SALACT] [numeric](15, 2) NOT NULL ,
	[MC_INTPAS] [numeric](12, 5) NOT NULL ,
	[MC_INTACT] [numeric](12, 5) NOT NULL ,
	[MC_FECCAP] [datetime] NOT NULL ,
	[MC_INTCAP] [numeric](12, 5) NOT NULL ,
	[MC_SALCAP] [numeric](15, 2) NOT NULL ,
	[MC_FECDEV] [datetime] NOT NULL ,
	[MC_DEVMES] [numeric](12, 5) NOT NULL ,
	[MC_CODBLO] [numeric](5, 0) NOT NULL ,
	[MC_IMPBLO] [numeric](15, 2) NOT NULL ,
	[MC_FECBLO] [datetime] NOT NULL ,
	[MC_CANEXT] [numeric](5, 0) NOT NULL ,
	[MC_CANCHE] [numeric](5, 0) NOT NULL ,
	[MC_CODEXT] [char] (1) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[MC_ENVEXT] [smallint] NOT NULL ,
	[MC_VAL000] [numeric](15, 2) NULL ,
	[MC_VAL024] [numeric](15, 2) NULL ,
	[MC_VAL048] [numeric](15, 2) NULL ,
	[MC_VAL072] [numeric](15, 2) NULL ,
	[MC_VAL096] [numeric](15, 2) NULL ,
	[MC_VAL120] [numeric](15, 2) NULL ,
	[MC_VAL144] [numeric](15, 2) NULL ,
	[MC_VAL999] [numeric](15, 2) NULL ,
	[MC_NUMERA] [numeric](12, 0) NOT NULL ,
	[MC_CODBAJ] [smallint] NOT NULL ,
	[MC_FECBAJ] [datetime] NOT NULL ,
	[MC_ESTADO] [smallint] NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[NOVEDA]    fecha de la secuencia de comandos: 24/04/2007 11:23:56 a.m. ******/
CREATE TABLE [dbo].[NOVEDA] (
	[NV_TIPNOV] [numeric](6, 0) NOT NULL ,
	[NV_TIPCTA] [numeric](5, 0) NOT NULL ,
	[NV_CUENTA] [numeric](15, 0) NOT NULL ,
	[NV_ORDEN] [numeric](15, 0) NOT NULL ,
	[NV_FECALT] [datetime] NOT NULL ,
	[NV_FECBAJ] [datetime] NOT NULL ,
	[NV_FECPRO] [datetime] NOT NULL ,
	[NV_USERID] [char] (8) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[NV_USERID_BAJ] [char] (8) COLLATE Modern_Spanish_CI_AS NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[TABGEN]    fecha de la secuencia de comandos: 24/04/2007 11:23:57 a.m. ******/
CREATE TABLE [dbo].[TABGEN] (
	[TG_CODTAB] [numeric](3, 0) NOT NULL ,
	[TG_CODCON] [numeric](6, 0) NOT NULL ,
	[TG_DESCRI] [char] (40) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[TG_DATO01] [numeric](5, 0) NOT NULL ,
	[TG_DATO02] [char] (40) COLLATE Modern_Spanish_CI_AS NOT NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_abonados_claves]    fecha de la secuencia de comandos: 24/04/2007 11:23:57 a.m. ******/
CREATE TABLE [dbo].[dn_abonados_claves] (
	[abonado] [varchar] (7) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[clave] [varchar] (30) COLLATE Modern_Spanish_CI_AS NULL ,
	[razon_social] [varchar] (50) COLLATE Modern_Spanish_CI_AS NULL ,
	[estado] [char] (1) COLLATE Modern_Spanish_CI_AS NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_abonados_cuentas]    fecha de la secuencia de comandos: 24/04/2007 11:23:57 a.m. ******/
CREATE TABLE [dbo].[dn_abonados_cuentas] (
	[cuenta] [varchar] (17) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[tipo_cuenta] [smallint] NOT NULL ,
	[abonado] [varchar] (7) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[ente] [int] NULL ,
	[tipo_cuenta_dn] [smallint] NULL ,
	[nro_esquema] [smallint] NULL ,
	[det_cheques_acred] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[det_cheques_rech] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[principal] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[estado] [char] (1) COLLATE Modern_Spanish_CI_AS NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_bancos_claves]    fecha de la secuencia de comandos: 24/04/2007 11:23:57 a.m. ******/
CREATE TABLE [dbo].[dn_bancos_claves] (
	[banco] [char] (3) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[clave] [varchar] (30) COLLATE Modern_Spanish_CI_AS NULL ,
	[estado] [char] (1) COLLATE Modern_Spanish_CI_AS NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_comisiones]    fecha de la secuencia de comandos: 24/04/2007 11:23:58 a.m. ******/
CREATE TABLE [dbo].[dn_comisiones] (
	[OPERACION] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[COMISION] [money] NULL ,
	[FECHA_INICIO] [datetime] NULL ,
	[FECHA_FIN] [datetime] NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_parametros]    fecha de la secuencia de comandos: 24/04/2007 11:23:58 a.m. ******/
CREATE TABLE [dbo].[dn_parametros] (
	[dn_id] [int] NOT NULL ,
	[dn_fecha_proceso] [datetime] NULL ,
	[dn_nro_cuenta] [nvarchar] (50) COLLATE Modern_Spanish_CI_AS NULL ,
	[dn_tipo_cuenta] [nvarchar] (5) COLLATE Modern_Spanish_CI_AS NULL ,
	[dn_registro_desde] [int] NULL ,
	[dn_registro_hasta] [int] NULL ,
	[dn_fecha_desde] [datetime] NULL ,
	[dn_fecha_hasta] [datetime] NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_tef_enviado]    fecha de la secuencia de comandos: 24/04/2007 11:23:58 a.m. ******/
CREATE TABLE [dbo].[dn_tef_enviado] (
	[fec_solicitud] [char] (8) COLLATE Modern_Spanish_CI_AS NULL ,
	[nro_transferencia] [char] (7) COLLATE Modern_Spanish_CI_AS NULL ,
	[estado] [char] (2) COLLATE Modern_Spanish_CI_AS NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_tef_online]    fecha de la secuencia de comandos: 24/04/2007 11:23:58 a.m. ******/
CREATE TABLE [dbo].[dn_tef_online] (
	[COD_ORIGEN] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_DESTINO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_REGISTRO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[ACTIVIDAD] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[CONTADOR] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[RESERVADO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[BCO_DEBITO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_SOLICITUD] [char] (8) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_TRANSFERENCIA] [char] (7) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[COD_ABONADO] [char] (7) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_OPERACION] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[IMPORTE] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[SUC_DEBITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[NOM_SOLICITANTE] [char] (29) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_CTA_DEB_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_CTA_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[CTA_DEBITO] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ENVIO_DEBITO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_DEBITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_DB_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_DB_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[MOTIVO_RECHAZO_DB] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[BCO_CREDITO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[SUC_CREDITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[NOM_BENEFICIARIO] [char] (29) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_CTA_CRED_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[CTA_CREDITO] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ENVIO_CREDITO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_CREDITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_CR_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_CR_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[MOTIVO_RECHAZO_CR] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_INGRESO] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_3] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[FECHA_AUTORIZACION] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_AUTORIZACION] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADO] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ESTADO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[OBSERVACION_1] [char] (60) COLLATE Modern_Spanish_CI_AS NULL ,
	[OBSERVACION_2] [char] (100) COLLATE Modern_Spanish_CI_AS NULL ,
	[CLAVE_MAC] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_REFERENCIA] [char] (7) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_ENVIO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[DEB_CONSOLIDADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_TITULAR] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[PAGO_PREACORDADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[TRATA_CR_DIFERIDO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[RIESGO_ABONADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[RIESGO_BANCO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_LOTE] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADO_TRATAMIENTO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADOS_ANTERIORES] [char] (140) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_RESPUESTA] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[CANT_RECLAMOS] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_RECLAMO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_USUARIO_OFICIAL] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_OFICIAL_RESP] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_RESPUESTA] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[ERROR_DE_MAC] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[CLAVE_MENVIADA] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[PEND_ENVIO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[SECUENCIA_DEBCRE] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_ALTERNO_DEBCRE] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[SECUENCIA_REVERSA] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_ALTERNO_REVERSA] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[ACCION] [char] (1) COLLATE Modern_Spanish_CI_AS NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dtn].[dn_tef_online_historico]    fecha de la secuencia de comandos: 24/04/2007 11:23:59 a.m. ******/
CREATE TABLE [dtn].[dn_tef_online_historico] (
	[COD_ORIGEN] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_DESTINO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_REGISTRO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[ACTIVIDAD] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[CONTADOR] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[RESERVADO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[BCO_DEBITO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_SOLICITUD] [char] (8) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_TRANSFERENCIA] [char] (7) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[COD_ABONADO] [char] (7) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_OPERACION] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[IMPORTE] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[SUC_DEBITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[NOM_SOLICITANTE] [char] (29) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_CTA_DEB_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_CTA_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[CTA_DEBITO] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ENVIO_DEBITO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_DEBITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_DB_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_DB_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[MOTIVO_RECHAZO_DB] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[BCO_CREDITO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[SUC_CREDITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[NOM_BENEFICIARIO] [char] (29) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_CTA_CRED_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[CTA_CREDITO] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ENVIO_CREDITO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_CREDITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_CR_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_CR_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[MOTIVO_RECHAZO_CR] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_INGRESO] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_3] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[FECHA_AUTORIZACION] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_AUTORIZACION] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADO] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ESTADO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[OBSERVACION_1] [char] (60) COLLATE Modern_Spanish_CI_AS NULL ,
	[OBSERVACION_2] [char] (100) COLLATE Modern_Spanish_CI_AS NULL ,
	[CLAVE_MAC] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_REFERENCIA] [char] (7) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_ENVIO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[DEB_CONSOLIDADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_TITULAR] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[PAGO_PREACORDADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[TRATA_CR_DIFERIDO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[RIESGO_ABONADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[RIESGO_BANCO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_LOTE] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADO_TRATAMIENTO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADOS_ANTERIORES] [char] (140) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_RESPUESTA] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[CANT_RECLAMOS] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_RECLAMO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_USUARIO_OFICIAL] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_OFICIAL_RESP] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_RESPUESTA] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[ERROR_DE_MAC] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[CLAVE_MENVIADA] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[PEND_ENVIO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[SECUENCIA_DEBCRE] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_ALTERNO_DEBCRE] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[SECUENCIA_REVERSA] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_ALTERNO_REVERSA] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[ACCION] [char] (1) COLLATE Modern_Spanish_CI_AS NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dbo].[dn_tef_online_historico]    fecha de la secuencia de comandos: 24/04/2007 11:23:59 a.m. ******/
CREATE TABLE [dbo].[dn_tef_online_historico] (
	[COD_ORIGEN] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_DESTINO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_REGISTRO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[ACTIVIDAD] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[CONTADOR] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[RESERVADO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[BCO_DEBITO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_SOLICITUD] [char] (8) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_TRANSFERENCIA] [char] (7) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[COD_ABONADO] [char] (7) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_OPERACION] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[IMPORTE] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[SUC_DEBITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[NOM_SOLICITANTE] [char] (29) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_CTA_DEB_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_CTA_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[CTA_DEBITO] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ENVIO_DEBITO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_DEBITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_DB_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_DB_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[MOTIVO_RECHAZO_DB] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[BCO_CREDITO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[SUC_CREDITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[NOM_BENEFICIARIO] [char] (29) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_CTA_CRED_RED] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[CTA_CREDITO] [char] (17) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ENVIO_CREDITO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_CREDITO] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_CR_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_CR_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[MOTIVO_RECHAZO_CR] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[OPERADOR_INGRESO] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_1] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_2] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[AUTORIZANTE_3] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[FECHA_AUTORIZACION] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_AUTORIZACION] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADO] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[FEC_ESTADO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[OBSERVACION_1] [char] (60) COLLATE Modern_Spanish_CI_AS NULL ,
	[OBSERVACION_2] [char] (100) COLLATE Modern_Spanish_CI_AS NULL ,
	[CLAVE_MAC] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_REFERENCIA] [char] (7) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_ENVIO] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[DEB_CONSOLIDADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[TIPO_TITULAR] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[PAGO_PREACORDADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[TRATA_CR_DIFERIDO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[RIESGO_ABONADO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[RIESGO_BANCO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[NRO_LOTE] [char] (3) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADO_TRATAMIENTO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[ESTADOS_ANTERIORES] [char] (140) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_RESPUESTA] [char] (4) COLLATE Modern_Spanish_CI_AS NULL ,
	[CANT_RECLAMOS] [char] (2) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_ENVIO_RECLAMO] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_USUARIO_OFICIAL] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_OFICIAL_RESP] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[HORA_RESPUESTA] [char] (6) COLLATE Modern_Spanish_CI_AS NULL ,
	[ERROR_DE_MAC] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[CLAVE_MENVIADA] [char] (12) COLLATE Modern_Spanish_CI_AS NULL ,
	[PEND_ENVIO] [char] (1) COLLATE Modern_Spanish_CI_AS NULL ,
	[SECUENCIA_DEBCRE] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_ALTERNO_DEBCRE] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[SECUENCIA_REVERSA] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[COD_ALTERNO_REVERSA] [char] (10) COLLATE Modern_Spanish_CI_AS NULL ,
	[ACCION] [char] (1) COLLATE Modern_Spanish_CI_AS NULL 
) ON [PRIMARY]
GO

/****** Objeto: tabla [dtn].[extcta_20070416]    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE TABLE [dtn].[extcta_20070416] (
	[EX_CODEMP] [numeric](5, 0) NOT NULL ,
	[EX_SUCURS] [numeric](5, 0) NOT NULL ,
	[EX_CODSIS] [smallint] NOT NULL ,
	[EX_TIPCTA] [numeric](5, 0) NOT NULL ,
	[EX_MONEDA] [numeric](5, 0) NOT NULL ,
	[EX_CUENTA] [numeric](15, 0) NOT NULL ,
	[EX_ORDEN] [numeric](15, 0) NOT NULL ,
	[EX_NROLOG] [numeric](12, 0) NOT NULL ,
	[EX_LOGCTA] [numeric](12, 0) NOT NULL ,
	[EX_NROCLI] [numeric](15, 0) NOT NULL ,
	[EX_NROCOM] [numeric](10, 0) NOT NULL ,
	[EX_NROCHE] [numeric](10, 0) NOT NULL ,
	[EX_CODBAN] [numeric](5, 0) NOT NULL ,
	[EX_CONCEP] [char] (40) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECOPE] [datetime] NOT NULL ,
	[EX_FECIMP] [datetime] NOT NULL ,
	[EX_CODTRA] [numeric](5, 0) NOT NULL ,
	[EX_TIPOPE] [smallint] NOT NULL ,
	[EX_IMPORT] [numeric](15, 2) NOT NULL ,
	[EX_CODCAM] [numeric](5, 0) NOT NULL ,
	[EX_PLACON] [smallint] NOT NULL ,
	[EX_FECCON] [datetime] NOT NULL ,
	[EX_HORCON] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_SUCORI] [numeric](5, 0) NOT NULL ,
	[EX_TIPMOV] [char] (2) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROTER] [numeric](5, 0) NOT NULL ,
	[EX_CODOPE] [numeric](5, 0) NOT NULL ,
	[EX_DATEOR] [datetime] NOT NULL ,
	[EX_TIMEOR] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_NROCAJ] [numeric](5, 0) NOT NULL ,
	[EX_CODCAJ] [numeric](5, 0) NOT NULL ,
	[EX_DATECA] [datetime] NOT NULL ,
	[EX_TIMECA] [char] (6) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_CODERR] [smallint] NOT NULL ,
	[EX_CODLIQ] [char] (1) COLLATE Modern_Spanish_CI_AS NOT NULL ,
	[EX_FECCHE] [datetime] NOT NULL ,
	[EX_ASIIMP] [numeric](5, 0) NOT NULL ,
	[EX_SALACT] [numeric](15, 2) NOT NULL ,
	[EX_ESTADO] [smallint] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[ACUERD] WITH NOCHECK ADD 
	CONSTRAINT [PK_ACUERD] PRIMARY KEY  CLUSTERED 
	(
		[AC_TIPCTA],
		[AC_MONEDA],
		[AC_CUENTA],
		[AC_ORDEN],
		[AC_NROCLI],
		[AC_FECALT],
		[AC_FECVTO]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[CLIENT] WITH NOCHECK ADD 
	CONSTRAINT [PK_CLIENT] PRIMARY KEY  CLUSTERED 
	(
		[CL_NROCLI]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[FERIAD] WITH NOCHECK ADD 
	CONSTRAINT [PK_FERIAD] PRIMARY KEY  CLUSTERED 
	(
		[FE_SUCURS],
		[FE_FECPRO]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[MAECUE] WITH NOCHECK ADD 
	CONSTRAINT [PK_MAECUE] PRIMARY KEY  CLUSTERED 
	(
		[MC_TIPCTA],
		[MC_CUENTA],
		[MC_ORDEN]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[NOVEDA] WITH NOCHECK ADD 
	CONSTRAINT [PK_NOVEDA] PRIMARY KEY  CLUSTERED 
	(
		[NV_TIPCTA],
		[NV_CUENTA],
		[NV_ORDEN]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[TABGEN] WITH NOCHECK ADD 
	CONSTRAINT [PK_TABGEN] PRIMARY KEY  CLUSTERED 
	(
		[TG_CODTAB],
		[TG_CODCON]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[dn_abonados_claves] WITH NOCHECK ADD 
	CONSTRAINT [PK_dn_abonados_claves] PRIMARY KEY  CLUSTERED 
	(
		[abonado]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[dn_abonados_cuentas] WITH NOCHECK ADD 
	CONSTRAINT [PK_dn_abonados_cuentas] PRIMARY KEY  CLUSTERED 
	(
		[cuenta],
		[tipo_cuenta],
		[abonado]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[dn_bancos_claves] WITH NOCHECK ADD 
	CONSTRAINT [PK_dn_bancos_claves] PRIMARY KEY  CLUSTERED 
	(
		[banco]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[dn_tef_online] WITH NOCHECK ADD 
	CONSTRAINT [PK_NRO_TRANSFERENCIA] PRIMARY KEY  CLUSTERED 
	(
		[NRO_TRANSFERENCIA]
	) WITH  FILLFACTOR = 90  ON [PRIMARY] 
GO

 CREATE  INDEX [IX_ACUERDO] ON [dbo].[ACUERD]([AC_TIPCTA], [AC_CUENTA], [AC_ORDEN], [AC_FECALT], [AC_FECVTO], [AC_ESTADO]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [IX_FECHA_VTO] ON [dbo].[ACUERD]([AC_FECVTO]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [IX_EXTCTA_CUENTA] ON [dbo].[EXTCTA]([EX_TIPCTA], [EX_CUENTA], [EX_ORDEN]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [ix_dateor] ON [dbo].[EXTCTA]([EX_DATEOR]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [ix_fecope] ON [dbo].[EXTCTA]([EX_FECOPE]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [ix_nrolog] ON [dbo].[EXTCTA]([EX_NROLOG]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [ix_feccon] ON [dbo].[EXTCTA]([EX_FECCON]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [IX_EXTCTA_COMPLETO] ON [dbo].[EXTCTA]([EX_TIPCTA], [EX_CUENTA], [EX_ORDEN], [EX_FECOPE], [EX_DATEOR], [EX_FECCON], [EX_ESTADO]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  UNIQUE  INDEX [ix_PK_NROLOG_FECCON_CONCEP] ON [dbo].[EXTCTA]([EX_NROLOG], [EX_FECCON], [EX_IMPORT], [EX_TIPOPE], [EX_CONCEP]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [IX_ESTADO_MAESTRO] ON [dbo].[MAECUE]([MC_ESTADO]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [IX_dn_abonados_cuentas] ON [dbo].[dn_abonados_cuentas]([abonado]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [IX_dn_abonados_cuentas_ente] ON [dbo].[dn_abonados_cuentas]([ente]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  UNIQUE  INDEX [pk_dn_comisiones] ON [dbo].[dn_comisiones]([OPERACION], [FECHA_INICIO], [FECHA_FIN]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  INDEX [IX_ESTADO] ON [dbo].[dn_tef_online]([ESTADO]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

 CREATE  UNIQUE  INDEX [IX_FECHA_TRANSFERENCIA] ON [dbo].[dn_tef_online]([FEC_SOLICITUD], [NRO_TRANSFERENCIA]) WITH  FILLFACTOR = 90 ON [PRIMARY]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto: vista dbo.VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE VIEW dbo.VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS
AS
SELECT                CONVERT(varchar(12), MAECUE.MC_CUENTA) AS CUENTA, 
	              EXTCTA.EX_FECCON AS fecha, 
                      EXTCTA.EX_NROLOG AS SECUENCIAL, MAECUE.MC_SALACT AS SALDO, 
                      EXTCTA.EX_IMPORT * EXTCTA.EX_TIPOPE AS IMPORTE, EXTCTA.EX_TIPOPE AS DB_CR, 
                      EXTCTA.EX_FECOPE AS FECHA_PROCESO, EXTCTA.EX_SUCORI AS SUCURSAL, 
                      EXTCTA.EX_NROCHE AS DEPOSITO, EXTCTA.EX_CONCEP AS DESCRIPCION, 
                      EXTCTA.EX_TIPMOV AS OPERACION, 
		      isnull( SUM(ACUERD.AC_MONACU),0) AS ACUERDO , 
                      CLIENT.CL_NOMCLI AS NOMBRE, EXTCTA.EX_DATEOR AS FECHA_VALOR, 
                      CONVERT(VARCHAR, MAECUE.MC_FECALT, 112) AS FECAPE,
  		      MAECUE.MC_TIPCTA AS TIPCTA
FROM         MAECUE INNER JOIN
                      NOVEDA ON NOVEDA.NV_TIPCTA = MAECUE.MC_TIPCTA AND 
                      NOVEDA.NV_CUENTA = MAECUE.MC_CUENTA AND 
                      NOVEDA.NV_ORDEN = MAECUE.MC_ORDEN AND NOVEDA.NV_FECBAJ = '1900-01-01' INNER JOIN
                      dbo.dn_parametros ON dbo.dn_parametros.dn_id = 1 LEFT OUTER JOIN
                      ACUERD ON ACUERD.AC_TIPCTA = MAECUE.MC_TIPCTA AND 
                      ACUERD.AC_CUENTA = MAECUE.MC_CUENTA AND 
                      ACUERD.AC_ORDEN = MAECUE.MC_ORDEN AND ACUERD.AC_ESTADO = 0 AND 
                      ACUERD.AC_FECVTO > dbo.dn_parametros.dn_fecha_proceso LEFT OUTER JOIN
                      CLIENT ON CLIENT.CL_NROCLI = MAECUE.MC_NROCLI LEFT OUTER JOIN
                      EXTCTA ON MAECUE.MC_TIPCTA = EXTCTA.EX_TIPCTA AND 
                      MAECUE.MC_CUENTA = EXTCTA.EX_CUENTA AND 
                      MAECUE.MC_ORDEN = EXTCTA.EX_ORDEN AND MAECUE.MC_ESTADO = 0 AND 
                      (EXTCTA.EX_ESTADO = 1 AND EXTCTA.EX_FECCON >= DATEADD(DAY, - dbo.dn_parametros.dn_registro_hasta, 
                      dbo.dn_parametros.dn_fecha_proceso) AND EXTCTA.EX_FECCON <= DATEADD(DAY, - dbo.dn_parametros.dn_registro_desde, 
                      dbo.dn_parametros.dn_fecha_proceso) AND dbo.dn_parametros.dn_id = 1 OR
                      EXTCTA.EX_ESTADO = 0 AND EXTCTA.EX_FECOPE <> EXTCTA.EX_FECCON AND 
                      EXTCTA.EX_FECCON >= DATEADD(DAY, - dbo.dn_parametros.dn_registro_hasta, dbo.dn_parametros.dn_fecha_proceso) AND 
                      dbo.dn_parametros.dn_id = 1 OR
                      EXTCTA.EX_ESTADO = 1 AND EXTCTA.EX_DATEOR <> EXTCTA.EX_FECCON AND 
                      EXTCTA.EX_DATEOR >= DATEADD(DAY, - dbo.dn_parametros.dn_registro_hasta, dbo.dn_parametros.dn_fecha_proceso) AND 
                      EXTCTA.EX_DATEOR <= DATEADD(DAY, - dbo.dn_parametros.dn_registro_desde, dbo.dn_parametros.dn_fecha_proceso) AND 
                      dbo.dn_parametros.dn_id = 1)
GROUP BY MAECUE.MC_TIPCTA, MAECUE.MC_CUENTA, MAECUE.MC_ORDEN, 
                      EXTCTA.EX_FECCON, EXTCTA.EX_NROLOG, MAECUE.MC_SALACT, 
                      EXTCTA.EX_IMPORT, EXTCTA.EX_TIPOPE, EXTCTA.EX_FECOPE, 
                      EXTCTA.EX_SUCORI, EXTCTA.EX_NROCHE, EXTCTA.EX_CONCEP, 
                      EXTCTA.EX_TIPMOV, CLIENT.CL_NOMCLI, EXTCTA.EX_DATEOR, 
                      MAECUE.MC_FECALT






GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  SELECT  ON [dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA
 @I_FECHA    AS  VARCHAR(20) = NULL,
 @I_CUENTA   AS  VARCHAR(20) = NULL,
 @I_DBCR     AS  INT         = NULL,
 @O_SALDOANT AS MONEY        = NULL OUTPUT ,
 @O_SUMADBCR AS MONEY        = NULL OUTPUT 
AS
DECLARE @V_FECHA AS DATETIME
IF @I_FECHA IS NULL
  SELECT @V_FECHA = CONVERT(DATETIME, GETDATE(), 111)
ELSE
  SELECT @V_FECHA = CONVERT(DATETIME, @I_FECHA, 111)
IF @I_CUENTA IS NULL
BEGIN
	SELECT CUENTA, 
	       isnull(SALDO - SUM(IMPORTE),0) AS SALANT, 
	       isnull(SUM(IMPORTE),0) AS SUMAIMP, 
	       SALDO
	FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS 
	WHERE FECHA >= @V_FECHA 
	GROUP BY CUENTA, SALDO
END
ELSE
BEGIN
	IF @I_DBCR IS NULL
	BEGIN
		SELECT CUENTA, 
		       SALDO - SUM(IMPORTE) AS SALANT, 
		       SUM(IMPORTE) AS SUMAIMP, 
		       SALDO
		FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS 
		WHERE FECHA >= @V_FECHA AND CUENTA = @I_CUENTA
		GROUP BY CUENTA, SALDO
	END
	ELSE IF @I_DBCR = 0
	BEGIN
		SELECT @O_SALDOANT = SALDO - SUM(IMPORTE), 
		       @O_SUMADBCR = SUM(IMPORTE)
		FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS 
		WHERE FECHA >= @V_FECHA AND CUENTA = @I_CUENTA
		GROUP BY CUENTA, SALDO
	END
	ELSE 
	BEGIN
		SELECT @O_SALDOANT = SALDO - SUM(IMPORTE) , 
		       @O_SUMADBCR = SUM(IMPORTE)                                
		FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS 
		WHERE FECHA = @V_FECHA AND CUENTA = @I_CUENTA AND DB_CR = @I_DBCR
		GROUP BY CUENTA, SALDO
	END
END


GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_VERIFICA_FERIADO    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROC SP_VERIFICA_FERIADO
@fecha   as datetime
AS
declare @feriado as datetime
select @feriado = fe_fecpro from feriad
where fe_fecpro = @fecha and fe_sucurs = 0
if  @@error <> 0 or @@rowcount = 0
	return 0 /* NO HAY ERROR, NO ES FERIADO */
else
	return 100 /* ERROR - EFECTIVAMENTE ES FERIADO */

GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_VERIFICA_FERIADO]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_FECHAS_DE_FERIADO    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROCEDURE SP_FECHAS_DE_FERIADO 
AS 
  SELECT FE_FECPRO FROM DBO.FERIAD


GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_FECHAS_DE_FERIADO]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main
 @I_FECHA  AS  VARCHAR(20)       ,
 @I_CUENTA AS  VARCHAR(20) = NULL,
 @I_DBCR   AS  INT         = NULL
AS
DECLARE @I_SALDODBCR AS MONEY,
	@I_SALDOANT AS MONEY,
        @I_SALDONUE AS MONEY,
        @I_SUMADBCR AS MONEY,
        @I_SUMADB   AS MONEY,
	@I_SUMACR   AS MONEY,
        @W_RETVAL   AS INT
/* FIX WEEKEND */
DECLARE @WK_FECHA     AS DATETIME, 
	@WK_SALDOANT  AS MONEY,
	@WK_SALDODBCR AS MONEY,
	@WK_SUMADBCR  AS MONEY,
        @WK_SUMADB    AS MONEY,
	@WK_SUMACR    AS MONEY
/* FIX WEEKEND */
  SELECT @I_SALDODBCR = 0, @I_SALDOANT = 0, @I_SUMADBCR = 0, @I_SUMADB = 0, @I_SUMACR = 0
  /* SALDO A FECHA */
  EXEC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA @I_FECHA, @I_CUENTA, 0,
	@I_SALDODBCR OUTPUT, @I_SUMADBCR OUTPUT
  /* END OF FILE => SUM(CR)SUM(DB) */
  IF (0 = @@ROWCOUNT )
  BEGIN
	RETURN 100 /* END OF DATA */
  END
  /* DEBITOS DE FECHA */
  EXEC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA @I_FECHA, @I_CUENTA, -1, 
	@I_SALDOANT OUTPUT, @I_SUMADB OUTPUT
  /* CREDITOS DE FECHA */
  EXEC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA @I_FECHA, @I_CUENTA, 1 ,
	@I_SALDOANT OUTPUT, @I_SUMACR OUTPUT
  /****************************************************************************/
  /* VERIFICAR SI ES LUNES y EL DIA 1 DEL MES CAYO FIN DE SEMANA              */
  /****************************************************************************/
  /* PRIMERO QUE NADA, INDEPENDIZAR EL DIA DE SEMANA DE LA CONF.REGIONAL (USA)*/
  /****************************************************************************/
  SET DATEFIRST 7 -- (predeterminado, Inglés EE.UU.) Domingo 
  SELECT @WK_FECHA = CONVERT( DATETIME, @I_FECHA, 110)
  IF (  DATEPART ( weekday , @WK_FECHA ) = 2   -- HOY ES LUNES (WEEKDAY=2), PERO...
	AND
	DATEPART ( day , @WK_FECHA )     <= 3  -- ES DIA 3 (DAY=3), ENTONCES BUSCAR SABADO
	AND
	DATEPART ( day , @WK_FECHA )     >= 2  -- ES DIA 2 (DAY=2), ENTONCES BUSCAR DOMINGO
     )
  BEGIN
  	  /* VARIABLES A CERO */
	  SELECT @WK_SUMADB = 0, @WK_SUMACR = 0, @WK_SALDOANT = 0, @WK_SALDODBCR = 0, @WK_SALDODBCR = 0
	  /* POSIONARSE EN NUEVO DIA si es fecha 3 o 2... restando 1 o 2 dias */
	  IF( DATEPART ( day , @WK_FECHA ) = 3 )
	    SELECT @WK_FECHA = DATEDIFF( day, 2, @WK_FECHA)
	  ELSE
	    SELECT @WK_FECHA = DATEDIFF( day, 1, @WK_FECHA)
	  /* SALDO A FECHA de fin de semana*/
	  EXEC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA @WK_FECHA, @I_CUENTA, 0,
		@WK_SALDODBCR OUTPUT, @WK_SUMADBCR OUTPUT
	  /* DEBITOS DE FECHA SABADO/DOMINGO */
	  EXEC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA @WK_FECHA, @I_CUENTA, -1, 
		@WK_SALDOANT OUTPUT, @WK_SUMADB OUTPUT
	  /* CREDITOS DE FECHA SABADO/DOMINGO */
	  EXEC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA @WK_FECHA, @I_CUENTA, 1, 
		@WK_SALDOANT OUTPUT, @WK_SUMACR OUTPUT
	  /* SUMA DE DEBITOS de fin de semana */
	  SELECT @I_SUMADB = @I_SUMADB + @WK_SUMADB
	  /* SUMA DE CREDITOS de fin de semana */
	  SELECT @I_SUMACR = @I_SUMACR + @WK_SUMACR
	  /* NUEVO SALDO ANTERIOR */
	  SELECT @I_SALDODBCR = @WK_SALDODBCR
  END
  /****************************************************************************/
  /* SALDO NUEVO */
  SELECT @I_SALDONUE = @I_SALDODBCR + @I_SUMACR + @I_SUMADB 
  /* RETORNAR */
  SELECT @I_SALDODBCR AS SALDOANT, @I_SUMADB AS TOTDEB, @I_SUMACR AS TOTCRE, @I_SALDONUE AS SALDO




GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dtn.SP_TEF_PASE_A_HISTORICO    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/
setuser N'dtn'
GO

CREATE PROC SP_TEF_PASE_A_HISTORICO
AS

insert into dn_tef_online_historico
select * from dn_tef_online
where nro_transferencia not in (select nro_transferencia from dn_tef_online_historico )

GO

setuser
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_UPDATE_DN_PARAMETROS    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/
CREATE PROCEDURE SP_UPDATE_DN_PARAMETROS 
  @FECHA_PROCESO AS VARCHAR(20) = NULL ,
  @FECHA_DESDE   AS VARCHAR(20) = NULL ,
  @FECHA_HASTA   AS VARCHAR(20) = NULL ,
  @REG_DESDE     AS INT = NULL ,
  @REG_HASTA     AS INT = NULL ,
  @NRO_CUENTA    AS VARCHAR(50) = NULL 
AS 

  IF (( SELECT COUNT(*) FROM DN_PARAMETROS ) = 0 )
  BEGIN
	INSERT INTO DN_PARAMETROS 
 	VALUES ( 1,  CONVERT(DATETIME, @FECHA_PROCESO, 111),'0','0',0,0,0,0)
  END

  IF @FECHA_PROCESO IS NOT NULL
	UPDATE DN_PARAMETROS 
	SET DN_FECHA_PROCESO = CONVERT(DATETIME, @FECHA_PROCESO, 111)
	WHERE DN_ID=1
  IF @FECHA_DESDE IS NOT NULL
	UPDATE DN_PARAMETROS 
	SET DN_FECHA_DESDE = CONVERT(DATETIME, @FECHA_DESDE, 111)
	WHERE DN_ID=1
  IF @FECHA_HASTA IS NOT NULL
	UPDATE DN_PARAMETROS 
	SET DN_FECHA_HASTA = CONVERT(DATETIME, @FECHA_HASTA, 111)
	WHERE DN_ID=1
  IF @REG_DESDE IS NOT NULL
	UPDATE DN_PARAMETROS 
	SET DN_REGISTRO_DESDE = @REG_DESDE
	WHERE DN_ID=1
  IF @REG_HASTA IS NOT NULL
	UPDATE DN_PARAMETROS 
	SET DN_REGISTRO_HASTA = @REG_HASTA
	WHERE DN_ID=1
  IF @REG_DESDE IS NOT NULL
	UPDATE DN_PARAMETROS 
	SET DN_REGISTRO_DESDE = @REG_DESDE
	WHERE DN_ID=1
  IF @REG_HASTA   IS NULL     AND @REG_DESDE   IS NULL  
     AND
     @FECHA_DESDE IS NOT NULL AND @FECHA_HASTA IS NOT NULL AND @FECHA_PROCESO IS NOT NULL
  BEGIN
     UPDATE DN_PARAMETROS
     SET DN_REGISTRO_DESDE = DATEDIFF(DAY,CONVERT(DATETIME, @FECHA_HASTA  , 111),
					  CONVERT(DATETIME, @FECHA_PROCESO, 111)),
         DN_REGISTRO_HASTA = DATEDIFF(DAY,CONVERT(DATETIME, @FECHA_DESDE  , 111),
                                          CONVERT(DATETIME, @FECHA_HASTA  , 111))
	WHERE DN_ID=1
  END
  IF @NRO_CUENTA IS NOT NULL
	UPDATE DN_PARAMETROS 
	SET DN_NRO_CUENTA = @NRO_CUENTA
	WHERE DN_ID=1


GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_UPDATE_DN_PARAMETROS]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_MAESTRO_DE_CUENTAS    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/
CREATE PROC SP_VIEW_MAESTRO_DE_CUENTAS
@i_tipcta1	as NUMERIC(3) = NULL,
@i_tipcta2	as NUMERIC(3) = NULL,
@i_tipcta3      as NUMERIC(3) = NULL,
@i_tipcta4      as NUMERIC(3) = NULL
AS
	IF( @i_tipcta1 IS NULL )
	BEGIN
		select @i_tipcta1 = 1  -- CTA.CTE.$
		select @i_tipcta2 = 11 -- CAJ.AHO.$
		select @i_tipcta3 = 7  -- CTA.CTE.US$
		select @i_tipcta4 = 15 -- CAJ.AHO.US$
	END
	/* ELEGIR POR TIPO DE CUENTA */
	SELECT convert(varchar(12),mc_cuenta) AS CUENTA,
	       mc_salact as SALDO 
	FROM MAECUE INNER JOIN NOVEDA
        ON   MC_TIPCTA = NV_TIPCTA
        AND  MC_CUENTA = NV_CUENTA
        AND  MC_ORDEN =  NV_ORDEN
        AND  NV_FECBAJ = '1900-01-01'        
	AND  MC_TIPCTA IN (@i_tipcta1,@i_tipcta2,@i_tipcta3,@i_tipcta4)
	ORDER BY mc_tipcta,mc_cuenta,mc_orden




GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_VIEW_MAESTRO_DE_CUENTAS]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/
CREATE PROC SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS
@i_tipcta1	as NUMERIC(3) = NULL,
@i_tipcta2	as NUMERIC(3) = NULL,
@i_tipcta3      as NUMERIC(3) = NULL,
@i_tipcta4      as NUMERIC(3) = NULL
AS
/*
declare @lastweekday as int ,
        @lastworkday as datetime
select @lastworkday = dateadd( day, - dn_registro_desde, dn_fecha_proceso ) 
from dn_parametros where dn_id=1
select @lastweekday = datepart( weekday, @lastworkday ) 
if( 7 = @lastweekday ) 
begin
	set @lastweekday = 2
	update dn_parametros 
	set dn_registro_desde = dn_registro_desde + @lastweekday 
end
else 
begin
if( 6 = @lastweekday ) 
	set @lastweekday = 1
	update dn_parametros 
	set dn_registro_desde = dn_registro_desde + @lastweekday 
end
*/
SELECT * FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS
WHERE FECHA IS NOT NULL 
	AND 
      TIPCTA IN (@i_tipcta1,@i_tipcta2,@i_tipcta3,@i_tipcta4)
ORDER BY FECHA, CUENTA, SECUENCIAL



GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]  TO [public]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/


CREATE PROC SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS
@i_tipcta1	as NUMERIC(3) = NULL,
@i_tipcta2	as NUMERIC(3) = NULL,
@i_tipcta3      as NUMERIC(3) = NULL,
@i_tipcta4      as NUMERIC(3) = NULL
AS
	IF( @i_tipcta1 IS NULL )
	BEGIN
		select @i_tipcta1 = 1  -- CTA.CTE.$
		select @i_tipcta2 = 11 -- CAJ.AHO.$
		select @i_tipcta3 = 7  -- CTA.CTE.US$
		select @i_tipcta4 = 15 -- CAJ.AHO.US$
	END
	/* ELEGIR POR TIPO DE CUENTA */
	SELECT CUENTA, SALDO, ACUERDO, NOMBRE, FECAPE 
	FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS
	WHERE ACUERDO IS NOT NULL 
		AND 
	      TIPCTA IN (@i_tipcta1,@i_tipcta2,@i_tipcta3,@i_tipcta4)
	UNION
	SELECT CUENTA, SALDO, 0 AS ACUERDO, NOMBRE, FECAPE FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS
	WHERE ACUERDO IS NULL 
		AND 
              TIPCTA IN (@i_tipcta1,@i_tipcta2,@i_tipcta3,@i_tipcta4)
	GROUP BY CUENTA, SALDO, ACUERDO, NOMBRE, FECAPE
        ORDER BY CUENTA, SALDO


GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  EXECUTE  ON [dbo].[SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS]  TO [public]
GO


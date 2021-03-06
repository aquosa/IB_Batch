USE [FUEGODNET]
GO
/****** Objeto:  Table [dbo].[FERIAD]    Fecha de la secuencia de comandos: 02/04/2013 14:15:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[FERIAD](
	[FE_SUCURS] [numeric](5, 0) NOT NULL,
	[FE_FECPRO] [datetime] NOT NULL,
 CONSTRAINT [PK_FERIAD] PRIMARY KEY CLUSTERED 
(
	[FE_SUCURS] ASC,
	[FE_FECPRO] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Objeto:  Table [dbo].[MAECUE]    Fecha de la secuencia de comandos: 02/04/2013 14:15:31 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[MAECUE](
	[MC_CODEMP] [numeric](5, 0) NOT NULL,
	[MC_SUCURS] [numeric](5, 0) NOT NULL,
	[MC_CODSIS] [smallint] NOT NULL,
	[MC_TIPCTA] [numeric](5, 0) NOT NULL,
	[MC_MONEDA] [numeric](5, 0) NOT NULL,
	[MC_CUENTA] [numeric](15, 0) NOT NULL,
	[MC_ORDEN] [numeric](15, 0) NOT NULL,
	[MC_NROCLI] [numeric](15, 0) NOT NULL,
	[MC_TIPORD] [smallint] NOT NULL,
	[MC_CODTAS] [char](1) NOT NULL,
	[MC_TASNOM] [numeric](12, 6) NOT NULL,
	[MC_CODCAP] [smallint] NOT NULL,
	[MC_COMMAN] [smallint] NOT NULL,
	[MC_COMEXT] [smallint] NOT NULL,
	[MC_FECALT] [datetime] NOT NULL,
	[MC_FECMOV] [datetime] NOT NULL,
	[MC_FECACT] [datetime] NOT NULL,
	[MC_FECPRO] [datetime] NOT NULL,
	[MC_SALACT] [numeric](15, 2) NOT NULL,
	[MC_INTPAS] [numeric](12, 5) NOT NULL,
	[MC_INTACT] [numeric](12, 5) NOT NULL,
	[MC_FECCAP] [datetime] NOT NULL,
	[MC_INTCAP] [numeric](12, 5) NOT NULL,
	[MC_SALCAP] [numeric](15, 2) NOT NULL,
	[MC_FECDEV] [datetime] NOT NULL,
	[MC_DEVMES] [numeric](12, 5) NOT NULL,
	[MC_CODBLO] [numeric](5, 0) NOT NULL,
	[MC_IMPBLO] [numeric](15, 2) NOT NULL,
	[MC_FECBLO] [datetime] NOT NULL,
	[MC_CANEXT] [numeric](5, 0) NOT NULL,
	[MC_CANCHE] [numeric](5, 0) NOT NULL,
	[MC_CODEXT] [char](1) NOT NULL,
	[MC_ENVEXT] [smallint] NOT NULL,
	[MC_VAL000] [numeric](15, 2) NULL,
	[MC_VAL024] [numeric](15, 2) NULL,
	[MC_VAL048] [numeric](15, 2) NULL,
	[MC_VAL072] [numeric](15, 2) NULL,
	[MC_VAL096] [numeric](15, 2) NULL,
	[MC_VAL120] [numeric](15, 2) NULL,
	[MC_VAL144] [numeric](15, 2) NULL,
	[MC_VAL999] [numeric](15, 2) NULL,
	[MC_NUMERA] [numeric](12, 0) NOT NULL,
	[MC_CODBAJ] [smallint] NOT NULL,
	[MC_FECBAJ] [datetime] NOT NULL,
	[MC_ESTADO] [smallint] NOT NULL,
 CONSTRAINT [PK_MAECUE] PRIMARY KEY CLUSTERED 
(
	[MC_TIPCTA] ASC,
	[MC_CUENTA] ASC,
	[MC_ORDEN] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
CREATE NONCLUSTERED INDEX [IX_ESTADO_MAESTRO] ON [dbo].[MAECUE] 
(
	[MC_ESTADO] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
/****** Objeto:  Table [dbo].[NOVEDA]    Fecha de la secuencia de comandos: 02/04/2013 14:15:33 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[NOVEDA](
	[NV_TIPNOV] [numeric](6, 0) NOT NULL,
	[NV_TIPCTA] [numeric](5, 0) NOT NULL,
	[NV_CUENTA] [numeric](15, 0) NOT NULL,
	[NV_ORDEN] [numeric](15, 0) NOT NULL,
	[NV_FECALT] [datetime] NOT NULL,
	[NV_FECBAJ] [datetime] NOT NULL,
	[NV_FECPRO] [datetime] NOT NULL,
	[NV_USERID] [char](8) NOT NULL,
	[NV_USERID_BAJ] [char](8) NOT NULL,
 CONSTRAINT [PK_NOVEDA] PRIMARY KEY CLUSTERED 
(
	[NV_TIPCTA] ASC,
	[NV_CUENTA] ASC,
	[NV_ORDEN] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Objeto:  Table [dbo].[TABGEN]    Fecha de la secuencia de comandos: 02/04/2013 14:15:35 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[TABGEN](
	[TG_CODTAB] [numeric](3, 0) NOT NULL,
	[TG_CODCON] [numeric](6, 0) NOT NULL,
	[TG_DESCRI] [char](40) NOT NULL,
	[TG_DATO01] [numeric](5, 0) NOT NULL,
	[TG_DATO02] [char](40) NOT NULL,
 CONSTRAINT [PK_TABGEN] PRIMARY KEY CLUSTERED 
(
	[TG_CODTAB] ASC,
	[TG_CODCON] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Objeto:  Table [dbo].[dn_abonados_claves]    Fecha de la secuencia de comandos: 02/04/2013 14:14:30 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[dn_abonados_claves](
	[abonado] [varchar](7) NOT NULL,
	[clave] [varchar](30) NULL,
	[razon_social] [varchar](50) NULL,
	[estado] [char](1) NULL,
 CONSTRAINT [PK_dn_abonados_claves] PRIMARY KEY CLUSTERED 
(
	[abonado] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Objeto:  Table [dbo].[dn_abonados_cuentas]    Fecha de la secuencia de comandos: 02/04/2013 14:14:33 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[dn_abonados_cuentas](
	[cuenta] [varchar](17) NOT NULL,
	[tipo_cuenta] [smallint] NOT NULL,
	[abonado] [varchar](7) NOT NULL,
	[ente] [int] NULL,
	[tipo_cuenta_dn] [smallint] NULL,
	[nro_esquema] [smallint] NULL,
	[det_cheques_acred] [char](1) NULL,
	[det_cheques_rech] [char](1) NULL,
	[principal] [char](1) NULL,
	[estado] [char](1) NULL,
 CONSTRAINT [PK_dn_abonados_cuentas] PRIMARY KEY CLUSTERED 
(
	[cuenta] ASC,
	[tipo_cuenta] ASC,
	[abonado] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
CREATE NONCLUSTERED INDEX [IX_dn_abonados_cuentas] ON [dbo].[dn_abonados_cuentas] 
(
	[abonado] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE NONCLUSTERED INDEX [IX_dn_abonados_cuentas_ente] ON [dbo].[dn_abonados_cuentas] 
(
	[ente] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
/****** Objeto:  Table [dbo].[dn_bancos_claves]    Fecha de la secuencia de comandos: 02/04/2013 14:14:34 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[dn_bancos_claves](
	[banco] [char](3) NOT NULL,
	[clave] [varchar](30) NULL,
	[estado] [char](1) NULL,
 CONSTRAINT [PK_dn_bancos_claves] PRIMARY KEY CLUSTERED 
(
	[banco] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Objeto:  Table [dbo].[dn_comisiones]    Fecha de la secuencia de comandos: 02/04/2013 14:14:35 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[dn_comisiones](
	[OPERACION] [char](2) NULL,
	[COMISION] [money] NULL,
	[FECHA_INICIO] [datetime] NULL,
	[FECHA_FIN] [datetime] NULL
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
CREATE UNIQUE NONCLUSTERED INDEX [pk_dn_comisiones] ON [dbo].[dn_comisiones] 
(
	[OPERACION] ASC,
	[FECHA_INICIO] ASC,
	[FECHA_FIN] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
/****** Objeto:  Table [dbo].[dn_parametros]    Fecha de la secuencia de comandos: 02/04/2013 14:14:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[dn_parametros](
	[dn_id] [int] NOT NULL,
	[dn_fecha_proceso] [datetime] NULL,
	[dn_nro_cuenta] [nvarchar](50) NULL,
	[dn_tipo_cuenta] [nvarchar](5) NULL,
	[dn_registro_desde] [int] NULL,
	[dn_registro_hasta] [int] NULL,
	[dn_fecha_desde] [datetime] NULL,
	[dn_fecha_hasta] [datetime] NULL
) ON [PRIMARY]
GO
/****** Objeto:  Table [dbo].[dn_tef_enviado]    Fecha de la secuencia de comandos: 02/04/2013 14:14:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[dn_tef_enviado](
	[fec_solicitud] [char](8) NULL,
	[nro_transferencia] [char](7) NULL,
	[estado] [char](2) NULL
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Objeto:  Table [dbo].[dn_tef_online]    Fecha de la secuencia de comandos: 02/04/2013 14:14:53 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[dn_tef_online](
	[COD_ORIGEN] [char](4) NULL,
	[COD_DESTINO] [char](4) NULL,
	[TIPO_REGISTRO] [char](3) NULL,
	[ACTIVIDAD] [char](1) NULL,
	[CONTADOR] [char](2) NULL,
	[RESERVADO] [char](6) NULL,
	[BCO_DEBITO] [char](3) NULL,
	[FEC_SOLICITUD] [char](8) NULL,
	[NRO_TRANSFERENCIA] [char](7) NOT NULL,
	[COD_ABONADO] [char](7) NULL,
	[TIPO_OPERACION] [char](2) NULL,
	[IMPORTE] [char](17) NULL,
	[SUC_DEBITO] [char](4) NULL,
	[NOM_SOLICITANTE] [char](29) NULL,
	[TIPO_CTA_DEB_RED] [char](2) NULL,
	[NRO_CTA_RED] [char](2) NULL,
	[CTA_DEBITO] [char](17) NULL,
	[FEC_ENVIO_DEBITO] [char](6) NULL,
	[HORA_ENVIO_DEBITO] [char](4) NULL,
	[OPERADOR_DB_1] [char](2) NULL,
	[OPERADOR_DB_2] [char](2) NULL,
	[MOTIVO_RECHAZO_DB] [char](4) NULL,
	[BCO_CREDITO] [char](3) NULL,
	[SUC_CREDITO] [char](4) NULL,
	[NOM_BENEFICIARIO] [char](29) NULL,
	[TIPO_CTA_CRED_RED] [char](2) NULL,
	[CTA_CREDITO] [char](17) NULL,
	[FEC_ENVIO_CREDITO] [char](6) NULL,
	[HORA_ENVIO_CREDITO] [char](4) NULL,
	[OPERADOR_CR_1] [char](2) NULL,
	[OPERADOR_CR_2] [char](2) NULL,
	[MOTIVO_RECHAZO_CR] [char](4) NULL,
	[OPERADOR_INGRESO] [char](2) NULL,
	[AUTORIZANTE_1] [char](2) NULL,
	[AUTORIZANTE_2] [char](2) NULL,
	[AUTORIZANTE_3] [char](2) NULL,
	[FECHA_AUTORIZACION] [char](6) NULL,
	[HORA_AUTORIZACION] [char](4) NULL,
	[ESTADO] [char](2) NULL,
	[FEC_ESTADO] [char](6) NULL,
	[OBSERVACION_1] [char](60) NULL,
	[OBSERVACION_2] [char](100) NULL,
	[CLAVE_MAC] [char](12) NULL,
	[NRO_REFERENCIA] [char](7) NULL,
	[NRO_ENVIO] [char](3) NULL,
	[DEB_CONSOLIDADO] [char](1) NULL,
	[TIPO_TITULAR] [char](1) NULL,
	[PAGO_PREACORDADO] [char](1) NULL,
	[TRATA_CR_DIFERIDO] [char](1) NULL,
	[RIESGO_ABONADO] [char](1) NULL,
	[RIESGO_BANCO] [char](1) NULL,
	[NRO_LOTE] [char](3) NULL,
	[ESTADO_TRATAMIENTO] [char](1) NULL,
	[ESTADOS_ANTERIORES] [char](140) NULL,
	[COD_RESPUESTA] [char](4) NULL,
	[CANT_RECLAMOS] [char](2) NULL,
	[HORA_ENVIO_RECLAMO] [char](6) NULL,
	[COD_USUARIO_OFICIAL] [char](10) NULL,
	[COD_OFICIAL_RESP] [char](10) NULL,
	[HORA_RESPUESTA] [char](6) NULL,
	[ERROR_DE_MAC] [char](12) NULL,
	[CLAVE_MENVIADA] [char](12) NULL,
	[PEND_ENVIO] [char](1) NULL,
	[SECUENCIA_DEBCRE] [char](10) NULL,
	[COD_ALTERNO_DEBCRE] [char](10) NULL,
	[SECUENCIA_REVERSA] [char](10) NULL,
	[COD_ALTERNO_REVERSA] [char](10) NULL,
	[ACCION] [char](1) NULL,
 CONSTRAINT [PK_NRO_TRANSFERENCIA] PRIMARY KEY CLUSTERED 
(
	[NRO_TRANSFERENCIA] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
CREATE NONCLUSTERED INDEX [IX_ESTADO] ON [dbo].[dn_tef_online] 
(
	[ESTADO] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE UNIQUE NONCLUSTERED INDEX [IX_FECHA_TRANSFERENCIA] ON [dbo].[dn_tef_online] 
(
	[FEC_SOLICITUD] ASC,
	[NRO_TRANSFERENCIA] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
/****** Objeto:  Table [dbo].[dn_tef_online_historico]    Fecha de la secuencia de comandos: 02/04/2013 14:15:09 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[dn_tef_online_historico](
	[COD_ORIGEN] [char](4) NULL,
	[COD_DESTINO] [char](4) NULL,
	[TIPO_REGISTRO] [char](3) NULL,
	[ACTIVIDAD] [char](1) NULL,
	[CONTADOR] [char](2) NULL,
	[RESERVADO] [char](6) NULL,
	[BCO_DEBITO] [char](3) NULL,
	[FEC_SOLICITUD] [char](8) NULL,
	[NRO_TRANSFERENCIA] [char](7) NOT NULL,
	[COD_ABONADO] [char](7) NULL,
	[TIPO_OPERACION] [char](2) NULL,
	[IMPORTE] [char](17) NULL,
	[SUC_DEBITO] [char](4) NULL,
	[NOM_SOLICITANTE] [char](29) NULL,
	[TIPO_CTA_DEB_RED] [char](2) NULL,
	[NRO_CTA_RED] [char](2) NULL,
	[CTA_DEBITO] [char](17) NULL,
	[FEC_ENVIO_DEBITO] [char](6) NULL,
	[HORA_ENVIO_DEBITO] [char](4) NULL,
	[OPERADOR_DB_1] [char](2) NULL,
	[OPERADOR_DB_2] [char](2) NULL,
	[MOTIVO_RECHAZO_DB] [char](4) NULL,
	[BCO_CREDITO] [char](3) NULL,
	[SUC_CREDITO] [char](4) NULL,
	[NOM_BENEFICIARIO] [char](29) NULL,
	[TIPO_CTA_CRED_RED] [char](2) NULL,
	[CTA_CREDITO] [char](17) NULL,
	[FEC_ENVIO_CREDITO] [char](6) NULL,
	[HORA_ENVIO_CREDITO] [char](4) NULL,
	[OPERADOR_CR_1] [char](2) NULL,
	[OPERADOR_CR_2] [char](2) NULL,
	[MOTIVO_RECHAZO_CR] [char](4) NULL,
	[OPERADOR_INGRESO] [char](2) NULL,
	[AUTORIZANTE_1] [char](2) NULL,
	[AUTORIZANTE_2] [char](2) NULL,
	[AUTORIZANTE_3] [char](2) NULL,
	[FECHA_AUTORIZACION] [char](6) NULL,
	[HORA_AUTORIZACION] [char](4) NULL,
	[ESTADO] [char](2) NULL,
	[FEC_ESTADO] [char](6) NULL,
	[OBSERVACION_1] [char](60) NULL,
	[OBSERVACION_2] [char](100) NULL,
	[CLAVE_MAC] [char](12) NULL,
	[NRO_REFERENCIA] [char](7) NULL,
	[NRO_ENVIO] [char](3) NULL,
	[DEB_CONSOLIDADO] [char](1) NULL,
	[TIPO_TITULAR] [char](1) NULL,
	[PAGO_PREACORDADO] [char](1) NULL,
	[TRATA_CR_DIFERIDO] [char](1) NULL,
	[RIESGO_ABONADO] [char](1) NULL,
	[RIESGO_BANCO] [char](1) NULL,
	[NRO_LOTE] [char](3) NULL,
	[ESTADO_TRATAMIENTO] [char](1) NULL,
	[ESTADOS_ANTERIORES] [char](140) NULL,
	[COD_RESPUESTA] [char](4) NULL,
	[CANT_RECLAMOS] [char](2) NULL,
	[HORA_ENVIO_RECLAMO] [char](6) NULL,
	[COD_USUARIO_OFICIAL] [char](10) NULL,
	[COD_OFICIAL_RESP] [char](10) NULL,
	[HORA_RESPUESTA] [char](6) NULL,
	[ERROR_DE_MAC] [char](12) NULL,
	[CLAVE_MENVIADA] [char](12) NULL,
	[PEND_ENVIO] [char](1) NULL,
	[SECUENCIA_DEBCRE] [char](10) NULL,
	[COD_ALTERNO_DEBCRE] [char](10) NULL,
	[SECUENCIA_REVERSA] [char](10) NULL,
	[COD_ALTERNO_REVERSA] [char](10) NULL,
	[ACCION] [char](1) NULL
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Objeto:  Table [dbo].[ACUERD]    Fecha de la secuencia de comandos: 02/04/2013 14:14:18 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[ACUERD](
	[AC_CODEMP] [numeric](5, 0) NOT NULL,
	[AC_SUCURS] [numeric](5, 0) NOT NULL,
	[AC_CODSIS] [smallint] NOT NULL,
	[AC_TIPCTA] [numeric](5, 0) NOT NULL,
	[AC_MONEDA] [numeric](5, 0) NOT NULL,
	[AC_CUENTA] [numeric](15, 0) NOT NULL,
	[AC_ORDEN] [numeric](15, 0) NOT NULL,
	[AC_NROCLI] [numeric](15, 0) NOT NULL,
	[AC_NROSOL] [numeric](15, 0) NOT NULL,
	[AC_TIPACU] [smallint] NOT NULL,
	[AC_CODLIN] [smallint] NOT NULL,
	[AC_CODGAR] [smallint] NOT NULL,
	[AC_PLATOT] [numeric](5, 0) NOT NULL,
	[AC_FECALT] [datetime] NOT NULL,
	[AC_FECVTO] [datetime] NOT NULL,
	[AC_TASNOM] [numeric](12, 6) NOT NULL,
	[AC_TASEFE] [numeric](12, 6) NOT NULL,
	[AC_MONACU] [numeric](15, 2) NOT NULL,
	[AC_TIPAPL] [smallint] NOT NULL,
	[AC_AUTO01] [numeric](5, 0) NOT NULL,
	[AC_AUTO02] [numeric](5, 0) NOT NULL,
	[AC_AUTO03] [numeric](5, 0) NOT NULL,
	[AC_FECPRO] [datetime] NOT NULL,
	[AC_ESTADO] [smallint] NOT NULL,
	[AC_FECCAN] [datetime] NOT NULL,
	[AC_COME01] [char](70) NOT NULL,
	[AC_COME02] [char](70) NOT NULL,
	[AC_COME03] [char](70) NOT NULL,
 CONSTRAINT [PK_ACUERD] PRIMARY KEY CLUSTERED 
(
	[AC_TIPCTA] ASC,
	[AC_MONEDA] ASC,
	[AC_CUENTA] ASC,
	[AC_ORDEN] ASC,
	[AC_NROCLI] ASC,
	[AC_FECALT] ASC,
	[AC_FECVTO] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
CREATE NONCLUSTERED INDEX [IX_ACUERDO] ON [dbo].[ACUERD] 
(
	[AC_TIPCTA] ASC,
	[AC_CUENTA] ASC,
	[AC_ORDEN] ASC,
	[AC_FECALT] ASC,
	[AC_FECVTO] ASC,
	[AC_ESTADO] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE NONCLUSTERED INDEX [IX_FECHA_VTO] ON [dbo].[ACUERD] 
(
	[AC_FECVTO] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
/****** Objeto:  Table [dbo].[CLIENT]    Fecha de la secuencia de comandos: 02/04/2013 14:14:29 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[CLIENT](
	[CL_NROCLI] [numeric](15, 0) NOT NULL,
	[CL_TIPPER] [char](1) NULL,
	[CL_CATEGO] [smallint] NULL,
	[CL_SECTOR] [smallint] NULL,
	[CL_RESIDE] [smallint] NULL,
	[CL_CODCON] [smallint] NULL,
	[CL_NOMCLI] [char](40) NOT NULL,
	[CL_DOMCLI] [char](40) NULL,
	[CL_POSCLI] [smallint] NULL,
	[CL_LOCCLI] [char](30) NULL,
	[CL_DOMCOR] [char](40) NULL,
	[CL_POSCOR] [smallint] NULL,
	[CL_LOCCOR] [char](30) NULL,
	[CL_TELEFO] [char](40) NULL,
	[CL_FECALT] [datetime] NOT NULL,
	[CL_FECBAJ] [datetime] NOT NULL,
	[CL_MOTBAJ] [smallint] NULL,
	[CL_SUCURS] [numeric](5, 0) NULL,
	[CL_NROFUN] [numeric](5, 0) NULL,
	[CL_TIPCLI] [smallint] NOT NULL,
	[CL_CLACOD] [smallint] NULL,
	[CL_CLAFEC] [datetime] NULL,
	[CL_CLAPRO] [datetime] NULL,
	[CL_CLAULT] [datetime] NULL,
	[CL_CLACES] [smallint] NULL,
	[CL_CLAVES] [datetime] NULL,
	[CL_CODIVA] [smallint] NULL,
	[CL_RETIVA] [char](1) NULL,
	[CL_CODACT] [numeric](5, 0) NULL,
	[CL_CODRAM] [numeric](5, 0) NULL,
	[CL_GRUPRO] [numeric](5, 0) NULL,
	[CL_GRUBCR] [numeric](5, 0) NULL,
	[CL_TIPDOC] [smallint] NULL,
	[CL_NRODOC] [numeric](15, 0) NULL,
	[CL_NATJUR] [smallint] NULL,
	[CL_CANEMP] [numeric](8, 0) NULL,
	[CL_INSRPC] [char](30) NULL,
	[CL_NOMPAD] [char](40) NULL,
	[CL_NROLEG] [char](25) NULL,
	[CL_CTABCR] [numeric](10, 0) NULL,
	[CL_CODBCO_LIQ] [numeric](5, 0) NULL,
	[CL_CUENTA_LIQ] [char](15) NULL,
	[CL_CODBCO_USA] [numeric](5, 0) NULL,
	[CL_CUENTA_USA] [char](15) NULL,
	[CL_COSELL] [char](1) NULL,
	[CL_COPUNI] [char](1) NULL,
	[CL_FECEST] [datetime] NOT NULL,
	[CL_FECPRO] [datetime] NOT NULL,
	[CL_NROPRO] [numeric](5, 0) NOT NULL,
 CONSTRAINT [PK_CLIENT] PRIMARY KEY CLUSTERED 
(
	[CL_NROCLI] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
/****** Objeto:  Table [dbo].[EXTCTA]    Fecha de la secuencia de comandos: 02/04/2013 14:15:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[EXTCTA](
	[EX_CODEMP] [numeric](5, 0) NOT NULL,
	[EX_SUCURS] [numeric](5, 0) NOT NULL,
	[EX_CODSIS] [smallint] NOT NULL,
	[EX_TIPCTA] [numeric](5, 0) NOT NULL,
	[EX_MONEDA] [numeric](5, 0) NOT NULL,
	[EX_CUENTA] [numeric](15, 0) NOT NULL,
	[EX_ORDEN] [numeric](15, 0) NOT NULL,
	[EX_NROLOG] [numeric](12, 0) NOT NULL,
	[EX_LOGCTA] [numeric](12, 0) NOT NULL,
	[EX_NROCLI] [numeric](15, 0) NOT NULL,
	[EX_NROCOM] [numeric](10, 0) NOT NULL,
	[EX_NROCHE] [numeric](10, 0) NOT NULL,
	[EX_CODBAN] [numeric](5, 0) NOT NULL,
	[EX_CONCEP] [varchar](20) NULL,
	[EX_FECOPE] [datetime] NOT NULL,
	[EX_FECIMP] [datetime] NOT NULL,
	[EX_CODTRA] [numeric](5, 0) NOT NULL,
	[EX_TIPOPE] [smallint] NOT NULL,
	[EX_IMPORT] [numeric](15, 2) NOT NULL,
	[EX_CODCAM] [numeric](5, 0) NOT NULL,
	[EX_PLACON] [smallint] NOT NULL,
	[EX_FECCON] [datetime] NOT NULL,
	[EX_HORCON] [char](6) NOT NULL,
	[EX_SUCORI] [numeric](5, 0) NOT NULL,
	[EX_TIPMOV] [char](2) NOT NULL,
	[EX_NROTER] [numeric](5, 0) NOT NULL,
	[EX_CODOPE] [numeric](5, 0) NOT NULL,
	[EX_DATEOR] [datetime] NOT NULL,
	[EX_TIMEOR] [char](6) NOT NULL,
	[EX_NROCAJ] [numeric](5, 0) NOT NULL,
	[EX_CODCAJ] [numeric](5, 0) NOT NULL,
	[EX_DATECA] [datetime] NOT NULL,
	[EX_TIMECA] [char](6) NOT NULL,
	[EX_CODERR] [smallint] NOT NULL,
	[EX_CODLIQ] [char](1) NOT NULL,
	[EX_FECCHE] [datetime] NOT NULL,
	[EX_ASIIMP] [numeric](5, 0) NOT NULL,
	[EX_SALACT] [numeric](15, 2) NOT NULL,
	[EX_ESTADO] [smallint] NOT NULL
) ON [PRIMARY]
GO
SET ANSI_PADDING OFF
GO
CREATE NONCLUSTERED INDEX [ix_dateor] ON [dbo].[EXTCTA] 
(
	[EX_DATEOR] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE NONCLUSTERED INDEX [IX_EXTCTA_COMPLETO] ON [dbo].[EXTCTA] 
(
	[EX_TIPCTA] ASC,
	[EX_CUENTA] ASC,
	[EX_ORDEN] ASC,
	[EX_FECOPE] ASC,
	[EX_DATEOR] ASC,
	[EX_FECCON] ASC,
	[EX_ESTADO] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE NONCLUSTERED INDEX [IX_EXTCTA_CUENTA] ON [dbo].[EXTCTA] 
(
	[EX_TIPCTA] ASC,
	[EX_CUENTA] ASC,
	[EX_ORDEN] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE NONCLUSTERED INDEX [ix_feccon] ON [dbo].[EXTCTA] 
(
	[EX_FECCON] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE NONCLUSTERED INDEX [ix_fecope] ON [dbo].[EXTCTA] 
(
	[EX_FECOPE] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE NONCLUSTERED INDEX [ix_nrolog] ON [dbo].[EXTCTA] 
(
	[EX_NROLOG] ASC
)WITH FILLFACTOR = 90 ON [PRIMARY]
GO
CREATE UNIQUE NONCLUSTERED INDEX [ix_PK_NROLOG_FECCON_CONCEP] ON [dbo].[EXTCTA] 
(
	[EX_NROLOG] ASC,
	[EX_FECCON] ASC,
	[EX_IMPORT] ASC,
	[EX_TIPOPE] ASC,
	[EX_CONCEP] ASC,
	[EX_CUENTA] ASC
) ON [PRIMARY]
GO
/****** Objeto:  StoredProcedure [dbo].[SP_VERIFICA_FERIADO]    Fecha de la secuencia de comandos: 02/04/2013 14:14:09 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Objeto:  procedimiento almacenado dbo.SP_VERIFICA_FERIADO    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROC [dbo].[SP_VERIFICA_FERIADO]
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
/****** Objeto:  StoredProcedure [dbo].[SP_FECHAS_DE_FERIADO]    Fecha de la secuencia de comandos: 02/04/2013 14:14:06 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Objeto:  procedimiento almacenado dbo.SP_FECHAS_DE_FERIADO    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROCEDURE [dbo].[SP_FECHAS_DE_FERIADO] 
AS 
  SELECT FE_FECPRO FROM DBO.FERIAD
GO
/****** Objeto:  StoredProcedure [dbo].[SP_VIEW_MAESTRO_DE_CUENTAS]    Fecha de la secuencia de comandos: 02/04/2013 14:14:09 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_MAESTRO_DE_CUENTAS    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/
CREATE PROC [dbo].[SP_VIEW_MAESTRO_DE_CUENTAS]
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
/****** Objeto:  View [dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]    Fecha de la secuencia de comandos: 02/04/2013 14:15:35 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS


CREATE VIEW [dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]
AS 

SELECT     CONVERT(varchar(12), dbo.MAECUE.MC_CUENTA) AS CUENTA, dbo.EXTCTA.EX_FECCON AS fecha, dbo.EXTCTA.EX_NROLOG AS SECUENCIAL, 
                      dbo.MAECUE.MC_SALACT AS SALDO, dbo.EXTCTA.EX_IMPORT * dbo.EXTCTA.EX_TIPOPE AS IMPORTE, dbo.EXTCTA.EX_TIPOPE AS DB_CR, 
                      dbo.EXTCTA.EX_FECOPE AS FECHA_PROCESO, dbo.EXTCTA.EX_SUCORI AS SUCURSAL, dbo.EXTCTA.EX_NROCHE AS DEPOSITO, 
                      dbo.EXTCTA.EX_CONCEP AS DESCRIPCION, dbo.EXTCTA.EX_TIPMOV AS OPERACION, ISNULL(SUM(dbo.ACUERD.AC_MONACU), 0) AS ACUERDO, 
                      dbo.CLIENT.CL_NOMCLI AS NOMBRE, dbo.EXTCTA.EX_DATEOR AS FECHA_VALOR, CONVERT(VARCHAR, dbo.MAECUE.MC_FECALT, 112) AS FECAPE, 
                      dbo.MAECUE.MC_TIPCTA AS TIPCTA,
/* FIX NOMBRE DE ABONADO, 2012.12.04, MDC */
                      dbo.DN_ABONADOS_CLAVES.RAZON_SOCIAL AS RAZON_SOCIAL
FROM         dbo.MAECUE 
INNER JOIN
                      dbo.NOVEDA ON dbo.NOVEDA.NV_TIPCTA = dbo.MAECUE.MC_TIPCTA AND dbo.NOVEDA.NV_CUENTA = dbo.MAECUE.MC_CUENTA AND 
                      dbo.NOVEDA.NV_ORDEN = dbo.MAECUE.MC_ORDEN AND dbo.NOVEDA.NV_FECBAJ = '1900-01-01' 
INNER JOIN
                      dbo.dn_parametros ON dbo.dn_parametros.dn_id = 1 
LEFT OUTER JOIN
                      dbo.ACUERD ON dbo.ACUERD.AC_TIPCTA = dbo.MAECUE.MC_TIPCTA AND dbo.ACUERD.AC_CUENTA = dbo.MAECUE.MC_CUENTA AND 
                      dbo.ACUERD.AC_ORDEN = dbo.MAECUE.MC_ORDEN AND dbo.ACUERD.AC_ESTADO = 0 AND 
                      dbo.ACUERD.AC_FECVTO > dbo.dn_parametros.dn_fecha_proceso 
LEFT OUTER JOIN
                      dbo.CLIENT ON dbo.CLIENT.CL_NROCLI = dbo.MAECUE.MC_NROCLI 
LEFT OUTER JOIN
                      dbo.EXTCTA ON dbo.MAECUE.MC_TIPCTA = dbo.EXTCTA.EX_TIPCTA AND dbo.MAECUE.MC_CUENTA = dbo.EXTCTA.EX_CUENTA AND 
                      dbo.MAECUE.MC_ORDEN = dbo.EXTCTA.EX_ORDEN AND dbo.MAECUE.MC_ESTADO = 0 AND (dbo.EXTCTA.EX_ESTADO = 1 AND 
                      dbo.EXTCTA.EX_FECCON >= DATEADD(DAY, - dbo.dn_parametros.dn_registro_hasta, dbo.dn_parametros.dn_fecha_proceso) AND 
                      dbo.EXTCTA.EX_FECCON <= DATEADD(DAY, - dbo.dn_parametros.dn_registro_desde, dbo.dn_parametros.dn_fecha_proceso) AND dbo.dn_parametros.dn_id = 1 OR
                      dbo.EXTCTA.EX_ESTADO = 0 AND dbo.EXTCTA.EX_FECOPE <> dbo.EXTCTA.EX_FECCON AND dbo.EXTCTA.EX_FECCON >= DATEADD(DAY, 
                      - dbo.dn_parametros.dn_registro_hasta, dbo.dn_parametros.dn_fecha_proceso) AND dbo.dn_parametros.dn_id = 1 OR
                      dbo.EXTCTA.EX_ESTADO = 1 AND dbo.EXTCTA.EX_DATEOR <> dbo.EXTCTA.EX_FECCON AND dbo.EXTCTA.EX_DATEOR >= DATEADD(DAY, 
                      - dbo.dn_parametros.dn_registro_hasta, dbo.dn_parametros.dn_fecha_proceso) AND dbo.EXTCTA.EX_DATEOR <= DATEADD(DAY, 
                      - dbo.dn_parametros.dn_registro_desde, dbo.dn_parametros.dn_fecha_proceso) AND dbo.dn_parametros.dn_id = 1)
/* FIX NOMBRE DE ABONADO, 2012.12.04, MDC */
LEFT OUTER JOIN
                      dbo.DN_ABONADOS_CUENTAS ON dbo.DN_ABONADOS_CUENTAS.CUENTA      = dbo.MAECUE.MC_CUENTA AND
                                                 dbo.DN_ABONADOS_CUENTAS.TIPO_CUENTA = dbo.MAECUE.MC_TIPCTA 
LEFT OUTER JOIN
                      dbo.DN_ABONADOS_CLAVES  ON dbo.DN_ABONADOS_CUENTAS.ABONADO     = dbo.DN_ABONADOS_CLAVES.ABONADO
/* AGRUPAMIENTO */
GROUP BY              dbo.MAECUE.MC_TIPCTA, dbo.MAECUE.MC_CUENTA, dbo.MAECUE.MC_ORDEN, dbo.EXTCTA.EX_FECCON, dbo.EXTCTA.EX_NROLOG, 
                      dbo.MAECUE.MC_SALACT, dbo.EXTCTA.EX_IMPORT, dbo.EXTCTA.EX_TIPOPE, dbo.EXTCTA.EX_FECOPE, dbo.EXTCTA.EX_SUCORI, 
                      dbo.EXTCTA.EX_NROCHE, dbo.EXTCTA.EX_CONCEP, dbo.EXTCTA.EX_TIPMOV, dbo.CLIENT.CL_NOMCLI, dbo.EXTCTA.EX_DATEOR, 
					  dbo.MAECUE.MC_FECALT, dbo.DN_ABONADOS_CLAVES.RAZON_SOCIAL
GO
/****** Objeto:  StoredProcedure [dbo].[SP_UPDATE_DN_PARAMETROS]    Fecha de la secuencia de comandos: 02/04/2013 14:14:08 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Objeto:  procedimiento almacenado dbo.SP_UPDATE_DN_PARAMETROS    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/
CREATE PROCEDURE [dbo].[SP_UPDATE_DN_PARAMETROS] 
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
/****** Objeto:  StoredProcedure [dbo].[SP_TEF_PASE_A_HISTORICO]    Fecha de la secuencia de comandos: 02/04/2013 14:14:08 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROC [dbo].[SP_TEF_PASE_A_HISTORICO]
AS

insert into dn_tef_online_historico
select * from dn_tef_online
where nro_transferencia not in (select nro_transferencia from dn_tef_online_historico )
GO
/****** Objeto:  StoredProcedure [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA]    Fecha de la secuencia de comandos: 02/04/2013 14:14:07 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Objeto:  procedimiento almacenado dbo.SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROC [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA]
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
/****** Objeto:  StoredProcedure [dbo].[SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]    Fecha de la secuencia de comandos: 02/04/2013 14:14:10 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Objeto:  procedimiento almacenado dbo.SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS    fecha de la secuencia de comandos: 24/04/2007 11:24:01 a.m. ******/
CREATE PROC [dbo].[SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]
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
/****** Objeto:  StoredProcedure [dbo].[SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS]    Fecha de la secuencia de comandos: 02/04/2013 14:14:10 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS
-- FIX NOMBRE DE ABONADO, 2012.12.04, MDC 

CREATE PROC [dbo].[SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS]
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
	SELECT CUENTA, SALDO, ACUERDO, NOMBRE, FECAPE, RAZON_SOCIAL 
	FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS
	WHERE ACUERDO IS NOT NULL 
		AND 
	      TIPCTA IN (@i_tipcta1,@i_tipcta2,@i_tipcta3,@i_tipcta4)
	UNION
	SELECT CUENTA, SALDO, 0 AS ACUERDO, NOMBRE, FECAPE, RAZON_SOCIAL 
    FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS
	WHERE ACUERDO IS NULL 
		AND 
              TIPCTA IN (@i_tipcta1,@i_tipcta2,@i_tipcta3,@i_tipcta4)
	GROUP BY CUENTA, SALDO, ACUERDO, NOMBRE, FECAPE, RAZON_SOCIAL
        ORDER BY CUENTA, SALDO
GO
/****** Objeto:  StoredProcedure [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main]    Fecha de la secuencia de comandos: 02/04/2013 14:14:07 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/****** Objeto:  procedimiento almacenado dbo.SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main    fecha de la secuencia de comandos: 24/04/2007 11:24:00 a.m. ******/
CREATE PROC [dbo].[SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main]
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

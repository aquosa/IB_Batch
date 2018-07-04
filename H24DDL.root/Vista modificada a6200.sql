USE [DTN_INTERFINANZAS]
GO

/****** Object:  View [dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]    Script Date: 10/08/2017 13:03:41 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO





/* VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS*/
ALTER VIEW [dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]
AS
SELECT        CONVERT(varchar(12), dbo.MAECUE.MC_CUENTA) AS CUENTA, dbo.EXTCTA.EX_FECCON AS fecha, dbo.EXTCTA.EX_NROLOG AS SECUENCIAL, dbo.MAECUE.MC_SALACT AS SALDO, 
                         dbo.EXTCTA.EX_IMPORT * dbo.EXTCTA.EX_TIPOPE AS IMPORTE, dbo.EXTCTA.EX_TIPOPE AS DB_CR, dbo.EXTCTA.EX_FECOPE AS FECHA_PROCESO, dbo.EXTCTA.EX_SUCORI AS SUCURSAL, 
                         dbo.EXTCTA.EX_NROCHE AS DEPOSITO, dbo.EXTCTA.EX_CONCEP AS DESCRIPCION, dbo.EXTCTA.EX_TIPMOV AS OPERACION, ISNULL(SUM(dbo.ACUERD.AC_MONACU), 0) AS ACUERDO, 
                         dbo.CLIENT.CL_NOMCLI AS NOMBRE, dbo.EXTCTA.EX_DATEOR AS FECHA_VALOR, CONVERT(VARCHAR, dbo.MAECUE.MC_FECALT, 112) AS FECAPE, dbo.MAECUE.MC_TIPCTA AS TIPCTA, 
                         dbo.dn_abonados_claves.razon_social, isnull(dbo.CodigosOperacion.CodOpeIB,'M83') AS CODIGOOPERACION, isnull(dbo.CodigosOperacion.DescripcionMOV,'VARIOS') AS DESCRIPCIONLARGA,
						 dbo.extcta.EX_MARCADIFERIDO as MarcaDiferido, dbo.EXTCTA.EX_CORTOS as MARCACORTOS, ISNULL(dbo.EXTCTA.EX_CUITOR,'000000000000') as CUIT, ISNULL(dbo.EXTCTA.ex_denoor,' ') as DENOM,
						 dbo.EXTCTA.EX_CONCEP as CODOPE
FROM            dbo.MAECUE INNER JOIN
                         dbo.NOVEDA ON dbo.NOVEDA.NV_TIPCTA = dbo.MAECUE.MC_TIPCTA AND dbo.NOVEDA.NV_CUENTA = dbo.MAECUE.MC_CUENTA AND dbo.NOVEDA.NV_ORDEN = dbo.MAECUE.MC_ORDEN AND 
                         dbo.NOVEDA.NV_FECBAJ = '1900-01-01' INNER JOIN
                         dbo.dn_parametros ON dbo.dn_parametros.dn_id = 1 inner JOIN
                         dbo.ACUERD ON dbo.ACUERD.AC_TIPCTA = dbo.MAECUE.MC_TIPCTA AND dbo.ACUERD.AC_CUENTA = dbo.MAECUE.MC_CUENTA AND dbo.ACUERD.AC_ORDEN = dbo.MAECUE.MC_ORDEN AND 
                         dbo.ACUERD.AC_ESTADO = 0 AND dbo.ACUERD.AC_FECVTO > dbo.dn_parametros.dn_fecha_proceso inner JOIN
                         dbo.CLIENT ON dbo.CLIENT.CL_NROCLI = dbo.MAECUE.MC_NROCLI inner JOIN
                         dbo.EXTCTA ON dbo.MAECUE.MC_TIPCTA = dbo.EXTCTA.EX_TIPCTA AND dbo.MAECUE.MC_CUENTA = dbo.EXTCTA.EX_CUENTA AND dbo.MAECUE.MC_ORDEN = dbo.EXTCTA.EX_ORDEN AND 
                         dbo.MAECUE.MC_ESTADO = 0 AND (dbo.EXTCTA.EX_ESTADO = 1 AND dbo.EXTCTA.EX_FECCON <= (dn_parametros.dn_fecha_proceso - DATEDIFF(DAY, dbo.dn_parametros.dn_fecha_proceso, dbo.dn_parametros.dn_fecha_hasta)) AND 
                         dbo.EXTCTA.EX_FECCON >= (dn_parametros.dn_fecha_proceso - DATEDIFF(DAY, dbo.dn_parametros.dn_fecha_desde, dbo.dn_parametros.dn_fecha_proceso)) AND dbo.dn_parametros.dn_id = 1 OR
                         dbo.EXTCTA.EX_ESTADO = 0 AND dbo.EXTCTA.EX_FECOPE <> dbo.EXTCTA.EX_FECCON AND dbo.EXTCTA.EX_FECCON <= (dbo.dn_parametros.dn_fecha_hasta - DATEDIFF(DAY, dbo.dn_parametros.dn_fecha_proceso, dbo.dn_parametros.dn_fecha_hasta)) AND dbo.dn_parametros.dn_id = 1 OR
                         dbo.EXTCTA.EX_ESTADO = 1 AND dbo.EXTCTA.EX_DATEOR <> dbo.EXTCTA.EX_FECCON AND dbo.EXTCTA.EX_DATEOR <= (dbo.dn_parametros.dn_fecha_hasta - DATEDIFF(DAY, dbo.dn_parametros.dn_fecha_proceso,  dbo.dn_parametros.dn_fecha_hasta)) AND dbo.EXTCTA.EX_DATEOR >= (dn_parametros.dn_fecha_proceso - DATEDIFF(DAY, dbo.dn_parametros.dn_fecha_desde, dbo.dn_parametros.dn_fecha_proceso)) AND dbo.dn_parametros.dn_id = 1) 
                         inner JOIN
                         dbo.dn_abonados_cuentas ON dbo.dn_abonados_cuentas.cuenta = dbo.MAECUE.MC_CUENTA AND dbo.dn_abonados_cuentas.tipo_cuenta = dbo.MAECUE.MC_TIPCTA inner JOIN
                         dbo.dn_abonados_claves ON dbo.dn_abonados_cuentas.abonado = dbo.dn_abonados_claves.abonado left outer JOIN
                         dbo.CodigosOperacion ON   dbo.EXTCTA.EX_CONCEP = dbo.CodigosOperacion.CodigoOMA

GROUP BY dbo.MAECUE.MC_TIPCTA, dbo.MAECUE.MC_CUENTA, dbo.MAECUE.MC_ORDEN, dbo.EXTCTA.EX_FECCON, dbo.EXTCTA.EX_NROLOG, dbo.MAECUE.MC_SALACT, dbo.EXTCTA.EX_IMPORT, 
                         dbo.EXTCTA.EX_TIPOPE, dbo.EXTCTA.EX_FECOPE, dbo.EXTCTA.EX_SUCORI, dbo.EXTCTA.EX_NROCHE, dbo.EXTCTA.EX_CONCEP, dbo.EXTCTA.EX_TIPMOV, dbo.CLIENT.CL_NOMCLI, dbo.EXTCTA.EX_DATEOR, 
                         dbo.MAECUE.MC_FECALT, dbo.dn_abonados_claves.razon_social, dbo.CodigosOperacion.CodOpeIB, dbo.CodigosOperacion.DescripcionMOV, dbo.extcta.ex_marcadiferido, dbo.EXTCTA.EX_CORTOS,
						 dbo.EXTCTA.EX_CUITOR, dbo.EXTCTA.EX_DENOOR, dbo.EXTCTA.EX_CODOPE








GO



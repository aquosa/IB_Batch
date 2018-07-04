-- VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS


ALTER VIEW [dbo].[VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS]
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
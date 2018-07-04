


-- tareas sugeridas de depuracion en base DATANET (FuegoDnet)
-- antes de depuracion de registros
select count(1) from noveda -- Q=161
select count(1) from acuerd -- Q=39741
select count(1) from maecue -- Q=39369
select count(1) from extcta -- Q=119556

-- borrar todos los registros de movimientos anteriores a 50 dias habiles
delete from extcta
where ex_fecope < convert(datetime,getdate() - 70)
-- Q=49344

-- sobran cuentas en el maestro que no estan abonadas!
-- Q= 39235
-- borrar todo lo que no sea novedad
delete from maecue
where mc_cuenta not in ( select distinct nv_cuenta from noveda )
delete from acuerd
where ac_cuenta not in ( select distinct nv_cuenta from noveda )




-- despues de depuracion de registros
select count(1) from noveda -- Q=161
select count(1) from acuerd -- Q=386
select count(1) from maecue -- Q=124
select count(1) from extcta -- Q=70212

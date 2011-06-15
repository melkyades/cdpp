                   XXX   XX            XXXXXX  XXXX      X      X
                   XXXX  XX           XXX      XX XX     X      X
                   XX XX XX    XXX   XXX       XX  XX  XXXXX  XXXXX
                   XX  XXXX           XXX      XX XX     X      X
                   XX   XXX            XXXXXX  XXXX      X      X

==============================================================================

Version 1.0: La version original de CD++.
	Autores:	Amir Barylko,
			Jorge Beyoglonian.
	Dirigido por:	Gabriel Wainer.

Version 2.0: N-CD++, basada en CD++.
	Autor:		Daniel A. Rodriguez.
	Dirigido por:	Gabriel Wainer.

==============================================================================
N-CD++
1998/1999. Universidad de Buenos Aires.
Facultad de Ciencias Exactas y Naturales.
==============================================================================

v.2.00 a v.2.27 - SEPTIEMBRE/1998 a ENERO/1999
	EXTENSION DE VALORES DE CELDAS - EXPANSION DEL LENGUAJE
	No fueron documentados los cambios realizados a cada version.

v.2.28	ENERO/1999 - PUERTOS
	Administracion de los puertos OUT en forma dinamica.

	Incorporacion de la funcion SEND para el envio de valores a traves
	de un puerto.


v.2.29	FEBRERO/1999 - N-DIMENSIONAL
	Incluye todo lo que es manejo de espacio de celdas n-dimensional.
	Pero solo esta implementado internamente. No se modifico GADCelLa,
	por lo que solo pueden seguir simulandose espacios bidimensionales
	pero que internamente trabaja como si fuese n-dimensional (ya no
	hay pares (x,y) sino listas {x1, ..., xn} y cosas por el estilo.

	Se extendio el vecindario de una celda, para que sus vecinos no sean
	necesariamente adyacentes a la misma.

	Bugs encontrados de versiones anteriores:
	* De la version original: el constructor de CellState tenia un
		array de punteros a Real. Para cada posicion del mismo
		hacia un new Real, pero nunca pedia espacio para el
		array en si.


v.2.30	MARZO/1999 - N-DIMENSIONAL
	El lenguaje GADCelLa ahora permite la definicion de modelos celulares
	n-dimensionales.
	
	Se mejoro el DRAWLOG. Ahora permite mostrar los graficos de los
	modelos cuando la dimension es 2 (como siempre), 3 (muestra por
	slices, en forma semejante al de 2 dimensiones) o mas (en este ultimo
	caso se muestra un listado que contiene la posicion de la celda y el
	valor en cada instante de tiempo). El drawlog detecta automaticamente
	la dimension del modelo y muestra el grafico adecuado.


v.2.31	ABRIL/1999 - OPTIMIZACION Y EXTENSION LENGUAJE
	Se mejoro la evaluacion de las reglas, con lo que AND, OR, XOR e IMP
	no siempre evaluan todos sus parametros, sino que lo hacen solo
	cuando es necesario debido a que en algunos casos alcanza con solo
	evaluar el primer parametro para generar una respuesta (por ej: si
	tengo x1 AND x2, y al evaluar x1 me da FALSE entonces la respuesta
	sera FALSE).

	Se agrego el modulo de Macro Expansiones.


v.2.32  ABRIL/1999 - OPTIMIZACION
	Se arreglo un grave error producido por el modulo nTupla, el cual
	no liberaba bien memoria. Para esto se redefinio completamente el
	modulo, aprovechando la situacion para optimizar ciertas funciones.

	Se agrego el modulo EvalDebug usado para redireccionar los distintos
	resultados segun los parametros indicados al simulador.
	
	Se revisaron gran parte de los modulos con el objetivo de optimizar
	las funcionalidades de los mismos.


v.2.33	MAYO/1999 - EXTENSION LENGUAJE
	Se agrego la funcion cellPos(i) que devuelve el elemento xi de la
	celda (x1,x2,...,xn) que esta actualmente ejecutando la funcion de
	transicion interna.

	Se mejoro la forma de mostrar en pantalla el modo de DEBUG de las
	reglas.


v.2.34	29/5/1999 - TESTEO
	Se agrego la herramienta makeRand para la generacion de valores
	aleatorios usados para evaluar los modelos con N-CD++.


v.2.35	04/6/1999 - TESTEO
	Arreglo de bug: No liberaba bien la memoria en el modo FLAT.
	Esto se debia al uso de LIST (de la STL) que aparentemente no
	funciona correctamente. Se creo el modulo MYLIST que contiene
	una implementacion de una lista para almacenar el vecindario.

	Se agrego el parametro -s al simulador, el cual muestra la hora de
	fin de simulacion por stdErr. De esta forma es posible redireccionar
	la salida estandar a un archivo, y el stderr a otro, junto con la
	informacion arrojada por el comando TIME (evaluando los tiempos de
	ejecucion de la herramienta).


v.2.36  06/6/1999 - OPTIMIZACION
	Se mejoro el modulo MYLIST optimizandolo para que sea mas rapido.

	Se optimizo la carga de valores iniciales de las celdas de un archivo
	de tipo .VAL, usando un buffer, reduciendo las lecturas del disco.

	Se arreglo un bug generado al introducir la lista MYLIST en la
	version 2.35.


v.2.37	13/6/1999 - OPTIMIZACION
	Se agrego la clausula InitialMapValue para la carga de valores
	iniciales en los modelos usando archivos .MAP.

	Se agrego el modulo toMap, para convertir archivos .VAL a .MAP.

	Se optimizo el seteo de valores a las celdas, tanto en modo
	jerarquico como achatado.

	Se optimizaron las funciones que trabajan sobre strings.

	Se agrego la opcion -O3 al Makefile, para crear versiones finales
	optimizadas por el compilador.


v.2.38	26/6/1999 - MENSAJES
	Se modifico el procedimiento Receive de Coordinator para el caso
	en que se recibia un mensaje representando un evento externo.

	Se agrego el modulo toCDPP, para convertir archivos .VAL a .MA
	soportados por CD++.


v.2.39	27/6/1999 - MODELO ACHATADO
	Se reemplazo la XYList por un Arreglo de VirtualPortList, lo que
	permite realizar un acceso mucho mas rapido a los puertos de una
	celda.

	Se modifico el modulo de Macros pues en ciertos casos incluia 2
	veces la ultima linea del archivo original.


v.2.40	18/7/1999 - BUGS - QUANTUM
	Se arreglo el manejo de Zonas, que no funcionaba bien.

	Se cambio el Makefile para Windows95, para que el compilador no
	optimice el codigo (-O3) para el modulo MacroExp pues genera errores
	de ejecucion. Para Linux funciona bien.

	Se agrego la opcion -q al simulador, que permite usar un quantum
	para el calculo de los valores de las celdas.

	Se agrego el parametro -w al simulador, para cambiar el ancho y la
	precision de los numeros reales que son mostrados por el simulador
	(ya sea en el archivo de log, en el de eventos de salida, en el modo
	de debug para la evaluacion de las reglas (-v), etc).

	Se agregaron nuevos parametros a MakeRand (para la generacion de
	valores para el Pinball y la Dispersion de Gases).


v.2.41	25/7/1999 - BUGS - PREPROCESADOR
	Se cambio el Drawlog que hacia comparacion de strings case sensitive
	en ciertos casos.

	Preprocesador de Macros: Ahora permite incluir varias macros en una
	misma linea.

	Preprocesador de Macros: Permite al usuario el ingreso de
	comentarios, que son ignorados por el preprocesador y no son pasados
	a N-CD++.

	El modulo del Preprocesador crea el archivo en el directorio
	especificado por la variable P_tmpdir de <stdio.h>.
	Si este directorio apunta al raiz, busca en las variables de entorno
	el directorio asignado a TEMP y si no esta el asignado a TMP.
	Si no lo encuentra deja el temporario en el directorio actual.


v.2.42	17/9/1999 - BUGS
	Se arreglo la clase InertialDelayCell, pues trabajaba mal con el
	vecindario en el momento de calcular la funcion de computo local.


v.2.43	10/10/1999 - BUGS - MAKERAND
	Se arreglo la impresion del vecindario cuando se generaba una
	excepcion, porque en todos los casos no funcionaba correctamente.

	Se cambio el formato del MakeRand a traves del uso de los parametros
	-i y -r.


v.2.44	22/11/1999 - BUGS - OPTIMIZACION
	Se cambio la lista de vecinos por una lista inversa de vecinos, para
	que funcione correctamente, dado que en algunos casos no se enviaban
	los mensajes a quien correspondia.

	Se optimizaron los modulos de FLATCOUPLED, TIME y NTUPLA.

	Se modifico la impresion del CellState pues mostraba estados
	indefinidos en casos que no debia.

v.2.45	07/12/1999 - BUGS - SALIDA DRAWLOG
	Se arreglo un bug que perdia eventos futuros.

	Se agrego el parametro -f al DrawLog para ver determinado slice de
	un automata celular en 3D.

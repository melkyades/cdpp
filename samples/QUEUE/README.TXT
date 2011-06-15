Este es un ejemplo del manejo de una cola. Como vemos se encolan tres valores:
1.5, 3 y -7.
Los dos primeros son recibidos por OUT (que demora 5 segundos) antes de hacer 
el DONE.
El tercero, se hace el DONE antes de que transcurra el tiempo de preparacion
especificado en el archivo QUEUE.MA, por lo que el valor sera borrado de la
cola antes de haber sido enviado por el puerto OUT (es por esto que se
recibe el valor 0).

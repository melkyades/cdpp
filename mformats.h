/*******************************************************************
*
*  DESCRIPCION: Definiciones comunes a los modelos del servidor y red
*               Definen el formato de los mensajes a intercambiar 
*  
*  AUTORES: Ing. Carlos Giorgetti
*           Iván Melgrati
*           Dra. Ana Rosa Tymoschuk
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*
*  FECHA: 09/10/1999
*
*******************************************************************/

#ifndef __MESSAGEFORMAT_H
#define __MESSAGEFORMAT_H

struct req{                         //Definir la estructura a usar para los pedidos
           unsigned  idtask:12;     // ID de la tarea (12 bits de largo)
           unsigned  origin:4;      // Dirección de Origen (0-15)
           unsigned  destino:4;     // Dirección de Destino (0-15)
           unsigned  disco:3;       // Disco a ser usado (1-8)
           unsigned  tamanio:9;     // Tamaño del pedido (0-512)
};

union request {
          double f;                 // Usado para compatibilidad con value.h
          struct req r;            // Contiene la información del mensaje
          request(const request& n) {f = n.f;}        // Constructores de la clase
          request(const float& d)  {f = d;}
          request(const double& d) {f = d;}
          request(){f = 0.0;}
};

#endif   // __MESSAGEFORMAT_H

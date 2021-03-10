# MOTOR

Para el movimiento del carrito se hizo uso de dos motores DC en conjunto con un puente H L298, esto para evitar que debido a un pico elevado de corriente se genera un daño en la tarjeta de arduino, las conexiones realizadas son las que podemos observar en la imagen que se encuentra a continuacion: 

<p align="center">

![Screenshot](/Imagenes/PuenteH.jpg)

</p> 

Observamos como se controlara el movimiento de los motores mediante el uso de arduino donde se polarizara cada uno de los motores dependiendo la accion que se necesite. Iniciaremos primero  para el caso de que el movimiento requerido sea hacia adelante, caso en el cual se polarizaran ambos motores de igual forma como se muestra a continuacion

``` 

void Adelante() {
  digitalWrite(Motor1_1, LOW);
  digitalWrite(Motor1_2, HIGH);
  digitalWrite(Motor2_2, HIGH);
  digitalWrite(Motor2_1, LOW);
}

```
Ahora si el movimiento requerido es hacia retroceder, se debe polarizaciar todo al contrario de como lo tenemos en la parte anterior, asi obtendremos el siguiente codigo: 

``` arduino

void Reversa() {
  digitalWrite(Motor1_1, HIGH);
  digitalWrite(Motor1_2, LOW);
  digitalWrite(Motor2_2, LOW);
  digitalWrite(Motor2_1, HIGH);
}

```

Ahora en los casos donde el movimiento sea hacia los lados, se deber asegurar que el robot gire sobre su propio eje, para lograr que se de esto se hacen girar ambas llantas en direcciones contrarias dependiendo de la funcion que se requiera, a continuacion podremos observar el codigo que se implemento para realizar esta rotacion: 

```
void Derecha() {
  digitalWrite(Motor1_1, LOW);
  digitalWrite(Motor1_2, HIGH);
  digitalWrite(Motor2_2, LOW);
  digitalWrite(Motor2_1, HIGH);
}

void Izquierda() {
  digitalWrite(Motor2_2, HIGH);
  digitalWrite(Motor2_1, LOW);
  digitalWrite(Motor1_1, HIGH);
  digitalWrite(Motor1_2, LOW);
}
```

Ademas de estas funciones que ayudan al movimiento del carro en las cuatro diferentes direcciones es necesario el uso de una funcion adicional para lograr que se detenga, para ello no se polariza el motor de manera alguna, lo que hace que esta se detenga o no se mueva, el codigo para esta seccion es el que se muestra a continuacion: 

```
void Alto() {
  digitalWrite(Motor2_2, LOW);
  digitalWrite(Motor2_1, LOW);
  digitalWrite(Motor1_1, LOW);
  digitalWrite(Motor1_2, LOW);
}

```

Ahora para realizar la eleccion de la funcion que sera usada se utiliza una switch-case, donde para 
ir adelante se debe enviar la letra G, si se quiere retroceder se envia la letra K, si se quiere ir hacia los lados debemos hacer uso de la letras H para la izquierda, y la letra J para la derecha, y finalmente para que el carro pare se envia la letra I o el default de la funcion. 


```

  switch (dato) {
    case 'G':
      Adelante();
      break;
    case 'H':
      Izquierda();
      break;
    case 'J':
      Derecha();
      break;
    case 'I':
      Alto();
      break;
    case 'K':
      Reversa();
      break;
    default:
      Alto();
      break;
  }

```

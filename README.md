# Practica 2 - Capa 2
Alumno: Hernández Islas Leonardo Daniel

Numero de cuenta: 321279808

Para la practica se implemento la clase emisor y receptor, el emisor arma la trama y manda el mensaje y el receptor recibe ese mensaje y lo muestra en pantalla

## Como compilar el programa
En la raíz del proyecto usamos

`make`

para compilar ambos archivos (receptor.c y emisor.c) para despues con ayuda de docker creamos la imagen, la red y con dos terminales mandamos y recibimos el mensaje asi

### 1. Construimos la imagen del Docker
`docker build -t capa2 .`

### 2. Creamos la red virtual 
`docker network create practica2redes`

### 3. Receptor
`docker run -it --rm --name receptor --network redes2027 \
  --cap-add=NET_RAW capa2 ./receptor`

### 4. Emisor (desde otra terminal)
`docker run -it --rm --name emisor --network redes2027 \
  --cap-add=NET_RAW capa2 ./emisor`

### Procedemos a enviar un mensaje y se puede observar desde la primera terminal

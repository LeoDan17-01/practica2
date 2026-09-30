# Construir la imagen
docker build -t capa2 .

# Crear red virtual aislada
docker network create practica2redes

# Terminal 1: Receptor
docker run -it --rm --name receptor --network practica2redes \
  --cap-add=NET_RAW capa2 ./receptor

# Terminal 2: Emisor
docker run -it --rm --name emisor --network practica2redes \
  --cap-add=NET_RAW capa2 ./emisor
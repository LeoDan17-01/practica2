FROM debian:stable-slim

# Instalar dependencias necesarias
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libc6-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copiar el proyecto y compilar
COPY . /app
RUN make

# Por defecto, mostrar ayuda
CMD ["echo", "Usa ./emisor o ./receptor"]
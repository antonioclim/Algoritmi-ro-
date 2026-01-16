# =============================================================================
# SĂPTĂMÂNA 19: ALGORITMI PENTRU IOT ȘI STREAM PROCESSING
# Dockerfile pentru mediu de dezvoltare standalone
# =============================================================================

FROM ubuntu:22.04

LABEL maintainer="ATP Course - ASE București"
LABEL description="Mediu de dezvoltare pentru Săptămâna 19: IoT și Stream Processing"
LABEL version="1.0"

# Evităm prompturile interactive în timpul instalării
ENV DEBIAN_FRONTEND=noninteractive

# Setăm timezone-ul
ENV TZ=Europe/Bucharest

# Actualizăm și instalăm dependențele
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    gdb \
    valgrind \
    git \
    vim \
    nano \
    less \
    tree \
    htop \
    curl \
    wget \
    python3 \
    python3-pip \
    man-db \
    && rm -rf /var/lib/apt/lists/*

# Instalăm pachete Python pentru comparații (opțional)
RUN pip3 install --no-cache-dir \
    numpy \
    pandas \
    scipy

# Creăm directorul de lucru
WORKDIR /workspace

# Copiem fișierele proiectului
COPY . /workspace/

# Setăm permisiunile corecte
RUN chmod -R 755 /workspace

# Compilăm proiectul
RUN make all || true

# Setăm variabilele de mediu pentru compilare
ENV CC=gcc
ENV CFLAGS="-Wall -Wextra -std=c11 -g -O2 -pedantic"

# Comandă implicită - bash interactiv
CMD ["/bin/bash"]

# =============================================================================
# Instrucțiuni de utilizare:
#
# Construire imagine:
#   docker build -t atp-week19 .
#
# Rulare container interactiv:
#   docker run -it --rm -v $(pwd):/workspace atp-week19
#
# Rulare comandă specifică:
#   docker run --rm atp-week19 make run
#
# =============================================================================

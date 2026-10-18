# Budowanie z katalogu zad01 (potrzebny jest ../Cpp/astar.hpp):
#   docker compose up --build        (z katalogu web/)

# 1) C++ -> WebAssembly
FROM emscripten/emsdk:3.1.74 AS wasm
WORKDIR /src
COPY Cpp/astar.hpp Cpp/astar.hpp
COPY web/wasm web/wasm
WORKDIR /src/web
RUN mkdir -p src/wasm && em++ wasm/bindings.cpp -std=c++17 -O2 -lembind \
    -sMODULARIZE=1 -sEXPORT_ES6=1 -sSINGLE_FILE=1 -sENVIRONMENT=web -o src/wasm/astar.js

# 2) React -> statyczne pliki
FROM node:22-alpine AS build
WORKDIR /app
COPY web/package.json web/package-lock.json ./
RUN npm ci
COPY web/ .
COPY --from=wasm /src/web/src/wasm/astar.js src/wasm/astar.js
RUN npm run build

# 3) Serwowanie przez nginx
FROM nginx:1.27-alpine
COPY --from=build /app/dist /usr/share/nginx/html
EXPOSE 80

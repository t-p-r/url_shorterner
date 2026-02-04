FROM alpine:latest

WORKDIR /app

RUN apk update
RUN apk add --no-cache build-base cmake fmt-dev spdlog-dev

COPY ./src ./src
COPY ./lib ./lib

COPY ./CMakeLists.txt ./

RUN cmake .
RUN cmake --build .

ENTRYPOINT ./bin/url_shorterner

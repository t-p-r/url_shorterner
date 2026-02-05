FROM alpine:latest AS build-stage

WORKDIR /app

RUN apk update
RUN apk add --no-cache build-base cmake fmt-dev spdlog-dev

COPY ./src ./src
COPY ./lib ./lib

COPY ./CMakeLists.txt ./

RUN cmake .
RUN cmake --build .

FROM alpine:latest as run-stage

RUN apk update
RUN apk add --no-cache libstdc++ fmt spdlog

COPY --from=build-stage /app/bin/url_shorterner .

ENTRYPOINT ["./url_shorterner"]

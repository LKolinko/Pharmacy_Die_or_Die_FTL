FROM alpine:latest as build

WORKDIR /app/build

RUN apk add g++ make cmake openssl-dev zlib-dev brotli-dev jsoncpp-dev

ADD . /app/build

WORKDIR /app/build

RUN cmake .
RUN cmake --build ./

FROM alpine:latest

WORKDIR /app

RUN apk --no-cache add brotli libstdc++ jsoncpp-dev
COPY --from=build /app/build/GenModul /bin/GenModul

ENTRYPOINT ["/bin/GenModul"]
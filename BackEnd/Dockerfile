FROM alpine:latest as build

WORKDIR /app/build

RUN echo >> /etc/apk/repositories http://dl-cdn.alpinelinux.org/alpine/edge/testing
RUN apk update
RUN apk add g++ make cmake openssl-dev zlib-dev brotli-dev jsoncpp-dev mongo-cxx-driver-dev libbsoncxx

ADD . /app/build

WORKDIR /app/build

RUN cmake .
RUN cmake --build .

FROM alpine:latest

WORKDIR /app

RUN echo >> /etc/apk/repositories http://dl-cdn.alpinelinux.org/alpine/edge/testing
RUN apk --no-cache add brotli libstdc++ jsoncpp-dev mongo-cxx-driver-dev libbsoncxx
COPY --from=build /app/build/BackEnd /bin/BackEnd

ENTRYPOINT ["/bin/BackEnd"]
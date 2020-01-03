FROM alpine:edge
WORKDIR /app
COPY . .
RUN apk update \
    && apk upgrade \
    && apk add --update  --update-cache --repository http://dl-3.alpinelinux.org/alpine/edge/testing/ \
        --virtual .build-deps \
        alpine-sdk \
        cmake \
        pkgconf \
        doxygen \
        build-base \
        openssl-dev \
        python3 \
        rabbitmq-c \
        rabbitmq-c-dev \
        libbson \
        libbson-dev \
        libbsoncxx \
        libbsoncxx-dev \
        snappy-dev \
        boost-dev \
        libgcc \
        libstdc++ \
        musl \
        fmt-dev \
        mongo-c-driver \
        mongo-c-driver-dev \
        mongo-cxx-driver \
        mongo-cxx-driver-dev \
    && mkdir lib/SimpleAmqpClient/build  \
    && cd lib/SimpleAmqpClient/build/ \
    && cmake .. \
    && make install \
    && mkdir /app/lib/spdlog/build  \
    && cd /app/lib/spdlog/build \
    && cmake .. \
    && make install \
    && cd /app \
    && cmake . \
    && cat ./CMakeCache.txt \
    && make
ENTRYPOINT ["/app/pipelineelement"]

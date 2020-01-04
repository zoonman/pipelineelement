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
    && cmake -DBUILD_GTEST=OFF .. \
    && make install \
    && mkdir /app/lib/spdlog/build  \
    && cd /app/lib/spdlog/build \
    && cmake  -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_BUILD_TESTS_HO=OFF -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_EXAMPLE=OFF .. \
    && make install \
    && cd /app \
    && cmake . \
    && make
ENTRYPOINT ["/app/pipelineelement"]

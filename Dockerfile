# syntax=docker/dockerfile:1

# ============================================================
# Stage 0: libcurl-impersonate assets (musl release)
# ============================================================
FROM alpine:3.20 AS gcurl-lib

ARG CURL_IMPERSONATE_VERSION=v2.0.0
RUN apk add --no-cache curl tar ca-certificates

ARG TARGETARCH
RUN set -eux; \
    case "${TARGETARCH:-amd64}" in \
        amd64) ARCH="x86_64" ;; \
        arm64) ARCH="aarch64" ;; \
        *) ARCH="x86_64" ;; \
    esac; \
    mkdir -p /opt/gcurl/bin /opt/gcurl/lib /opt/gcurl/include; \
    TMP_DIR="$(mktemp -d)"; \
    curl -fL "https://github.com/lexiforest/curl-impersonate/releases/download/${CURL_IMPERSONATE_VERSION}/libcurl-impersonate-${CURL_IMPERSONATE_VERSION}.${ARCH}-linux-musl.tar.gz" \
        | tar -zx -C "${TMP_DIR}"; \
    cp -a "${TMP_DIR}/include/"* /opt/gcurl/include/; \
    cp -a "${TMP_DIR}"/libcurl-impersonate* /opt/gcurl/lib/; \
    chmod 755 /opt/gcurl/lib/libcurl-impersonate.so*; \
    curl -fL "https://github.com/lexiforest/curl-impersonate/releases/download/${CURL_IMPERSONATE_VERSION}/curl-impersonate-${CURL_IMPERSONATE_VERSION}.${ARCH}-linux-musl.tar.gz" \
        | tar -zx -C /opt/gcurl/bin; \
    chmod 755 /opt/gcurl/bin/*; \
    rm -rf "${TMP_DIR}"

# ============================================================
# Stage 1: Base image
# ============================================================
FROM php:8.4-fpm-alpine AS base

# ============================================================
# Stage 2: Build extensions
# ============================================================
FROM base AS build

# install system deps
RUN apk add --no-cache \
    bash git curl unzip \
    curl-dev icu-dev libzip-dev \
    oniguruma-dev \
    freetype-dev libjpeg-turbo-dev libpng-dev \
    libxml2-dev \
    postgresql-dev \
    bzip2-dev \
    gettext-dev \
    linux-headers \
    sqlite-dev \
    $PHPIZE_DEPS

# configure gd
RUN docker-php-ext-configure gd \
    --with-freetype \
    --with-jpeg

# install core extensions
RUN docker-php-ext-install -j$(nproc) \
    bcmath \
    bz2 \
    curl \
    exif \
    ftp \
    fileinfo \
    gettext \
    gd \
    intl \
    mbstring \
    mysqli \
    pdo \
    pdo_mysql \
    pdo_sqlite \
    soap \
    sockets \
    zip

# Copy libcurl-impersonate files from gcurl-lib stage into /usr/local
COPY --from=gcurl-lib /opt/gcurl/include/ /usr/local/include/
COPY --from=gcurl-lib /opt/gcurl/lib/ /usr/local/lib/

# Compile & install gcurl extension
COPY ext /tmp/gcurl-ext
WORKDIR /tmp/gcurl-ext
RUN phpize && \
    ./configure --with-gcurl=/usr/local && \
    make -j$(nproc) && \
    make install && \
    docker-php-ext-enable gcurl && \
    rm -rf /tmp/gcurl-ext

# ============================================================
# Stage 3: Final Image (Lean Production)
# ============================================================
FROM base

# runtime deps
RUN apk add --no-cache \
    icu \
    libzip \
    freetype \
    libjpeg-turbo \
    libpng \
    libxml2 \
    postgresql-libs \
    libbz2 \
    gettext \
    python3 \
    libreoffice \
    fontconfig \
    ttf-dejavu \
    ca-certificates

# Copy libcurl-impersonate shared libraries and CLI binary
COPY --from=gcurl-lib /opt/gcurl/lib/libcurl-impersonate.so* /usr/local/lib/
COPY --from=gcurl-lib /opt/gcurl/bin/ /usr/local/bin/

# Preload libcurl-impersonate so all PHP curl operations use impersonate engine
ENV LD_PRELOAD=/usr/local/lib/libcurl-impersonate.so

# Copy compiled PHP extensions and configs from build stage
COPY --from=build /usr/local/lib/php/extensions /usr/local/lib/php/extensions
COPY --from=build /usr/local/etc/php/conf.d /usr/local/etc/php/conf.d

# composer
COPY --from=composer:2 /usr/bin/composer /usr/bin/composer

# config
COPY php.ini /usr/local/etc/php/php.ini
COPY www.conf /usr/local/etc/php-fpm.d/www.conf

WORKDIR /var/www/html
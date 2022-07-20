ARG DOCKER_VULKAN_VER="1.3-470"
FROM nvidia/vulkan:${DOCKER_VULKAN_VER}

ARG APP_NAME="liblava-demo"
ARG APP_VER="0.1"

LABEL \
    maintainer="Lava Block" \
    org.label-schema.name="${APP_NAME}" \
    org.label-schema.version="${APP_VER}" \
    org.label-schema.schema-version="2.0" \
    org.label-schema.vendor="Lava Block" \
    org.label-schema.url="https://lava-block.com/" \
    org.label-schema.vcs-url="https://github.com/liblava/liblava"

COPY [".", "/usr/src/${APP_NAME}"]
WORKDIR "/usr/src/${APP_NAME}"

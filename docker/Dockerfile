FROM debian:bookworm

# 设置时区与非交互模式以防卡死
ENV DEBIAN_FRONTEND=noninteractive

# 安装构建和调试所需工具
RUN apt update && apt install -y \
    git \
    cmake \
    clang \
    check \
    gdb \
    curl \
    tree \
    build-essential \
    pkg-config \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# 创建工作目录并进入
WORKDIR /app

# 默认进入 shell，支持挂载 + 编译 + 调试
# CMD ["/bin/bash"]
CMD ["./app/build/minikv", "2"]

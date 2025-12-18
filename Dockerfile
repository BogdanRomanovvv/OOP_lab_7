# Используем официальный образ GCC с поддержкой C++17
FROM gcc:13 AS builder

# Устанавливаем CMake и необходимые зависимости
RUN apt-get update && apt-get install -y \
    cmake \
    make \
    git \
    && rm -rf /var/lib/apt/lists/*

# Создаём рабочую директорию
WORKDIR /app

# Копируем исходный код
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/
COPY CMakeLists.txt .

# Создаём директорию для сборки
RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build .

# Финальный образ
FROM gcc:13-slim

# Устанавливаем только необходимые runtime библиотеки
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Создаём рабочую директорию
WORKDIR /app

# Копируем собранные бинарники из builder
COPY --from=builder /app/build/dungeon_editor ./dungeon_editor
COPY --from=builder /app/build/dungeon_async ./dungeon_async
COPY --from=builder /app/build/dungeon_tests ./dungeon_tests

# Копируем тестовые данные
COPY tests/test_data.txt ./tests/test_data.txt

# Устанавливаем права на выполнение
RUN chmod +x dungeon_editor dungeon_async dungeon_tests

# По умолчанию запускаем асинхронную версию (Лаб 7)
CMD ["./dungeon_async"]

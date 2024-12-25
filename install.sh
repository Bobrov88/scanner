#!/bin/bash

# Проверка существования архива
if [ ! -f libs.tar.gz ]; then
    echo "Архив libs.tar.gz не найден."
    exit 1
fi

# Распаковка архива в папку libs
mkdir -p libs
tar -xzf libs.tar.gz -C libs

# Удаление архива
rm libs.tar.gz

# Поиск исполняемого файла scanner***
scanner_file=$(ls scanner*)
if [ -z "$scanner_file" ]; then
    echo "Исполняемый файл scanner* не найден."
    exit 1
fi

# Установка переменной окружения LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/libs:$LD_LIBRARY_PATH

chmod -R 777 scanner_*
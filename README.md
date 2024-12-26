# Проект SCANNER NEO FOX SD Utility

## 1. Введение
* **Название проекта:** NEO FOX SD Utility
* **Авторские права:** ИП Бобров Д.А.
* **Описание и цель проекта:** Проект NEO FOX SD Utility (далее - Проект) разработан на языке С++ и продолжает совершенствоваться по сей день в связи с требованием клиентов, изменением действующего законодательства, появлением идей по улучшению, обновлением прошивки со стороны разработчика оборудования, а также исправлением ошибок при наличии таковых.
Проект представляет из себя консольное приложение согласно техническому заданию Заказчика, запускается с определёнными параметрами, описанными ниже и предназначено для настройки ручных проводных сканеров NEO FOX SD.
* **Зависимости:** 
    - Boost
    - hidapi
    - utf8cpp
    - cppconsoletable
    - fwdownload (предоставлено разработчиков оборудования)
* **Инструменты для сборки:**
    - Conan
    - CMake
* **Компиляторы:**
    - GCC 9.4.0
    - GCC 12
    - MSVC 2019
* **Поддерживаемые платформы:**
    - Debian 12 x64
    - Debian 11 x86
    - Debian 11 i386
    - Centos7 i386
    - Windows 10 x64
* **Язык интерфейса и логов**
    - Русский
    - Английский

## 2. Структура каталогов
```
scanner
├── CMakeLists.txt
├── Firmwares
├── centos7-x86.Dockerfile
├── conan
├── conan_profiles
├── conanfile.txt
├── debian11-i386.Dockerfile
├── debian11-x64.Dockerfile
├── debian12-x64.Dockerfile
├── private.pem
└── src
```
- **/CMakeLists.txt** - Основной файл с инструкцией по сборке консольного приложения
- **/Firmwares** - прошивки от производителя оборудования
- **/*.Dockerfile** - файлы со сценариями сборки контейнеров для последующей сборки консольного приложения в естественной среде запуска
- **/conan** - настройки пакетного менеджера conan
- **/conan_profiles** - профили для сценариев сборки
- **/private.pem** - SSH-ключ, используемый conan
- **/src** - исходные файлы проекта

## 3. Варианты сборки
Прежде всего, сборка из-под Linux для любой из вышеперечисленных архитектур требует наличия [docker](https://www.docker.com/products/docker-desktop/) на компьютер. Для сборки из-под Windows же - дополнительно установить пакетный менеджер [conan](https://github.com/conan-io/conan/tree/release/1.66). 
Ниже доступные варианты сборки:

### Windows10 x64
```
conan install -pr .\conan_profiles\win10\win10_build -if build_win_x86_64 . -s build_type=Debug -s compiler="Visual Studio" -s compiler.runtime=MDd --build=missing
cmake -Bbuild_win_x86_64 . -DTARGET_SYSTEM=WINDOWS -D_DEBUG=TRUE -DLANG=ru_RU -G "Visual Studio 16" -A x64
cmake --build ./build_win_x86_64
```

### Centos7 x32 (i386)
```
docker build -t scanner -f ./centos7-x86.Dockerfile .
docker run --rm -it --volume=./:/home/project/scanner scanner
conan install -pr linux_x86_build -if build_centos_x86 /home/project/scanner/ --build=missing
cmake -Bbuild_centos_x86 . -DTARGET_SYSTEM=CENTOS -D_DEBUG=TRUE -DLANG=ru_RU
cmake --build ./build_centos_x86
```

### Debian12 x64
```
docker build -t scanner_x64 -f ./debian12-x64.Dockerfile .
docker run --rm -it --volume=./:/home/project/scanner scanner_x64
conan install -pr linux_x86_64_build -if build_debian_x86_64 /home/project/scanner/ --build=missing 
cmake -Bbuild_debian_x86_64 . -DTARGET_SYSTEM=DEBIAN -D_DEBUG=FALSE -DLANG=en_US
cmake --build ./build_debian_x86_64
```

### Debian11 x64
```
docker build -t scanner_debian_11_x64 -f ./debian11-x64.Dockerfile .
docker run --rm -it --volume=./:/home/project/scanner scanner_debian_11_x64
conan install -pr linux_x86_64_build -if build_debian11_x86_64 /home/project/scanner/ --build=missing 
cmake -Bbuild_debian11_x86_64 . -DTARGET_SYSTEM=DEBIAN -D_DEBUG=FALSE -DLANG=ru_RU -Dstatic-libsystemd=true -Dstatic-libudev=true
cmake --build ./build_debian11_x86_64
```
### Debian 11 i386
```
docker build -t scanner_debian11_i386 -f ./debian11-i386.Dockerfile .
docker run --rm -it --volume=./:/home/project/scanner scanner_debian11_i386
conan install -pr linux_x86_build -if build_debian11_x86 /home/project/scanner/ --build=missing 
cmake -Bbuild_debian11_x86 . -DTARGET_SYSTEM=DEBIANi386 -D_DEBUG=FALSE -DLANG=en_US -Dstatic-libsystemd=true -Dstatic-libudev=true
cmake --build ./build_debian11_x86
```
Параметр _DEBUG может принимать значение TRUE / FALSE
Параметр LANG может принимать значение en_US / ru_RU

## 4. Артефакты сборки
Результатом сборки является исполняемый файл с именем scanner_[LANG] в папке build*/bin/

## 5. Запуск приложения

Приложение необходимо запускать из консоли с параметрами.
При запуске без параметров в консоли появляется меню с описанием.

scanner.exe [-параметр] (для Windows)
./scanner [-параметр] (для Debian и Centos, для Centos7 i386)

Краткое описание основных функций:
1. При запуске без параметров или с параметром -h (--help) - основное меню
2. -i (--info) - получение списка доступных сканеров, переведённых в HID-режим
3. -a (--all_info) - получение информации обо всех версиях устройства (прошивка, загрузчик и т.д.)
3. -s (--save) - сохранение в json-файл параметров из внутренней памяти выбранных сканеров
                 в текущую директорию с именем [серийный номер].json
4. -w (--write) - запись параметров из json-файла во внутреннюю память выбранных сканеров  
                  в качестве пользовательской настройки. Требуется наличие файла json в текущей директории
    Примечание: Описание и доступные значения прилагаются в файле json.xlsx. Сам json-файл
                можно получить из пункта 3
5. -f (--restore-factory) - откат выбранных сканеров к заводским настройкам
6. -c (--restore-custom) - откат выбранных сканеров к пользовательским настройкам
7. -d (--download) - процедура прошивки выбранных сканеров
    Примечание: файлы прошивки с именем QRR*.sig прилагаются


## 6. Решение проблем при запуске
Запускать всегда от имени администратора / с root-правами

При запуске на Centos7 i386 - распаковать архив Centos7install.tar.gz и запустить ./install.sh

При запуске на Windows10 x64 - наличие в текущей директории libfwdownload.dll (взять из папки src/win10)

## 7. Логгирование
Приложение производит записи в логгер в файл scanner_utility.log в текущей директории.
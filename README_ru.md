Данное приложение предназначено для настройки сканеров NEO FOX SD.
Приложение направляется для предварительного тестирования и замечаний/предложений.
Поддерживаемые платформы: Debian12 x64, Centos7 x32
Приложение пока на английском языке, русский язык будет добавлен.

При запуске приложения выходим самодокументируемое меню. Приложение запускается из консоли командой ./scanner

Краткое описание основных функций:
1. При запуске без параметров или с параметором -h (--help) - основное меню
2. -i (--info) - получение списка доступных сканеров, переведённых в HID-режим
3. -s (--save) - сохранение в json-файл параметров из внутренней памяти выбранных сканеров
                 в текущую директорую с именем [серийный номер].json
4. -w (--write) - запись параметров из json-файла во внутренную память выбранных сканеров  
                  в качестве пользовательской настройки. Требуется наличие файла json в текущей директории
    Примечание: Описание и доступные значения прилагаются в файле json.xlsx. Сам json-файл
                можно получить из пункта 3
5. -f (--restore-factory) - откат выбранных сканеров к заводским настройкам
6. -c (--restore-custom) - откат выбранных сканеров к пользовательским настройкам
7. -d (--download) - процедура прошивки выбранных сканеров
    Примечание: файлы прошивки с именем QRR*.sig прилагаются
#pragma once
#define EXCEPTION "Исключение"s
#define UNKNOWN "Неизвестный параметр"s
#define USAGE "Параметры меню"s
#define GET_DESC "\t -h --help           \t\tОписание доступных функций"s
#define GET_INFO "\t -i --info           \t\tИнформация о USB-устройствах"s
#define GET_ALL_INFO "\t -a --all_info   \t\tПолучить полную информацию о версии ПО"s
#define GET_SAVE "\t -s --save           \t\tСохранение настроек в json-файл"s
#define GET_WRITE "\t -w --write          \t\tЗапись настроек из json-файла"s
#define GET_REST_FACT "\t -f --restore-factory\t\tВосстановление заводских настроек"s
#define GET_REST_CUST "\t -c --restore-custom \t\tВосстановление пользовательских настроек"s
#define GET_DWNLD "\t -d --download       \t\tПрошивка сканера"s
#define COM2HID "Примечание:         Сканеры в COM-режиме (если таковые имеются) \n                    будут автоматически переведены в HID-режим"s
#define WAIT4SCAN "Поиск доступных сканеров среди USB-устройств..."s
#define PASS_FAIL "перевод невозможен"s
#define SAVE_OK "Настройки сканера были успешно сохранены в json-файл"s
#define SAVE_FAIL "Сохранены не все настройки!"s
#define SAVING_IN_PROCESS "Идёт запись в файл..."s
#define WRITE_FAIL "Сохранение не удалось"s
#define WRITE_CUS_FAIL "Сохранение настроек как пользовательских не удалось"s
#define _SUCCESS_ "Успех"s
#define _FAIL_ "Провал"s
#define REST_OK "возврат к заводским настройкам прошёл успешно"s
#define REST_FAIL "возврат к заводским настройкам не удался"s
#define CUST_OK "возврат к пользовательским настройкам прошёл успешно"s
#define CUST_FAIL "возврат к пользовательским настройкам не удался"s
#define FW_CORRUPT "Файл прошивки повреждён. Обновление невозможно!"s
#define HID2COM_FAIL "переход в COM-режим не удался!"s
#define CONN_ABORT "соединение разорвано"s
#define FWDL "обновление прошивки"s
#define HAS_FW "уже содержит текущую прошивку"s
#define FW_OK "обновление прошивки УСПЕШНО"s
#define PROD "Наименование"s
#define SN "Серийный номер"s
#define MODEL "Модель"s
#define FW_VER "Версия прошивки"s
#define HW_VER "Версия оборудования"s
#define BOOT_VER "Версия загрузчика"s
#define LIB_VER "Версия библиотеки"s
#define NO_SCAN "Сканеры не найдены"s
#define COM "COM"s
#define JSON_NAME "Имя json-файла"s
#define ST "Статус"s
#define NO_JSON "Json-файлы не найдены"s
#define FW_NAME "Файл прошивки"s
#define OK "ОК"s
#define ERR "Ошибка"s
#define NO_FW "Файлы прошивки не найдены"s
#define CHOOSE_SCANNER "Введите номера через пробел, напр.: 1 4 5 \nлибо введите VID через пробел, напр.: 0x34eb\nлибо введите 0 для выбора всех сканеров"s
#define CHOOSE "Введите номер файла"s
#define INCORR_STRING "некорректное значение, строка должна быть не более"s
#define SYM "символов"s
#define INCORR_NUM "значение должно быть от 0 до 255"s
#define V_IN "в"s
#define INCORR_BYTES "Некорректные байты"s
#define INCORR_DATA "Некорректные данные"s
#define POSS_VALUES "Допустимые значения для"s
#define PARSE_FAIL "Ошибка парсинга"s
#define NEW_FILE "Файл создан"s
#define NO_NEW_FILE "невозможно создать файл"s
#define NOT_KEY "Ключ не найден"s
#define READ_ERROR "Ошибка чтения данных. Переподключите устройство и попробуйте снова"s
#define FLAGS_FILLED_ERR "Пожалуйста, проверьте чтоб ключи имели допустимые названия и значения"s
#define LOOK_TO_LOG "Для получения детальной информации обратитесь к логам"s
#define FW_DOWNLOAD_ATTENTION "Внимание!\nВо время процедуры обновления ПО\nкрайне не рекомендуется выключать компьютер,\nа также оказывать прочее воздействие\nна подключённое оборудование!"s
#define SAVING_TO_INTERNAL_FLASH "Сохранение во внутреннюю память сканера"
#define RESTORE_CUSTOM_SETTINGS "Восстановление пользовательских настроек"
#define RESTORE_FACTORY_SETTINGS "Восстановление заводских настроек"
#define ERASING_DUPLICATES "Попытка соединения со сканером с учётом его двойного определения системой"
#define SWITCH_COM_2_HID "Переключение из режима COM в HID-POS "
#define SWITCH_HID_2_COM "Переключение из режима HID-POS в COM"
#define PRINT_DEVICES "Печать доступных устройств"
#define PRINT_SOFTWARE "Печать версии ПО"
#define SAVE_SETTINGS "Сохранение настроек в файл json"
#define SAVE_FROM "Сохранение настроек из "
#define WRITE_FROM_JSON "Запись настроек из json файла"
#define HARD_RESET "Сброк устройства!"
#define SOFT_RESET "Откат настроек"
#define DOWNLOAD_FIRMWARE "Загрузка новой прошивки"
#define PORT_RECONNECT "Происходит переподключение к порту: "
#define LAST_STATE "Последний статус загрузки: "
#define SUCC_READ "Успешно прочитано с "
#define ATT " попытки"
#define READ_FAILED ": ошибка чтения -> "
#define FOUND "Найдено устройств: "
#define CONNECT "Соединение: "
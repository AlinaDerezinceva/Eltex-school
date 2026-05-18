# Отчёт: символьное устройство `chardev`

# Сборка

make

Модуль chardev.ko собран успешно для ядра 6.8.0-111-generic 

Сборка make:
(Screenshots/Build.png)

# Метаданные модуля

modinfo ./chardev.ko


В выводе: имя модуля, лицензия GPL, автор, версия ядра

Вывод modinfo:
(Screenshots/Modinfo.png)

# Загрузка и проверка в ядре

sudo insmod chardev.ko
lsmod | grep '^chardev'

Модуль chardev отображается в списке загруженных.

insmod и lsmod:
(Screenshots/Load_verify.png)

# Журнал после загрузки

sudo dmesg | tail -n 10


В логе видно назначенный major и сообщение о создании устройства /dev/chardev.

dmesg: major и создание устройства:
(Screenshots/Dmesg_init.png)

# Узел /dev/chardev

ls -l /dev/chardev


Тип c (символьное устройство), major/minor совпадают с сообщением в dmesg (510, 0), права по умолчанию у root.

ls -l /dev/chardev:
(Screenshots/Dev_node.png)

# Чтение из устройства

sudo cat /dev/chardev


Вывод соответствует логике device_open / device_read: строка вида «I already told you 0 times Hello world!»

cat /dev/chardev:
(Screenshots/Cat_read.png)

# Эксклюзивное открытие

Пока в одном терминале удерживается дескриптор (exec 3<>/dev/chardev и пауза), во втором sudo cat /dev/chardev завершается ошибкой «Устройство или ресурс занято» — ожидаемое поведение при занятом устройстве.

Два терминала: удержание open и EBUSY:
(Screenshots/Exclusive_busy.png)

# Запись не поддерживается

sudo sh -c 'echo test > /dev/chardev'
sudo dmesg | tail -n 3

В оболочке — ошибка ввода-вывода; в dmesg — сообщение модуля «Sorry, this operation is not supported.»

Попытка записи и dmesg:
(Screenshots/Write_notsupported.png)

# Выгрузка

После закрытия всех открытых файлов /dev/chardev:

sudo rmmod chardev

Иначе rmmod может сообщить, что устройство или модуль заняты.

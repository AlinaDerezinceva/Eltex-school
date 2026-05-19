# Отчёт: обмен сообщениями с ядром через Netlink

# Сборка

Сборка выполняется из каталога модуля командой make: компилируется программа user и модуль ядра netlink.ko против дерева заголовков текущего ядра (`/lib/modules/$(uname -r)/build`).

Результат сборки make:
(Screenshots/Build.png)

# Метаданные модуля

modinfo netlink.ko


В выводе указаны путь к файлу модуля, лицензия GPL, поле vermagic для ядра 6.8.0-111-generic

Вывод modinfo:
(Screenshots/Modinfo.png)

# Загрузка модуля и проверка

cd ~/work/trash/Eltex/module5/netlink
sudo insmod netlink.ko
lsmod | grep netlink


insmod и lsmod:
(Screenshots/Load_verify.png)

# Журнал ядра после загрузки

sudo dmesg | tail -1

После успешной регистрации netlink-сокета в функции инициализации модуля в кольцевой буфер попадает сообщение Entering: hello_init.

Последняя строка dmesg после insmod:
(Screenshots/Dmesg_init.png)

# Обмен с пользовательским процессом

./user
sudo dmesg | tail -10

Программа user отправляет в ядро строку Hello, ожидает ответ и выводит полученный payload. В dmesg видно вызов обработчика hello_nl_recv_msg и строку о принятом payload (Netlink received msg payload: Hello), что подтверждает прохождение сообщения через netlink до кода модуля.

На скриншоте совмещены вывод ./user и хвост dmesg.

Запуск user и фрагмент dmesg:
(Screenshots/Exchange_verify.png)

# Выгрузка модуля

sudo rmmod netlink
lsmod | grep netlink

rmmod и проверка lsmod:
(Screenshots/Unload_verify.png)

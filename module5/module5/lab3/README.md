# Отчёт: модуль управления индикаторами клавиатуры 

# Сборка

Сборка выполняется командой make; на выходе получается kled.ko.

Сборка модуля:
(Screenshots/Build.png)

# Загрузка и проверка в списке модулей

sudo insmod kled.ko
lsmod | grep '^kled'


На скриншоте модуль kled присутствует в выводе lsmod.
(Screenshots/Load_verify.png)

# Журнал ядра при инициализации

После insmod в dmesg видны сообщения kbleds: loading, номер передней консоли, строки обхода консолей с префиксом poet_atkm и строка kbleds: finished scanning consoles.

Инициализация и обход консолей в dmesg:
(Screenshots/Dmesg_init.png)

# Выгрузка и проверка

sudo rmmod kled
lsmod | grep '^kled'
sudo dmesg | tail -n 10


После rmmod фильтр по kled не выдаёт строк — модуль выгружен. В конце журнала появляется kbleds: unloading..., что соответствует функции очистки модуля.

(Screenshots/Unload_verify.png)

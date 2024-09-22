# Автоподъём сервера QuakeHost
Небольшая программа (**исполняемый файл**), которая позволит Вам автоматически перезапускать Ваш игровой сервер, который Вы хостите на хостинге `quakehost`.<br><br>
Несмотря на то, что `quakehost` предоставляет возможность в настройках панели управления сервером указать `автоматический перезапуск игрового сервера во время его падения` - к сожалению, это не работает должным образом и после краша сервера ничего не происходит. Пример:<br><br>
![Screenshot_1](https://github.com/kekekekkek/QuakeHost_ServerRestarter/blob/main/Images/Screenshot_1.png)<br><br>
Посмотрев форумы и поискав в них соответствующую информацию по этому поводу, я пришёл к выводу, что лучшим вариантом будет сделать отдельный экстернальный файл, который будет отправлять запрос к серверу через протокол `Source Query` для проверки его работоспособности. В случае, если сервер не отвечает - авторизуемся в админской панели `quakehost`, парсим необходимые поля для их последующей передачи в другом запросе (**запросе на перезапуск сервера**) и отправляем этот самый запрос с полученными данными для того, чтобы `запустить/перезапустить` наш игровой сервер.<br><br>
Задав определённый интервал запросов к игровому серверу, мы можем мониторить его работу и в случаях его падения - автоматически его перезапускать.<br><br>
Скачать программу можно по этой [ссылке]().<br>

# Использование
Для того, чтобы правильно использовать данную программу, Вам необходимо следовать следующим шагам:
1. Для начала, у Вас должны быть установлены последние версии библиотек `msvcp` (**Microsoft Visual C++**). Скачать библиотеки можно с официального сайта [Microsoft](https://learn.microsoft.com/ru-ru/cpp/windows/latest-supported-vc-redist?view=msvc-170); 
2. Если у Вас установлены данные библиотеки, Вы можете без проблем запускать программу;
3. После запуска программы у Вас на экране появится консольное окно со следующим содержимым:<br><br>
![Screenshot_2](https://github.com/kekekekkek/QuakeHost_ServerRestarter/blob/main/Images/Screenshot_2.png)<br><br>
4. Здесь программа покажет Вам, какие аргументы необходимо ей передать для проверки сервера и его перезапуска;
5. После правильной передачи аргументов, результат её выполнения проверки должен будет выглядеть примерно так:<br><br>
![Screenshot_3](https://github.com/kekekekkek/QuakeHost_ServerRestarter/blob/main/Images/Screenshot_3.png)<br><br>
6. Теперь, в случаях падения или краша сервера, программа будет выполнять запрос для его проверки и при обнаружении проблем, его перезапускать.

# Аргументы
Пример передачи аргументов программе представлен ниже:
### [+] Формирование аргументов
* `QuakeHost_ServerRestarter %IPv4% %QuakeHostLogin% %QuakeHostPassword% %QueryInterval%` - Аргументы должны быть переданы именно в этом порядке, иначе произойдёт ошибка или запросы будут выполняться некорректно.
### [+] Пример передачи аргументов
* `QuakeHost_ServerRestarter 46.174.55.192 MyLogin1 MyPassword1 15` - Пример для сервера 1;
* `QuakeHost_ServerRestarter 46.177.57.152 MyLogin2 MyPassword2 25` - Пример для сервера 2.<br><br>
Вы можете создать отдельный `.bat/.cmd`-файл и указать в нём нужные Вам аргументы, после чего запускать и наблюдать результат выполнения.<br>
Также, это можно сделать запустив `командную строку`, выбрав необходимый каталог при помощи команды `cd` и запустив программу так, как показано в примерах выше.<br><br>
В случаях, если Ваш `логин` или `пароль` содержат `пробелы`, то при передачи данных аргументов Вам необходимо будет заключить их в кавычки `""`. Пример:<br>
* `QuakeHost_ServerRestarter 46.174.55.192 "My Login" "My Password" 25` - Пример для данных с пробелами.<br><br>
В программе отсутствует мьютекс, поэтому Вы можете запускать несколько её экземпляров для проверки работоспособности нескольких серверов сразу.

# Дополнительно
**ЗАПОМНИТЕ**: В программе отсутствует проверка на правильность написания `IPv4`, поэтому не рекомендуется указывать строки или другие случайные значения.<br>
**ЗАПОМНИТЕ**: В программе отсутствует проверка на указанный интервал ожидания для выполнения запросов, поэтому не рекомендуется ставить значение интервала меньше нуля.<br>
**ЗАПОМНИТЕ**: Также, в случаях обновления или изменения названий `.php`-файлов на сервере `quakehost`, или изменения данных для передачи в запросе, данное решение может сломаться и оказаться нерабочим.<br>

# Компиляция
Для запуска проекта Вам понадобится среда разработки `Visual Studio`, желательно `2019` года и выше.<br>
Для компилияции программы Вам небходимо будет использовать следующую конфигурацию: `Release/x86`.<br>
Также, для успешной компиляции проекта, в его свойствах необходимо будет указать компоновщику статическую библиотеку `WinInet.lib`, так как программа использует функции `WinInet` для отправки запросов в сеть.

## Выключение убунты по кнопке питания
А также привязка своих скриптов к кнопке питания

### Если вы залогинились
Под логином имеется ввиду ввод логина и пароля в графическом интерфейсе.    
В таком случае все просто - убираем запуск диалогового окна, которое вызывается при 
нажатии кнопки питания:   
   
```shell 
~$ gsettings set org.gnome.SessionManager logout-prompt false
```

### Без логина (например сессия по ssh)
Тут уже сложнее. Открываем файл ``/etc/systemd/logind.conf``   
Ищем строку ``#HandlePowerKey=poweroff`` раскомменчиваем ее, если там стоит не poweroff, ставим poweroff  
    
Ищем есть ли в нашей системе файл ``/etc/acpi/powerbtn.sh``
Если он существует, открываем его:   

```shell
~$ sudo gedit /etc/acpi/powerbtn.sh
```

и в начале файла (после заголовочной шапки) пишем 
```shell
/sbin/shutdown -h now "Power Button Pressed"
```

Сохраняем файл и перезапускаем acpid:
```shell
~$ sudo /etc/init.d/acpid restart
``` 

Пробуем выключиться по кнопке.

Если файла ``/etc/acpi/powerbtn.sh`` не существует, то создадим свое событие и его обработчик.
Создаем, допустим, файл ``/etc/acpi/events/power`` и пишем в него следующее   

```shell
event=button/power
action=/etc/acpi/power.sh "%e"
```

Затем создаем файл ``/etc/acpi/power.sh`` и пишем в него:

```shell
/sbin/shutdown -h now "Power button pressed"
```

Сохраняем файл и перезапускаем acpid:
```shell
~$ sudo /etc/init.d/acpid restart
``` 

Пробуем выключиться по кнопке.
## Автозагрузка скриптов в linux

Создаем сервис-файл, например ``autorun.service`` в папке ``/lib/systemd/system/``

```shell
~$ sudo nano /lib/systemd/system/autorun.service
```
  
Файл должен выглядеть [следующим образом](https://github.com/RTC-SCTB/Johny/blob/master/autorun.service)    
В секции **Unit** мы даем краткое описание нашему файлу и говорим с помощью опции **After**, что нужно запускать этот скрипт в
многопользовательском режиме (multi-user). Секция **Service** самая важная, здесь мы указываем тип сервиса, в параметре **ExecStart** указываем
полный путь к нашему скрипту. Подробнее про все тонкости сервис-файлов можно узнать
[тут](https://habr.com/ru/company/southbridge/blog/255845/) и [тут](https://ualinux.com/ru/stream/avtozapusk-prilozhenij-v-linux)

Всем скриптам нужно раздать правильные права:

```shell
~$ sudo chmod 644 /lib/systemd/system/autorun.service
```

```shell
~$ sudo chmod +x <ваш исполняемый скрипт>
```

Затем обновить конфигурацию и добавить в автозагрузку Linux новый скрипт  

```shell
~$ sudo systemctl daemon-reload
~$ sudo systemctl enable autorun.service
```

После перезагрузки скрипт будет запущен автоматически, или его можно сразу запустить вручную   

```shell
~$ sudo systemctl start autorun.service
```

Если по какой-то причине скрипт не запускается можно просто посмотреть причину этого   

```shell
~$ sudo systemctl status autorun.service
```


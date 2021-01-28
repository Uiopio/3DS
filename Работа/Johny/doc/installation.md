## Процесс установки ПО Джонни

### Серверная часть 

##### Устанавливаем gstreamer и все нужные пакеты:   

```shell
~$ sudo apt install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad
gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-omx libatlas-base-dev

~$ sudo apt install python3-gi python3-pip

~$ sudo pip3 install numpy pyserial
```    

python-can устанавливаем не из стандартных репозиториев, а из [форка](https://github.com/ArtemZaZ/python-can) и ветки *with-seedstudio*   

```shell
git clone https://github.com/ArtemZaZ/python-can
git checkout with-seedstudio
python3 setup.py build
sudo python3 setup.py install 
```

Если python-can уже был установлен, то удаляйте его и проделывайте вышеописанную операцию

##### Добавляем автозапуск сервера при включении
Если нужно, редактируем пути до скрипта autorun.sh в файле [autorun.service](https://github.com/RTC-SCTB/Johny/blob/master/autorun.service),
а также указываем рабочую директорию *WorkingDirectory*   
Перетаскиваем файл [autorun.service](https://github.com/RTC-SCTB/Johny/blob/master/autorun.service) в папку */lib/systemd/system/* 
и проделываем примерно то же, что описано [тут](https://github.com/RTC-SCTB/Johny/blob/master/doc/autostart.md)

##### Добавляем выключение бортового ПК при нажатию на кнопку выключения [смотреть тут](https://github.com/RTC-SCTB/Johny/blob/master/doc/powerbtn.md)   

### Пультовая часть

##### Прописываем пути
В файлах [pult.sh](https://github.com/RTC-SCTB/Johny/blob/master/pult.sh) и [Johny.desktop](https://github.com/RTC-SCTB/Johny/blob/master/Johny.desktop),
если нужно, переписываем пути до исполняемых скриптов и файлов проекта
##### Устанавливаем OpenHMD и python OpenHMD [отсюда](https://github.com/ArtemZaZ/python-openhmd)
##### Устанавливаем gstreamer и все нужные пакеты:   

```shell
~$ sudo apt install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad
gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc gstreamer1.0-tools gstreamer1.0-omx libatlas-base-dev

~$ sudo apt install python3-gi python3-pip

~$ sudo pip3 install numpy

~$ sudo apt install mesa-utils libegl1-mesa libgl1-mesa-dev

~$ sudo apt install freeglut3-dev freeglut3
```    
P.S. На 18.04 ubuntu возможно необходимо будет установить GTK3
+ В дополнение, если показывается, что HDMI-1 disconnected(что-то из этого должно помочь):
```shell
~$ sudo apt install arandr tasksel
~$ sudo tasksel install ubuntu-desktop
~$ sudo apt install libgtk-3-dev libva-dev mesa-va-drivers
При включенном шлеме:
~$ xrandr --output HDMI-1 --mode 2160x1200 --auto
~$ sudo service gdm3 start
~$ sudo apt update
~$ sudo apt upgrade
~$ sudo reboot now
```   

##### Редактируем sudoers ([подробнее](https://help.ubuntu.ru/wiki/%D1%81%D1%83%D0%BF%D0%B5%D1%80%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C_%D0%B2_ubuntu))

```shell
~$ sudo visudo 
```

После строки *# Cmnd alias specification* пишем:

```shell
# Cmnd alias specification
Cmnd_Alias SHUTDOWN_CMDS = /home/pult/GitHub/Johny/./pult.sh
```

И в конце файла пишем:

```shell
pult ALL=(ALL) NOPASSWD: SHUTDOWN_CMDS
```

##### Создаем desktop приложение
Если файл [pult.sh](https://github.com/RTC-SCTB/Johny/blob/master/pult.sh) не является исполняемым, нужно сделать его таковым:

```shell
~$ sudo chmod +x pult.sh
```

Перетаскиваем файл [Johny.desktop](https://github.com/RTC-SCTB/Johny/blob/master/Johny.desktop) в папку */usr/share/applications*    
Дальше можно подождать обновить сервис или перезагрузиться, приложение должно появиться в графе поиска, дальше можно перетащить ярлык на рабочий стол   
Подробнее про это все, можно почитать [тут](https://www.maketecheasier.com/create-desktop-file-linux/)    


## P.S. 
Можно проверять промежуточные результаты запуская файлы sudo ./onpult.py и ./onboard.py на пульте и бортовом ПК
соответственно, если при этом система работает нормально, но конечная система система не работает (нет автозапуска и т.д.)
значит, что вы где-то ошиблись с развертыванием проекта, проверьте пути во всех файлах и т.д. Но, если система не работает 
при запуске этих скриптов, значит происходит ошибка в самой программе, необходимо доустановить необходимые библиотеки и настроить
конфигурационные файлы    
    
Конфигурационные файлы для [робота](https://github.com/RTC-SCTB/Johny/blob/master/rise/board/robotconf.json) и [пульта](https://github.com/RTC-SCTB/Johny/blob/master/conf.json)

##### Возможно этот раздел еще будет дополняться

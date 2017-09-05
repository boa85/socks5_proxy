## Сборка и запуск:
### Зависимости для сборки:
* git
* build-essential
* cmake
* libboost-all-dev
* libboost-filesystem версии не ниже 1.58.0
* libboost-program-options версии не ниже 1.58.0
### Пример установки зависимостей для Debian 8.5
      sudo apt install git build-essential cmake  libboost-all-dev libboost-filesystem1.58.0 libboost-program-options1.58.0
### Сборка проекта

* Клонирование из репозитория в локальный каталог

      git clone https://github.com/boa85/socks5_proxy.git
* Переход в каталог с исходным кодом

      cd ${project_source_dir}
* Создание дирректории для работы cmake и переход в неё

      mkdir cmake-build-debug && cd ./cmake-build-debug/
* Запуск cmake для генерации Makefile

      cmake ..
* Сборка проекта 

      make

###  Запуск

* Отобразить справку

      ./soks5-proxy --help
* запустить прокси-сервер

      ./socks5_proxy -p 1080 --http-host 192.168.100.6 --http-port 6060 --socks5-host 192.168.100.6 --socks5-port 1080

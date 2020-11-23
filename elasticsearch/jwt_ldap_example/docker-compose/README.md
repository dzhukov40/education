# Прототип аутентификации и авторизации

Настройка работы open distro плагина для elasticsearch и kibana.
Аутентификации при помощи JWT, а авторизация через LDAP.

## Запуск проекта

`docker-compose up`

`docker-compose down -v`

## Аутентификация по JWT

`http://127.0.0.1:5601?jwtToken=<JWT>`

## Проверка аутентификации и авторизации

`chmod +x ./authorization.sh && ./authorization.sh`

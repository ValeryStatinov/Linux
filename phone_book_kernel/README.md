## Решение первого дз Kernel-mode phone book
---
## Installation
Для запуска необыходимо выполнить:
1. **make** - сборка phone_book_kernel.c и компиляция программ для использования телефонной книги(add, get, del)
2. **sudo insmod phone_book_kernel.ko** - внедрение модуля
3. **dmesg** - проверить номер, с которым прошло внедрение
4. **sudo mknod /dev/phone_book_kernel c [ваш номер из пункта 3] 0** - создание символьного устройства 

---
## Usage
При создании в телефонной книге содержится один контакт - Statinov Valery 22 +79254500123 valery.statinov@gmail.com
### Получение информации о контакте по фамилии
```bash
sudo ./get [Surname]
```
### Добавление нового контакта
```bash
sudo ./add [Surname] [Name] [age] [phone] [email]
```
### Удаление контакта
```bash
sudo ./del [Surname]
```
---
## Uninstall
Чтобы отсоединить модуль и удалить все лишние файлы необходимо выполнить **make unmount**, затем **make clean**.

## Решение первого дз Kernel-mode phone book
---
## Installation
Для запуска необыходимо выполнить:
1. **make** - сборка phone_book_kernel.c и компиляция программ для использования телефонной книги(add, get, del)
2. **make mount** - внедрение модуля в ядро и создание символьного устройства

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
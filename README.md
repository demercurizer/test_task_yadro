# Computer Club System

Прототип системы учёта работы компьютерного клуба: обработка событий (приход, посадка, ожидание, уход), расчет выручки и времени занятости столов.

## Структура репозитория

```
computer-club-system/
├── README.md
├── .gitignore
├── CMakeLists.txt
├── include/
│   └── utils.h
├── src/
│   ├── main.cpp
│   ├── ClubSystem.h
│   └── ClubSystem.cpp
├── tests/
│   ├── CMakeLists.txt
│   ├── test\_main.cpp
│   ├── ClubSystem\_test.cpp
│   └── fixtures/
│       ├── simple.txt
│       ├── before\_open.txt
│       ├── busy.txt
│       ├── queue\_full.txt
│       ├── auto\_seat.txt
│       ├── billing.txt
│       ├── end\_day.txt
│       ├── example.txt          ← Тест из задания
│       ├── dup\_arrival.txt
│       ├── seat\_unknown.txt
│       ├── wait\_with\_free.txt
│       ├── auto\_seat\_after\_leave.txt
│       ├── at\_open.txt
│       ├── minute\_session.txt
│       └── two\_hour\_session.txt
└── build/                      # каталог сборки
```


## Сборка

```bash
mkdir build && cd build
cmake ..
make
````

## Запуск программы

```bash
./task <path_to_input_file>
```

## Авторский тест

```bash
./task tests/fixtures/example.txt
```

## Юнит‑тесты

Все тесты собраны в `tests/ClubSystem_test.cpp`. Запустить их можно так:

```bash
cd build
./tests/runTests --gtest_color=yes
```


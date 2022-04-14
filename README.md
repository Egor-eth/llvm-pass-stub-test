# Задание

## Statepoint counter
<font color="#696969">(*counter/statepoint_counter.cpp*)</font>

Выполняет подсчет максимального числа ссылок, переданных в *@llvm.experimental.gc.statepoint*. Для каждой функции переданного файла, будет выведено максимальное число ссылок, переданных в gc-live.

## Statepoint checker
<font color="#696969">(*counter/statepoint_checker.cpp*)</font>

В случае превышения необходимого количества ссылок, переданных в *@llvm.experimental.gc.statepoint*, выводит предупреждение и ошибочную строку. 

*statepoint_checker.so* принимает в качестве необязательного параметра командной строки *-m* максимальное допустимое число ссылок. В случае пропуска параметра, используется значение по умолчанию, равное 2. Следующая команда выполнит этот Pass с макс. числом ссылок равным 3. 
```
opt -enable-new-pm=0 -load statepoint_checker.so -m 3 -S statepoint_checker
```

## Тестирование и сборка
Для сборки Pass используется команда:

```
make
```

Для тестирования:

```
make test
```


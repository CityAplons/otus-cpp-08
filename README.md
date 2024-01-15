# bayan

### Building

```bash
mkdir -p build && cd build/
cmake -DCMAKE_BUILD_TYPE=Release ../
cmake --build .
```

You may add a `-DMAKE_PACKAGE=<OFF|ON>` to disable or enable packages of a project (it's enabled by default).

### Running

Пример запуска:

```
./bayan -d . -r -c 256
```

Параметры:

```
Find same file in a directory:
  -h [ --help ]              Print this message
  -d [ --directories ] arg   List of directories to scan
  -e [ --exclude ] arg       List of directories to exclude
  -m [ --mask ] arg          List of file masks to process
  -r [ --recursive ]         Use recursive scan. Default: false
  -s [ --min_file_size ] arg Minimum file size to process, bytes. Default: 1
  -c [ --chunk_size ] arg    Maximum chunk size for reading a file. Default: 5
```

#### Пример на build/

```
./bayan -d . -r -c 128
bayan   ver. 0.0.1
"./CMakeFiles/mapreduce.dir/src/main.cpp.o"
"./CMakeFiles/bayan.dir/src/main.cpp.o"

"./cmake/packaging/CTestTestfile.cmake"
"./CTestTestfile.cmake"

"./cmake/packaging/cmake_install.cmake"
"./cmake_install.cmake"
```

### Условие

Пользуясь имеющимися в библиотеке Boost структурами и алгоритмами разработать утилиту для обнаружения файлов-дубликатов. Утилита должна иметь возможность через параметры командной строки
указывать

* директории для сканирования (может быть несколько)
* директории для исключения из сканирования (может быть несколько)
* уровень сканирования (один на все директории, 0 - только указанная
  директория без вложенных)
* маски имен файлов разрешенных для сравнения (не зависят от
  регистра)
* размер блока, которым производится чтения файлов, в задании этот
  размер упоминается как S
* один из имеющихся алгоритмов хэширования (crc32, md5 -
  конкретные варианты определить самостоятельно), в задании
  эта функция упоминается как H

Результатом работы утилиты должен быть список полных путей файлов с идентичным содержимым, выводимый на стандартный вывод. На одной строке один файл. Идентичные файлы должны подряд, одной группой. Разные группы разделяются пустой строкой. Обязательно свойство утилиты - бережное обращение с дисковым вводом выводом. Каждый файл может быть представлен в виде списка блоков размера S. Если размер файла не кратен, он дополняется бинарными нулями.

Файлы считаются идентичными при полном совпадении последовательности
хешей блоков.

### Самоконтроль

   • блок файла читается с диска не более одного раза
   • блок файла читается только в случае необходимости
   • не забыть, что дубликатов может быть больше чем два
   • описание параметров в файле README.md корне репозитория

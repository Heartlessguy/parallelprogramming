# Паралельное Программирование (ПП)
## PP repo guidelines:
* Каждая лаба в отдельнном каталоге в виде **PPLab{номер}**
* В каждом каталоге содержится **PPLab{номер}**.MD, содержащий задание, ссылки на необходимый материал и заметки.
* Скомпилированные бинарники класть в подкаталог /bin
* Для MPI - **PPLab{номер}**.mpi
* Для OpenMP - **PPLab{номер}**.omp
* Исходники, полученные извне (от препода, краденые), класть в /src/ext
* *Рекомендовано:* <del>Снабжать каждую лабу .sh/makefile для автоматической сборки</del> 
* В [tasks.json](.vscode/tasks.json) лежит таск сборки для mpich
* *Рекомендовано:* Снабжать код изобилием комментариев, ибо надо
## Как какать:
1. Для запуска убедитесь в том, что у вас есть бинарники mpich - 
    ```bash
    whereis mpirun
    ```
2. Для сборки запустите таск: для этого вызовите **Command Palette** сочитанием клавиш **Ctrl+Shift+P**, введите **Run Task**, в выпадающем меню выберите **mpicc build active file**.
3. Запуск и дебаг ручками. Не удалось подружить gdb с mpi внутри VSCode, ну чёрт бы с ним, правиьно?
    ```bash
    mpirun -np <processnumber> ../bin/./Labnumber.mpi
    ```
    ```bash
    mpirun -np <processnumber> gdb ../bin/./Labnumber.mpi
    ```
## Тесты:
![Не нужны](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRZ28cPkjuFgXPD_8FtNGcAkbUjOjbT2BrMxUUJ8GtpeCyphIUfDg)

## Дополнительно
* ~~в каталоге POC содержатся хелловорлды для проврки~~

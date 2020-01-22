//? LAB4-20i
//? Alexandr Komnantyy
//! OpenMP Basics
//? for more info see ../PPlab4.MD

#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>

// Прототипы функций, которые мы будем использовать ниже
static double f(double, double);
static double fi(double, double);

int main(int argc, char *argv[])
{
  double a, // Нижняя граница ~ -1.0
        b, // Верхняя граница ~ 0.8
        c, // Переменная ~ 1.2
        Eps; // Точность ~ < 1
  int done = 0; // Переменная для сообщения о завершении работы
  int i, n; // Служебные переменные
  double Sum, Gsum, // Переменные для сумм частей вычисляемого интеграла
        time1, // Момент начала работы программы
        time2, // Момент конца работы программы
        x; // Промежуточная переменная для хранения значения

  int nt, // Число потоков
    myid = 0, // Номер потока
    chunk, // Размер блока итераций
    numproc; // Максимально возможное число потоков

  Eps = 1.0;

  numproc = omp_get_max_threads(); // Получаем макс. возмож. число потоков
  printf("Максимально возможное число потоков: %d\n\n", numproc);

  nt = atoi(argv[1]); // Принимаем число потоков, введеное как аргумент функции

  printf("Введите нижнюю границу a:\n"); scanf("%lf", &a);
  printf("Введите верхнюю границу b:\n"); scanf("%lf", &b);
  printf("Введите c:\n"); scanf("%lf", &c);


  // Основной цикл
  while (!done)
  {
    // Введ значения точности
    printf("Enter Eps ( > 0.1 - quit) "); fflush(stdout);
    scanf("%lf",&Eps); printf("%10.9lf\n", Eps);

    // Проверка на завершение работы программы
    if(Eps > 0.1)
    {
        done = 1;
    }
    else
    {
        // Отмечаем время работы программы
        time1 = omp_get_wtime();

        n = 1.0/(Eps);
        chunk = n/numproc/6; // Определяем размер блока итераций

        // Считаем сумму в каждом потоке
        Gsum = 0.0;

        // Устанавливаем число потоков в конкретной области
        omp_set_num_threads(nt);
        #pragma omp parallel private(i, x, Sum, myid)\
                            default(shared)\
                            shared(a, b, c, n, chunk) reduction(+:Gsum)
        {
            // В первую очередь выводим число потоков
            #pragma omp master
            {
                printf("Потоков в параллели %5d\n", omp_get_num_threads());
            }

            // Получаем номер потока
            myid = omp_get_thread_num();
            printf("Поток %d запущен.\n", myid);

            // Инициализируем переменную Sum
            Sum = 0.0;

            // Распределение нагрузки на известное число потоков
            #pragma omp for schedule(static, chunk) // reduction(+:Gsum)

            // Расчет части интеграла
            for (int i = 1; i <= n; i++)
            {
                x = a+(b-a)*((double)i-0.5)/n;
                Sum += f(x, c) * (b-a)/n;
            }

            printf("Поток %d. Часть интеграла: %18.16f \n", myid, Sum);
            Gsum += Sum; // Суммирование частей интеграла
        }

        // Отмечаем время завершения работы программы
        time2 = omp_get_wtime();

        // Выводим результаты работы программы
        printf("\nИнтеграл функции ln(1/x) от %5.2f до %5.2f..\n", a, b);
        printf("Eps %9.7f. Интеграл = %18.16f, Погрешность = %18.16f\n",
                Eps, Gsum, fi(b, c) - fi(a, c) - Gsum);
        printf("Время работы программы = %8.6f c. \n", time2 - time1);
        fflush(stdout);

    }
 }

  return 0;
}

// Определяем функции
static double f(double a, double c)
{
    return a * sinh(a * c);
}

static double fi(double a, double c)
{
    return (a * cosh(a * c) / c) - (sinh(a * c) / pow(c, 2));
}

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <math.h>

//Объявление типа указателя на функцию
typedef double (*TFun)(double);

//Прототипы математических функций
double funY(double);
double funV(double);
double funS(double);

//Прототипы служебных функций и действий меню
TFun select_func();
void Out_Rez(TFun, double, double, double);
void plot(double, double, TFun);
double* calculate(TFun prtf, double* x, int n);

int main()
{
    setlocale(LC_CTYPE, "ru_RU.UTF-8");
    int menu_choice;
    double x, a, b, h;
    TFun current_fun = NULL;

    double x_arr[10] = { -2.0, -1.5, -0.5, 0.0, 0.2, 0.5, 1.0, 1.5, 2.0, 3.0 };
    int n = 10;

    do {
        puts("\n(^._.^) МЕНЮ (^._.^)");
        puts("1. Вычислить значение");
        puts("2. Табулировать");
        puts("3. Выполнить операцию (построить график)");
        puts("4. Вычислить массив из 10 модулей значений");
        puts("5. Выход");
        printf("Выберите пункт меню: ");
        scanf("%d", &menu_choice);

        if (menu_choice >= 1 && menu_choice <= 4) {
            current_fun = select_func();
            if (current_fun == NULL) continue;
        }

        switch (menu_choice)
        {
        case 1:
            printf("Введите значение x: ");
            scanf("%lf", &x);
            printf("Результат: f(%.4lf) = %.4lf\n", x, current_fun(x));
            break;

        case 2:
            printf("Введите начало (a), конец (b) и шаг (h): ");
            scanf("%lf%lf%lf", &a, &b, &h);
            if (h <= 0) {
                puts("Ошибка: шаг h должен быть больше 0!");
            }
            else if (a >= b) {
                puts("Ошибка: начало интервала (a) должно быть меньше конца (b)!");
            }
            else {
                Out_Rez(current_fun, a, b, h);
            }
            break;

        case 3:
            printf("Введите интервал построения графика (a и b): ");
            scanf("%lf%lf", &a, &b);
            if (a >= b) {
                puts("Ошибка: начало интервала должно быть меньше конца!");
            }
            else {
                plot(a, b, current_fun);
            }
            break;

        case 4: {
            double* modules = calculate(current_fun, x_arr, n);
            if (modules != NULL) {
                puts("\nМассив модулей значений функции:");
                for (int i = 0; i < 10; i++) {
                    printf(" |f(%5.2lf)| = %8.4lf\n", x_arr[i], modules[i]);
                }
                free(modules);
            }
            else {
                puts("Ошибка выделения памяти!");
            }
            break;
        }

        case 5:
            puts("Выход из программы.");
            break;

        default:
            puts("Ошибка: неверный пункт меню!");
        }

    } while (menu_choice != 5);

    return 0;
}

//Реализации функций пользователя

TFun select_func() {
    int f_choice;
    puts("\nВыберите функцию:");
    puts("1 - Y(x)");
    puts("2 - V(x)");
    puts("3 - S(x)");
    printf("Ваш выбор: ");
    scanf("%d", &f_choice);

    switch (f_choice) {
    case 1: return funY;
    case 2: return funV;
    case 3: return funS;
    default:
        puts("Ошибка: неверный номер функции!");
        return NULL;
    }
}

//Y(x) = ln|sqrt(x^3)|, при x > 0
double funY(double x) {
    if (x <= 0) return NAN;
    return log(fabs(sqrt(pow(x, 3))));
}

//V(x) - кусочно-заданная функция
double funV(double x) {
    if (x > 1) {
        double t = tan(x * x - 1);
        if (t < 0) return NAN;
        return sqrt(t);
    }
    else if (x >= 0 && x <= 1) {
        return -2.0 * x;
    }
    else {
        return exp(cos(x));
    }
}

//S(x) - сумма первых 4 членов ряда
double funS(double x) {
    if (x == -1.0) return NAN;
    double t = (x - 1.0) / (x + 1.0);
    return t + (1.0 / 3.0) * pow(t, 3) + (1.0 / 5.0) * pow(t, 5) + (1.0 / 7.0) * pow(t, 7);
}

//Табулирование функции
void Out_Rez(TFun f, double xn, double xk, double h) {
    puts("\nРезультаты табуляции:");
    for (double x = xn; x <= xk + h / 2.0; x += h) {
        printf(" x = %5.2lf, y = %8.4lf\n", x, f(x));
    }
}

//Построение графика функции
void plot(double x0, double x1, TFun f)
{
    int SCREENW = 60, SCREENH = 40;
    char screen[60][40];
    double x, y[60];
    double ymin = 0, ymax = 0;
    double hx, hy;
    int i, j;
    int xz, yz;

    hx = (x1 - x0) / (SCREENW - 1);
    for (i = 0, x = x0; i < SCREENW; ++i, x += hx) {
        y[i] = f(x);
        if (!isnan(y[i])) {
            if (y[i] < ymin) ymin = y[i];
            if (y[i] > ymax) ymax = y[i];
        }
    }

    if (ymax == ymin) ymax = ymin + 1.0;

    hy = (ymax - ymin) / (SCREENH - 1);
    yz = (int)floor(ymax / hy + 0.5);
    xz = (int)floor((0. - x0) / hx + 0.5);

    for (j = 0; j < SCREENH; ++j) {
        for (i = 0; i < SCREENW; ++i) {
            if (j == yz && i == xz) screen[i][j] = '+';
            else if (j == yz) screen[i][j] = '-';
            else if (i == xz) screen[i][j] = '|';
            else screen[i][j] = ' ';
        }
    }

    for (i = 0; i < SCREENW; ++i) {
        if (!isnan(y[i])) {
            j = (int)floor((ymax - y[i]) / hy + 0.5);
            if (j >= 0 && j < SCREENH) {
                screen[i][j] = '*';
            }
        }
    }

    puts("");
    for (j = 0; j < SCREENH; ++j) {
        for (i = 0; i < SCREENW; ++i) putchar(screen[i][j]);
        putchar('\n');
    }
}

//Расчёт массива из 10 модулей значений функции
double* calculate(TFun prtf, double* x, int n) {
    double* res = (double*)malloc(10 * sizeof(double));
    if (res == NULL) {
        return NULL;
    }

    for (int i = 0; i < 10; i++) {
        if (i < n) {
            res[i] = fabs(prtf(x[i]));
        }
        else {
            res[i] = 0.0;
        }
    }

    return res;
}

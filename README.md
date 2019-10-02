# Лабы Ивана
## Как добавить бэдж
Идем в Settings - General - Badges - Expand

Добавляем бэдж:

1. Link: `https://gitlab.ccfit.nsu.ru/%{project_path}`

2. Badge image URL: `https://gitlab.ccfit.nsu.ru/%{project_path}/-/jobs/artifacts/master/raw/build_lab0/status.svg?job=lab0`

`lab0` во втором пункте заменяем на желаемую лабу

## Scanf ошибки
Компилятор тут настроен на нечеловеческую строгость, так что все scanf пишем так:

```c
if (!scanf(....)) {
    // сообщить об ошибке
    exit(1);
}
```

или так:
```c
if (scanf(....) != numbers_of_arguments) {
    // сообщить об ошибке
    exit(1);
}
```

<!-- или даже так (я не проверял, но может работать):
```c
// В начале
#include<stdarg.h>

// Функция
int ec_scanf(const char *fmt, ...) {
    int rc;

    va_list args;
    va_start(args, fmt);
    rc = vscanf(fmt, args);
    va_end(args);

    if (!rc) {
        perror("Scanf error");
        exit(1);
    }

    return rc;
}
``` -->
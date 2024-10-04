# Tarea 1 - Hashing Lineal

Esta tarea implementa un Hashing Lineal

## Requisitos

- [C++17 o superior](https://en.cppreference.com/w/cpp/filesystem).

## Estructura del proyecto

La totalidad del codigo se encuentra dentro de `main.cpp`

## Configuración

La función `main()` que ejecuta los experimentos se encuentra dentro de `main.cpp`.

Dentro de `main.cpp` podemos configurar principalmente las variaciones del parametro $C_{max}$ y la cantidad de puntos con las que se va a ejecutar.

Por defecto `main()` viene configurado de la siguiente forma:

- $C_{max}$

```c++
ll C_MAX_values[] = {10, 100, 500, 750, 1024, 2056, 5000, 10000};
```

- Cantidad de puntos $2^i$

```c++
for(int i=10; i < 25; i++)
```

Si se quisiese cambiar el rango en el cual se generan los valores aleatorios a insertar, se debe modificar la funcion `experimento2`. Que por defecto genera valores aleatorios de 64bits entre $[1,  2^{63}-1]$

```c++
uniform_int_distribution<ll> dis(1, numeric_limits<long long>::max());
```

> [!NOTE]
> Todos estos valores por defecto, son los mismos pedidos en el enunciado de la tarea.

## Ejecucion

El `main` esta configurado para que a partir de la configuración inicial, se ejecuten **todos** los experimentos.

Para poder compilar el proyecto se requiere:

- Usar la extension de C/C++ de VSCode

## Extensión C/C++

### Requerimientos

- VSCode
- Extensión de C++ para Visual Studio Code

Para esta forma de ejecutar, recomendamos seguir la documentación oficial de la extension y de VSCode.

- [Windows](https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites)
- [Linux](https://code.visualstudio.com/docs/cpp/config-linux)

> [!NOTE]
> Estos tutoriales, lo que hacen es usar `g++` como compilador de C, y mostrar como ser configurado para VSCode usando la extension. En particular para Windows, se debe instalar `MinGW-w64` para poder tener el compilador de C en Windows (mas información en el tutorial).

La extension de C++, al configurarse como se muestra en el tutorial (seccion _"Run helloworld.cpp"_, punto 2 y 3), se generan varios archivos `.json` para compilar un proyecto. En particular, genera uno llamado `tasks.json`, el cual es quien tiene los parametros que luego son entregado a `g++`

Finalmente se debe ejecutar `main.cpp` usando la extension, lo cual compila y genera un ejecutable `main.exe` que debe ser ejecutado como un programa normal.

```bash
$ main.exe
```

> [!NOTE]
> Lo que hace la extension es ejecutar `g++` con la configuración de `tasks.json` de una forma mas automática y mas simple, pero de todas formas se puede ejecutar manualmente. [Aquí están los links de los comandos de g++ (o preguntarle a Mateu 🙂)](https://courses.cs.washington.edu/courses/cse326/00wi/unix/g++.html).

## Experimentos

Al ejecutar los experimentos, se genera un archivo `.tsv` en el directorio `/experiments_data` que contiene diversas metricas calculadas durante el experimento. Adicionalmente, dentro de `/experiments_data/data_grupo` se pueden encontrar los datos obtenidos por el equipo con la configuración por defecto.

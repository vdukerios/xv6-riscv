# Documentación: Implementación de Lottery Scheduling en xv6-riscv

## Descripción General

Este documento describe la implementación completa del algoritmo de **Lottery Scheduling** en el sistema operativo xv6-riscv, reemplazando el scheduler Round-Robin original. La implementación se realizó siguiendo 5 pasos específicos, cada uno con sus archivos modificados y pruebas correspondientes.

---

## PASO 1: Campo tickets en cada proceso

### Objetivo

-   Agregar un campo `tickets` a la estructura de proceso
-   Inicializar procesos con un valor por defecto de tickets

### Archivos Modificados

#### **kernel/proc.h**

-   **Modificación**: Agregado campo `int tickets` a `struct proc`
-   **Propósito**: Almacenar número de tickets por proceso

#### **kernel/proc.c**

-   **Modificación**: Inicialización de `tickets = 1` en función `allocproc()`
-   **Propósito**: Establecer valor por defecto de 1 ticket para nuevos procesos

### Pruebas para este Paso

Sin pruebas específicas - este paso establece la infraestructura base.

---

## PASO 2: Syscall settickets(int n)

### Objetivo

-   Implementar syscall para establecer número de tickets de un proceso
-   Validar parámetros de entrada
-   Actualizar tickets del proceso actual

### Archivos Modificados

#### **kernel/sysproc.c**

-   **Modificación**: Implementación de `sys_settickets()` y `sys_gettickets()`
-   **Propósito**: Funciones syscall para establecer y consultar tickets con validación `n >= 1`

#### **kernel/syscall.h**

-   **Modificación**: Agregados `#define SYS_settickets` y `#define SYS_gettickets`
-   **Propósito**: Números de syscall únicos para nuevas funciones

#### **kernel/syscall.c**

-   **Modificación**: Agregadas entradas en tabla de dispatch
-   **Propósito**: Conectar números de syscall con implementaciones

#### **user/usys.pl**

-   **Modificación**: Agregadas declaraciones `entry("settickets")` y `entry("gettickets")`
-   **Propósito**: Generar stubs de assembly para interfaz user-kernel

#### **user/user.h**

-   **Modificación**: Declaraciones `int settickets(int)` e `int gettickets(void)`
-   **Propósito**: Interfaz de funciones para programas de usuario

#### **Makefile**

-   **Modificación**: Agregada entrada `$U/_tickettest`
-   **Propósito**: Incluir programa de prueba en imagen del sistema

### Pruebas para este Paso

#### **user/tickettest.c**

-   **Propósito**: Prueba básica de syscalls `settickets()` y `gettickets()`
-   **Funcionalidad**:
    -   Verifica establecimiento correcto de tickets
    -   Confirma consulta de tickets funciona
    -   Valida que los valores se mantienen

---

## PASO 3: Algoritmo lottery

### Objetivo

-   Reemplazar el scheduler Round-Robin con lottery scheduling
-   Implementar selección probabilística basada en tickets
-   Usar generación de números pseudo-aleatorios

### Archivos Modificados

#### **kernel/proc.c**

-   **Modificación**: Reemplazo completo de la función `scheduler()`
-   **Propósito**: Implementar algoritmo lottery con selección probabilística
-   **Modificación**: Agregada función `random()` para generación pseudo-aleatoria
-   **Propósito**: Proveer números aleatorios para selección de procesos

#### **Makefile**

-   **Modificación**: Agregada entrada `$U/_lotterytest`
-   **Propósito**: Incluir programa de prueba del algoritmo lottery

### Pruebas para este Paso

#### **user/lotterytest.c**

-   **Propósito**: Prueba del algoritmo lottery con múltiples procesos
-   **Funcionalidad**:
    -   Crea procesos con diferentes números de tickets
    -   Demuestra selección probabilística
    -   Verifica que procesos con más tickets son seleccionados más frecuentemente

---

## PASO 4: Contabilidad y monitoreo

### Objetivo

-   Implementar conteo de CPU slices por proceso
-   Agregar syscall para consultar CPU slices obtenidos
-   Monitorear asignación de CPU time

### Archivos Modificados

#### **kernel/proc.h**

-   **Modificación**: Agregado campo `int cpu_slices` a `struct proc`
-   **Propósito**: Contar número de veces que un proceso fue seleccionado por el scheduler

#### **kernel/proc.c**

-   **Modificación**: Incremento de `p->cpu_slices++` en el scheduler cuando un proceso es seleccionado
-   **Propósito**: Contabilizar selecciones del scheduler por proceso
-   **Modificación**: Inicialización de `cpu_slices = 0` en `allocproc()`
-   **Propósito**: Establecer contador inicial para nuevos procesos

#### **kernel/sysproc.c**

-   **Modificación**: Implementación de `sys_getcpuslices()`
-   **Propósito**: Syscall para consultar CPU slices del proceso actual

#### **kernel/syscall.h**

-   **Modificación**: Agregado `#define SYS_getcpuslices`
-   **Propósito**: Número de syscall único para consulta de CPU slices

#### **kernel/syscall.c**

-   **Modificación**: Agregada entrada en tabla de dispatch para `sys_getcpuslices`
-   **Propósito**: Conectar syscall con implementación

#### **user/usys.pl**

-   **Modificación**: Agregada declaración `entry("getcpuslices")`
-   **Propósito**: Generar stub de assembly para nueva syscall

#### **user/user.h**

-   **Modificación**: Declaración `int getcpuslices(void)`
-   **Propósito**: Interfaz para programas de usuario

#### **Makefile**

-   **Modificación**: Agregada entrada `$U/_cpuslicetest`
-   **Propósito**: Incluir programa de prueba de monitoreo

### Pruebas para este Paso

#### **user/cpuslicetest.c**

-   **Propósito**: Prueba del monitoreo de CPU slices
-   **Funcionalidad**:
    -   Verifica que el conteo de CPU slices funciona
    -   Demuestra proporcionalidad entre tickets y slices obtenidos
    -   Monitorea progreso de contadores durante ejecución

---

## PASO 5: Robustez

### Objetivo

-   Manejar casos extremos (tickets negativos, cero, muy altos)
-   Crear programa de prueba con N procesos (mínimo 10)
-   Usar fórmula específica: `settickets(50 * (i + 1))`
-   Verificar proporcionalidad en asignación de CPU

### Archivos Modificados

#### **kernel/proc.c**

-   **Modificación**: Agregada validación `if(p->tickets < 1) p->tickets = 1` en scheduler
-   **Propósito**: Garantizar robustez evitando división por cero y manteniendo al menos 1 ticket

#### **kernel/sysproc.c**

-   **Modificación**: Validación `if(n < 1) n = 1` en `sys_settickets()`
-   **Propósito**: Prevenir establecimiento de tickets inválidos desde user space

#### **Makefile**

-   **Modificación**: Agregadas entradas `$U/_demo` y `$U/_robusttest`
-   **Propósito**: Incluir programas de prueba comprehensiva y robustez

### Pruebas para este Paso

#### **user/demo.c**

-   **Propósito**: Demostración completa con 10 procesos según especificaciones
-   **Funcionalidad**:
    -   Crea exactamente 10 procesos concurrentes
    -   Usa fórmula especificada: `settickets(50 * (i + 1))`
    -   Monitorea progreso y CPU slices en tiempo real
    -   Analiza proporcionalidad de resultados finales
    -   Demuestra comportamiento concurrente real

#### **user/robusttest.c**

-   **Propósito**: Prueba exhaustiva de robustez y casos extremos
-   **Funcionalidad**:
    -   Prueba tickets negativos (deben ajustarse automáticamente a 1)
    -   Prueba tickets cero (deben ajustarse automáticamente a 1)
    -   Prueba tickets muy altos (deben aceptarse sin problemas)
    -   Prueba competencia padre-hijo con diferentes asignaciones de tickets
    -   Verifica que el proceso con más tickets obtiene más CPU time

---

## Resumen de Implementación

### Características Implementadas

-   **Algoritmo Lottery Scheduling**: Selección probabilística basada en tickets
-   **Sistema de Tickets**: Rango válido con validación automática (mínimo 1)
-   **Monitoreo**: Conteo de CPU slices en tiempo real
-   **Robustez**: Manejo de casos extremos y validación de entrada
-   **Testing Comprehensivo**: 5 programas de prueba para validar funcionalidad

### Resultados Esperados

-   Procesos con más tickets reciben proporcionalmente más CPU time
-   Comportamiento probabilístico (no determinístico)
-   Salida intercalada demuestra concurrencia real
-   Ausencia de starvation y deadlocks
-   Manejo robusto de casos extremos

### Conclusión

La implementación cumple completamente las especificaciones, proporcionando un lottery scheduler robusto que demuestra proporcionalidad estadística, mantiene fairness probabilístico, y exhibe el comportamiento concurrente esperado en un sistema operativo real.

# Lottery Scheduling en xv6-riscv

## Funcionamiento y Lógica de la Implementación

### Algoritmo Lottery Scheduling

El **Lottery Scheduling** es un algoritmo de planificación probabilístico que asigna CPU time a procesos basándose en "tickets". Cada proceso posee un número de tickets que representa su prioridad relativa - más tickets significa mayor probabilidad de ser seleccionado.

#### Lógica del Scheduler

1. **Cálculo del Total**: El scheduler suma todos los tickets de procesos ejecutables
2. **Generación Aleatoria**: Se genera un número aleatorio entre 1 y el total de tickets
3. **Selección del Ganador**: Se recorre la lista de procesos acumulando tickets hasta encontrar el "ganador"
4. **Ejecución**: El proceso seleccionado obtiene un quantum de CPU

#### Ejemplo de Funcionamiento

```
Proceso A: 50 tickets  (rango 1-50)
Proceso B: 100 tickets (rango 51-150)
Proceso C: 150 tickets (rango 151-300)
Total: 300 tickets

Si random() = 75 → Proceso B es seleccionado
Si random() = 200 → Proceso C es seleccionado
```

#### Proporcionalidad Estadística

Un proceso con N tickets debería recibir aproximadamente N/(suma_total) del tiempo de CPU a largo plazo.

### Características Implementadas

-   **Sistema de Tickets**: Cada proceso mantiene un contador de tickets (mínimo 1)
-   **Generador Pseudo-aleatorio**: Función `random()` en kernel space para selección
-   **Monitoreo**: Conteo de CPU slices para análisis de fairness
-   **Robustez**: Validación de entrada y manejo de casos extremos

---

## Modificaciones Realizadas

### Archivos del Kernel Modificados

-   **`kernel/proc.h`**: Agregados campos `tickets` y `cpu_slices` a `struct proc`
-   **`kernel/proc.c`**: Reemplazo completo del scheduler + función `random()` + inicialización
-   **`kernel/sysproc.c`**: Implementación de syscalls `sys_settickets()`, `sys_gettickets()`, `sys_getcpuslices()`
-   **`kernel/syscall.h` & `kernel/syscall.c`**: Definiciones y dispatch de nuevas syscalls

### Archivos User Space Modificados

-   **`user/usys.pl`**: Stubs de assembly para nuevas syscalls
-   **`user/user.h`**: Declaraciones de funciones para programas de usuario
-   **`Makefile`**: Inclusión de 5 programas de prueba

### Programas de Prueba Creados

-   **`tickettest.c`**: Prueba básica de syscalls
-   **`lotterytest.c`**: Validación del algoritmo lottery
-   **`cpuslicetest.c`**: Verificación de monitoreo
-   **`demo.c`**: Demostración con 10 procesos (especificaciones del proyecto)
-   **`robusttest.c`**: Testing de casos extremos

---

## Instrucciones de Ejecución

### ⚠️ Importante: Usar WSL, NO Git Bash ni Powershell

**DEBE ejecutarse en WSL (Windows Subsystem for Linux), NO en Git Bash de Windows.**

### Prerequisitos

-   **WSL2 con Ubuntu** instalado y configurado
-   **Toolchain RISC-V**: `gcc-riscv64-unknown-elf` instalado en WSL
-   **QEMU**: `qemu-system-riscv64` para emulación (en WSL)
-   **Make**: Para compilación del proyecto (en WSL)

### Instalación de Dependencias en WSL

Si no tienes las herramientas instaladas, ejecutar en WSL:

```bash
sudo apt update
sudo apt install gcc-riscv64-unknown-elf qemu-system-misc build-essential
```

### Compilación y Ejecución

**Abrir terminal WSL** (NO Git Bash) y navegar al proyecto:

```bash
# Desde PowerShell de Windows, abrir WSL:
wsl

# Navegar al directorio del proyecto:
cd /mnt/c/Users/vduke/OneDrive/Documentos/GitHub/xv6-riscv

# Compilar el sistema:
make clean
make fs.img

# Iniciar xv6 en QEMU:
make qemu
```

### Alternativa: Usar comando directo desde PowerShell

Desde PowerShell de Windows, puedes ejecutar directamente:

```powershell
wsl bash -c "cd /mnt/c/Users/vduke/OneDrive/Documentos/GitHub/xv6-riscv && make clean && make fs.img"
wsl bash -c "cd /mnt/c/Users/vduke/OneDrive/Documentos/GitHub/xv6-riscv && make qemu"
```

3. **Ejecutar el demo principal**:
   Una vez iniciado xv6, en la shell del sistema ejecutar:
    ```bash
    $ demo
    ```

### Qué Esperar del Demo

El programa `demo` ejecutará:

-   **10 procesos concurrentes** con tickets según fórmula: `50 * (i + 1)`
-   **Distribución de tickets**: Proceso 1=50, Proceso 2=100, ..., Proceso 10=500
-   **Salida intercalada**: Los procesos escriben simultáneamente (comportamiento esperado)
-   **Monitoreo en tiempo real**: Progreso y CPU slices de cada proceso
-   **Análisis final**: Verificación de proporcionalidad en asignación de CPU

### Interpretación de Resultados

-   **Salida mezclada es normal**: Demuestra concurrencia real del scheduler
-   **Buscar patrones**: Procesos con más tickets deben obtener más CPU slices
-   **Proporcionalidad estadística**: No exacta, pero tendencia clara observable
-   **Todos terminan**: Confirma ausencia de starvation

### Otras Pruebas Disponibles

```bash
$ tickettest     # Prueba básica de syscalls
$ lotterytest    # Algoritmo lottery con pocos procesos
$ cpuslicetest   # Verificación de monitoreo
$ robusttest     # Casos extremos y robustez
```

### Salir de QEMU

Para terminar la emulación:

```
Ctrl+A, luego X
```

---

## Dificultades Encontradas y Soluciones Implementadas

### 1. **Generación de Números Aleatorios en Kernel**

-   **Problema**: xv6 no incluye generador de números aleatorios
-   **Solución**: Implementación de Linear Congruential Generator (LCG) simple usando `ticks` como seed

### 2. **Validación de Robustez**

-   **Problema**: Tickets negativos o cero podrían causar división por cero o comportamiento indefinido
-   **Solución**: Validación automática que establece mínimo de 1 ticket tanto en syscall como en scheduler

### 3. **Salida Intercalada en Pruebas Concurrentes**

-   **Problema**: Output mezclado e "ilegible" durante pruebas con múltiples procesos
-   **Solución**: Reconocimiento de que esto es comportamiento esperado que demuestra concurrencia real

### 4. **Configuración del Entorno de Desarrollo**

-   **Problema**: Compilación cruzada en WSL2 con toolchain RISC-V
-   **Solución**: Instalación y configuración correcta de `gcc-riscv64-unknown-elf` y dependencias

### 5. **Testing de Proporcionalidad**

-   **Problema**: Verificar que la distribución de CPU sea estadísticamente proporcional
-   **Solución**: Implementación de contadores `cpu_slices` y análisis de ratios en programas de prueba

---

## Posibles Problemas del Lottery Scheduling

### 1. **Problema de Starvation Potencial**

Aunque teóricamente todos los procesos tienen probabilidad > 0 de ser seleccionados, en la práctica un proceso con muy pocos tickets podría experimentar períodos largos sin ejecución. Esto es especialmente problemático en sistemas con gran disparidad de tickets entre procesos.

**Impacto**: Procesos críticos con pocos tickets podrían no cumplir deadlines en sistemas de tiempo real.

### 2. **Overhead Computacional del Algoritmo**

El lottery scheduling requiere:

-   Sumar todos los tickets de procesos ejecutables en cada decisión de scheduling
-   Generar número aleatorio
-   Iterar por la lista de procesos para encontrar el ganador

**Complejidad**: O(n) en cada decisión vs O(1) del Round-Robin, donde n = número de procesos ejecutables.

**Impacto**: En sistemas con muchos procesos, el overhead puede ser significativo.

### 3. **Falta de Predictibilidad y Determinismo**

La naturaleza probabilística hace que el comportamiento sea inherentemente impredecible:

-   Imposible garantizar latencias específicas
-   Dificultad para debugging de problemas de timing
-   Comportamiento variable entre ejecuciones idénticas

**Problema específico**: Sistemas que requieren comportamiento determinístico (control industrial, sistemas embebidos críticos) no pueden usar lottery scheduling.

### 4. **Problema de Fairness a Corto Plazo**

Aunque estadísticamente fair a largo plazo, puede ser muy unfair en períodos cortos:

-   Un proceso con muchos tickets podría ser seleccionado repetidamente
-   Procesos con pocos tickets podrían no ejecutar durante largos períodos
-   No hay garantías de tiempo máximo entre selecciones

**Ejemplo**: Proceso con 1000 tickets vs proceso con 1 ticket - el segundo podría no ejecutar por largos períodos por pura mala suerte.

### 5. **Dificultad en la Asignación de Tickets**

Determinar el número "correcto" de tickets es complejo:

-   No hay métricas claras para asignar tickets apropiados
-   Requiere conocimiento profundo de los requerimientos de cada proceso
-   Cambios dinámicos en carga de trabajo requieren reajustes constantes

**Problema práctico**: ¿Cuántos tickets debe tener un editor de texto vs un compilador vs un juego?

### 6. **Vulnerabilidad a Ataques de Resource Hogging**

Un proceso malicioso podría:

-   Solicitar números extremadamente altos de tickets
-   Monopolizar CPU time injustamente
-   Causar denial of service a otros procesos

**Falta de control**: Sin límites superiores o políticas de control, el sistema es vulnerable a abuso.

### 7. **Problema de Prioridad Inversa**

En sistemas complejos con dependencias entre procesos:

-   Un proceso de alta prioridad (muchos tickets) podría esperar por uno de baja prioridad
-   El lottery scheduling no maneja herencia de prioridad
-   Puede causar inversión de prioridades no deseada

### 8. **Ineficiencia en Cargas de Trabajo Específicas**

Para ciertos patrones de trabajo, lottery scheduling es subóptimo:

-   **Cargas CPU-bound uniformes**: Round-robin sería más eficiente
-   **Tareas interactivas**: Necesitan respuesta rápida, no proporcionalidad estadística
-   **Cargas batch**: Podrían beneficiarse más de algoritmos como FCFS

### 9. **Dificultad de Debugging y Profiling**

La naturaleza aleatoria complica:

-   Reproducción de bugs relacionados con timing
-   Profiling de rendimiento (resultados variables)
-   Análisis de comportamiento del sistema
-   Testing determinístico de aplicaciones

### 10. **Problema de Granularidad de Control**

El control de prioridad es "granular" pero no preciso:

-   Solo se puede controlar proporción relativa, no timing absoluto
-   Difícil implementar políticas complejas de scheduling
-   No permite control fino sobre latencias específicas

---

## Conclusión

El Lottery Scheduling implementado demuestra ser una alternativa viable al Round-Robin para ciertos tipos de cargas de trabajo, especialmente donde se requiere control proporcional de recursos. Sin embargo, sus limitaciones inherentes lo hacen inadecuado para sistemas que requieren predictibilidad, determinismo o garantías de tiempo real estrictas.

La implementación exitosa en xv6-riscv proporciona una base sólida para experimentación y educación sobre algoritmos de scheduling probabilísticos, cumpliendo todos los objetivos planteados mientras expone las complejidades reales de los sistemas de scheduling en sistemas operativos.

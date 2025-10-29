#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PROCESSES 10
#define WORK_ITERATIONS 50000

void
worker_process(int id, int tickets)
{
  printf("Proceso %d iniciado - PID: %d, Tickets: %d\n", id, getpid(), tickets);
  
  // Establecer tickets para este proceso
  settickets(tickets);
  
  // Confirmar tickets establecidos
  int actual_tickets = gettickets();
  int initial_slices = getcpuslices();
  
  printf("Proceso %d confirmado - PID: %d, Tickets: %d, CPU slices iniciales: %d\n", 
         id, getpid(), actual_tickets, initial_slices);
  
  int start_time = uptime();
  
  // Trabajo computacional intensivo
  for(int i = 0; i < WORK_ITERATIONS; i++) {
    // Simular trabajo
    volatile int x = i * i + i;
    (void)x; // Evitar warning de variable no utilizada
    
    // Reportar progreso cada cierto tiempo
    if(i % (WORK_ITERATIONS/5) == 0 && i > 0) {
      int current_slices = getcpuslices();
      printf("Proceso %d (PID: %d, %d tickets) - Progreso %d%%, CPU slices: %d\n", 
             id, getpid(), tickets, (i * 100) / WORK_ITERATIONS, current_slices);
    }
  }
  
  int end_time = uptime();
  int final_slices = getcpuslices();
  int total_slices = final_slices - initial_slices;
  
  printf("Proceso %d TERMINADO - PID: %d, Tickets: %d, Tiempo: %d ticks, CPU slices: %d (ganó %d)\n", 
         id, getpid(), tickets, end_time - start_time, final_slices, total_slices);
  
  exit(0);
}

int
main(void)
{
  printf("=== DEMO DE LOTTERY SCHEDULING ===\n");
  printf("⚠️IMPORTANTE⚠️: Los resultados iniciales son ilegibles dado que el scheduler intenta ejecutar varios procesos concurrentemente, lo que mezcla las salidas por consola.\n");
  printf("Creando %d procesos con diferentes números de tickets\n", NUM_PROCESSES);
  printf("Fórmula de tickets: 50 * (i + 1) donde i es el índice del proceso\n");
  printf("Se espera proporcionalidad entre tickets y CPU slices obtenidos\n\n");
  
  int tickets[NUM_PROCESSES];
  
  // Crear N procesos
  for(int i = 0; i < NUM_PROCESSES; i++) {
    tickets[i] = 50 * (i + 1);  // 50, 100, 150, 200, ..., 500
    
    int pid = fork();
    if(pid == 0) {
      // Proceso hijo
      worker_process(i + 1, tickets[i]);
      // No debería llegar aquí
      exit(0);
    } else if(pid > 0) {
      // Proceso padre
      printf("Creado proceso %d con PID %d y %d tickets\n", i + 1, pid, tickets[i]);
    } else {
      printf("Error al crear proceso %d\n", i + 1);
      exit(1);
    }
  }
  
  printf("\nTodos los procesos creados. Esperando que terminen...\n\n");
  
  // Esperar que todos los procesos terminen
  for(int i = 0; i < NUM_PROCESSES; i++) {
    int status;
    int finished_pid = wait(&status);
    printf("Proceso hijo PID %d terminó con status %d\n", finished_pid, status);
  }
  printf("\n=== ANÁLISIS DE RESULTADOS ===\n");
  printf("Verificar que:\n");
  printf("1. Los procesos con más tickets obtuvieron más CPU slices\n");
  printf("2. La proporción de CPU slices es aproximadamente proporcional a los tickets\n");
  printf("3. Proceso 1 (50 tickets) < Proceso 2 (100 tickets) < ... < Proceso 10 (500 tickets)\n");
  printf("4. Todos los procesos tuvieron oportunidad de ejecutar\n");
  
  printf("\nTickets asignados por proceso:\n");
  for(int i = 0; i < NUM_PROCESSES; i++) {
    printf("Proceso %d: %d tickets\n", i + 1, tickets[i]);
  }
  
  printf("\n=== DEMO COMPLETADO ===\n");
  
  exit(0);
}
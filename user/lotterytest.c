#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define DURATION 1000000  // Número de iteraciones para cada proceso

void
spin(int id, int tickets)
{
  printf("Proceso %d iniciado con %d tickets\n", id, tickets);
  
  // Establecer el número de tickets para este proceso
  settickets(tickets);
  
  // Verificar que se establecieron correctamente
  int actual_tickets = gettickets();
  int initial_slices = getcpuslices();
  printf("Proceso %d confirmado con %d tickets (solicitó %d), CPU slices iniciales: %d\n", 
         id, actual_tickets, tickets, initial_slices);
  
  int count = 0;
  int start_time = uptime();
  
  // Ejecutar trabajo computacional intensivo
  for(int i = 0; i < DURATION; i++) {
    count++;
    // Cada cierto número de iteraciones, imprimir progreso
    if(i % (DURATION/10) == 0) {
      printf("Proceso %d (%d tickets): progreso %d%%\n", id, tickets, (i * 100) / DURATION);
    }
  }
  
  int end_time = uptime();
  int runtime = end_time - start_time;
  int final_slices = getcpuslices();
  
  printf("Proceso %d TERMINADO: tickets=%d, iteraciones=%d, tiempo=%d ticks, CPU slices=%d\n", 
         id, tickets, count, runtime, final_slices);
  
  exit(0);
}

int
main(void)
{
  printf("=== TEST DE LOTTERY SCHEDULING ===\n");
  printf("Iniciando procesos con diferentes números de tickets\n");
  printf("Se espera que los procesos con más tickets obtengan más tiempo de CPU\n\n");
  
  int pid1, pid2, pid3;
  
  // Crear proceso con 10 tickets (pocos)
  if((pid1 = fork()) == 0) {
    spin(1, 10);
  }
  
  // Crear proceso con 50 tickets (medio)
  if((pid2 = fork()) == 0) {
    spin(2, 50);
  }
  
  // Crear proceso con 100 tickets (muchos)
  if((pid3 = fork()) == 0) {
    spin(3, 100);
  }
  
  // Proceso padre espera a todos los hijos
  printf("Proceso padre esperando a que terminen los hijos...\n");
  
  for(int i = 0; i < 3; i++) {
    wait(0);
    printf("Un proceso hijo terminó\n");
  }
  
  printf("\n=== TEST COMPLETADO ===\n");
  printf("Revisar los tiempos de ejecución y CPU slices:\n");
  printf("- Proceso 1 (10 tickets) debería tener menos CPU slices\n");
  printf("- Proceso 2 (50 tickets) debería tener CPU slices intermedios\n");
  printf("- Proceso 3 (100 tickets) debería tener más CPU slices\n");
  printf("La proporción de CPU slices debería ser aproximadamente 1:5:10\n");
  
  exit(0);
}
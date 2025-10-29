#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  printf("=== TEST DE ROBUSTEZ DEL SCHEDULER ===\n");
  
  int pid = getpid();
  printf("Proceso actual - PID: %d\n", pid);
  
  // Test 1: Intentar establecer tickets negativos
  printf("\nTest 1: Establecer tickets negativos (-10)\n");
  settickets(-10);
  int tickets = gettickets();
  printf("Resultado: %d tickets (debería ser 1)\n", tickets);
  
  // Test 2: Intentar establecer tickets cero
  printf("\nTest 2: Establecer tickets a cero (0)\n");
  settickets(0);
  tickets = gettickets();
  printf("Resultado: %d tickets (debería ser 1)\n", tickets);
  
  // Test 3: Establecer tickets muy altos
  printf("\nTest 3: Establecer tickets muy altos (999999)\n");
  settickets(999999);
  tickets = gettickets();
  printf("Resultado: %d tickets (debería ser 999999)\n", tickets);
  
  // Test 4: Crear proceso hijo con tickets bajos y altos
  printf("\nTest 4: Proceso hijo con tickets diferentes\n");
  
  int child_pid = fork();
  if(child_pid == 0) {
    // Proceso hijo
    settickets(1);
    int child_tickets = gettickets();
    int child_slices_start = getcpuslices();
    
    printf("Proceso hijo - PID: %d, Tickets: %d, CPU slices iniciales: %d\n", 
           getpid(), child_tickets, child_slices_start);
    
    // Hacer algo de trabajo
    for(int i = 0; i < 10000; i++) {
      volatile int x = i;
      (void)x; // Evitar warning
    }
    
    int child_slices_end = getcpuslices();
    printf("Proceso hijo terminando - CPU slices finales: %d (ganó %d)\n", 
           child_slices_end, child_slices_end - child_slices_start);
    
    exit(0);
  } else if(child_pid > 0) {
    // Proceso padre
    settickets(1000);
    int parent_tickets = gettickets();
    int parent_slices_start = getcpuslices();
    
    printf("Proceso padre - PID: %d, Tickets: %d, CPU slices iniciales: %d\n", 
           getpid(), parent_tickets, parent_slices_start);
    
    // Hacer trabajo mientras el hijo también trabaja
    for(int i = 0; i < 10000; i++) {
      volatile int x = i;
      (void)x; // Evitar warning
    }
    
    // Esperar al hijo
    wait(0);
    
    int parent_slices_end = getcpuslices();
    printf("Proceso padre terminando - CPU slices finales: %d (ganó %d)\n", 
           parent_slices_end, parent_slices_end - parent_slices_start);
    
    printf("\nEl proceso padre (1000 tickets) debería haber ganado más CPU slices que el hijo (1 ticket)\n");
  } else {
    printf("Error al crear proceso hijo\n");
    exit(1);
  }
  
  printf("\n=== TEST DE ROBUSTEZ COMPLETADO ===\n");
  printf("Verificar que:\n");
  printf("1. Los tickets negativos y cero se ajustaron a 1\n");
  printf("2. Los tickets muy altos se aceptaron\n");
  printf("3. El proceso con más tickets obtuvo más CPU time\n");
  
  exit(0);
}
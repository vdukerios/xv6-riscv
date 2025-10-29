#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void mostrar_info(void) {
  int pid = getpid();
  int tickets = gettickets();
  printf("PID: %d | TICKETS: %d\n", pid, tickets);
}

int
main(int argc, char *argv[])
{
  printf("=== PRUEBA DE LOTTERY TICKETS ===\n");
  
  printf("Estado inicial:\n");
  mostrar_info();
  
  printf("\n1. Configurando 50 tickets:\n");
  settickets(50);
  mostrar_info();
  
  printf("\n2. Configurando -10 tickets (debería ser 1):\n");
  settickets(-10);
  mostrar_info();
  
  printf("\n3. Configurando 0 tickets (debería ser 1):\n");
  settickets(0);
  mostrar_info();
  
  printf("\n4. Configurando 500 tickets:\n");
  settickets(500);
  mostrar_info();
  
  printf("\n5. Configurando 1 ticket:\n");
  settickets(1);
  mostrar_info();
  
  printf("\n=== PRUEBA COMPLETADA ===\n");
  exit(0);
}
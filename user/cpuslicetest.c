#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  printf("=== TEST DE GETCPUSLICES() ===\n");
  
  int pid = getpid();
  int tickets = gettickets();
  int slices = getcpuslices();
  
  printf("PID: %d | Tickets: %d | CPU Slices: %d\n", pid, tickets, slices);
  
  // Cambiar tickets
  printf("Cambiando a 200 tickets...\n");
  settickets(200);
  
  tickets = gettickets();
  slices = getcpuslices();
  
  printf("PID: %d | Tickets: %d | CPU Slices: %d\n", pid, tickets, slices);
  
  // Hacer algo de trabajo para generar más CPU slices
  printf("Haciendo trabajo computacional...\n");
  for(int i = 0; i < 100000; i++) {
    // Trabajo simple para consumir CPU
    if(i % 25000 == 0) {
      slices = getcpuslices();
      printf("Iteración %d - CPU Slices: %d\n", i, slices);
    }
  }
  
  tickets = gettickets();
  slices = getcpuslices();
  
  printf("FINAL - PID: %d | Tickets: %d | CPU Slices: %d\n", pid, tickets, slices);
  
  exit(0);
}
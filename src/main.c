#include <stdlib.h>
#include <stdio.h>

/*
- to get the interface list read from proc/net/dev for network interface data, if cannot be read then read from sys/class/net
- VnStat:
- 	handleifselection(&p) in VnStat to select the interface to be read
- 	ifcount = getiflist(&ifl, 0, 1) - to populat the linked list of pointers to the iflist structs


- struct for iflist:
  typedef struct iflist {
    char interface[MAXIFLEN];
    int64_t id;
    uint32_t bandwidth;
    struct iflist *next;
  } iflist;


*/

#define SYSCLASSNET "/sys/class/net"
#define PROCNETDEV "proc/net/dev"

int main() {

}
// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include "inet.h"

char * get_ip() {

  char hostbuffer[256];

  if (gethostname(hostbuffer, sizeof(hostbuffer)) < 0){
      perror("gethostname");
      return NULL;
  }

  struct hostent * host_entry = gethostbyname(hostbuffer);
  if (host_entry == NULL){
      perror("gethostbyname");
      return NULL;
  }

  char * IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
  if (IPbuffer == NULL){
      perror("inet_ntoa");
      return NULL;
  }

  return strdup(IPbuffer);
}

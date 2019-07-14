/* un serveur TCP qui reçoit le Bonjour du client 
*  Auteurs: emery.assogba@gmail.com
*  Abomey-Calavi 09 Juillet 2019
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MY_ADDR ""
#define MY_PORT 56789
#define LISTEN_BACKLOG 50

int main(int argc, char *argv[]){
   FILE* fichier = NULL;
   char line[100];
   int sfd; // socket du serveur
   int cfd; // socket du client
   struct sockaddr_in my_addr; // socket addr du serveur 
   struct sockaddr_in peer_addr; // socket addr d'un client

   socklen_t peer_addr_size; // la taille du sock
   pid_t child;

   char buffer[10]; // pour recevoir la salutation du client

   sfd = socket(AF_INET, SOCK_STREAM, 0); // création de la socket
   if (sfd < 0){ // Une erreur s'est produite la socket n'a pas pu être créer
      printf("Le SE n'a pas pu créer la socket %d\n", sfd);
      exit(-1);
   }

   /* La structure est remise à 0 car elle doit servir à stocker
    * l'identité du serveur*/
   memset(&my_addr, 0, sizeof(struct sockaddr_in));
   
    /* donne une identite a la socket. */
   my_addr.sin_family = AF_INET;
   my_addr.sin_port = htons (MY_PORT);
   inet_aton(MY_ADDR, (struct in_addr *)&my_addr.sin_addr.s_addr);
  
   /* on demande au SE de confirmer l'identité de la socket
    * Cela autorise le SE d'exploitation à forwarder les requête
    * Sur le port 56789 à ce processus */
   if (bind(sfd, (struct sockaddr *) &my_addr,
                   sizeof(struct sockaddr_in)) < 0){
       printf("bind error\n"); // l'association a echouée
       exit(-1);
  }

   /* on demande au SE de définir le nombre de clients 
    * que le serveur peut 
    * traiter en même temps dans sa file d'attente */
   if (listen(sfd, LISTEN_BACKLOG) < -1)
               perror("listen\n");

   // on se bloque en attendant les connexion des client
   peer_addr_size = sizeof(struct sockaddr_in);
   while(1){
         cfd = accept(sfd, (struct sockaddr *) &peer_addr,
                   &peer_addr_size);  
        if (cfd < 0){
            perror("accept\n");
            exit(-1); // Une erreur s'est produite
        }
        /* Nous créons un fils pour gérer ce client */
        child = fork();
        if(child < 0){ // le fils n'a pas pu être créé
           perror("errreur de création du fils\n");
        }
        if(child==0){
             /* Nous sommes dans le fils nous attendons la requête du client */
             printf("indentité du client %d\n", peer_addr.sin_port);
             /*Lecture des donnees envoyées par le client*/

		read(cfd, buffer, 10);
			printf("ok\n");
			if((fichier = fopen(buffer, "w")) != NULL)
				printf("Fichier %s bien creer", buffer);
		

             while(read(cfd, buffer, 10))
		fputs(buffer,fichier);

             printf("Fin de creation\n");
             /*Fin du traitement le fils se termine*/
             close(sfd);
             break;
        }

        else{
          /*Dans le père: le père attent un autre client*/
          close(cfd);
        }
  }
fclose(fichier);

}

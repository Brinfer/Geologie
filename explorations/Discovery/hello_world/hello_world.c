// SPDX-identifier: GPL-2.0
/*
 * Copyright (C) STMicroelectronics SA 2018
 *
 * Authors: Jean-Christophe Trotin <jean-christophe.trotin@st.com>
 * modifié par : Nathan BRIENT
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) //argc=nb arguments et argv = ce qu'on passe en argument premier argument est le nom du bianaire
{
	int i =11;

	printf("\nHello World, bienvenue/Welcome\n");
	printf("compte à rebours\n");
	setbuf(stdout,NULL); //libération mémoire et buffer
	while (i--) {
		printf("%i ", i);
		sleep(1);
	}
	printf("\nAu Revoir et à Bienôt\n");

	return(0);
}

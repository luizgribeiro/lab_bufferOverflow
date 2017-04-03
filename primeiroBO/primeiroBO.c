/*Autor: Luiz Guilherme Ribeiro

parâmetros de compilação: -g -m32 -o primeiroBO.o
*/

#include <stdio.h>
#include <unistd.h>

int main( int argc, char *argv[] ){

	volatile int tested;
	char buffer[65];

	tested=1;

	gets(buffer);


	if( tested==1 ) printf("O valor não foi modificado\n");
	else if ( tested==0 ) printf("A variável foi modificada de maneira inesperada. Novo valor: 0x%x\n", tested);
	else printf("A variável foi corrompida pelo buffer propositalmente. Novo valor: 0x%x\n", tested);

	

	return 0;
}

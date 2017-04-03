//programa que retorna para lib c

char param[]="/bin/sh";

void called (){
	int *p = &p;

	p[2]=0xf7e27f40;	//coloca no endereco do rip e endereco da funcao system()
	p[3]=0xf7e1b940;	//coloca no endereco que será rip de system() o endereço de exit 
	p[4]=param;			//coloca o endereco de da string /bin/sh na pilha (parametro para system())
}

int main (){

	called();
	return 0;
}

# Visão geral de buffer overflows
### uma abordagem prática

### Motivação:

Por vezes vi amigos falando que aprender assembly era bobagem, que não agregava nada. Graças a isso, junto ao professor Paulo Aguiar em minha iniciação científica desenvolvemos um projeto sobre buffer overflows que resultou nesse material, como forma de incentivar os alunos a intenderem o que de fato está acontecendo no computador.

### Introdução:

### Histórico:

Buffer overflow é um tipo de vulnerabilidade conhecida desde o começo dos anos 90, com o primeiro artigo de destaque sobre o tema divulgado na phrack magazine por Aleph1 em 1996. De uma maneira geral, todo problema gira em torno da execução de dados fornecidos pelo usuário. Mas antes de aprofundarmos um pouco mais nessa questão devemos definir alguns conceitos.

### Definições iniciais

* Buffer:
	* Uma região de memória contígua, responsável por armazenar determinada estrutura de dados.
* Overflow:
	* Ter o conteúdo transbordando ou derramando
* Buffer overflow:
	* Preencher um buffer além do seu limite, causando um transbordamento e, possivelmente, corrompendo outros dados contidos na memória.
* Pilha:
	* Área de memória de um processo que funciona da maneira FILO (first in last out). Nela, são armazenadas e trabalhadas as variáveis locais e quaisquer outras informações necessárias em tempo de execução.


Nesse ponto, devemos nos preparar para os laboratórios, executando o arquivo setup presente na home do usuário overflow. Ele desliga a randomização da memória, o que facilita a exploração dessas vulnerabilidades.
	

### Lab1: Seu primeiro buffer overflow

O laboratório está nomeado como PrimeiroBO. As seguintes questões são levantadas quanto ao código C presente no diretório de mesmo nome.

* O que o código C está fazendo?
	* Declarando duas variáveis (um inteiro e um ponteiro para um buffer);
	* Armazenando uma string dentro do buffer;
	* Verificando o conteúdo da variável inteira para que sejam exibidos diferentes prints.

* Qual a utilidade de volatile no código?
	* Volatile diz ao compilador o que não deve otimizar em caso algum. Nesse caso, se o compilador fosse "esperto" o suficiente, identificaria que o valor da variável não é alterado ao longo do código e otimizaria todas as comparações realizadas pelos ifs com o caso em que é considerado sempre verdade. Isso ocorreria quando tested igual a 1. Com isso, a mensagem "O valor não foi modificado" seria exibida sempre.

* Qual a função da pilha?
	* Nesse contexto, a pilha é responsável por armazenar a variável inteira tested, o ponteiro para string buffer e o buffer propriamente dito.

* Qual o tamanho de um char?
	* Através de um print da função sizeof, utilizando como parâmetro char, chegamos a conclusão que o tamanho de um char corresponde a 1 byte.

* Quantos chars são necessários para corromper a variável inteira de maneira proposital?
	* 69

### Fluxo de controle utilizando a pilha

Muitas vezes em um programa vemos uma chamada de função dentro de uma outra função. Como exemplo, temos o código abaixo, em que a função main chama a função printf duas vezes. No entando, sabendo que as funções estão em diferentes regiões de memória, devemos ter algum mecanismo para que a execução pule da função main para o primeiro printf, do printf para a main novamente, da main para o segundo printf e do segundo printf para a main finalmente. Esse controle de fluxo de código é feito através da pilha. A instrução de máquina CALL é responsável por colocar na pilha (PUSH) o que o registrador %eip aponta. O %eip por sua vez, armazena o endereço da próxima instrução a ser executada naquele escopo. Ao término da função chamada, a instrução RET é executada, e com isso, o endereço da próxima instrução é retirado da pilha (POP) e utilizado para a continuidade da execução do programa.


    #include <stdio.h>

    int main(){
	    printf("Olá mundo!");
	    printf("Tchau mundo!");
	    return 0;
    }


### Lab2: Revivendo Código morto

Como acabamos de ver, ao entrar em uma função é guardado um endereço de instrução a ser executada ao término da chamada de função. 
O intuito desse laboratório é mostrar como utilizando C, uma linguagem dita "perigosa" por deixar que algumas coisas como essa sejam feitas, podemos andar pela pilha, descobrir o local em que o endereço dessa próxima instrução está armazenado e modificá-lo para executar uma função que não havia sido chamada previamente no programa.

* Como declarar um ponteiro e atribuir a ele um endereço na pilha?
	* int *pointer = (int *) &pointer
* Como descobrir o endereço da instrução guardada?
	* Bem, nessa parte precisamos de um pouco de engenharia reversa. No entanto, consigo dizer exatamente onde ele estará pois nesse caso os passos da execução não mudarão. Sabendo que o ponteiro está 12 endereços abaixo da base da pilha. Então, o endereço de retorno estará no primeiro endereço acima da base da pilha dessa forma, a 16 acima do nosso ponteiro (contamos endereços de 4 em 4 para 32bits). como estamos trabalhando em C, devemos levar em conta a aritmética de ponteiros. Isso quer dizer que para acessarmos essa área de memória basta utilizarmos pointer[4] ou *(pointer + 4).
* O que colocar nesse endereço?
	* Se queremos "reviver" deadCode, colocaríamos ali o endereço de deadCode (p[4] = &deadCode). Tudo certo. No entanto, utilizaremos a distância estática entre diferentes áreas de código em um mesmo processo para realizar essa tarefa. Dessa forma, com o gdb:
		* disas main descobrimos o endereço da próxima instrução após call (0x08048454)
		* disas deadCode descobrimos o endereços da primeira instrução de deadCode (0x080483fb)	
		* então, se estamos em 0x08048454 e queremos ficar em 0x080483fb, subtraímos 0x59 unidades nessa região (p[4] -= 0x59).


### As funções escondidas do seu programa: 

No processo de link de qualquer binário criado com o gcc diversas funções são adicionadas ao programa para que ele funcione. Algumas delas já estarão carregadasno binário final enquanto outras serão carregadas em tempo de execução (dinamicamente). 
A libc é uma biblioteca adicionada nesse processo que contém duas funções em especial: system e exit.
Diversas vezes o objetivo de um atacante é conseguir uma shell (remota ou não) para executar comandos.
Com o auxílio da função system é possível executar comandos em uma shell e até abrir uma. A utilidade da função exit é encerrar a execução do programa sem deixar rastros de uma execução mal sucedida.

### Lab3: Retornando para funções da libc

O primeiro passo nesse laboratório é observar o local em que o rip (return instruction pointer), criado pelo desvio de fluxo com CALL, está. A partir disso, devemos colocar nessa ordem na pilha os endereços de system() e exit() e os endereço do parâmetro para system ("/bin/sh").

*   Como descobrir o endereço de system e exit?
    *   Primeiro, no gdb, devemos colocar um breakpoint na função main e rodar o programa até que esse ponto de parada seja alcançado para que as funções system e exit sejam carregadas (b main)(run). Após isso, os endereços das funções devem ser encontrados  com o comando p <nome da função> (print). 
*   Como modificar o código?
    *   A análise do código de montagem mostra que o endereço de system deve ser colocada em p[2], o endereço de exit em p[3] e o endereço do parâmetro em p[4].


### O próximo passo - Shellcode:

Shellcode é um termo utilizado para identificar código injetado em um buffer que ao ser executado fornece uma shell (o grande propósito é esse). Ele é formado por instruções de máquina, mas é um pouco mais restritivo por conta de ser injetado em um buffer.
Ele pode ser feito tanto na mão (utilizando C ou assembly) e tratando de todas as suas particularidades manualmente, quanto fornecido por uma framework como Veil.
A maior parte deles visa conseguir uma shell reversa ou uma bind shell para que o atacante tenha acesso ao sistema e consiga operá-lo.

### O clássico buffer overflow

De uma forma geral, o caso clássico de buffer overflow consiste em injetar código de máquina (bytecode/shellcode) em um buffer, estourar sua fronteira e sobrescrever o endereço da instrução de retorno (rip) com o endereço inicial do buffer, para que o código injetado seja executado. Entretanto, ao longo do tempo os sistemas operacionais implementaram algumas proteções que dificultam a exploração desse tipo de vulnerabilidade.


### Técnicas de proteção dos sistemas operacionais

*   Canários de pilha:
    *   Canários são mecanismos de verificar a integridade dos dados guardados na pilha. Assim como antigamente, nas minas, os canários eram utilizados como alarme (se o canário morresse/desmaiasse todos os mineiros saíam dela pois o nível de oxigênio estava baixo de mais), em computação os canários são posicionados ao final de um buffer. Caso o buffer seja estourado, o canário será "morto" e a execução do programa será abortada.
*   Randomização de pilha:
    *   O sistema operacional randomiza as áreas de memória (não utiliza a seguinte/vazia) para que fique mais difícil de sobrescrever o endereço da instrução de retorno (rip) com o endereço do primeiro elemento do buffer/primeira instrução do shellcode.
*   Área de memória não executável:
    *   Áreas de um processo como a pilha são marcadas como não executáveis. Assim, ao tentar executar qualquer instrução que tenha sido injetada em buffer, a execução será abortada.


### Técnicas contra as proteções

*   Randomização de pilha -> nop sled:
    * Consiste em colocar antes do shellcode uma série de nops. Essa instrução "queima" um ciclo de máquina. dessa forma, o espaço para "chutar" o começo do shellcode em um buffer fica muito maior e assim, mais fácil de ser acertado.
*   Área de mamória não executável -> return to libc:
    *   Como visto no lab3, vimos que é possível obter uma shell sem necessáriamente executar código injetado em um buffer. Dessa forma, a shell é fornecida e a execução não é abortada.   




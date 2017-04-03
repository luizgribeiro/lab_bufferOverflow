Devemos pegar os endereços das funções system() e exit() via GDB
b main
run 
p system
p exit

Além disso, é necessário desligar a randomização da pilha
echo 0 > /pro/sys/randomize\_va\_space

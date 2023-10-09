**Relatório de Trabalho Prático de Sistemas Operacionais**

Equipe de Desenvolvimento:

Integrantes: Paulo Gualberto, David Lanna e João Miguel

Descrição da participação de cada integrante do grupo no trabalho:

Obs.: Não foi possível a integração com o github e os commits que seriam
realizados.

Paulo Gualberto:

\- Revisar o código existente relacionado ao escalonamento e introduzir
a lógica necessária para que o sistema operacional pudesse ajustar as
prioridades dos processos em tempo real com base em fatores dinâmicos.

\- Detecção e resolução de problemas que surgiram durante a fase de
implementação.

\- Realização de testes na depuração das alterações no código.

David Lanna:

\- Implementar a política de escalonamento do usuário no Nanvix. Isso
envolveu a modificação das estruturas de dados do sistema para incluir
informações sobre as prioridades dos processos de usuário e garantir que
essas prioridades fossem consideradas durante o escalonamento.

\- Testar as alterações no Nanvix, garantindo que a nova política de
escalonamento funcionasse como esperado.

João Miguel:

\- Documentação

\- Implementação do times()

**Mudanças no Nanvix**

As mudanças implementadas no Nanvix incluíram a adição de duas novas
políticas de escalonamento: dinâmica e de usuário.

Todas as implementações solicitadas no trabalho foram realizadas com
sucesso. No entanto, ao testar as alterações nos arquivos que sofreram
modificações (pm.c, test.c, sched.c), enfrentamos diversos erros que
impossibilitaram a execução dos testes de desempenho. Essas questões
técnicas impediram a comparação do Nanvix original (sem as alterações)
com o Nanvix modificado (com as novas políticas de escalonamento).

PM.c

Inclusão de cabeçalhos: Os seguintes cabeçalhos foram incluídos:

#include \<sys/stat.h\>

#include \<signal.h\>

#include \<limits.h\>

Isso permite que o código acesse funções e constantes relacionadas a
estatísticas de arquivos, sinais e limites de sistema.

Adição de funções de teste: O código test.c foi incluído como parte das
mudanças. Ele contém funções de teste para avaliar o desempenho do
sistema.

Alterações em variáveis globais:

next_pid agora é incrementado ao criar o processo IDLE.

nprocs é incrementado ao criar o processo IDLE.

position_counter foi adicionado e é usado para atribuir posições aos
processos.

Foram feitas várias alterações nos atributos do processo IDLE, incluindo
configuração de prioridade.

Nova função set_priority: Foi adicionada a função set_priority para
definir a prioridade de um processo com base em sua posição na fila de
pronto.

Test.c

Inclusão de cabeçalhos: Os seguintes cabeçalhos foram incluídos:

#include \<sys/sem.h\>

#include \<signal.h\>

Isso permite o uso de semáforos e manipulação de sinais no código de
teste.

> Adição de funções de teste: Foram adicionadas funções de teste para
> avaliar o desempenho do sistema em várias situações, como testes de
> swap, I/O, escalonamento e outros.

Testes de desempenho: Foram adicionados testes para verificar o
desempenho do sistema, incluindo testes de swap, I/O, escalonamento, FPU
e semáforos.

Sched.c

Nova função yield: A função yield foi modificada para incluir a medição
de tempo. Agora, ela registra o tempo de início e término da função
times para calcular o tempo decorrido. Além disso, o tempo decorrido é
impresso no final da função.

Novas funções work_cpu e work_io: Foram adicionadas funções work_cpu e
work_io para simular trabalho intensivo em CPU e E/S

Testes de escalonamento: Foram adicionados testes de escalonamento que
criam processos com diferentes cargas de trabalho (CPU e E/S) e
verificam como o escalonador lida com eles.

Adição de funções relacionadas a semáforos: Foram incluídas funções para
criar, inicializar e destruir semáforos. Essas funções são usadas nos
testes de semáforos.

Testes de semáforos: Foram adicionados testes de semáforos para
verificar a comunicação entre processos usando semáforos.

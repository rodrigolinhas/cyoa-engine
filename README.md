# Choose Your Own Adventure engine

## Descrição do Projeto
Este projeto implementa um motor para jogos de aventura textual no estilo "Choose Your Own Adventure" em C, utilizando estruturas de dados avançadas como listas ligadas e grafos para representar a narrativa ramificada.
O programa foi desenvolvido como parte de um trabalho em grupo feito na cadeira LP em LEI na UALG

## Funcionalidades Principais
- **Sistema de cenas interligadas** com múltiplos caminhos narrativos
- **Três tipos de desfechos**: Vitória (WON), Derrota (FAILED) ou Jogo Incompleto (WAITING)
- **Parser de arquivos de história** com formato específico
- **Navegação interativa** pelas cenas do jogo
- **Gestão de memória eficiente** com alocação dinâmica

## Estrutura do Projeto
```
.
├── main/
│   ├── main.c           # Implementação principal do motor do jogo
├── tests/
│   ├── test.c           # Testes unitários
│   ├── historia.txt     # Exemplo de arquivo de história
│   ├── WON.txt          # Caso de teste com vitória
│   ├── FAILED.txt       # Caso de teste com derrota
│   └── ...              # Outros arquivos de teste
└── Makefile             # Sistema de build
```

## Como Compilar e Executar

### Pré-requisitos
- Compilador GCC
- Make

### Comandos Disponíveis
```bash
make all      # Compila o programa principal e os testes
make run      # Executa o programa com o arquivo historia.txt
make test     # Executa os testes unitários
make clean    # Remove arquivos criados
```

## Formato do Arquivo de História
O arquivo deve seguir este formato:
1. Primeira linha: número total de cenas (N)
2. Sequência de N cenas no formato:
   ```
   [ID]
   <<<DESCRIÇÃO>>>
   <TIPO>
   ***
   + X. Opção 1
   + Y. Opção 2
   ***
   ```
3. Sequência de escolhas do jogador

## Exemplo de Uso
```bash
make
make run  # Executa com historia.txt
```

## Algoritmos e Estruturas de Dados
- **Grafos direcionados** para representar a estrutura narrativa
- **Listas ligadas** para gestão das cenas
- **Enumerações** para estados do jogo (WON/FAILED/WAITING)
- **Alocação dinâmica** para eficiência de memória

## Testes
O projeto inclui testes para:
- Cenas normais e finais
- Limites de texto (MAXTXT)
- Número máximo de opções (MAXCHOICE)
- Casos de vitória/derrota
- Fluxos de jogo incompletos

Execute os testes com:
```bash
make test
```

## Contribuição

Este projeto em grupo foi desenvolvido como parte de um trabalho acadêmico. Contribuições externas não são esperadas, mas o código está disponível para estudo e referência.

## Licença
Este projeto é para fins educacionais e não possui licença específica.
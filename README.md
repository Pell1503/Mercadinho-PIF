# PDV Mercadinho (C)

Sistema de ponto de venda para mercadinho, em Linguagem C, rodando no terminal com
**todos os dados em memória** (sem arquivos).

## Funcionalidades
1. Cadastro de produtos (código automático, validação de nome/preço/estoque, nome único)
2. Entrada em estoque
3. Listagem (por código ou por nome) com alerta de estoque baixo/zerado
4. Consulta por código ou parte do nome
5. Venda com vários itens: soma itens repetidos, valida estoque, remove último item, cancela venda, calcula troco e imprime cupom
6. Relatório do dia: nº de vendas, unidades, faturamento, ticket médio, maior venda, troco, produtos vendidos, mais vendido, alertas de estoque
7. Carregar 8 produtos de exemplo (facilita testes e apresentação)

## Compilar e executar
```bash
make          # ou: gcc -std=c99 -Wall -Wextra -o pdv main.c
./pdv
make test     # executa os 13 casos de teste automatizados
```
No Windows (MinGW): `gcc -std=c99 -o pdv.exe main.c` e `pdv.exe`.

## Conteúdos aplicados
| Conteúdo | Onde |
|----------|------|
| Condicionais | `if/else` em validações; `switch` no menu |
| Repetição | `do-while` (menu), `for(;;)` (leitura/venda), `for` (listagens, buscas) |
| Vetores | `produtos[]`, `itens[]` (carrinho), `ord[]` |
| Funções | uma função por tarefa (cadastro, venda, relatório, leitura…) |
| struct | `Produto`, `ItemVenda`, `ResumoDia` |
| Ponteiros (bônus) | `Produto *buscarPorCodigo()`, `int *total`, `ItemVenda.produto`, vetor de ponteiros para ordenar |

## Decisões de projeto
- Valores em **centavos** (`long long`) para evitar erros de arredondamento de `float`.
- Leitura com `fgets` + `strtol/strtod` (nunca `scanf`), então letras no lugar de números não travam o programa.
- Mensagens sem acentos para funcionar em qualquer terminal (inclusive `cmd` do Windows).

## Estrutura do repositório
```
main.c              código-fonte
Makefile
README.md
relatorio.pdf       relatório do grupo (1–2 páginas)
roteiro_testes.md   roteiro com 13 casos
tests/              entradas (.in) e run_tests.sh
```

## Integrantes
- [Nome 1] – [matrícula]
- [Nome 2] – [matrícula]
- ...

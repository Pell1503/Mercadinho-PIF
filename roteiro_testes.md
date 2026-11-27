# Roteiro de Testes – PDV Mercadinho

**Como executar:** `make` e depois `make test` (roda os 13 casos automaticamente).
Para rodar um caso manualmente: `./pdv < tests/ct04.in`

> Em todos os casos (exceto CT01, CT02, CT11 e CT13) a opção **7** (carregar produtos de exemplo)
> é usada primeiro. Produtos de exemplo: 1001 Arroz 5kg R$ 25,90 (20 un.) · 1002 Feijao 1kg R$ 8,49 (30) ·
> 1003 Acucar 1kg R$ 4,59 (25) · 1004 Cafe 500g R$ 12,90 (15) · 1005 Leite Integral 1L R$ 5,49 (40) ·
> 1006 Biscoito Recheado R$ 2,99 (3) · 1007 Refrigerante 2L R$ 8,99 (12) · 1008 Sabao em Po 1kg R$ 18,90 (8).

| Caso | Objetivo | Entradas (resumo) | Resultado esperado | Arquivo |
|------|----------|-------------------|--------------------|---------|
| CT01 | Cadastro válido e listagem | Opção 1: `Arroz 5kg`, `25,90`, `10`; depois opção 3 | "Produto cadastrado com sucesso! Codigo: 1001"; listagem mostra R$ 25,90 e estoque 10 | `ct01.in` |
| CT02 | Validações do cadastro | Nome vazio; preço `abc`, `0`, `-3`; estoque `-1`, `x`; nome repetido (`leite` x `Leite`) | Mensagens de erro para cada entrada inválida; o programa pede novamente; nome repetido é recusado (sem diferenciar maiúsculas) | `ct02.in` |
| CT03 | Entrada em estoque | Código 1001 + 20 un.; código inexistente 9999; consulta 1001 | "Novo estoque de Arroz 5kg: 40"; "Produto nao encontrado"; consulta mostra estoque 40 | `ct03.in` |
| CT04 | Venda simples com troco | 2 × Arroz (1001); finalizar; pagar R$ 100 | Total R$ 51,80; troco R$ 48,20; estoque do arroz cai de 20 para 18 | `ct04.in` |
| CT05 | Estoque insuficiente, código inválido, venda vazia | 5 un. do 1006 (só tem 3); código 9999; finalizar sem itens | "Estoque insuficiente! Disponivel: 3"; "Produto nao encontrado"; "Venda sem itens" | `ct05.in` |
| CT06 | Vários itens, item repetido e remoção | 1001×1, 1002×2, 1001×1 (soma), `-2` (remove último), 1005×3; pagar R$ 200 | Linha do arroz vira "2 x"; "Item removido: Feijao 1kg"; total R$ 68,27; troco R$ 131,73 | `ct06.in` |
| CT07 | Cancelar venda | 1001×5; `-1`; listar | "Venda cancelada"; estoque do arroz continua 20 | `ct07.in` |
| CT08 | Pagamento insuficiente e depois exato | 2 × Café (R$ 25,80); pagar R$ 20 e depois R$ 25,80 | "Valor insuficiente! Faltam R$ 5,80"; depois troco R$ 0,00 | `ct08.in` |
| CT09 | Consulta por nome parcial e por código | `leite`; `zzz`; `1002` | Encontra "Leite Integral 1L"; "Nenhum produto encontrado"; mostra Feijao 1kg | `ct09.in` |
| CT10 | Relatório do dia (2 vendas) | Venda 1: 1001×2 + 1006×3 (R$ 60,77, paga R$ 100); Venda 2: 1002×1 (R$ 8,49, paga R$ 10); opção 6 | 2 vendas; 6 un.; faturamento R$ 69,26; ticket médio R$ 34,63; maior venda R$ 60,77; troco total R$ 40,74; mais vendido: Biscoito Recheado (3 un.); alerta: Biscoito [ZERADO] | `ct10.in` |
| CT11 | Relatório sem vendas | Opção 6 logo ao iniciar | 0 vendas; "Nenhuma venda registrada hoje" | `ct11.in` |
| CT12 | Listagem ordenada por nome | Opção 7; opção 3 → ordem 2 | "Acucar 1kg" aparece antes de "Arroz 5kg" | `ct12.in` |
| CT13 | Venda sem produtos | Opção 5 sem cadastrar nada | "Nenhum produto cadastrado..." | `ct13.in` |

## Cálculos conferidos manualmente
- CT04: 2 × 25,90 = 51,80 → 100,00 − 51,80 = **48,20**
- CT06: 2 × 25,90 + 3 × 5,49 = 51,80 + 16,47 = **68,27** → 200,00 − 68,27 = **131,73**
- CT10: (2×25,90 + 3×2,99) = 60,77; + 8,49 = **69,26**; ticket médio 69,26 / 2 = **34,63**; troco 39,23 + 1,51 = **40,74**

## Registro de execução (preencher pelo grupo)
| Caso | Passou? | Observações |
|------|---------|-------------|
| CT01 | | |
| CT02 | | |
| ... | | |

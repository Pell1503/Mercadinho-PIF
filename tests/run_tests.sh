#!/bin/bash
# Executa os casos de teste automaticamente.
# Uso: ./tests/run_tests.sh   (a partir da raiz do projeto, apos compilar: make)
cd "$(dirname "$0")/.." || exit 1
BIN=./pdv
[ -x "$BIN" ] || { echo "Compile primeiro: make"; exit 1; }

PASS=0; FAIL=0
# check <caso> <descricao> <regex-1> [<regex-2> ...]  (grep -E sobre a saida)
check() {
    local caso="$1" desc="$2"; shift 2
    local out; out=$("$BIN" < "tests/$caso.in" 2>&1)
    local ok=1
    for pat in "$@"; do
        if ! grep -Eq -- "$pat" <<< "$out"; then
            ok=0; echo "   faltou: $pat"
        fi
    done
    if [ $ok -eq 1 ]; then echo "[OK]   $caso - $desc"; PASS=$((PASS+1))
    else echo "[FALHA] $caso - $desc"; FAIL=$((FAIL+1)); fi
}

check ct01 "Cadastro valido e listagem" \
    'Produto cadastrado com sucesso! Codigo: 1001' \
    '1001 +Arroz 5kg +R\$ 25,90 +10$'
check ct02 "Validacoes do cadastro" \
    'O nome nao pode ser vazio' 'Entrada invalida' \
    'O valor deve ser de pelo menos R\$ 0,01' 'Valor fora do limite' \
    'Valor fora do intervalo' 'Erro: ja existe um produto com esse nome'
check ct03 "Entrada em estoque e consulta" \
    'Novo estoque de Arroz 5kg: 40' 'Produto nao encontrado' 'Estoque \.+: 40'
check ct04 "Venda simples com troco e baixa no estoque" \
    'TOTAL \.+: R\$ 51,80' 'PAGO \.+: R\$ 100,00' 'TROCO \.+: R\$ 48,20' \
    '1001 +Arroz 5kg +R\$ 25,90 +18$'
check ct05 "Estoque insuficiente / codigo inexistente / venda vazia" \
    'Estoque insuficiente! Disponivel: 3' 'Produto nao encontrado' \
    'Venda sem itens'
check ct06 "Venda com varios itens, soma de item repetido e remocao" \
    'Item removido: Feijao 1kg' '2 x +R\$ 25,90' \
    'TOTAL \.+: R\$ 68,27' 'TROCO \.+: R\$ 131,73'
check ct07 "Cancelamento nao altera o estoque" \
    'Venda cancelada' '1001 +Arroz 5kg +R\$ 25,90 +20$'
check ct08 "Pagamento insuficiente e depois exato" \
    'Valor insuficiente! Faltam R\$ 5,80' 'TROCO \.+: R\$ 0,00'
check ct09 "Consulta por nome parcial e por codigo" \
    'Leite Integral 1L' 'Nenhum produto encontrado para "zzz"' 'Nome \.+: Feijao 1kg'
check ct10 "Relatorio do dia com 2 vendas" \
    'Vendas realizadas \.+: 2' 'Unidades vendidas \.+: 6' \
    'Faturamento total \.+: R\$ 69,26' 'Ticket medio \.+: R\$ 34,63' \
    'Maior venda \.+: R\$ 60,77' 'Total de troco \.+: R\$ 40,74' \
    'Produto mais vendido: Biscoito Recheado \(3 un\.\)' \
    'Biscoito Recheado \(cod 1006\): 0 un\. \[ZERADO\]'
check ct11 "Relatorio sem vendas" \
    'Vendas realizadas \.+: 0' 'Nenhuma venda registrada hoje'
check ct13 "Venda sem produtos cadastrados" \
    'Nenhum produto cadastrado'

# ct12: ordem alfabetica (Acucar deve aparecer antes de Arroz)
out=$("$BIN" < tests/ct12.in 2>&1)
la=$(grep -n 'Acucar 1kg' <<< "$out" | tail -1 | cut -d: -f1)
lb=$(grep -n 'Arroz 5kg'  <<< "$out" | tail -1 | cut -d: -f1)
if [ -n "$la" ] && [ -n "$lb" ] && [ "$la" -lt "$lb" ]; then
    echo "[OK]   ct12 - Listagem ordenada por nome"; PASS=$((PASS+1))
else
    echo "[FALHA] ct12 - Listagem ordenada por nome"; FAIL=$((FAIL+1))
fi

echo; echo "Resultado: $PASS passaram, $FAIL falharam."
[ $FAIL -eq 0 ]

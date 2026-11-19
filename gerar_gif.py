"""
Gera um GIF animado comparando a árvore de recursão de fib(n)
na versão ingênua e na versão com memoização.

Requisito:  pip install pillow
Executar:   python gerar_gif.py        (gera fibonacci.gif com n = 5)
"""
from PIL import Image, ImageDraw, ImageFont

N = 5
W, H = 1080, 1080
FUNDO = "#0f172a"
TEXTO = "#e2e8f0"
SUAVE = "#94a3b8"
CORES = {
    "novo": "#3b82f6",      # subproblema calculado pela 1ª vez
    "repetido": "#ef4444",  # subproblema recalculado (desperdício)
    "base": "#64748b",      # caso base fib(0) / fib(1)
    "cache": "#22c55e",     # resposta veio da memória
}


def fonte(tam, negrito=False):
    nome = "DejaVuSans-Bold.ttf" if negrito else "DejaVuSans.ttf"
    try:
        return ImageFont.truetype(nome, tam)
    except OSError:
        return ImageFont.load_default()


F_TITULO, F_SUB, F_NO, F_LEG = fonte(46, True), fonte(30), fonte(24, True), fonte(24)


# ---------- monta as árvores (lista de nós na ordem em que são chamados) ----------
def arvore_ingenua(n):
    nos, vistos = [], set()

    def rec(k, pai, prof):
        idx = len(nos)
        tipo = "base" if k <= 1 else ("repetido" if k in vistos else "novo")
        vistos.add(k)
        nos.append({"k": k, "pai": pai, "prof": prof, "tipo": tipo, "filhos": []})
        if pai is not None:
            nos[pai]["filhos"].append(idx)
        if k > 1:
            rec(k - 1, idx, prof + 1)
            rec(k - 2, idx, prof + 1)

    rec(n, None, 0)
    return nos


def arvore_memo(n):
    nos, memo = [], set()

    def rec(k, pai, prof):
        idx = len(nos)
        if k <= 1:
            tipo = "base"
        elif k in memo:
            tipo = "cache"
        else:
            tipo = "novo"
        nos.append({"k": k, "pai": pai, "prof": prof, "tipo": tipo, "filhos": []})
        if pai is not None:
            nos[pai]["filhos"].append(idx)
        if tipo == "novo":
            rec(k - 1, idx, prof + 1)
            rec(k - 2, idx, prof + 1)
            memo.add(k)

    rec(n, None, 0)
    return nos


def posicionar(nos, topo=230, base=900):
    """x: folhas igualmente espaçadas, pais no meio dos filhos; y: pela profundidade."""
    folhas = [i for i, no in enumerate(nos) if not no["filhos"]]
    passo = (W - 120) / len(folhas)
    for j, i in enumerate(folhas):
        nos[i]["x"] = 60 + passo * (j + 0.5)
    for i in reversed(range(len(nos))):
        if nos[i]["filhos"]:
            xs = [nos[f]["x"] for f in nos[i]["filhos"]]
            nos[i]["x"] = sum(xs) / len(xs)
    prof_max = max(no["prof"] for no in nos)
    for no in nos:
        no["y"] = topo + (base - topo) * no["prof"] / max(prof_max, 1)


# ---------- desenho ----------
def centralizado(d, y, texto, f, cor=TEXTO):
    largura = d.textlength(texto, font=f)
    d.text(((W - largura) / 2, y), texto, font=f, fill=cor)


def legenda(d, itens):
    x = 70
    for tipo, rotulo in itens:
        d.rounded_rectangle([x, 1000, x + 26, 1026], 6, fill=CORES[tipo])
        d.text((x + 36, 1000), rotulo, font=F_LEG, fill=SUAVE)
        x += 50 + d.textlength(rotulo, font=F_LEG)


def quadro(nos, visiveis, titulo, sub, itens_leg):
    img = Image.new("RGB", (W, H), FUNDO)
    d = ImageDraw.Draw(img)
    centralizado(d, 50, titulo, F_TITULO)
    centralizado(d, 120, sub, F_SUB, SUAVE)

    for i in range(visiveis):  # arestas primeiro
        p = nos[i]["pai"]
        if p is not None:
            d.line([(nos[p]["x"], nos[p]["y"]), (nos[i]["x"], nos[i]["y"])], fill="#334155", width=4)

    for i in range(visiveis):
        no = nos[i]
        rotulo = f"fib({no['k']})"
        lw = d.textlength(rotulo, font=F_NO) + 24
        x, y = no["x"], no["y"]
        borda = "#fde047" if i == visiveis - 1 else None  # destaca a chamada atual
        d.rounded_rectangle([x - lw / 2, y - 24, x + lw / 2, y + 24], 12,
                            fill=CORES[no["tipo"]], outline=borda, width=4 if borda else 0)
        d.text((x - lw / 2 + 12, y - 15), rotulo, font=F_NO, fill="white")

    legenda(d, itens_leg)
    return img


def gerar():
    frames, duracoes = [], []

    def add(img, ms):
        frames.append(img)
        duracoes.append(ms)

    # capa
    capa = Image.new("RGB", (W, H), FUNDO)
    d = ImageDraw.Draw(capa)
    centralizado(d, 400, f"Quantas chamadas para fib({N})?", fonte(56, True))
    centralizado(d, 500, "Recursão ingênua  vs  Programação Dinâmica", F_SUB, SUAVE)
    add(capa, 2200)

    # 1) ingênua
    nos = arvore_ingenua(N)
    posicionar(nos)
    leg = [("novo", "1ª vez"), ("repetido", "repetido"), ("base", "caso base")]
    for v in range(1, len(nos) + 1):
        add(quadro(nos, v, "Recursão ingênua", f"Chamadas: {v}", leg), 450)
    repetidos = sum(1 for no in nos if no["tipo"] == "repetido")
    add(quadro(nos, len(nos), "Recursão ingênua",
               f"{len(nos)} chamadas — {repetidos} subproblemas recalculados!", leg), 3000)

    # 2) memoização
    nos = arvore_memo(N)
    posicionar(nos, topo=230, base=760)
    leg = [("novo", "calcula e guarda"), ("cache", "veio do cache"), ("base", "caso base")]
    for v in range(1, len(nos) + 1):
        add(quadro(nos, v, "Com memoização", f"Chamadas: {v}", leg), 550)
    add(quadro(nos, len(nos), "Com memoização",
               f"{len(nos)} chamadas — cada subproblema resolvido 1 vez", leg), 3000)

    # fechamento
    fim = Image.new("RGB", (W, H), FUNDO)
    d = ImageDraw.Draw(fim)
    centralizado(d, 300, "E para fib(40)?", fonte(56, True))
    centralizado(d, 440, "Ingênua: 331.160.281 chamadas", fonte(44, True), CORES["repetido"])
    centralizado(d, 520, "Memoização: 79 chamadas", fonte(44, True), CORES["cache"])
    centralizado(d, 660, "O(2ⁿ)  →  O(n)", fonte(52, True))
    centralizado(d, 760, "Não recalcule: lembre.", F_SUB, SUAVE)
    add(fim, 4000)

    frames[0].save("fibonacci.gif", save_all=True, append_images=frames[1:],
                   duration=duracoes, loop=0, optimize=True)
    print(f"fibonacci.gif gerado com {len(frames)} quadros.")


if __name__ == "__main__":
    gerar()
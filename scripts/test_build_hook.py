# scripts/test_build_hook.py
#
# TEST INFRAESTRUTURA (SPRINT TEST-INFRA-01)
# =====================================================================
# O test runner native do PlatformIO compila apenas os arquivos em test/
# e NÃO anexa as bibliotecas de lib_extra_dirs (.a) aos executáveis de
# teste, causando "undefined reference" em todos os testes (test_fault,
# test_self_test, test_storage).
#
# Este hook customizado adiciona as fontes de produção
# (core/, system/, communication/, lib/) ao build de teste via SCons,
# resolvendo todos os símbolos sem duplicar código e sem alterar a
# arquitetura em camadas.
#
# Requisitos atendidos:
#   - Não modifica o código de produção (core/ etc.).
#   - Não altera FaultManager / ProtectionManager / FaultStorage.
#   - Não desabilita testes nem ignora símbolos indefinidos.
#   - Não usa caminhos absolutos da máquina do usuário.
#   - Funciona no Windows.
#   - Usa apenas caminhos relativos ao projeto ($PROJECT_DIR).
# =====================================================================

from os.path import join, isdir, normpath
from os import walk

from SCons.Script import Import

Import("env")

# Caminho relativo ao projeto (sem caminhos absolutos da máquina do usuário).
project_dir = env.subst("$PROJECT_DIR")


def _collect_sources(dirs, extensions=(".c", ".cpp", ".cc")):
    """Coleta recursivamente os arquivos-fonte de produção."""
    sources = []
    for name in dirs:
        base = normpath(join(project_dir, name))
        if not isdir(base):
            continue
        for root, _, files in walk(base):
            for fname in files:
                if fname.endswith(extensions):
                    sources.append(normpath(join(root, fname)).replace("\\", "/"))
    return sources


# Diretórios-fonte de produção a expor ao build de teste.
PRODUCTION_DIRS = [
    "core",
    "system",
    "communication",
    "lib",
]

# Expõe os diretórios de produção de forma RECURSIVA para que o LDF e o
# build de teste compilem/linkem todos os módulos (fault, battery,
# protection, state, storage, logger, etc.) sem depender de globs não
# recursivos (que deixariam de fora subdiretórios como core/fault/).
env.AppendUnique(
    SRC_FILTER=[
        "+<../core/>",
        "+<../system/>",
        "+<../communication/>",
        "+<../lib/>",
    ]
)

# Garante que os includes de produção estejam disponíveis aos testes.
env.AppendUnique(
    CPPPATH=[
        project_dir,
        normpath(join(project_dir, "core")),
        normpath(join(project_dir, "system")),
        normpath(join(project_dir, "communication")),
        normpath(join(project_dir, "lib")),
    ]
)

# Força a coleta das fontes de produção para o link dos test executables.
if "test" in env.get("PIOENV", "").lower() or env.get("PIOPLATFORM") == "native":
    extra_sources = _collect_sources(PRODUCTION_DIRS)
    if extra_sources:
        # Adiciona as fontes de produção aos objetos do programa de teste,
        # resolvendo todos os símbolos (FaultManager, FaultRegistry,
        # ProtectionManager, SelfTest, StorageManager, crc32, etc.).
        env.AppendUnique(PROGSOURCES=extra_sources)

# scripts/test_build_hook.py
#
# TEST INFRAESTRUTURA (SPRINT TEST-INFRA/NATIVE)
# =====================================================================
# PROBLEMA:
#   O test runner native do PlatformIO compila apenas os arquivos em
#   `test_dir` (via `ProcessProjectDeps` -> `BuildSources("$BUILD_TEST_DIR",
#   "$PROJECT_TEST_DIR", "$PIOTEST_SRC_FILTER")`). As fontes de produção
#   (core/ system/ communication/ lib/) e os mocks de HAL (src/hal/)
#   NÃO são compilados/linkados, causando "undefined reference" para
#   símbolos reais (SelfTest::run, crc32, StorageManager, etc.).
#
# SOLUÇÃO (sem mascarar erros de link):
#   Este hook (pre) usa `env.BuildSources(...)` (mecanismo real do
#   PlatformIO) para adicionar os arquivos-fonte de produção ao build de
#   teste. O `BuildSources` anexa corretamente os objetos a
#   `PIOBUILDFILES` (que é o que o `BuildProgram` usa para linkar).
#
#   - NÃO modifica o código de produção.
#   - NÃO desabilita testes nem ignora símbolos indefinidos.
#   - Resolve os símbolos compilando as fontes reais.
#   - Usa apenas caminhos baseados em $PROJECT_DIR (independente de máquina).
#   - Funciona no Windows.
# =====================================================================

import os
from os.path import abspath, isdir, join, normpath

from SCons.Script import Import

Import("env")

project_dir = env.subst("$PROJECT_DIR")
build_dir = env.subst("$BUILD_DIR")


def _build_dirs(subdirs):
    """Compila recursivamente todos os .cpp de cada diretório de produção
    no build (via BuildSources), anexando os objetos a PIOBUILDFILES."""
    for rel in subdirs:
        src_dir = abspath(normpath(join(project_dir, rel)))
        if not isdir(src_dir):
            continue
        # Variant dir dedicado para não colidir com o build de teste.
        safe_name = rel.replace("/", "__").replace(os.sep, "__")
        variant_dir = join(build_dir, "bms_prod", safe_name)
        # BuildSources aplica o filtro padrão (+<*>) e compila .c/.cpp/.cc.
        env.BuildSources(variant_dir, src_dir)


# ---------------------------------------------------------------
# Diretórios de produção (camadas core/system/communication/lib).
# Incluímos TODOS para resolver qualquer símbolo usado pelos testes,
# sem duplicar código e sem alterar a arquitetura.
# ---------------------------------------------------------------
PRODUCTION_DIRS = [
    "core",
    "system",
    "communication",
    "lib",
]

# ---------------------------------------------------------------
# Mocks de HAL nativos usados por SelfTest / BatteryManager /
# HeartbeatManager (definem as globais: adc, currentSensor,
# temperatureSensor, storageDriver, can).
# ---------------------------------------------------------------
HAL_MOCK_DIRS = [
    "src/hal/adc",
    "src/hal/current",
    "src/hal/temperature",
    "src/hal/gpio",
]

# Garante os includes de produção disponíveis aos testes.
env.AppendUnique(
    CPPPATH=[
        project_dir,
        join(project_dir, "core"),
        join(project_dir, "system"),
        join(project_dir, "communication"),
        join(project_dir, "lib"),
        join(project_dir, "src"),
        join(project_dir, "config"),
    ]
)

# Se estamos num build de teste (env de teste define BUILD_TEST_DIR),
# adiciona as fontes de produção. O normal `pio run` não é afetado.
is_test_build = bool(env.get("PIOTEST_SRC_FILTER")) or env.GetProjectOption(
    "test_dir", ""
) or "test" in env.get("BUILD_TYPE", "")

if is_test_build:
    _build_dirs(PRODUCTION_DIRS)
    _build_dirs(HAL_MOCK_DIRS)


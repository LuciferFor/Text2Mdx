param(
    [string]$Config = "Release",
    [string]$Backend = "Auto"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Build = Join-Path $Root "build"

cmake -S $Root -B $Build -DTEXT2MDX_IMAGE_BACKEND=$Backend
cmake --build $Build --config $Config

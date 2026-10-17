param(
    [string]$OutputPath = (Join-Path $PSScriptRoot "CartRunner.zip")
)

$ErrorActionPreference = "Stop"

$projectRoot = $PSScriptRoot
$gameRoot = Join-Path $projectRoot "DinoGame"
$executablePath = Join-Path $gameRoot "build\CartRunner.exe"
$stagingDirectory = Join-Path ([System.IO.Path]::GetTempPath()) ("CartRunnerPackage-" + [guid]::NewGuid())
$archiveRoot = Join-Path $stagingDirectory "CartRunner"

if (-not (Test-Path -LiteralPath $executablePath -PathType Leaf)) {
    throw "CartRunner.exe was not found. Build the game before packaging it."
}

$requiredFiles = @(
    "CMakeLists.txt",
    "buildcmd.txt",
    "Project.md",
    "schedule.md",
    "package.ps1"
)

foreach ($relativePath in $requiredFiles) {
    $fullPath = Join-Path $projectRoot $relativePath
    if (-not (Test-Path -LiteralPath $fullPath -PathType Leaf)) {
        throw "Required package file is missing: $relativePath"
    }
}

try {
    $packagedGameRoot = Join-Path $archiveRoot "DinoGame"
    New-Item -ItemType Directory -Path (Join-Path $packagedGameRoot "src") -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $packagedGameRoot "include") -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $packagedGameRoot "build") -Force | Out-Null

    Copy-Item -Path (Join-Path $gameRoot "src\*.cpp") -Destination (Join-Path $packagedGameRoot "src")
    Copy-Item -Path (Join-Path $gameRoot "include\*.h") -Destination (Join-Path $packagedGameRoot "include")
    Copy-Item -LiteralPath $executablePath -Destination (Join-Path $packagedGameRoot "build\CartRunner.exe")

    $highScorePath = Join-Path $gameRoot "build\highscore.txt"
    if (Test-Path -LiteralPath $highScorePath -PathType Leaf) {
        Copy-Item -LiteralPath $highScorePath -Destination (Join-Path $packagedGameRoot "build\highscore.txt")
    }

    foreach ($relativePath in $requiredFiles) {
        Copy-Item -LiteralPath (Join-Path $projectRoot $relativePath) -Destination $archiveRoot
    }

    $outputDirectory = Split-Path -Parent $OutputPath
    if ($outputDirectory -and -not (Test-Path -LiteralPath $outputDirectory)) {
        New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
    }

    if (Test-Path -LiteralPath $OutputPath) {
        Remove-Item -LiteralPath $OutputPath -Force
    }

    Compress-Archive -LiteralPath $archiveRoot -DestinationPath $OutputPath -CompressionLevel Optimal

    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $archive = [System.IO.Compression.ZipFile]::OpenRead($OutputPath)
    try {
        # Compress-Archive can expose Windows separators through the .NET API.
        # Normalize them so validation behaves consistently across PowerShell versions.
        $entryNames = @($archive.Entries | ForEach-Object { $_.FullName.Replace('\', '/') })
        $expectedEntries = @(
            "CartRunner/DinoGame/build/CartRunner.exe",
            "CartRunner/DinoGame/src/main.cpp",
            "CartRunner/DinoGame/include/Game.h",
            "CartRunner/CMakeLists.txt",
            "CartRunner/Project.md"
        )

        foreach ($entryName in $expectedEntries) {
            if ($entryNames -notcontains $entryName) {
                throw "Archive validation failed; missing entry: $entryName"
            }
        }

        $archiveSize = (Get-Item -LiteralPath $OutputPath).Length
        Write-Output "Created $OutputPath"
        Write-Output "Validated $($archive.Entries.Count) entries ($archiveSize bytes)."
    }
    finally {
        $archive.Dispose()
    }
}
finally {
    if (Test-Path -LiteralPath $stagingDirectory) {
        Remove-Item -LiteralPath $stagingDirectory -Recurse -Force
    }
}

param (
    [string]$config = "Release",
    [switch]$generate = $false
)

$config_lower = $config.ToLower()
$build_path = "./build/${config_lower}"

if ($generate) {
    cmake -B $build_path -DCMAKE_BUILD_TYPE="$config"

    if ($? -eq $false) {
        exit 1
    }
}

cmake --build $build_path
$build_result = $?

# Copy compile_commands.json regardless of build result.
Copy-Item "$($build_path)/compile_commands.json" -Destination ./build

if ($build_result -eq $false) {
    exit 1
}

$bin_path = "./bin/${config_lower}"
New-Item -ItemType Directory -Path $bin_path -Force | Out-Null
Copy-Item -Force "${build_path}/eac-extractor.exe" -Destination $bin_path | Out-Null
Copy-Item -Force "${build_path}/_deps/curl-build/lib/libcurl.dll" -Destination $bin_path | Out-Null

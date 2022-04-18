# flake8: noqa
{
    'conditions': [
        ['OS=="win"', {
            'variables': {
                'VS_SDK%': '<!(node ./scripts/gyp-find-sdk.js)'
            },
            'conditions': [
                ['target_arch=="ia32"', {
                    'variables': {
                        'VS_Lib%': '<(VS_SDK)/lib32'
                    }
                }],
                ['target_arch=="x64"', {
                    'variables': {
                        'VS_Lib%': '<(VS_SDK)/lib64'
                    }
                }]
            ]
        }]
    ],
    "targets": [{
        'target_name': 'vapoursynthts',
        'sources': [
            "./src/lib/core.cpp", "./src/lib/pyscript.cpp",
            "./src/lib/plugin.cpp", "./src/lib/function.cpp",
            "./src/lib/formats.cpp",  # "./src/lib/props.cpp",
            "./src/lib/frames/rawframe.cpp", "./src/lib/frames/videoframe.cpp",  # "./src/lib/frames/audioframe.cpp",
            "./src/lib/nodes/rawnode.cpp", "./src/lib/nodes/videonode.cpp",  # "./src/lib/nodes/audionode.cpp",
            "./src/lib/utils/sharenodes.cpp"
        ],
        'include_dirs': [
            '<(VS_SDK)/include/vapoursynth',
            '<!@(node -p "require(\'node-addon-api\').include")'
        ],
        'dependencies': [
            '<!(node -p "require(\'node-addon-api\').gyp")'
        ],
        'conditions': [
            ["OS=='win'", {
                'libraries': ['-l<(VS_Lib)/vapoursynth.lib', '-l<(VS_Lib)/vsscript.lib']
            }, {
                'libraries': ['-lvapoursynth', '-lvsscript']
            }]
        ],
        'default_configuration': 'Release',
        'configurations': {
            'Release': {
                'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS', 'NDEBUG'],
                "cflags": ["-fno-exceptions", "-Ofast"],
                "cflags_cc": ["-fno-exceptions", "-Ofast", "-std=c++17"],
                "xcode_settings": {
                    'GCC_OPTIMIZATION_LEVEL': '3',
                    "CLANG_CXX_LIBRARY": "libc++",
                    "CLANG_CXX_LANGUAGE_STANDARD": "c++17",
                    'MACOSX_DEPLOYMENT_TARGET': '10.15'
                },
                "msvs_settings": {
                    "VCCLCompilerTool": {
                        'ExceptionHandling': 0,
                        'MultiProcessorCompilation': 'true',
                        'StringPooling': 'true',
                        'RuntimeTypeInfo': 'false',
                        'Optimization': 4,
                        'InlineFunctionExpansion': 3,
                        "AdditionalOptions": ["/std:c++17", "/Ob3", "/GL", "/DNDEBUG"],
                    }
                }
            },
            'Debug': {
                'defines': ['DEBUG', 'NAPI_CPP_EXCEPTIONS'],
                'cflags': ['-g', '-O0', '-ldl'],
                "cflags_cc": [
                    '-g',
                    '-ldl',
                    '-fexceptions',

                    # C++ standard
                    "-std=c++17", "-stdlib=libc++",

                    # Clang warnings
                    "-Wall",                     # all warnings
                    "-Wextra",                   # reasonable and standard
                    "-Wshadow",                  # warn the user if a variable declaration shadows one from a parent context
                    "-Wnon-virtual-dtor",        # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
                    "-Wold-style-cast",          # warn for c-style casts
                    "-Wcast-align",              # warn for potential performance problem casts
                    "-Wunused",                  # warn on anything being unused
                    "-Woverloaded-virtual",      # warn if you overload (not override) a virtual function
                    "-Wpedantic",                # warn if non-standard C++ is used
                    "-Wconversion",              # warn on type conversions that may lose data
                    "-Wsign-conversion",         # warn on sign conversions
                    "-Wnull-dereference",        # warn if a null dereference is detected
                    "-Wdouble-promotion",        # warn if float is implicit promoted to double
                    # warn on security issues around functions that format output (ie printf)
                    "-Wformat=2",

                    # GCC Warnings
                    "-Wmisleading-indentation",  # warn if indentation implies blocks where blocks do not exist
                    "-Wduplicated-cond",         # warn if if / else chain has duplicated conditions
                    "-Wduplicated-branches",     # warn if if / else branches have duplicated code
                    "-Wlogical-op",              # warn about logical operations being used where bitwise were probably wanted
                    "-Wuseless-cast",            # warn if you perform a cast to the same type
                ],
                "msvs_settings": {
                    "VCCLCompilerTool": {
                        'ExceptionHandling': 2,
                        'BasicRuntimeChecks': 3,
                        'MinimalRebuild': 'false',
                        'Optimization': 0,
                        "AdditionalOptions": [
                            "/DEBUG",

                            # C++ standard
                            "/std:c++17",

                            # MSVC Warnings
                            "/experimental:external",   # disable warnings on external headers
                            "/external:W0",
                            "/external:I <(VS_SDK)/include/vapoursynth;<!@(node -p \"require('node-addon-api').include\")",
                            "/external:anglebrackets",
                            "/Wall",                    # all warnings,
                            "/wd5044",                  # include path missing
                            "/wd4710",                  # function not inlined
                            "/wd4514",                  # unreferenced inline function has been removed
                            "/wd5045",                  # spectre mitigation insertion warning
                            "/wd4820",                  # 'bytes' bytes padding added after construct 'member_name'
                            "/wd4626",                  # assignment operator was implicitly defined as deleted
                            "/wd5027"                   # move operator was implicitly defined as deleted
                        ],
                    },
                },
                "xcode_settings": {
                    'GCC_OPTIMIZATION_LEVEL': '0',
                    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                    "CLANG_CXX_LIBRARY": "libc++",
                    "CLANG_CXX_LANGUAGE_STANDARD": "c++17",
                    'MACOSX_DEPLOYMENT_TARGET': '10.15'
                }
            },
        },
    }]
}

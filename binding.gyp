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
    'targets': [
        {
            'target_name': 'core',
            'sources': ["./src/lib/core.cpp", "./src/lib/function.cpp", "./src/lib/plugin.cpp", "./src/lib/nodes.cpp"],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions'],
            'conditions': [
                ["OS=='win'", {
                    'libraries': [
                        '-l<(VS_Lib)/vapoursynth.lib',
                        '-l<(VS_Lib)/vsscript.lib'
                    ]
                }, {
                    'libraries': [
                        '-lvapoursynth',
                        '-lvsscript'
                    ]
                }]
            ],
            'include_dirs': [
                '<(VS_SDK)/include/vapoursynth',
                '<!@(node -p "require(\'node-addon-api\').include")'
            ],
            'dependencies': [
                '<!(node -p "require(\'node-addon-api\').gyp")'
            ],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'msvs_settings': {
                'VCCLCompilerTool': {'ExceptionHandling': 1},
            }
        },
        {
            'target_name': 'pyscript',
            'sources': ["./src/lib/pyscript.cpp"],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions'],
            'conditions': [
                ["OS=='win'", {
                    'libraries': [
                        '-l<(VS_Lib)/vapoursynth.lib',
                        '-l<(VS_Lib)/vsscript.lib'
                    ]
                }, {
                    'libraries': [
                        '-lvapoursynth',
                        '-lvsscript'
                    ]
                }]
            ],
            'include_dirs': [
                '<(VS_SDK)/include/vapoursynth',
                '<!@(node -p "require(\'node-addon-api\').include")'
            ],
            'dependencies': [
                '<!(node -p "require(\'node-addon-api\').gyp")'
            ],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'msvs_settings': {
                'VCCLCompilerTool': {'ExceptionHandling': 1},
            }
        }
    ]
}

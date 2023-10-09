{   
    'variables': {
        'platform_and_arch': '<!(node -p "require(\'./platform\')()")',
    },
    'targets': [{
        'target_name': 'jxl',
        'cflags!': [ '-fno-exceptions' ],
        'cflags_cc!': [ '-fno-exceptions' ],
        'defines': [
            'NAPI_VERSION=7'
        ],
        'sources': [
            'src/main.cc',
            'src/wrapper.cc',
            'src/core.cc',
            'src/base/cache_aligned.cc',
            'src/base/data_parallel.cc',
            'src/base/padded_bytes.cc',
            'src/base/random.cc',
            'src/encode.cc',
            'src/exif.cc',
            'src/jpg.cc'
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            './libjxl/<(platform_and_arch)/include', './libjpeg/<(platform_and_arch)/include'
        ],
        'conditions': [
            ['OS == "win"', {
                'link_settings': {
                    'library_dirs': ['./libjxl/<(platform_and_arch)/lib', './libjpeg/<(platform_and_arch)/lib'],
                    'libraries': [
                        'brotlicommon.lib',
                        'brotlidec.lib',
                        'brotlienc.lib',
                        'jxl.lib',
                        'jxl_dec.lib',
                        'jxl_threads.lib',
                        'jpeg.lib',
                        'turbojpeg.lib'
                    ]
                },
            }],
            ['OS == "mac"', {
                'link_settings': {
                    'library_dirs': ['../libjxl/<(platform_and_arch)/lib', '../libjpeg/<(platform_and_arch)/lib'],
                    'libraries': [
                        'libjxl.0.8.dylib',
                        'libjxl_threads.0.8.dylib',
                        'libjpeg.62.4.0.dylib',
                    ]
                },
                'xcode_settings': {
                    'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
                    'MACOSX_DEPLOYMENT_TARGET': '13.2',
                    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                    'GCC_ENABLE_CPP_RTTI': 'YES',
                    'OTHER_CPLUSPLUSFLAGS': [
                        '-fexceptions',
                        '-Wall',
                        '-Oz'
                    ],
                    'OTHER_LDFLAGS': [
                        # Ensure runtime linking is relative to sharp.node
                        '-Wl,-rpath,\'@loader_path\'',
                        '-Wl,-rpath,\'@loader_path\''
                    ]
                },
            }],
        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
        'cflags_cc': [
            '-std=c++0x',
            '-fexceptions',
            '-Wall',
            '-Os'
        ],
    }]
}
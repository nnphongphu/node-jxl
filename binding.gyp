{
    "targets": [{
        "target_name": "libjxl",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        'defines': [
            'NAPI_VERSION=7'
        ],
        "sources": [
            "cppsrc/main.cc",
            "cppsrc/wrapper.cc",
            "cppsrc/core.cc",
            "cppsrc/base/cache_aligned.cc",
            "cppsrc/base/data_parallel.cc",
            "cppsrc/base/padded_bytes.cc",
            "cppsrc/base/random.cc",
            "cppsrc/encode.cc",
            "cppsrc/exif.cc",
            "cppsrc/jpg.cc"
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "libjxl/darwin_x86_64/include",
            "libjpeg/darwin_x86_64/include"
        ],
        'link_settings': {
            'library_dirs': ['../libjxl/darwin_x86_64/lib', '../libjpeg/darwin_x86_64/lib'],
            'libraries': [
                'libjxl.0.8.dylib',
                'libjxl_threads.0.8.dylib',
                'libjpeg.62.4.0.dylib',
            ]
        },
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
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
                '-Wl,-rpath,\'@loader_path\''
            ]
        },
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
        'cflags_cc': [
            '-std=c++0x',
            '-fexceptions',
            '-Wall',
            '-Os',
            '-Wno-psabi'
        ],
    }]
}
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
            "cppsrc/no_png.cc",
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
            "./libjxl/x86Clang/include",
            "./libjpeg/x86/include"
        ],
        'link_settings': {
            'library_dirs': ['./libjxl/x86Clang/lib', './libjpeg/x86/lib'],
            'libraries': [
                'brotlicommon.lib',
                'brotlidec.lib',
                'jxl_dec.lib',
                'jxl_threads.lib',
                'jpeg.lib',
                'turbojpeg.lib'
            ]
        },
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
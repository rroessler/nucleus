{
    "targets": [
        {
            "cflags": ["-std=c++17", "-stdlib=libc++", "-O2"],
            "cflags_cc": ["-std=c++17", "-stdlib=libc++", "-O2"],
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "include_dirs": ["<!(node -e \"require('nan')\")"],
            "target_name": "binding",
            "sources": ["main.cc"],
            "xcode_settings": {
                "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                "CLANG_CXX_LIBRARY": "libc++",
                "MACOSX_DEPLOYMENT_TARGET": "10.7",
                "OTHER_CFLAGS": ["-std=c++17", "-O2"],
            },
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "ExceptionHandling": 1,
                    "RuntimeTypeInfo": "true",
                    "AdditionalOptions": ["/std:c++17", "/O2"],
                }
            },
        }
    ]
}

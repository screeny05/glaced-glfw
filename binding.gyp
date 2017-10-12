{
    "targets": [
        {
            "target_name": "glfw",
            "sources": [
                "src/native/glfw.cc"
            ],
            "include_dirs": [
                "src/native/deps/include",
                "<!@(pkg-config glfw3 --cflags-only-I | sed s/-I//g)"
            ],
            "libraries": [
                "<!@(pkg-config --libs glfw3)",
            ],
            "library_dirs": [
                "/usr/local/lib"
            ]
        }
    ]
}

struct window_bridge_info {
    napi_env _env;

    napi_ref _errorCallbackReference;
    napi_ref _cursorPosCallbackReference;
    napi_ref _keyCallbackReference;
    napi_ref _framebufferSizeCallbackReference;
    napi_ref _mouseButtonCallbackReference;
    napi_ref _windowCloseCallbackReference;
    napi_ref _windowSizeCallbackReference;
}

_nativeCursorPosCallback(window){
    window_bridge_info* info = glfwGetWindowStuff(window);
    info->_cursorPosCallbackReference();
}

setCursorPosCallback(window, fn){
    window_bridge_info* info = glfwGetWindowStuff(window);
    info->_cursorPosCallbackReference = create_ref(fn);
    glfwSetCursorPosCallback(_nativeCursorPosCallback);
}

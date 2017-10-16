struct WindowUserInfo {
    napi_env env;

    napi_ref cbrefError;
    napi_ref cbrefWindowPos;
    napi_ref cbrefWindowSize;
    napi_ref cbrefWindowClose;
    napi_ref cbrefWindowRefresh;
    napi_ref cbrefWindowFocus;
    napi_ref cbrefWindowIconify;
    napi_ref cbrefFramebufferSize;
    napi_ref cbrefMouseButton;
    napi_ref cbrefCursorPos;
    napi_ref cbrefCursorEnter;
    napi_ref cbrefScroll;
    napi_ref* cbrefKey;
    napi_ref cbrefChar;
    napi_ref cbrefCharMods;
    napi_ref cbrefDrop;
    napi_ref cbrefJoystick;
    napi_ref cbrefMonitor;

    napi_ref userInfo;
};

_nativeCursorPosCallback(window){
    WindowUserInfo* info = glfwGetWindowStuff(window);
    info->_cursorPosCallbackReference();
}

setCursorPosCallback(window, fn){
    WindowUserInfo* info = glfwGetWindowStuff(window);
    info->_cursorPosCallbackReference = create_ref(fn);
    glfwSetCursorPosCallback(_nativeCursorPosCallback);
}

createWindow(){
    struct WindowUserInfo userInfo;
    window = glfwCreateWindow();
    glfwSetWindowUserPointer(window, &userInfo);
}

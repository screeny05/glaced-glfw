#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <node_api.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <common.h>

#include "scn_napi.h"

napi_env _env;

napi_ref _errorCallbackReference;
napi_ref _cursorPosCallbackReference;
napi_ref _keyCallbackReference;
napi_ref _framebufferSizeCallbackReference;
napi_ref _mouseButtonCallbackReference;
napi_ref _windowCloseCallbackReference;
napi_ref _windowSizeCallbackReference;

DECLARE_NAPI_METHOD(napi_makeContextCurrent){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwMakeContextCurrent(window);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_getCurrentContext){
    RETURN_NAPI_INT64((uint64_t)glfwGetCurrentContext());
}

DECLARE_NAPI_METHOD(napi_swapInterval){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(interval, 0);

    glfwSwapInterval(interval);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_extensionSupported){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_STRING(extension, 0);

    RETURN_NAPI_BOOL(glfwExtensionSupported(extension));
}

DECLARE_NAPI_METHOD(napi_getProcAddress){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_STRING(procname, 0);

    RETURN_NAPI_INT64((uint64_t)glfwGetProcAddress(procname));
}




DECLARE_NAPI_METHOD(napi_init){
    RETURN_NAPI_BOOL(glfwInit());
}

DECLARE_NAPI_METHOD(napi_terminate){
    glfwTerminate();
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_getVersion){
    int major;
    int minor;
    int rev;
    glfwGetVersion(&major, &minor, &rev);

    napi_value val;
    napi_value returnValue;

    NAPI_CALL(env, napi_create_object(env, &returnValue));

    NAPI_CALL(env, napi_create_int32(env, major, &val));
    NAPI_CALL(env, napi_set_named_property(env, returnValue, "major", val));

    NAPI_CALL(env, napi_create_int32(env, minor, &val));
    NAPI_CALL(env, napi_set_named_property(env, returnValue, "minor", val));

    NAPI_CALL(env, napi_create_int32(env, rev, &val));
    NAPI_CALL(env, napi_set_named_property(env, returnValue, "rev", val));

    return returnValue;
}

DECLARE_NAPI_METHOD(napi_getVersionString){
    RETURN_NAPI_STRING(glfwGetVersionString());
}

void napiGlfwErrorCallback(int error, const char* description){
    napi_value callbackArgs[2];
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, error, &callbackArgs[0]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_string_utf8(_env, description, -1, &callbackArgs[1]));
    printf("Error called: %d %s\n", error, description);

    CALL_NAPI_FUNCTION_GLFW_CALLBACK(_errorCallbackReference, 2, callbackArgs);
}

DECLARE_NAPI_METHOD(napi_setErrorCallback){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_FUNCTION(errorCallback, 0);

    NAPI_CALL(env, napi_create_reference(env, errorCallback, 1, &_errorCallbackReference));

    glfwSetErrorCallback(napiGlfwErrorCallback);
    RETURN_NAPI_UNDEFINED();
}



DECLARE_NAPI_METHOD(napi_getTime){
    double time = glfwGetTime();
    RETURN_NAPI_DOUBLE(time);
}

DECLARE_NAPI_METHOD(napi_createStandardCursor){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(shape, 0);

    GLFWcursor* cursor = glfwCreateStandardCursor(shape);
    RETURN_NAPI_INT64((uint64_t)cursor);
}

DECLARE_NAPI_METHOD(napi_destroyCursor){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_cursor, 0);
    GLFWcursor* cursor = (GLFWcursor*)_cursor;

    glfwDestroyCursor(cursor);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_getClipboardString){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    RETURN_NAPI_STRING(glfwGetClipboardString(window));
}

DECLARE_NAPI_METHOD(napi_getCursorPos){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    RETURN_NAPI_2NUMBER_OBJECT(xpos, ypos, napi_create_double);
}

DECLARE_NAPI_METHOD(napi_getInputMode){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_INT64(mode, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    RETURN_NAPI_INT32(glfwGetInputMode(window, mode));
}

DECLARE_NAPI_METHOD(napi_getJoystickAxes){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(joy, 0);

    int count;
    const float* values;

    values = glfwGetJoystickAxes(joy, &count);

    RETURN_NAPI_ARRAY_NUMBER(values, count, napi_create_double);
}

DECLARE_NAPI_METHOD(napi_getJoystickButtons){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(joy, 0);

    int count;
    const unsigned char* values;

    values = glfwGetJoystickButtons(joy, &count);

    RETURN_NAPI_ARRAY_NUMBER(values, count, napi_create_uint32);
}

DECLARE_NAPI_METHOD(napi_getJoystickName){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(joy, 0);

    RETURN_NAPI_STRING(glfwGetJoystickName(joy));
}

DECLARE_NAPI_METHOD(napi_getKey){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_INT64(key, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    RETURN_NAPI_INT32(glfwGetKey(window, key));
}

DECLARE_NAPI_METHOD(napi_getKeyName){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(key, 0);
    GET_NAPI_PARAM_INT64(scancode, 1);

    RETURN_NAPI_STRING(glfwGetKeyName(key, scancode));
}

DECLARE_NAPI_METHOD(napi_getMouseButton){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_INT64(button, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    RETURN_NAPI_INT32(glfwGetMouseButton(window, button));
}

DECLARE_NAPI_METHOD(napi_getTimerFrequency){
    RETURN_NAPI_INT64(glfwGetTimerFrequency());
}

DECLARE_NAPI_METHOD(napi_getTimerValue){
    RETURN_NAPI_INT64(glfwGetTimerValue());
}

DECLARE_NAPI_METHOD(napi_joystickPresent){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(joy, 0);

    RETURN_NAPI_BOOL(glfwJoystickPresent(joy));
}

DECLARE_NAPI_METHOD(napi_setClipboardString){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_STRING(content, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwSetClipboardString(window, content);

    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_setCursor){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_INT64(_cursor, 1);
    GLFWwindow* window = (GLFWwindow*)_window;
    GLFWcursor* cursor = (GLFWcursor*)_cursor;

    glfwSetCursor(window, cursor);

    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_setCursorPos){
    GET_NAPI_PARAMS_INFO(3);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_DOUBLE(xpos, 1);
    GET_NAPI_PARAM_DOUBLE(ypos, 2);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwSetCursorPos(window, xpos, ypos);

    RETURN_NAPI_UNDEFINED();
}

void napiGlfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos){
    napi_value callbackArgs[3];
    NAPI_CALL_RETURN_VOID(_env, napi_create_int64(_env, (uint64_t)window, &callbackArgs[0]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_double(_env, xpos, &callbackArgs[1]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_double(_env, ypos, &callbackArgs[2]));

    CALL_NAPI_FUNCTION_GLFW_CALLBACK(_cursorPosCallbackReference, 3, callbackArgs);
}

DECLARE_NAPI_METHOD(napi_setCursorPosCallback){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_FUNCTION(cursorPosCallback, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    NAPI_CALL(env, napi_create_reference(env, cursorPosCallback, 1, &_cursorPosCallbackReference));

    glfwSetCursorPosCallback(window, napiGlfwCursorPosCallback);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_setInputMode){
    GET_NAPI_PARAMS_INFO(3);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_DOUBLE(mode, 1);
    GET_NAPI_PARAM_DOUBLE(value, 2);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwSetInputMode(window, mode, value);

    RETURN_NAPI_UNDEFINED();
}

void napiGlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    napi_value callbackArgs[5];
    NAPI_CALL_RETURN_VOID(_env, napi_create_int64(_env, (uint64_t)window, &callbackArgs[0]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, key, &callbackArgs[1]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, scancode, &callbackArgs[2]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, action, &callbackArgs[3]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, mods, &callbackArgs[4]));

    CALL_NAPI_FUNCTION_GLFW_CALLBACK(_keyCallbackReference, 5, callbackArgs);
}

DECLARE_NAPI_METHOD(napi_setKeyCallback){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_FUNCTION(keyCallback, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    NAPI_CALL(env, napi_create_reference(env, keyCallback, 1, &_keyCallbackReference));

    glfwSetKeyCallback(window, napiGlfwKeyCallback);
    RETURN_NAPI_UNDEFINED();
}

void napiGlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    napi_value callbackArgs[4];
    NAPI_CALL_RETURN_VOID(_env, napi_create_int64(_env, (uint64_t)window, &callbackArgs[0]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, button, &callbackArgs[1]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, action, &callbackArgs[2]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, mods, &callbackArgs[3]));

    CALL_NAPI_FUNCTION_GLFW_CALLBACK(_mouseButtonCallbackReference, 4, callbackArgs);
}

DECLARE_NAPI_METHOD(napi_setMouseButtonCallback){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_FUNCTION(mouseButtonCallback, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    NAPI_CALL(env, napi_create_reference(env, mouseButtonCallback, 1, &_mouseButtonCallbackReference));

    glfwSetMouseButtonCallback(window, napiGlfwMouseButtonCallback);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_setTime){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_DOUBLE(time, 0);

    glfwSetTime(time);

    RETURN_NAPI_UNDEFINED();
}



DECLARE_NAPI_METHOD(napi_windowHint){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT32(hint, 0);
    GET_NAPI_PARAM_INT32(value, 1);

    glfwWindowHint(hint, value);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_createWindow){
    GET_NAPI_PARAMS_INFO(3);
    GET_NAPI_PARAM_INT32(width, 0);
    GET_NAPI_PARAM_INT32(height, 1);
    GET_NAPI_PARAM_STRING(title, 2);

    GLFWmonitor* monitor = NULL;
    GLFWwindow* share = NULL;

    if(argc >= 4){
        GET_NAPI_PARAM_INT64(_monitor, 3);
        monitor = (GLFWmonitor*)_monitor;
    }

    if(argc >= 5){
        GET_NAPI_PARAM_INT64(_share, 4);
        share = (GLFWwindow*)_share;
    }

    RETURN_NAPI_INT64((uint64_t)glfwCreateWindow(width, height, title, monitor, share));
}

DECLARE_NAPI_METHOD(napi_destroyWindow){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwDestroyWindow(window);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_windowShouldClose){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    RETURN_NAPI_BOOL(glfwWindowShouldClose(window));
}

DECLARE_NAPI_METHOD(napi_setWindowTitle){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_STRING(title, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwSetWindowTitle(window, title);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_getWindowSize){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    int width;
    int height;

    glfwGetWindowSize(window, &width, &height);
    RETURN_NAPI_2NUMBER_OBJECT(width, height, napi_create_int32);
}

DECLARE_NAPI_METHOD(napi_setWindowSize){
    GET_NAPI_PARAMS_INFO(3);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_INT32(width, 1);
    GET_NAPI_PARAM_INT32(height, 2);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwSetWindowSize(window, width, height);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_getFramebufferSize){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    int width;
    int height;

    glfwGetFramebufferSize(window, &width, &height);
    RETURN_NAPI_2NUMBER_OBJECT(width, height, napi_create_int32);
}

void napiGlfwWindowSizeCallback(GLFWwindow* window, int width, int height){
    napi_value callbackArgs[3];
    NAPI_CALL_RETURN_VOID(_env, napi_create_int64(_env, (uint64_t)window, &callbackArgs[0]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, width, &callbackArgs[1]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, height, &callbackArgs[2]));

    CALL_NAPI_FUNCTION_GLFW_CALLBACK(_windowSizeCallbackReference, 3, callbackArgs);
}

DECLARE_NAPI_METHOD(napi_setWindowSizeCallback){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_FUNCTION(windowSizeCallback, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    NAPI_CALL(env, napi_create_reference(env, windowSizeCallback, 1, &_windowSizeCallbackReference));

    glfwSetWindowSizeCallback(window, napiGlfwWindowSizeCallback);
    RETURN_NAPI_UNDEFINED();
}

void napiGlfwWindowCloseCallback(GLFWwindow* window){
    napi_value callbackArgs[1];
    NAPI_CALL_RETURN_VOID(_env, napi_create_int64(_env, (uint64_t)window, &callbackArgs[0]));

    CALL_NAPI_FUNCTION_GLFW_CALLBACK(_windowCloseCallbackReference, 1, callbackArgs);
}

DECLARE_NAPI_METHOD(napi_setWindowCloseCallback){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_FUNCTION(windowCloseCallback, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    NAPI_CALL(env, napi_create_reference(env, windowCloseCallback, 1, &_windowCloseCallbackReference));

    glfwSetWindowCloseCallback(window, napiGlfwWindowCloseCallback);
    RETURN_NAPI_UNDEFINED();
}

void napiGlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height){
    napi_value callbackArgs[3];
    NAPI_CALL_RETURN_VOID(_env, napi_create_int64(_env, (uint64_t)window, &callbackArgs[0]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, width, &callbackArgs[1]));
    NAPI_CALL_RETURN_VOID(_env, napi_create_int32(_env, height, &callbackArgs[2]));

    CALL_NAPI_FUNCTION_GLFW_CALLBACK(_framebufferSizeCallbackReference, 3, callbackArgs);
}


DECLARE_NAPI_METHOD(napi_setFramebufferSizeCallback){
    GET_NAPI_PARAMS_INFO(2);
    GET_NAPI_PARAM_INT64(_window, 0);
    GET_NAPI_PARAM_FUNCTION(framebufferSizeCallback, 1);
    GLFWwindow* window = (GLFWwindow*)_window;

    NAPI_CALL(env, napi_create_reference(env, framebufferSizeCallback, 1, &_framebufferSizeCallbackReference));

    glfwSetFramebufferSizeCallback(window, napiGlfwFramebufferSizeCallback);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_pollEvents){
    glfwPollEvents();
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_swapBuffers){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_window, 0);
    GLFWwindow* window = (GLFWwindow*)_window;

    glfwSwapBuffers(window);
    RETURN_NAPI_UNDEFINED();
}

DECLARE_NAPI_METHOD(napi_getMonitors){
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

    napi_value arrayValue;
    napi_value singleValue;
    napi_create_array_with_length(env, monitorCount, &arrayValue);
    for(int i = 0; i < monitorCount; i++){
        NAPI_CALL(env, napi_create_int64(env, (uint64_t)monitors[i], &singleValue));
        NAPI_CALL(env, napi_set_element(env, arrayValue, i, singleValue));
    }
    return arrayValue;
}

DECLARE_NAPI_METHOD(napi_getPrimaryMonitor){
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    RETURN_NAPI_INT64((uint64_t)monitor);
}

napi_value glfwVideoModeToNapiValue(napi_env env, const GLFWvidmode* mode){
    napi_value returnValue;
    napi_value singleValue;
    NAPI_CALL_BASE(env, napi_create_object(env, &returnValue), returnValue);
    NAPI_CALL_BASE(env, napi_create_int32(env, mode->redBits, &singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_set_named_property(env, returnValue, "redBits", singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_create_int32(env, mode->greenBits, &singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_set_named_property(env, returnValue, "greenBits", singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_create_int32(env, mode->blueBits, &singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_set_named_property(env, returnValue, "blueBits", singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_create_int32(env, mode->height, &singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_set_named_property(env, returnValue, "height", singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_create_int32(env, mode->width, &singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_set_named_property(env, returnValue, "width", singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_create_int32(env, mode->refreshRate, &singleValue), returnValue);
    NAPI_CALL_BASE(env, napi_set_named_property(env, returnValue, "refreshRate", singleValue), returnValue);
    return returnValue;
}

DECLARE_NAPI_METHOD(napi_getVideoModes){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_monitor, 0);
    GLFWmonitor* monitor = (GLFWmonitor*)_monitor;

    int videoModeCount;
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &videoModeCount);

    napi_value videoModesValue;
    napi_value videoModeValue;
    napi_create_array_with_length(env, videoModeCount, &videoModesValue);
    for(int i = 0; i < videoModeCount; i++){
        videoModeValue = glfwVideoModeToNapiValue(env, &modes[i]);
        NAPI_CALL(env, napi_set_element(env, videoModesValue, i, videoModeValue));
    }
    return videoModesValue;
}

DECLARE_NAPI_METHOD(napi_getVideoMode){
    GET_NAPI_PARAMS_INFO(1);
    GET_NAPI_PARAM_INT64(_monitor, 0);
    GLFWmonitor* monitor = (GLFWmonitor*)_monitor;

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    return glfwVideoModeToNapiValue(env, mode);
}

void onExit(){
    glfwTerminate();
}

void onSigInt(int dummy){
    exit(0);
}

napi_value Init(napi_env env, napi_value exports) {
    _env = env;

    napi_value val;

    atexit(onExit);
    signal(SIGINT, onSigInt);

    napi_property_descriptor properties[] = {
        // context
        DECLARE_NAPI_PROPERTY("makeContextCurrent", napi_makeContextCurrent),
        DECLARE_NAPI_PROPERTY("getCurrentContext", napi_getCurrentContext),
        DECLARE_NAPI_PROPERTY("swapInterval", napi_swapInterval),
        DECLARE_NAPI_PROPERTY("extensionSupported", napi_extensionSupported),
        DECLARE_NAPI_PROPERTY("getProcAddress", napi_getProcAddress),

        // init
        DECLARE_NAPI_PROPERTY("init", napi_init),
        DECLARE_NAPI_PROPERTY("terminate", napi_terminate),
        DECLARE_NAPI_PROPERTY("getVersion", napi_getVersion),
        DECLARE_NAPI_PROPERTY("getVersionString", napi_getVersionString),
        DECLARE_NAPI_PROPERTY("setErrorCallback", napi_setErrorCallback),

        // input
        //#DECLARE_NAPI_PROPERTY("createCursor", CreateCursor), - no GLFWImage support yet
        DECLARE_NAPI_PROPERTY("createStandardCursor", napi_createStandardCursor),
        DECLARE_NAPI_PROPERTY("getClipboardString", napi_getClipboardString),
        DECLARE_NAPI_PROPERTY("destroyCursor", napi_destroyCursor),
        DECLARE_NAPI_PROPERTY("getCursorPos", napi_getCursorPos),
        DECLARE_NAPI_PROPERTY("getInputMode", napi_getInputMode),
        DECLARE_NAPI_PROPERTY("getJoystickAxes", napi_getJoystickAxes),
        DECLARE_NAPI_PROPERTY("getJoystickButtons", napi_getJoystickButtons),
        DECLARE_NAPI_PROPERTY("getJoystickName", napi_getJoystickName),
        DECLARE_NAPI_PROPERTY("getKey", napi_getKey),
        DECLARE_NAPI_PROPERTY("getKeyName", napi_getKeyName),
        DECLARE_NAPI_PROPERTY("getMouseButton", napi_getMouseButton),
        DECLARE_NAPI_PROPERTY("getTime", napi_getTime),
        DECLARE_NAPI_PROPERTY("getTimerFrequency", napi_getTimerFrequency),
        DECLARE_NAPI_PROPERTY("getTimerValue", napi_getTimerValue),
        DECLARE_NAPI_PROPERTY("joystickPresent", napi_joystickPresent),
        //#DECLARE_NAPI_PROPERTY("setCharCallback", SetCharCallback), - TODO
        //#DECLARE_NAPI_PROPERTY("setCharModsCallback", SetCharModsCallback), - TODO
        DECLARE_NAPI_PROPERTY("setClipboardString", napi_setClipboardString),
        DECLARE_NAPI_PROPERTY("setCursor", napi_setCursor),
        //#DECLARE_NAPI_PROPERTY("setCursorEnterCallback", SetCursorEnterCallback), - TODO
        DECLARE_NAPI_PROPERTY("setCursorPos", napi_setCursorPos),
        DECLARE_NAPI_PROPERTY("setCursorPosCallback", napi_setCursorPosCallback),
        //#DECLARE_NAPI_PROPERTY("setDropCallback", SetDropCallback), - TODO
        DECLARE_NAPI_PROPERTY("setInputMode", napi_setInputMode),
        //#DECLARE_NAPI_PROPERTY("setJoystickCallback", SetJoystickCallback), - TODO
        DECLARE_NAPI_PROPERTY("setKeyCallback", napi_setKeyCallback),
        DECLARE_NAPI_PROPERTY("setMouseButtonCallback", napi_setMouseButtonCallback),
        //#DECLARE_NAPI_PROPERTY("setScrollCallback", SetScrollCallback), - TODO
        DECLARE_NAPI_PROPERTY("setTime", napi_setTime),

        // monitor
        DECLARE_NAPI_PROPERTY("getMonitors", napi_getMonitors),
        DECLARE_NAPI_PROPERTY("getPrimaryMonitor", napi_getPrimaryMonitor),
        //DECLARE_NAPI_PROPERTY("getMonitorPos", napi_getMonitorPos),
        //DECLARE_NAPI_PROPERTY("getMonitorPhysicalSize", napi_getMonitorPhysicalSize),
        //DECLARE_NAPI_PROPERTY("getMonitorName", napi_getMonitorName),
        //DECLARE_NAPI_PROPERTY("setMonitorCallback", napi_setMonitorCallback),
        DECLARE_NAPI_PROPERTY("getVideoModes", napi_getVideoModes),
        DECLARE_NAPI_PROPERTY("getVideoMode", napi_getVideoMode),
        //DECLARE_NAPI_PROPERTY("setGamma", napi_setGamma),
        //DECLARE_NAPI_PROPERTY("getGammaRamp", napi_getGammaRamp),
        //DECLARE_NAPI_PROPERTY("setGammaRamp", napi_setGammaRamp),

        // window
        //DECLARE_NAPI_PROPERTY("defaultWindowHints", napi_defaultWindowHints),
        DECLARE_NAPI_PROPERTY("windowHint", napi_windowHint),
        DECLARE_NAPI_PROPERTY("createWindow", napi_createWindow),
        DECLARE_NAPI_PROPERTY("destroyWindow", napi_destroyWindow),
        DECLARE_NAPI_PROPERTY("windowShouldClose", napi_windowShouldClose),
        //DECLARE_NAPI_PROPERTY("setWindowShouldClose", napi_setWindowShouldClose),
        DECLARE_NAPI_PROPERTY("setWindowTitle", napi_setWindowTitle),
        //DECLARE_NAPI_PROPERTY("setWindowIcon", napi_setWindowIcon),
        //DECLARE_NAPI_PROPERTY("getWindowPos", napi_getWindowPos),
        //DECLARE_NAPI_PROPERTY("setWindowPos", napi_setWindowPos),
        DECLARE_NAPI_PROPERTY("getWindowSize", napi_getWindowSize),
        //DECLARE_NAPI_PROPERTY("setWindowSizeLimit", napi_setWindowSizeLimit),
        //DECLARE_NAPI_PROPERTY("setWindowAspectRatio", napi_setWindowAspectRatio),
        DECLARE_NAPI_PROPERTY("setWindowSize", napi_setWindowSize),
        DECLARE_NAPI_PROPERTY("getFramebufferSize", napi_getFramebufferSize),
        //DECLARE_NAPI_PROPERTY("getWindowFrameSize", napi_getWindowFrameSize),
        //DECLARE_NAPI_PROPERTY("iconifyWindow", napi_iconifyWindow),
        //DECLARE_NAPI_PROPERTY("restoreWindow", napi_restoreWindow),
        //DECLARE_NAPI_PROPERTY("maximizeWindow", napi_maximizeWindow),
        //DECLARE_NAPI_PROPERTY("showWindow", napi_showWindow),
        //DECLARE_NAPI_PROPERTY("hideWindow", napi_hideWindow),
        //DECLARE_NAPI_PROPERTY("focusWindow", napi_focusWindow),
        //DECLARE_NAPI_PROPERTY("getWindowMonitor", napi_getWindowMonitor),
        //DECLARE_NAPI_PROPERTY("setWindowMonitor", napi_setWindowMonitor),
        //DECLARE_NAPI_PROPERTY("getWindowAttrib", napi_getWindowAttrib),
        //DECLARE_NAPI_PROPERTY("setWindowUserPointer", napi_setWindowUserPointer),
        //DECLARE_NAPI_PROPERTY("getWindowUserPointer", napi_getWindowUserPointer),
        //DECLARE_NAPI_PROPERTY("setWindowPosCallback", napi_setWindowPosCallback),
        DECLARE_NAPI_PROPERTY("setWindowSizeCallback", napi_setWindowSizeCallback),
        DECLARE_NAPI_PROPERTY("setWindowCloseCallback", napi_setWindowCloseCallback),
        //DECLARE_NAPI_PROPERTY("setWindowRefreshCallback", napi_setWindowRefreshCallback),
        //DECLARE_NAPI_PROPERTY("setWindowIconifyCallback", napi_setWindowIconifyCallback),
        DECLARE_NAPI_PROPERTY("setFramebufferSizeCallback", napi_setFramebufferSizeCallback),
        DECLARE_NAPI_PROPERTY("pollEvents", napi_pollEvents),
        //DECLARE_NAPI_PROPERTY("waitEvents", napi_waitEvents),
        //DECLARE_NAPI_PROPERTY("waitEventsTimeout", napi_waitEventsTimeout),
        //DECLARE_NAPI_PROPERTY("postEmptyEvent", napi_postEmptyEvent),
        DECLARE_NAPI_PROPERTY("swapBuffers", napi_swapBuffers)
    };

    NAPI_CALL(env, napi_define_properties(env, exports, 50, properties));



    // consts
    EXPORT_NAPI_CONST_NUMBER("VERSION_MAJOR", GLFW_VERSION_MAJOR);
    EXPORT_NAPI_CONST_NUMBER("VERSION_MINOR", GLFW_VERSION_MINOR);
    EXPORT_NAPI_CONST_NUMBER("VERSION_REVISION", GLFW_VERSION_REVISION);

    EXPORT_NAPI_CONST_NUMBER("NOT_INITIALIZED", GLFW_NOT_INITIALIZED);
    EXPORT_NAPI_CONST_NUMBER("NO_CURRENT_CONTEXT", GLFW_NO_CURRENT_CONTEXT);
    EXPORT_NAPI_CONST_NUMBER("INVALID_ENUM", GLFW_INVALID_ENUM);
    EXPORT_NAPI_CONST_NUMBER("INVALID_VALUE", GLFW_INVALID_VALUE);
    EXPORT_NAPI_CONST_NUMBER("OUT_OF_MEMORY", GLFW_OUT_OF_MEMORY);
    EXPORT_NAPI_CONST_NUMBER("API_UNAVAILABLE", GLFW_API_UNAVAILABLE);
    EXPORT_NAPI_CONST_NUMBER("VERSION_UNAVAILABLE", GLFW_VERSION_UNAVAILABLE);
    EXPORT_NAPI_CONST_NUMBER("PLATFORM_ERROR", GLFW_PLATFORM_ERROR);
    EXPORT_NAPI_CONST_NUMBER("FORMAT_UNAVAILABLE", GLFW_FORMAT_UNAVAILABLE);
    EXPORT_NAPI_CONST_NUMBER("NO_WINDOW_CONTEXT", GLFW_NO_WINDOW_CONTEXT);

    EXPORT_NAPI_CONST_NUMBER("PRESS", GLFW_PRESS);
    EXPORT_NAPI_CONST_NUMBER("RELEASE", GLFW_RELEASE);
    EXPORT_NAPI_CONST_NUMBER("REPEAT", GLFW_REPEAT);

    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_1", GLFW_JOYSTICK_1);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_2", GLFW_JOYSTICK_2);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_3", GLFW_JOYSTICK_3);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_4", GLFW_JOYSTICK_4);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_5", GLFW_JOYSTICK_5);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_6", GLFW_JOYSTICK_6);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_7", GLFW_JOYSTICK_7);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_8", GLFW_JOYSTICK_8);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_9", GLFW_JOYSTICK_9);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_10", GLFW_JOYSTICK_10);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_12", GLFW_JOYSTICK_12);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_13", GLFW_JOYSTICK_13);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_14", GLFW_JOYSTICK_14);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_15", GLFW_JOYSTICK_15);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_16", GLFW_JOYSTICK_16);
    EXPORT_NAPI_CONST_NUMBER("JOYSTICK_LAST", GLFW_JOYSTICK_LAST);

    EXPORT_NAPI_CONST_NUMBER("KEY_UNKNOWN", GLFW_KEY_UNKNOWN);
    EXPORT_NAPI_CONST_NUMBER("KEY_SPACE", GLFW_KEY_SPACE);
    EXPORT_NAPI_CONST_NUMBER("KEY_APOSTROPHE", GLFW_KEY_APOSTROPHE);
    EXPORT_NAPI_CONST_NUMBER("KEY_COMMA", GLFW_KEY_COMMA);
    EXPORT_NAPI_CONST_NUMBER("KEY_MINUS", GLFW_KEY_MINUS);
    EXPORT_NAPI_CONST_NUMBER("KEY_PERIOD", GLFW_KEY_PERIOD);
    EXPORT_NAPI_CONST_NUMBER("KEY_SLASH", GLFW_KEY_SLASH);
    EXPORT_NAPI_CONST_NUMBER("KEY_0", GLFW_KEY_0);
    EXPORT_NAPI_CONST_NUMBER("KEY_1", GLFW_KEY_1);
    EXPORT_NAPI_CONST_NUMBER("KEY_2", GLFW_KEY_2);
    EXPORT_NAPI_CONST_NUMBER("KEY_3", GLFW_KEY_3);
    EXPORT_NAPI_CONST_NUMBER("KEY_4", GLFW_KEY_4);
    EXPORT_NAPI_CONST_NUMBER("KEY_5", GLFW_KEY_5);
    EXPORT_NAPI_CONST_NUMBER("KEY_6", GLFW_KEY_6);
    EXPORT_NAPI_CONST_NUMBER("KEY_7", GLFW_KEY_7);
    EXPORT_NAPI_CONST_NUMBER("KEY_8", GLFW_KEY_8);
    EXPORT_NAPI_CONST_NUMBER("KEY_9", GLFW_KEY_9);
    EXPORT_NAPI_CONST_NUMBER("KEY_SEMICOLON", GLFW_KEY_SEMICOLON);
    EXPORT_NAPI_CONST_NUMBER("KEY_EQUAL", GLFW_KEY_EQUAL);
    EXPORT_NAPI_CONST_NUMBER("KEY_A", GLFW_KEY_A);
    EXPORT_NAPI_CONST_NUMBER("KEY_B", GLFW_KEY_B);
    EXPORT_NAPI_CONST_NUMBER("KEY_C", GLFW_KEY_C);
    EXPORT_NAPI_CONST_NUMBER("KEY_D", GLFW_KEY_D);
    EXPORT_NAPI_CONST_NUMBER("KEY_E", GLFW_KEY_E);
    EXPORT_NAPI_CONST_NUMBER("KEY_F", GLFW_KEY_F);
    EXPORT_NAPI_CONST_NUMBER("KEY_G", GLFW_KEY_G);
    EXPORT_NAPI_CONST_NUMBER("KEY_H", GLFW_KEY_H);
    EXPORT_NAPI_CONST_NUMBER("KEY_I", GLFW_KEY_I);
    EXPORT_NAPI_CONST_NUMBER("KEY_J", GLFW_KEY_J);
    EXPORT_NAPI_CONST_NUMBER("KEY_K", GLFW_KEY_K);
    EXPORT_NAPI_CONST_NUMBER("KEY_L", GLFW_KEY_L);
    EXPORT_NAPI_CONST_NUMBER("KEY_M", GLFW_KEY_M);
    EXPORT_NAPI_CONST_NUMBER("KEY_N", GLFW_KEY_N);
    EXPORT_NAPI_CONST_NUMBER("KEY_O", GLFW_KEY_O);
    EXPORT_NAPI_CONST_NUMBER("KEY_P", GLFW_KEY_P);
    EXPORT_NAPI_CONST_NUMBER("KEY_Q", GLFW_KEY_Q);
    EXPORT_NAPI_CONST_NUMBER("KEY_R", GLFW_KEY_R);
    EXPORT_NAPI_CONST_NUMBER("KEY_S", GLFW_KEY_S);
    EXPORT_NAPI_CONST_NUMBER("KEY_T", GLFW_KEY_T);
    EXPORT_NAPI_CONST_NUMBER("KEY_U", GLFW_KEY_U);
    EXPORT_NAPI_CONST_NUMBER("KEY_V", GLFW_KEY_V);
    EXPORT_NAPI_CONST_NUMBER("KEY_W", GLFW_KEY_W);
    EXPORT_NAPI_CONST_NUMBER("KEY_X", GLFW_KEY_X);
    EXPORT_NAPI_CONST_NUMBER("KEY_Y", GLFW_KEY_Y);
    EXPORT_NAPI_CONST_NUMBER("KEY_Z", GLFW_KEY_Z);
    EXPORT_NAPI_CONST_NUMBER("KEY_LEFT_BRACKET", GLFW_KEY_LEFT_BRACKET);
    EXPORT_NAPI_CONST_NUMBER("KEY_BACKSLASH", GLFW_KEY_BACKSLASH);
    EXPORT_NAPI_CONST_NUMBER("KEY_RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET);
    EXPORT_NAPI_CONST_NUMBER("KEY_GRAVE_ACCENT", GLFW_KEY_GRAVE_ACCENT);
    EXPORT_NAPI_CONST_NUMBER("KEY_WORLD_1", GLFW_KEY_WORLD_1);
    EXPORT_NAPI_CONST_NUMBER("KEY_WORLD_2", GLFW_KEY_WORLD_2);
    EXPORT_NAPI_CONST_NUMBER("KEY_ESCAPE", GLFW_KEY_ESCAPE);
    EXPORT_NAPI_CONST_NUMBER("KEY_ENTER", GLFW_KEY_ENTER);
    EXPORT_NAPI_CONST_NUMBER("KEY_TAB", GLFW_KEY_TAB);
    EXPORT_NAPI_CONST_NUMBER("KEY_BACKSPACE", GLFW_KEY_BACKSPACE);
    EXPORT_NAPI_CONST_NUMBER("KEY_INSERT", GLFW_KEY_INSERT);
    EXPORT_NAPI_CONST_NUMBER("KEY_DELETE", GLFW_KEY_DELETE);
    EXPORT_NAPI_CONST_NUMBER("KEY_RIGHT", GLFW_KEY_RIGHT);
    EXPORT_NAPI_CONST_NUMBER("KEY_LEFT", GLFW_KEY_LEFT);
    EXPORT_NAPI_CONST_NUMBER("KEY_DOWN", GLFW_KEY_DOWN);
    EXPORT_NAPI_CONST_NUMBER("KEY_UP", GLFW_KEY_UP);
    EXPORT_NAPI_CONST_NUMBER("KEY_PAGE_UP", GLFW_KEY_PAGE_UP);
    EXPORT_NAPI_CONST_NUMBER("KEY_PAGE_DOWN", GLFW_KEY_PAGE_DOWN);
    EXPORT_NAPI_CONST_NUMBER("KEY_HOME", GLFW_KEY_HOME);
    EXPORT_NAPI_CONST_NUMBER("KEY_END", GLFW_KEY_END);
    EXPORT_NAPI_CONST_NUMBER("KEY_CAPS_LOCK", GLFW_KEY_CAPS_LOCK);
    EXPORT_NAPI_CONST_NUMBER("KEY_SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK);
    EXPORT_NAPI_CONST_NUMBER("KEY_NUM_LOCK", GLFW_KEY_NUM_LOCK);
    EXPORT_NAPI_CONST_NUMBER("KEY_PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN);
    EXPORT_NAPI_CONST_NUMBER("KEY_PAUSE", GLFW_KEY_PAUSE);
    EXPORT_NAPI_CONST_NUMBER("KEY_F1", GLFW_KEY_F1);
    EXPORT_NAPI_CONST_NUMBER("KEY_F2", GLFW_KEY_F2);
    EXPORT_NAPI_CONST_NUMBER("KEY_F3", GLFW_KEY_F3);
    EXPORT_NAPI_CONST_NUMBER("KEY_F4", GLFW_KEY_F4);
    EXPORT_NAPI_CONST_NUMBER("KEY_F5", GLFW_KEY_F5);
    EXPORT_NAPI_CONST_NUMBER("KEY_F6", GLFW_KEY_F6);
    EXPORT_NAPI_CONST_NUMBER("KEY_F7", GLFW_KEY_F7);
    EXPORT_NAPI_CONST_NUMBER("KEY_F8", GLFW_KEY_F8);
    EXPORT_NAPI_CONST_NUMBER("KEY_F9", GLFW_KEY_F9);
    EXPORT_NAPI_CONST_NUMBER("KEY_F10", GLFW_KEY_F10);
    EXPORT_NAPI_CONST_NUMBER("KEY_F11", GLFW_KEY_F11);
    EXPORT_NAPI_CONST_NUMBER("KEY_F12", GLFW_KEY_F12);
    EXPORT_NAPI_CONST_NUMBER("KEY_F13", GLFW_KEY_F13);
    EXPORT_NAPI_CONST_NUMBER("KEY_F14", GLFW_KEY_F14);
    EXPORT_NAPI_CONST_NUMBER("KEY_F15", GLFW_KEY_F15);
    EXPORT_NAPI_CONST_NUMBER("KEY_F16", GLFW_KEY_F16);
    EXPORT_NAPI_CONST_NUMBER("KEY_F17", GLFW_KEY_F17);
    EXPORT_NAPI_CONST_NUMBER("KEY_F18", GLFW_KEY_F18);
    EXPORT_NAPI_CONST_NUMBER("KEY_F19", GLFW_KEY_F19);
    EXPORT_NAPI_CONST_NUMBER("KEY_F20", GLFW_KEY_F20);
    EXPORT_NAPI_CONST_NUMBER("KEY_F21", GLFW_KEY_F21);
    EXPORT_NAPI_CONST_NUMBER("KEY_F22", GLFW_KEY_F22);
    EXPORT_NAPI_CONST_NUMBER("KEY_F23", GLFW_KEY_F23);
    EXPORT_NAPI_CONST_NUMBER("KEY_F24", GLFW_KEY_F24);
    EXPORT_NAPI_CONST_NUMBER("KEY_F25", GLFW_KEY_F25);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_0", GLFW_KEY_KP_0);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_1", GLFW_KEY_KP_1);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_2", GLFW_KEY_KP_2);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_3", GLFW_KEY_KP_3);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_4", GLFW_KEY_KP_4);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_5", GLFW_KEY_KP_5);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_6", GLFW_KEY_KP_6);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_7", GLFW_KEY_KP_7);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_8", GLFW_KEY_KP_8);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_9", GLFW_KEY_KP_9);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_DECIMAL", GLFW_KEY_KP_DECIMAL);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_DIVIDE", GLFW_KEY_KP_DIVIDE);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_ADD", GLFW_KEY_KP_ADD);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_ENTER", GLFW_KEY_KP_ENTER);
    EXPORT_NAPI_CONST_NUMBER("KEY_KP_EQUAL", GLFW_KEY_KP_EQUAL);
    EXPORT_NAPI_CONST_NUMBER("KEY_LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT);
    EXPORT_NAPI_CONST_NUMBER("KEY_LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL);
    EXPORT_NAPI_CONST_NUMBER("KEY_LEFT_ALT", GLFW_KEY_LEFT_ALT);
    EXPORT_NAPI_CONST_NUMBER("KEY_LEFT_SUPER", GLFW_KEY_LEFT_SUPER);
    EXPORT_NAPI_CONST_NUMBER("KEY_RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT);
    EXPORT_NAPI_CONST_NUMBER("KEY_RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL);
    EXPORT_NAPI_CONST_NUMBER("KEY_RIGHT_ALT", GLFW_KEY_RIGHT_ALT);
    EXPORT_NAPI_CONST_NUMBER("KEY_RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER);
    EXPORT_NAPI_CONST_NUMBER("KEY_MENU", GLFW_KEY_MENU);
    EXPORT_NAPI_CONST_NUMBER("KEY_LAST", GLFW_KEY_LAST);

    EXPORT_NAPI_CONST_NUMBER("MOD_SHIFT", GLFW_MOD_SHIFT);
    EXPORT_NAPI_CONST_NUMBER("MOD_CONTROL", GLFW_MOD_CONTROL);
    EXPORT_NAPI_CONST_NUMBER("MOD_ALT", GLFW_MOD_ALT);
    EXPORT_NAPI_CONST_NUMBER("MOD_SUPER", GLFW_MOD_SUPER);

    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_1", GLFW_MOUSE_BUTTON_1);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_2", GLFW_MOUSE_BUTTON_2);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_3", GLFW_MOUSE_BUTTON_3);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_4", GLFW_MOUSE_BUTTON_4);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_5", GLFW_MOUSE_BUTTON_5);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_6", GLFW_MOUSE_BUTTON_6);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_7", GLFW_MOUSE_BUTTON_7);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_8", GLFW_MOUSE_BUTTON_8);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_LAST", GLFW_MOUSE_BUTTON_LAST);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_LEFT", GLFW_MOUSE_BUTTON_LEFT);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_RIGHT", GLFW_MOUSE_BUTTON_RIGHT);
    EXPORT_NAPI_CONST_NUMBER("MOUSE_BUTTON_MIDDLE", GLFW_MOUSE_BUTTON_MIDDLE);

    EXPORT_NAPI_CONST_NUMBER("ARROW_CURSOR", GLFW_ARROW_CURSOR);
    EXPORT_NAPI_CONST_NUMBER("IBEAM_CURSOR", GLFW_IBEAM_CURSOR);
    EXPORT_NAPI_CONST_NUMBER("CROSSHAIR_CURSOR", GLFW_CROSSHAIR_CURSOR);
    EXPORT_NAPI_CONST_NUMBER("HAND_CURSOR", GLFW_HAND_CURSOR);
    EXPORT_NAPI_CONST_NUMBER("HRESIZE_CURSOR", GLFW_HRESIZE_CURSOR);
    EXPORT_NAPI_CONST_NUMBER("VRESIZE_CURSOR", GLFW_VRESIZE_CURSOR);

    EXPORT_NAPI_CONST_NUMBER("RESIZABLE", GLFW_RESIZABLE);
    EXPORT_NAPI_CONST_NUMBER("VISIBLE", GLFW_VISIBLE);
    EXPORT_NAPI_CONST_NUMBER("DECORATED", GLFW_DECORATED);
    EXPORT_NAPI_CONST_NUMBER("FOCUSED", GLFW_FOCUSED);
    EXPORT_NAPI_CONST_NUMBER("AUTO_ICONIFY", GLFW_AUTO_ICONIFY);
    EXPORT_NAPI_CONST_NUMBER("FLOATING", GLFW_FLOATING);
    EXPORT_NAPI_CONST_NUMBER("MAXIMIZED", GLFW_MAXIMIZED);
    EXPORT_NAPI_CONST_NUMBER("RED_BITS", GLFW_RED_BITS);
    EXPORT_NAPI_CONST_NUMBER("GREEN_BITS", GLFW_GREEN_BITS);
    EXPORT_NAPI_CONST_NUMBER("BLUE_BITS", GLFW_BLUE_BITS);
    EXPORT_NAPI_CONST_NUMBER("ALPHA_BITS", GLFW_ALPHA_BITS);
    EXPORT_NAPI_CONST_NUMBER("DEPTH_BITS", GLFW_DEPTH_BITS);
    EXPORT_NAPI_CONST_NUMBER("STENCIL_BITS", GLFW_STENCIL_BITS);
    EXPORT_NAPI_CONST_NUMBER("ACCUM_RED_BITS", GLFW_ACCUM_RED_BITS);
    EXPORT_NAPI_CONST_NUMBER("ACCUM_GREEN_BITS", GLFW_ACCUM_GREEN_BITS);
    EXPORT_NAPI_CONST_NUMBER("ACCUM_BLUE_BITS", GLFW_ACCUM_BLUE_BITS);
    EXPORT_NAPI_CONST_NUMBER("ACCUM_ALPHA_BITS", GLFW_ACCUM_ALPHA_BITS);
    EXPORT_NAPI_CONST_NUMBER("AUX_BUFFERS", GLFW_AUX_BUFFERS);
    EXPORT_NAPI_CONST_NUMBER("SAMPLES", GLFW_SAMPLES);
    EXPORT_NAPI_CONST_NUMBER("REFRESH_RATE", GLFW_REFRESH_RATE);
    EXPORT_NAPI_CONST_NUMBER("STEREO", GLFW_STEREO);
    EXPORT_NAPI_CONST_NUMBER("SRGB_CAPABLE", GLFW_SRGB_CAPABLE);
    EXPORT_NAPI_CONST_NUMBER("DOUBLEBUFFER", GLFW_DOUBLEBUFFER);
    EXPORT_NAPI_CONST_NUMBER("CLIENT_API", GLFW_CLIENT_API);
    EXPORT_NAPI_CONST_NUMBER("CONTEXT_CREATION_API", GLFW_CONTEXT_CREATION_API);
    EXPORT_NAPI_CONST_NUMBER("CONTEXT_VERSION_MAJOR", GLFW_CONTEXT_VERSION_MAJOR);
    EXPORT_NAPI_CONST_NUMBER("CONTEXT_VERSION_MINOR", GLFW_CONTEXT_VERSION_MINOR);
    EXPORT_NAPI_CONST_NUMBER("CONTEXT_ROBUSTNESS", GLFW_CONTEXT_ROBUSTNESS);
    EXPORT_NAPI_CONST_NUMBER("CONTEXT_RELEASE_BEHAVIOR", GLFW_CONTEXT_RELEASE_BEHAVIOR);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_FORWARD_COMPAT", GLFW_OPENGL_FORWARD_COMPAT);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_DEBUG_CONTEXT", GLFW_OPENGL_DEBUG_CONTEXT);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_PROFILE", GLFW_OPENGL_PROFILE);

    EXPORT_NAPI_CONST_NUMBER("DONT_CARE", GLFW_DONT_CARE);
    EXPORT_NAPI_CONST_NUMBER("ICONIFIED", GLFW_ICONIFIED);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_API", GLFW_OPENGL_API);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_ES_API", GLFW_OPENGL_ES_API);
    EXPORT_NAPI_CONST_NUMBER("NO_API", GLFW_NO_API);
    EXPORT_NAPI_CONST_NUMBER("NATIVE_CONTEXT_API", GLFW_NATIVE_CONTEXT_API);
    EXPORT_NAPI_CONST_NUMBER("EGL_CONTEXT_API", GLFW_EGL_CONTEXT_API);
    EXPORT_NAPI_CONST_NUMBER("NO_ROBUSTNESS", GLFW_NO_ROBUSTNESS);
    EXPORT_NAPI_CONST_NUMBER("NO_RESET_NOTIFICATION", GLFW_NO_RESET_NOTIFICATION);
    EXPORT_NAPI_CONST_NUMBER("LOSE_CONTEXT_ON_RESET", GLFW_LOSE_CONTEXT_ON_RESET);
    EXPORT_NAPI_CONST_NUMBER("ANY_RELEASE_BEHAVIOR", GLFW_ANY_RELEASE_BEHAVIOR);
    EXPORT_NAPI_CONST_NUMBER("RELEASE_BEHAVIOR_FLUSH", GLFW_RELEASE_BEHAVIOR_FLUSH);
    EXPORT_NAPI_CONST_NUMBER("RELEASE_BEHAVIOR_NONE", GLFW_RELEASE_BEHAVIOR_NONE);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_ANY_PROFILE", GLFW_OPENGL_ANY_PROFILE);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_COMPAT_PROFILE", GLFW_OPENGL_COMPAT_PROFILE);
    EXPORT_NAPI_CONST_NUMBER("OPENGL_CORE_PROFILE", GLFW_OPENGL_CORE_PROFILE);

    EXPORT_NAPI_CONST_NUMBER("CURSOR", GLFW_CURSOR);
    EXPORT_NAPI_CONST_NUMBER("STICKY_KEYS", GLFW_STICKY_KEYS);
    EXPORT_NAPI_CONST_NUMBER("STICKY_MOUSE_BUTTONS", GLFW_STICKY_MOUSE_BUTTONS);
    EXPORT_NAPI_CONST_NUMBER("CURSOR_NORMAL", GLFW_CURSOR_NORMAL);
    EXPORT_NAPI_CONST_NUMBER("CURSOR_HIDDEN", GLFW_CURSOR_HIDDEN);
    EXPORT_NAPI_CONST_NUMBER("CURSOR_DISABLED", GLFW_CURSOR_DISABLED);

    EXPORT_NAPI_CONST_NUMBER("CONNECTED", GLFW_CONNECTED);
    EXPORT_NAPI_CONST_NUMBER("DISCONNECTED", GLFW_DISCONNECTED);

    return exports;
}

NAPI_MODULE(glfw, Init);

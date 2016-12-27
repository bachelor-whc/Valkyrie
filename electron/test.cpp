#include <node.h>
#include <node_buffer.h>
#include <string>
#include <Windows.h>
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void Method(const FunctionCallbackInfo<Value>& args) {
    const int width = (int)args[0]->NumberValue();
    const int height = (int)args[1]->NumberValue();
    HWND* p_window_handle = (HWND*)node::Buffer::Data(args[2]);
    Isolate* isolate = args.GetIsolate();
    std::string str = std::to_string(width) + " " + std::to_string(height) + " " + std::to_string((unsigned long long)(*p_window_handle)) + " " + std::to_string(sizeof(HWND));
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, str.c_str()));
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "InitializeValkyrie", Method);
}

NODE_MODULE(addon, init)
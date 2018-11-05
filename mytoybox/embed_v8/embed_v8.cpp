#include <sstream>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <libplatform/libplatform.h>
#include <v8.h>

static void TestCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    auto result = v8::Integer::New(isolate, 0x12345678);

    args.GetReturnValue().Set(result);
}

void thread_func0()
{
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    std::chrono::milliseconds(100 + std::rand() % 347);
    std::cout << __func__ << " In thread " << std::this_thread::get_id() << std::endl;

    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

        global->Set(
            v8::String::NewFromUtf8(isolate, "testCall", v8::NewStringType::kNormal)
                .ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, TestCallback));

        v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);
        v8::Context::Scope context_scope(context);
        std::string script_src = "testCall();\n";
        v8::Local<v8::String> source =
            v8::String::NewFromUtf8(isolate, script_src.c_str(),
                                    v8::NewStringType::kNormal)
                .ToLocalChecked();

        v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

        v8::String::Utf8Value output(isolate, result);
        std::cout << "JavaScript output: " << *output << "\n";
    }
    isolate->Dispose();
    std::chrono::milliseconds(100 + std::rand() % 523);
    delete create_params.array_buffer_allocator;
}

void thread_func1(v8::Isolate::CreateParams& create_params)
{
    std::chrono::milliseconds(100 + std::rand() % 347);
    std::cout << "In thread " << std::this_thread::get_id() << std::endl;

    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

        global->Set(
            v8::String::NewFromUtf8(isolate, "testCall", v8::NewStringType::kNormal)
                .ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, TestCallback));

        v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);
        v8::Context::Scope context_scope(context);
        std::string script_src = "testCall();\n";
        v8::Local<v8::String> source =
            v8::String::NewFromUtf8(isolate, script_src.c_str(),
                                    v8::NewStringType::kNormal)
                .ToLocalChecked();

        v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

        v8::String::Utf8Value output(isolate, result);
        std::cout << "JavaScript output: " << *output << "\n";
    }
    isolate->Dispose();
    std::chrono::milliseconds(100 + std::rand() % 523);
}

int main(int argc, char* argv[])
{
	std::srand(std::time(nullptr));
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    std::vector<std::thread> thrds;

    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
    	thrds.emplace_back(thread_func1, std::ref(create_params));
    	thrds.emplace_back(thread_func0);
    }

    std::for_each(thrds.begin(), thrds.end(), std::mem_fn(&std::thread::join));
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;

    return 0;
}

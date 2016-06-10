#include <node.h>
#include <v8.h>
#include <Windows.h>
#include <iostream>

using namespace v8;
using node::AtExit;
extern "C" long int __stdcall Addup(__int64, __int64, __int64);//Addup(ParamsNum-2,call addr,params...)
void CallProcMethod(const FunctionCallbackInfo<Value>& args);

HINSTANCE currentDllHandle = NULL;
void* currentCallProc = NULL;
v8::Local<v8::Object> gTarget;

void AtExitCall(void* args)
{
	Isolate* isolate = static_cast<Isolate*>(args);
	HandleScope scope(isolate);
	Local<Object> obj = Object::New(isolate);
	assert(!obj.IsEmpty()); // assert VM is still alive
	assert(obj->IsObject());
	printf("exit!\n");
}

void GetCurrentProcessIdMethod(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  args.GetReturnValue().Set(Integer::New(isolate, GetCurrentProcessId()));
}

void RunCallback(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();
	Local<Function> callback = Local<Function>::Cast(args[0]);
	const unsigned argc = 1;
	Local<Value> argv[argc] = { String::NewFromUtf8(isolate,"hello function") };
	callback->Call(Null(isolate), argc, argv);
}

void LoadLibraryMethod(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if(args.Length()!=1)
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}
	if(!args[0]->IsString())
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong argument(need string)")));
		return;
	}
	String::Utf8Value param(args[0]->ToString());
	std::string paramStr = std::string(*param);
	currentDllHandle=LoadLibrary(paramStr.c_str());
	args.GetReturnValue().Set(Integer::New(isolate, GetLastError()));
}

void GetProcAddressMethod(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	if (args.Length() != 1)
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}
	if (!args[0]->IsString())
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong argument(need string)")));
		return;
	}
	if(currentDllHandle==NULL)
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "No dll loaded")));
		return;
	}
	String::Utf8Value param(args[0]->ToString());
	std::string paramStr = std::string(*param);
	currentCallProc = GetProcAddress(currentDllHandle, paramStr.c_str());
	std::cout << currentCallProc << std::endl;
	Local<FunctionTemplate> tp = FunctionTemplate::New(isolate, CallProcMethod);
	Local<Function> fn = tp->GetFunction();
	fn->SetName(String::NewFromUtf8(isolate, paramStr.c_str()));
	args.GetReturnValue().Set(fn);
}

void CallProcMethod(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	int argsCount = args.Length();
	__int64* argAry = (__int64*)malloc(argsCount*sizeof(__int64));
	std::string* paramStrTmp;
	for (int x = 0; x < argsCount;x++)
	{
		if(args[x]->IsString())
		{
			String::Utf8Value param(args[x]->ToString());
			paramStrTmp = new std::string(*param);
			argAry[x] = (__int64)paramStrTmp->c_str();
		}
		if(args[x]->IsNumber())
		{
			Local<Number> num = Number::New(isolate, args[x]->NumberValue());
			argAry[x] = num->IntegerValue();
		}
	}
	__int64 ret=Addup(argsCount, (__int64)currentCallProc, (__int64)argAry);
}

void printfMethod(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	int argsCount = args.Length();
	__int64* argAry = (__int64*)malloc(argsCount*sizeof(__int64));
	std::string* paramStrTmp;
	for (int x = 0; x < argsCount; x++)
	{
		if (args[x]->IsString())
		{
			String::Utf8Value param(args[x]->ToString());
			paramStrTmp = new std::string(*param);
			argAry[x] = (__int64)paramStrTmp->c_str();
		}
		if (args[x]->IsNumber())
		{
			Local<Number> num = Number::New(isolate, args[x]->NumberValue());
			argAry[x] = num->IntegerValue();
		}
	}
	__int64 ret = Addup(argsCount, (__int64)printf, (__int64)argAry);
}

void init(v8::Local<v8::Object> target) {
	gTarget = target;
	NODE_SET_METHOD(target, "GetCurrentProcessId", GetCurrentProcessIdMethod);
	NODE_SET_METHOD(target, "GetProcAddress", GetProcAddressMethod);
	NODE_SET_METHOD(target, "RunCallback", RunCallback);
	NODE_SET_METHOD(target, "LoadLibrary", LoadLibraryMethod);
	NODE_SET_METHOD(target, "printf", printfMethod);
	AtExit(AtExitCall, target->GetIsolate());
}

NODE_MODULE(binding, init);
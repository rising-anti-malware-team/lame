#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include "lame_interface.h"

using namespace v8;

static CLameInterface g_lame_library;

enum ExceptionType {
	RangeError = 1,
	ReferenceError,
	SyntaxError,
	TypeError,
	Error,
};

static inline void throw_exception(Isolate* isolate, ExceptionType type, char* info) {
	switch (type)
	{
	case RangeError:
		isolate->ThrowException(Exception::RangeError(String::NewFromUtf8(isolate, info)));
		break;
	case ReferenceError:
		isolate->ThrowException(Exception::ReferenceError(String::NewFromUtf8(isolate, info)));
		break;
	case SyntaxError:
		isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, info)));
		break;
	case TypeError:
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, info)));
		break;
	case Error:
		isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, info)));
		break;
	default:
		break;
	}
}

static inline bool IsNull(Local<Value> val) {
	Local<Object> obj = val->ToObject();
	return NULL == *obj;
}

static inline std::string get_string_from_argument(Local<Value> val) {
	if (IsNull(val)) return "";
	Local<String> lstring = Local<String>::Cast(val);
	String::Utf8Value utfValue(lstring);
	return *utfValue;
}

static inline void* get_pointer_from_argument(Local<Value> val) {
	Local<External> lext = Local<External>::Cast(val);
	return lext->Value();
}

static inline int32_t get_number_from_argument(Local<Value> val) {
	Local<Number> ln = Local<Number>::Cast(val);
	return ln->Int32Value();
}

static inline void wrap_scan_result(Isolate* isolate, lame_scan_result* result, Handle<Object>& instance) {
	instance->Set(String::NewFromUtf8(isolate, "mklass"), Integer::New(isolate, result->mklass));
	instance->Set(String::NewFromUtf8(isolate, "kclass"), String::NewFromUtf8(isolate, result->kclass));
	instance->Set(String::NewFromUtf8(isolate, "kclass_desc_a"), String::NewFromUtf8(isolate, result->kclass_desc_a));
	instance->Set(String::NewFromUtf8(isolate, "kclass_desc_w"), String::NewFromUtf8(isolate, (char*)result->kclass_desc_w));
	instance->Set(String::NewFromUtf8(isolate, "engid"), String::NewFromUtf8(isolate, result->engid));
	instance->Set(String::NewFromUtf8(isolate, "vname"), String::NewFromUtf8(isolate, result->vname));
	instance->Set(String::NewFromUtf8(isolate, "vid32"), Integer::New(isolate, result->vid32));
	instance->Set(String::NewFromUtf8(isolate, "vid40"), Integer::New(isolate, result->vid40));
	instance->Set(String::NewFromUtf8(isolate, "hitag"), String::NewFromUtf8(isolate, result->hitag));
	instance->Set(String::NewFromUtf8(isolate, "treat"), Integer::New(isolate, result->treat));
}

static inline void wrap_version(Isolate* isolate, lame_info* info, Handle<Object>& instance) {
	instance->Set(String::NewFromUtf8(isolate, "engv"), String::NewFromUtf8(isolate, info->engv));
	instance->Set(String::NewFromUtf8(isolate, "vdbv"), String::NewFromUtf8(isolate, info->vdbv));
}

static inline void wrap_licence_info(Isolate* isolate, rx_licence_info* info, Handle<Object>& instance) {
	instance->Set(String::NewFromUtf8(isolate, "Version"), String::NewFromUtf8(isolate, info->Version));
	instance->Set(String::NewFromUtf8(isolate, "Owner"), String::NewFromUtf8(isolate, info->Owner));
	instance->Set(String::NewFromUtf8(isolate, "Date"), String::NewFromUtf8(isolate, info->Date));
	instance->Set(String::NewFromUtf8(isolate, "Authm"), String::NewFromUtf8(isolate, info->Authm));
	instance->Set(String::NewFromUtf8(isolate, "Data"), String::NewFromUtf8(isolate, info->Data));
}

//////////////////////////////////////////////////////////////////////////

void lame_open_vdb(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() > 1) {
		throw_exception(isolate, TypeError, "Wrong number of arguments");
		return;
	}
	std::string vlibf;
	if (args[0]->IsString()) {
		vlibf = get_string_from_argument(args[0]);
	}
	pfn_lame_open_vdb pfn = g_lame_library.lame_open_vdb;
	if (pfn) {
		void* vdb = pfn(vlibf.empty() ? NULL : vlibf.c_str());
		args.GetReturnValue().Set(External::New(isolate, vdb));
	} else {
		args.GetReturnValue().SetNull();
	}
}

void lame_close_vdb(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 1 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* vdb = get_pointer_from_argument(args[0]);
	if (NULL == vdb) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	pfn_lame_close_vdb pfn = g_lame_library.lame_close_vdb;
	if (pfn) {
		pfn(vdb);
	}
}

void lame_create(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 1 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* handle = get_pointer_from_argument(args[0]);
	if (NULL == handle) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	pfn_lame_create pfn = g_lame_library.lame_create;
	if (pfn) {
		void* lame = pfn(handle);
		args.GetReturnValue().Set(External::New(isolate, lame));
	} else {
		args.GetReturnValue().SetNull();
	}
}

void lame_destroy(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 1 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	pfn_lame_destroy pfn = g_lame_library.lame_destroy;
	if (pfn) {
		pfn(lame);
	}
}

void lame_fork(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 1 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	pfn_lame_fork pfn = g_lame_library.lame_fork;
	if (pfn) {
		void* new_lame = pfn(lame);
		args.GetReturnValue().Set(External::New(isolate, new_lame));
	} else {
		args.GetReturnValue().SetNull();
	}
}

void lame_param_set(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 2 || !args[0]->IsExternal() || !args[1]->IsString()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	std::string parameter = get_string_from_argument(args[1]);
	pfn_lame_param_set pfn = g_lame_library.lame_param_set;
	if (pfn) {
		long hr = pfn(lame, parameter.c_str());
		Local<Boolean> flag = Boolean::New(isolate, hr == 0 ? true : false);
		args.GetReturnValue().Set(flag);
	} else {
		args.GetReturnValue().Set(Boolean::New(isolate, false));
	}
}

void lame_init(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 1 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	pfn_lame_init pfn = g_lame_library.lame_init;
	if (pfn) {
		long hr = pfn(lame);
		Local<Boolean> flag = Boolean::New(isolate, hr == 0 ? true : false);
		args.GetReturnValue().Set(flag);
	} else {
		args.GetReturnValue().Set(Boolean::New(isolate, false));
	}
}

void lame_scan_file(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 2 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	std::string target = get_string_from_argument(args[1]);
	pfn_lame_scan_file pfn = g_lame_library.lame_scan_file;
	if (pfn) {
		lame_scan_result result = {};
		if (0 == pfn(lame, target.c_str(), &result)) {
			Local<Object> dr = Object::New(isolate);
			wrap_scan_result(isolate, &result, dr);
			args.GetReturnValue().Set(dr);
		} else {
			args.GetReturnValue().SetNull();
		}
	} else {
		args.GetReturnValue().SetNull();
	}
}

void lame_scan_mem(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 3 || !args[0]->IsExternal() || !args[1]->IsObject() || !args[2]->IsInt32()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	uint8_t* bufferData = (uint8_t*)node::Buffer::Data(args[1]->ToObject());
	size_t size = args[2]->Int32Value();
	pfn_lame_scan_mem pfn = g_lame_library.lame_scan_mem;
	if (pfn) {
		lame_scan_result result = {};
		if (0 == pfn(lame, bufferData, size, &result)) {
			Local<Object> dr = Object::New(isolate);
			wrap_scan_result(isolate, &result, dr);
			args.GetReturnValue().Set(dr);
		} else {
			args.GetReturnValue().SetNull();
		}
	} else {
		args.GetReturnValue().SetNull();
	}
}

typedef struct node_lame_scan_feedback {
	Local<Function> enter_file;
	Local<Function> leave_file;
	Local<Function> alarm;
	Local<Object> user_data;
} node_lame_scan_feedback;

static LSCT __stdcall local_scan_enter_file(const char* file_name, uint32_t depth, void* user_data) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	node_lame_scan_feedback* ncb = (node_lame_scan_feedback*)user_data;
	if (NULL == ncb) return LSCT_CONTINUE;
	const int argc = 3;
	Local<Value> argv[argc] = {
		String::NewFromUtf8(isolate, file_name),
		Number::New(isolate, depth),
		ncb->user_data
	};
	Local<Value> ret = ncb->enter_file->Call(Null(isolate), argc, argv);
	return (LSCT)ret->IntegerValue();
}

static void __stdcall local_scan_leave_file(const char* file_name, uint32_t depth, void* user_data, LXLVT l)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	node_lame_scan_feedback* ncb = (node_lame_scan_feedback*)user_data;
	if (NULL == ncb) return;
	const int argc = 4;
	Local<Value> argv[argc] = {
		String::NewFromUtf8(isolate, file_name),
		Number::New(isolate, depth),
		ncb->user_data,
		Number::New(isolate, l),
	};
	ncb->leave_file->Call(Null(isolate), argc, argv);
}

static LSCT __stdcall local_scan_alarm(const char* file_name, lame_scan_result* result, void* user_data)
{
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	node_lame_scan_feedback* ncb = (node_lame_scan_feedback*)user_data;
	if (NULL == ncb) return LSCT_CONTINUE;
	const int argc = 3;
	Local<Value> argv[argc] = {
		String::NewFromUtf8(isolate, file_name),
		Null(isolate),
		ncb->user_data
	};
	if (result) {
		Local<Object> instance;
		instance = Object::New(isolate);
		wrap_scan_result(isolate, result, instance);
		argv[1] = instance;
	}
	Local<Value> ret = ncb->alarm->Call(Null(isolate), argc, argv);
	return (LSCT)ret->IntegerValue();
}

void lame_scan_file_with_callback(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 6 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	std::string target = get_string_from_argument(args[1]);

	pfn_lame_scan_file_with_callback pfn = g_lame_library.lame_scan_file_with_callback;
	if (pfn) {
		node_lame_scan_feedback ncb;
		ncb.enter_file = Local<Function>::Cast(args[2]);
		ncb.leave_file = Local<Function>::Cast(args[3]);
		ncb.alarm = Local<Function>::Cast(args[4]);
		ncb.user_data = Local<Object>::Cast(args[5]);

		lame_scan_feedback lcb;
		lcb.enter_file = local_scan_enter_file;
		lcb.leave_file = local_scan_leave_file;
		lcb.alarm = local_scan_alarm;
		long hr = pfn(lame, target.c_str(), &lcb, &ncb);
		args.GetReturnValue().Set(hr);
	} else {
		args.GetReturnValue().Set(E_FAIL);
	}
}

void lame_scan_mem_with_callback(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 7 || !args[0]->IsExternal() || !args[1]->IsObject() || !args[2]->IsInt32()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	uint8_t* bufferData = (uint8_t*)node::Buffer::Data(args[1]->ToObject());
	size_t size = args[2]->Int32Value();

	pfn_lame_scan_mem_with_callback pfn = g_lame_library.lame_scan_mem_with_callback;
	if (pfn) {
		node_lame_scan_feedback ncb;
		ncb.enter_file = Local<Function>::Cast(args[3]);
		ncb.leave_file = Local<Function>::Cast(args[4]);
		ncb.alarm = Local<Function>::Cast(args[5]);
		ncb.user_data = Local<Object>::Cast(args[6]);

		lame_scan_feedback lcb;
		lcb.enter_file = local_scan_enter_file;
		lcb.leave_file = local_scan_leave_file;
		lcb.alarm = local_scan_alarm;
		long hr = pfn(lame, bufferData, size, &lcb, &ncb);
		args.GetReturnValue().Set(hr);
	} else {
		args.GetReturnValue().Set(E_FAIL);
	}
}

void lame_get_version(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 0) {
		throw_exception(isolate, Error, "Wrong number of arguments");
		return;
	}
	pfn_lame_get_version pfn = g_lame_library.lame_get_version;
	if (pfn) {
		lame_info info = {};
		if (0 == pfn(&info)) {
			Local<Object> ver = Object::New(isolate);
			wrap_version(isolate, &info, ver);
			args.GetReturnValue().Set(ver);
		} else {
			args.GetReturnValue().SetNull();
		}
	} else {
		args.GetReturnValue().SetNull();
	}
}

void lame_get_licence_info(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 0) {
		throw_exception(isolate, Error, "Wrong number of arguments");
		return;
	}
	pfn_lame_get_licence_info pfn = g_lame_library.lame_get_licence_info;
	if (pfn) {
		rx_licence_info info = {};
		if (0 == pfn(&info)) {
			Local<Object> li = Object::New(isolate);
			wrap_licence_info(isolate, &info, li);
			args.GetReturnValue().Set(li);
		} else {
			args.GetReturnValue().SetNull();
		}
	} else {
		args.GetReturnValue().SetNull();
	}
}

typedef struct node_lame_extract_feedback {
	Local<Function> enter_file;
	Local<Function> leave_file;
	Local<Object> user_data;
} node_lame_extract_feedback;

static LXCT __stdcall local_extract_enter_file(const char* file_name, const char* fmt, uint32_t depth, uint64_t flag, void* handle, void* user_data){
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	node_lame_extract_feedback* ncb = (node_lame_extract_feedback*)user_data;
	if (NULL == ncb) return LCT_CONTINUE;
	const int argc = 6;
	Local<Value> argv[argc] = {
		String::NewFromUtf8(isolate, file_name),
		String::NewFromUtf8(isolate, fmt),
		Number::New(isolate, depth),
		Number::New(isolate, (int)flag),
		Null(isolate),
		ncb->user_data
	};
	if (NULL != handle) {
		argv[4] = External::New(isolate, handle);
	}
	Local<Value> ret = ncb->enter_file->Call(Null(isolate), argc, argv);
	return (LXCT)ret->IntegerValue();
}

static void __stdcall local_extract_leave_file(const char* file_name, const char* fmt, uint32_t depth, uint64_t flag, void* handle, void* user_data, LXLVT lv) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	node_lame_extract_feedback* ncb = (node_lame_extract_feedback*)user_data;
	if (NULL == ncb) return;
	const int argc = 7;
	Local<Value> argv[argc] = {
		String::NewFromUtf8(isolate, file_name),
		String::NewFromUtf8(isolate, fmt),
		Number::New(isolate, depth),
		Number::New(isolate, (int)flag),
		External::New(isolate, handle),
		ncb->user_data,
		Number::New(isolate, lv),
	};
	ncb->leave_file->Call(Null(isolate), argc, argv);
}

void lame_extract_file(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 6 || !args[0]->IsExternal() || !args[1]->IsString() || !args[3]->IsFunction() || !args[4]->IsFunction() ) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* lame = get_pointer_from_argument(args[0]);
	if (NULL == lame) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	std::string target = get_string_from_argument(args[1]);
	std::string passwd = get_string_from_argument(args[2]);

	pfn_lame_extract_file pfn = g_lame_library.lame_extract_file;
	if (pfn) {
		node_lame_extract_feedback ncb;
		ncb.enter_file = Local<Function>::Cast(args[3]);
		ncb.leave_file = Local<Function>::Cast(args[4]);
		ncb.user_data = Local<Object>::Cast(args[5]);

		lame_extract_feedback lcb;
		lcb.enter_file = local_extract_enter_file;
		lcb.leave_file = local_extract_leave_file;
		long hr = pfn(lame, target.c_str(), passwd.c_str(), &lcb, &ncb);
		args.GetReturnValue().Set(hr);
	} else {
		args.GetReturnValue().Set(E_FAIL);
	}
}

void lame_file_get_size(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 1 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* handle = get_pointer_from_argument(args[0]);
	if (NULL == handle) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	pfn_lame_file_get_size pfn = g_lame_library.lame_file_get_size;
	if (pfn) {
		long size = pfn(handle);
		args.GetReturnValue().Set(Number::New(isolate, size));
	} else {
		args.GetReturnValue().Set(Number::New(isolate, 0));
	}
}

void lame_file_seek(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 3 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* handle = get_pointer_from_argument(args[0]);
	if (NULL == handle) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	int32_t offset = get_number_from_argument(args[1]);
	LSMETHOD method = (LSMETHOD)get_number_from_argument(args[2]);
	pfn_lame_file_seek pfn = g_lame_library.lame_file_seek;
	if (pfn) {
		long hr = pfn(handle, offset, method);
		args.GetReturnValue().Set(Boolean::New(isolate, hr==0));
	} else {
		args.GetReturnValue().Set(Boolean::New(isolate, false));
	}
}

void lame_file_tell(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 1 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* handle = get_pointer_from_argument(args[0]);
	if (NULL == handle) {
		throw_exception(isolate, Error, "argument is null");
		return;
	}
	pfn_lame_file_tell pfn = g_lame_library.lame_file_tell;
	if (pfn) {
		long offset = pfn(handle);
		args.GetReturnValue().Set(Number::New(isolate, offset));
	} else {
		args.GetReturnValue().Set(Number::New(isolate, -1));
	}
}

void buffer_delete_callback(char* data, void* lpBuffer) {
	delete [] reinterpret_cast<char*> (lpBuffer);
}

void lame_file_read(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	if (args.Length() != 2 || !args[0]->IsExternal()) {
		throw_exception(isolate, TypeError, "Wrong number of arguments or type");
		return;
	}
	void* handle = get_pointer_from_argument(args[0]);
	long size = get_number_from_argument(args[1]);
	if (NULL == handle || size < 0) {
		throw_exception(isolate, Error, "argument error");
		return;
	}
	pfn_lame_file_read pfn = g_lame_library.lame_file_read;
	if (pfn) {
		char* lpBuf = new char[size];
		if (lpBuf) {
			if (size == pfn(handle, (uint8_t*)lpBuf, size)) {
				MaybeLocal<Object> o = node::Buffer::New(isolate, lpBuf, size, buffer_delete_callback, lpBuf);
				Local<Value> lo;
				o.ToLocal(&lo);
				args.GetReturnValue().Set(lo);
			}
		} else {
			args.GetReturnValue().SetNull();
		}
	} else {
		args.GetReturnValue().SetNull();
	}
}

void init(Handle<Object> exports) {
	g_lame_library.init();

	NODE_SET_METHOD(exports, "lame_open_vdb", lame_open_vdb);
	NODE_SET_METHOD(exports, "lame_close_vdb", lame_close_vdb);

	NODE_SET_METHOD(exports, "lame_create", lame_create);
	NODE_SET_METHOD(exports, "lame_destroy", lame_destroy);
	NODE_SET_METHOD(exports, "lame_fork", lame_fork);

	NODE_SET_METHOD(exports, "lame_param_set", lame_param_set);

	NODE_SET_METHOD(exports, "lame_init", lame_init);
	NODE_SET_METHOD(exports, "lame_scan_file", lame_scan_file);
	NODE_SET_METHOD(exports, "lame_scan_mem", lame_scan_mem);
	NODE_SET_METHOD(exports, "lame_scan_file_with_callback", lame_scan_file_with_callback);
	NODE_SET_METHOD(exports, "lame_scan_mem_with_callback", lame_scan_mem_with_callback);

	NODE_SET_METHOD(exports, "lame_get_version", lame_get_version);
	NODE_SET_METHOD(exports, "lame_get_licence_info", lame_get_licence_info);

	NODE_SET_METHOD(exports, "lame_extract_file", lame_extract_file);
	NODE_SET_METHOD(exports, "lame_file_get_size", lame_file_get_size);
	NODE_SET_METHOD(exports, "lame_file_seek", lame_file_seek);
	NODE_SET_METHOD(exports, "lame_file_tell", lame_file_tell);
	NODE_SET_METHOD(exports, "lame_file_read", lame_file_read);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init)

#include <unistd.h>
#include <pthread.h>
#include <uv.h>
#include <napi.h>

static uv_async_t async_g;
static Napi::FunctionReference callback_ref_g;

void DispatchEvent(uv_async_t* handle) {
  Napi::Env env = callback_ref_g.Env();
  Napi::HandleScope scope(env);

  // TODO - If the callback throws how can it be handled?
  callback_ref_g.Call({ Napi::String::New(env, "hello world") });

// The following code doesn't work
/*  if (env.IsExceptionPending()) {
    printf("c++: after callback - exception pending\n");
    Napi::Error e = env.GetAndClearPendingException();
    e.ThrowAsJavaScriptException(); // <- This doesn't work
  } else {
    printf("c++: after callback\n");
  }*/
}

void *InfiniteLoop(void *arg) {
  while (true) {
    sleep(1);
    uv_async_send(&async_g);
  }

  return 0;
}

int StartInfiniteLoop() {
  pthread_t theread_id;

  int err = uv_async_init(uv_default_loop(), &async_g, DispatchEvent);
  if (err < 0) {
    return -err;
  }

  // Prevent async_g from keeping event loop alive initially.
  uv_unref((uv_handle_t *) &async_g);

  err = pthread_create(&theread_id, 0, InfiniteLoop, 0);
  if (err != 0) {
    uv_close((uv_handle_t *) &async_g, 0);
    return err;
  }

  return 0;
}

void Start(const Napi::CallbackInfo& info) {
  static bool running_g = false;

  if (running_g == false) {
    Napi::Function callback = info[0].As<Napi::Function>();
    callback_ref_g = Napi::Reference<Napi::Function>::New(callback, 1);
    callback_ref_g.SuppressDestruct();

    // TODO evaruate return code from StartInfiniteLoop
    StartInfiniteLoop();

    running_g = true;
  }
}

void KeepEventLoopAlive(const Napi::CallbackInfo& info) {
  uv_ref((uv_handle_t *) &async_g);
}

void DontKeepEventLoopAlive(const Napi::CallbackInfo& info) {
  uv_unref((uv_handle_t *) &async_g);
}

Napi::Value RunCallback(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Function cb = info[0].As<Napi::Function>();
  cb.Call({ Napi::String::New(env, "hello world") });

  return env.Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports["start"] = Napi::Function::New(env, Start);
  exports["keepEventLoopAlive"] =
    Napi::Function::New(env, KeepEventLoopAlive);
  exports["dontKeepEventLoopAlive"] =
    Napi::Function::New(env, DontKeepEventLoopAlive);
  exports["runCallback"] =
    Napi::Function::New(env, RunCallback);

  return exports;
}

NODE_API_MODULE(hello, Init)


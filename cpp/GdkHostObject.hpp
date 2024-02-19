#pragma once
#include <jsi/jsi.h>
#include <vector>
#include <gdk.h>
#include "utils.hpp"
#include "json.hpp"
#include <ReactCommon/CallInvoker.h>
#include "ThreadPool.hpp"

using namespace facebook;

class JSI_EXPORT GdkHostObject: public jsi::HostObject {
public:
    GdkHostObject(std::string dirUrl, jsi::Runtime& rt, std::shared_ptr<react::CallInvoker> jsCallInvoker);
    ~GdkHostObject();
    GA_session* session;
    std::string uuid;
    std::map<std::string, std::shared_ptr<jsi::Function>> handler;
    jsi::Runtime& rt;
    std::weak_ptr<react::CallInvoker> invoker;
    std::shared_ptr<ThreadPool> pool;
    
private:
    std::string sessionDirectoryUrl;

public:
    jsi::Value get(jsi::Runtime&, const jsi::PropNameID& name) override;
    std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime& rt) override;
};

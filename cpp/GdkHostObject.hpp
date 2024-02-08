#pragma once
#include <jsi/jsi.h>
#include <vector>
#include <gdk.h>
#include "utils.hpp"
#include "json.hpp"
#include "TwoFactorCall.hpp"

using namespace facebook;

class JSI_EXPORT GdkHostObject: public jsi::HostObject {
public:
    GdkHostObject(std::string dirUrl, jsi::Runtime& rt);
    ~GdkHostObject();
    GA_session* session;
    std::string uuid;
    std::map<std::string, std::shared_ptr<jsi::Function>> handler;
    jsi::Runtime& rt;
    
private:
    std::string sessionDirectoryUrl;

public:
    jsi::Value get(jsi::Runtime&, const jsi::PropNameID& name) override;
    std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime& rt) override;
};

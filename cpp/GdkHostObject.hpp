#pragma once
#include <jsi/jsi.h>
#include <vector>
#include <gdk.h>
#include "utils.hpp"

using namespace facebook;

class JSI_EXPORT GdkHostObject: public jsi::HostObject {
public:
    GdkHostObject();

public:
  jsi::Value get(jsi::Runtime&, const jsi::PropNameID& name) override;
  std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime& rt) override;
};

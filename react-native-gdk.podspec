require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

Pod::Spec.new do |s|
  s.name         = "react-native-gdk"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  s.platforms    = { :ios => "11.0" }
  s.source       = { :git => "https://github.com/vulpemventures/react-native-gdk/react-native-gdk.git", :tag => "#{s.version}" }

  s.source_files = "ios/**/*.{h,m,mm}", "cpp/**/*.{hpp,cpp,c,h}"
  s.vendored_framework = "libgreenaddress_full.xcframework"

  s.dependency "React-Core"
  s.dependency "React"
  s.dependency "React-callinvoker"
  s.dependency "ReactCommon/turbomodule/core"
end

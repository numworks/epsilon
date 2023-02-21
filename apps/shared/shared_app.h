#ifndef SHARED_APP_H
#define SHARED_APP_H

#include <escher/app.h>

namespace Shared {

class SharedApp : public Escher::App {
 public:
  class Snapshot : public App::Snapshot {
   public:
    void pack(App* app) override;
  };
};

}  // namespace Shared
#endif

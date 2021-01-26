#ifndef SHARED_APP_H
#define SHARED_APP_H

#include <escher/app.h>

class SharedApp : public App {
  public:
  class Snapshot : public App::Snapshot {
    public:
    void pack(App * app) override;
  };
};

#endif
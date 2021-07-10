//
// 2017/6/3 Waync created.
//

#define GOOD_SUPPORT_STGE
#include "app/emsc_app.h"

class GoodApp : public good::rt::EmscApplication<GoodApp>
{
  GoodApp()
  {
  }
public:
  static GoodApp& getInst()
  {
    static GoodApp i;
    return i;
  }
};

GoodApp &app = GoodApp::getInst();

#include "app/emsc_lib.h"

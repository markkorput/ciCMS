#include "catch.hpp"

#include "ciCMS/State.h"

using namespace cms;

TEST_CASE("cms::State"){
  SECTION("isInitialized(), val(), push(), stopPushes()"){
    State<float> value;
    float target = 0.0f;
    float delta = 0.0f;
    REQUIRE(value.isInitialized() == false);
    // REQUIRE(value.val() == 0.0f); // not guaranteed
    value.push([&target](const float &val){ target = val; }, &target);
    value.changeSignal.connect([&delta](State<float>::ChangeArgs& args){ delta = args.current - args.previous; });
    REQUIRE(target == 0.0f);
    value = 5.0f;
    REQUIRE(value.val() == 5.0f);
    REQUIRE(target == 5.0f);
    REQUIRE(delta == 5.0f);
    value = 6.0f;
    REQUIRE(value.val() == 6.0f);
    REQUIRE(target == 6.0f);
    REQUIRE(delta == 1.0f);
    value.stopPushes(&target);
    value = 7.0f;
    REQUIRE(value.val() == 7.0f);
    REQUIRE(target == 6.0f);
    REQUIRE(delta == 1.0f);
  }

  SECTION("when"){
    State<int> state;
    state = 4;
    int four = 0;
    int notfour = 0;
    state.when(4, [&four](){four++;});
    state.whenNot(4, [&notfour](){notfour++;});
    REQUIRE(four == 1);
    REQUIRE(notfour == 0);
    state = 2;
    REQUIRE(four == 1);
    REQUIRE(notfour == 1);
    state = 4;
    REQUIRE(four == 2);
    REQUIRE(notfour == 1);
    int three = 0;
    state.whenOnce(3, [&three](){three++;});
    REQUIRE(three == 0);
    state = 3;
    REQUIRE(three == 1);
    state = 2;
    REQUIRE(three == 1);
    state = 3;
    REQUIRE(three == 1);
  }

  SECTION("push-to-other-state"){
    State<float> s1, s2;
    s1 = 1.0f;
    s2 = 2.0f;
    s1.push(s2);
    s1 = 3.0f;
    REQUIRE(s2.val() == 3.0f);
  }
}

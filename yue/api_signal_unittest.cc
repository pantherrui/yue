// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"
#include "yue/builtin_loader.h"

class YueSignalTest : public testing::Test {
 protected:
  void SetUp() override {
    luaL_openlibs(state_);
    yue::InsertBuiltinModuleLoader(state_);
    luaL_dostring(state_, "win = require('yue.GUI').Window.new{}");
  }

  lua::ManagedState state_;
};

void OnClose(bool* ptr) {
  *ptr = true;
}

TEST_F(YueSignalTest, Connect) {
  bool closed = false;
  lua::Push(state_, base::Bind(&OnClose, &closed));
  lua_setglobal(state_, "callback");
  ASSERT_FALSE(luaL_dostring(state_,
      "win.onclose:connect(callback)\n"
      "win:close()"));
  EXPECT_TRUE(closed);
}

TEST_F(YueSignalTest, ConnectCheckParam) {
  ASSERT_TRUE(luaL_dostring(state_, "win.onclose:connect(123)\n"));
  std::string error;
  ASSERT_TRUE(lua::Pop(state_, &error));
  EXPECT_EQ(error, "first arg must be function");
}

TEST_F(YueSignalTest, Disconnect) {
  bool closed = false;
  lua::Push(state_, base::Bind(&OnClose, &closed));
  lua_setglobal(state_, "callback");
  ASSERT_FALSE(luaL_dostring(state_,
      "local id = win.onclose:connect(callback)\n"
      "win.onclose:disconnect(id)\n"
      "win:close()"));
  EXPECT_FALSE(closed);
}

TEST_F(YueSignalTest, DisconnectAll) {
  bool closed = false;
  lua::Push(state_, base::Bind(&OnClose, &closed));
  lua_setglobal(state_, "callback");
  ASSERT_FALSE(luaL_dostring(state_,
      "local id = win.onclose:connect(callback)\n"
      "win.onclose:disconnectall()\n"
      "win:close()"));
  EXPECT_FALSE(closed);
}

TEST_F(YueSignalTest, Cached) {
  ASSERT_FALSE(luaL_dostring(state_, "return win.onclose"));
  ASSERT_FALSE(luaL_dostring(state_, "return win.onclose"));
  EXPECT_TRUE(lua::Compare(state_, 1, 2, lua::CompareOp::EQ));
}

TEST_F(YueSignalTest, GarbageCollected) {
  ASSERT_FALSE(luaL_dostring(state_,
      "signal = win.onclose\n"
      "win = nil"));
  lua::CollectGarbage(state_);
  std::string error;
  ASSERT_TRUE(luaL_dostring(state_, "signal:disconnect(1)\n"));
  ASSERT_TRUE(lua::Pop(state_, &error));
  EXPECT_EQ(error, "owner of signal is gone");
  ASSERT_TRUE(luaL_dostring(state_, "signal:connect(function() end)\n"));
  ASSERT_TRUE(lua::Pop(state_, &error));
  EXPECT_EQ(error, "owner of signal is gone");
}

/**
* Copyright 2018 Dynatrace LLC
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef _TEST_CACHING_MOCKOBSERVER_H
#define _TEST_CACHING_MOCKOBSERVER_H

#include "caching/IObserver.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace caching;
namespace test
{
	class MockObserver : public IObserver
	{
	public:
		MockObserver()
		{
		}

		virtual ~MockObserver() {}
		
		MOCK_METHOD0(update, void());
	};
}
#endif

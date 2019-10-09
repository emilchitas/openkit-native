/**
* Copyright 2018-2019 Dynatrace LLC
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

#include "CustomMatchers.h"
#include "Types.h"
#include "MockTypes.h"
#include "../objects/MockTypes.h"
#include "../../protocol/NullLogger.h"
#include "../../protocol/Types.h"
#include "../../protocol/MockTypes.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace test::types;

class BeaconSendingFlushSessionsStateTest : public testing::Test
{
protected:

	BeaconSendingFlushSessionsStateTest()
		: mLogger(nullptr)
		, mMockContext(nullptr)
		, mMockSession1Open(nullptr)
		, mMockSession2Open(nullptr)
		, mMockSession3Closed(nullptr)
	{
	}

	void SetUp()
	{
		mLogger = std::make_shared<NullLogger>();
		mMockSession1Open = std::make_shared<MockNiceSession_t>(mLogger);
		mMockSession2Open = std::make_shared<MockNiceSession_t>(mLogger);
		mMockSession3Closed = std::make_shared<MockNiceSession_t>(mLogger);

		mMockContext = std::make_shared<MockNiceBeaconSendingContext_t>(mLogger);
		ON_CALL(*mMockContext, getHTTPClient())
			.WillByDefault(testing::Return(mMockHTTPClient));

		ON_CALL(*mMockContext, getAllNewSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &MockBeaconSendingContext_t::RealGetAllNewSessions));
		ON_CALL(*mMockContext, getAllOpenAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &MockBeaconSendingContext_t::RealGetAllOpenAndConfiguredSessions));
		ON_CALL(*mMockContext, getAllFinishedAndConfiguredSessions())
			.WillByDefault(testing::Invoke(&*mMockContext, &MockBeaconSendingContext_t::RealGetAllFinishedAndConfiguredSessions));
		ON_CALL(*mMockContext, finishSession(testing::_))
			.WillByDefault(testing::WithArgs<0>(testing::Invoke(&*mMockContext, &MockBeaconSendingContext_t::RealFinishSession)));

		auto httpClientConfiguration = std::make_shared<HttpClientConfiguration_t>(Utf8String_t(""), 0, Utf8String_t(""));
		mMockHTTPClient = std::make_shared<MockNiceHttpClient_t>(httpClientConfiguration);
		ON_CALL(*mMockHTTPClient, sendNewSessionRequestRawPtrProxy())
			.WillByDefault(testing::Invoke([&]() -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 200, Response_t::ResponseHeaders());
			})
		);

		ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 200, Response_t::ResponseHeaders());
			})
		);
		ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 200, Response_t::ResponseHeaders());
			})
		);
		ON_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
			.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
			{
				return std::make_shared<StatusResponse_t>(mLogger, "", 200, Response_t::ResponseHeaders());
			})
		);
		ON_CALL(*mMockSession1Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<BeaconConfiguration_t>()));
		ON_CALL(*mMockSession2Open, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<BeaconConfiguration_t>()));
		ON_CALL(*mMockSession3Closed, getBeaconConfiguration())
			.WillByDefault(testing::Return(std::make_shared<BeaconConfiguration_t>()));

		mMockContext->startSession(mMockSession1Open);
		mMockContext->startSession(mMockSession2Open);
		mMockContext->startSession(mMockSession3Closed);
		mMockContext->finishSession(mMockSession3Closed);

	}

	void TearDown()
	{
		mLogger = nullptr;
		mMockContext = nullptr;
		mMockSession1Open = nullptr;
		mMockSession2Open = nullptr;
		mMockSession3Closed = nullptr;
	}

	std::ostringstream devNull;
	ILogger_sp mLogger;
	MockNiceBeaconSendingContext_sp mMockContext;
	MockNiceSession_sp mMockSession1Open;
	MockNiceSession_sp mMockSession2Open;
	MockNiceSession_sp mMockSession3Closed;
	MockNiceHttpClient_sp mMockHTTPClient;
};

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateIsNotATerminalState)
{
	// given
	auto target = BeaconSendingFlushSessionState_t();

	// verify that BeaconSendingCaptureOffState is not a terminal state
	EXPECT_FALSE(target.isTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateHasTerminalStateBeaconSendingTerminalState)
{
	// given
	auto target = BeaconSendingFlushSessionState_t();
	auto terminalState = target.getShutdownState();

	// verify that terminal state is BeaconSendingTerminalState
	ASSERT_THAT(terminalState, IsABeaconSendingTerminalState());
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateTransitionsToTerminalStateWhenDataIsSent)
{
	// given
	auto target = BeaconSendingFlushSessionState_t();

	// verify transition to terminal state
	EXPECT_CALL(*mMockContext, setNextState(IsABeaconSendingTerminalState()))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateRequestsNewSessionAndMulitplicity)
{
	// given
	auto target = BeaconSendingFlushSessionState_t();

	// verify that new sessions are handled correctly
	EXPECT_CALL(*mMockSession1Open, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, setBeaconConfiguration(testing::_))
		.Times(testing::Exactly(1));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionsStateClosesOpenSessions)
{
	// given
	auto target = BeaconSendingFlushSessionState_t();

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, end())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, end())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Closed, end())
		.Times(testing::Exactly(0));//has already been closed

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateSendsAllOpenAndClosedBeacons)
{
	// given
	auto target = BeaconSendingFlushSessionState_t();

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));

	// move open sessions to finished session by calling BeaconSendinContext::finishSessions
	mMockContext->finishSession(mMockSession1Open);
	mMockContext->finishSession(mMockSession2Open);

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateDoesNotSendIfSendingIsNotAllowed)
{
	//given
	auto target = BeaconSendingFlushSessionState_t();

	auto beaconConfiguration = std::make_shared<BeaconConfiguration_t>(0, DataCollectionLevel_t::OFF, CrashReportingLevel_t::OFF);

	ON_CALL(*mMockSession1Open, getBeaconConfiguration())
		.WillByDefault(testing::Return(beaconConfiguration));
	ON_CALL(*mMockSession2Open, getBeaconConfiguration())
		.WillByDefault(testing::Return(beaconConfiguration));
	ON_CALL(*mMockSession3Closed , getBeaconConfiguration())
		.WillByDefault(testing::Return(beaconConfiguration));

	//verify that session is closed without reporting data
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));

	// when calling execute
	target.execute(*mMockContext);
}

TEST_F(BeaconSendingFlushSessionsStateTest, getStateNameReturnsCorrectStateName)
{
	// given
	BeaconSendingFlushSessionState_t target;

	// when
	auto stateName = target.getStateName();

	// then
	ASSERT_STREQ(stateName, "FlushSessions");
}

TEST_F(BeaconSendingFlushSessionsStateTest, aBeaconSendingFlushSessionStateStopsSendingIfTooManyRequestsResponseWasReceived)
{
	//given
	auto target = BeaconSendingFlushSessionState_t();

	auto responseHeaders = Response_t::ResponseHeaders
	{
		{ "retry-after",  { "123456" } }
	};
	ON_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 429, responseHeaders);
		})
	);
	ON_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 429, responseHeaders);
		})
	);
	ON_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.WillByDefault(testing::Invoke([&](IHttpClientProvider_sp) -> StatusResponse_sp
		{
			return std::make_shared<StatusResponse_t>(mLogger, "", 429, responseHeaders);
		})
	);

	// verify that open sessions are closed
	EXPECT_CALL(*mMockSession1Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession3Closed, sendBeaconRawPtrProxy(testing::_))
		.Times(testing::Exactly(0));
	EXPECT_CALL(*mMockSession1Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession2Open, clearCapturedData())
		.Times(testing::Exactly(1));
	EXPECT_CALL(*mMockSession3Closed, clearCapturedData())
		.Times(testing::Exactly(1));

	// move open sessions to finished session by calling BeaconSendinContext::finishSessions
	mMockContext->finishSession(mMockSession1Open);
	mMockContext->finishSession(mMockSession2Open);

	// when calling execute
	target.execute(*mMockContext);
}
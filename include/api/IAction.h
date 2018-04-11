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

#ifndef _API_IACTION_H
#define _API_IACTION_H

#include "api/IRootAction.h"
#include <stdint.h>
#include <memory>

namespace api
{
	///
	/// This interface provides functionality to report events/values/errors and traces web requests.
	///
	class IAction
	{
	public:
		///
		/// Destructor
		///
		virtual ~IAction() {}

		///
		/// Reports an event with a specified name (but without any value).
		///
		/// If given @c eventName is @c nullptr then no event is reported to the system.
		///
		/// @param eventName name of the event
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportEvent(const char* eventName) = 0;

		///
		/// Reports an int value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportValue(const char* valueName, int32_t value) = 0;

		///
		/// Reports a double value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportValue(const char* valueName, double value) = 0;

		///
		/// Reports a String value with a specified name.
		///
		/// @param valueName name of this value
		/// @param value     value itself
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportValue(const char* valueName, const char* value) = 0;

		///
		/// Reports an error with a specified name, error code and reason.
		///
		/// @param errorName name of this error
		/// @param errorCode numeric error code of this error
		/// @param reason    reason for this error
		/// @return this Action (for usage as fluent API)
		///
		virtual std::shared_ptr<IAction> reportError(const char* errorName, int32_t errorCode, const char* reason) = 0;

		///
		/// Leaves this Action.
		/// @returns the parent Action, or @c nullptr if there is no parent Action
		///
		virtual std::shared_ptr<IRootAction> leaveAction() = 0;

		///
		/// Returns true if this instance of IAction is an object using 
		/// the NullObject pattern
		/// @returns @c true if the current IAction is a NullObject, @c false in all other cases
		///
		virtual bool isNullObject() const = 0;
	};
}

#endif
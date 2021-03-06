// Copyright 2020 Sapphire development team. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_CORE_EVENT_GUARD
#define SAPPHIRE_CORE_EVENT_GUARD

#include <vector>

#include <Collections/Debug>

namespace Sa
{
	namespace Internal
	{
		template <typename FuncHandle, typename FuncMemberHandle, typename R, typename... Args>
		class EventBase
		{
		protected:
			std::vector<FuncHandle> mFunctions;
			std::vector<FuncMemberHandle> mMemberFunctions;

		public:
			EventBase() = default;

			EventBase(EventBase&&) = delete;
			EventBase(const EventBase&) = delete;

			~EventBase();


			bool IsEmpty() const noexcept;

			void Clear();


			void Add(R(*_func)(Args...));

			template <typename C>
			void Add(C* _caller, R(C::* _func)(Args...));


			void Remove(R(*_func)(Args...));

			template <typename C>
			void Remove(const C* _caller, R(C::* _func)(Args...));


			void RRemove(R(*_func)(Args...));

			template <typename C>
			void RRemove(const C* _caller, R(C::* _func)(Args...));


			void Execute(const Args&... _args);


			EventBase& operator=(EventBase&&) = delete;
			EventBase& operator=(const EventBase&) = delete;

			void operator+=(R(*_func)(Args...));
			void operator-=(R(*_func)(Args...));

			void operator()(const Args&... _args);
		};

		struct FuncMemberDataBase
		{
			// Ensure correct destruction on delete.
			virtual ~FuncMemberDataBase() = default;
		};

		template <typename C, typename R, typename... Args>
		struct FuncMemberData : public FuncMemberDataBase
		{
			C* caller = nullptr;
			R(C::* func)(Args...) = nullptr;

			FuncMemberData(C* _caller, R(C::* _func)(Args...)) :
				caller{ _caller },
				func{ _func }
			{}
		};


		template <typename... Args>
		struct FuncHandle_void
		{
			void(*func)(Args...) = nullptr;
		};

		template <typename... Args>
		struct FuncMemberHandle_void
		{
			FuncMemberDataBase* data = nullptr;
			void(*func)(void*, Args...) = nullptr;
		};


		template <typename R, typename... Args>
		struct FuncHandle_R
		{
			R(*func)(Args...) = nullptr;
			R* result = nullptr;
		};

		template <typename R, typename... Args>
		struct FuncMemberHandle_R
		{
			FuncMemberDataBase* data = nullptr;
			R(*func)(void*, Args...) = nullptr;
			R* result = nullptr;
		};
	}


	template <typename R, typename... Args>
	class Event;

	template <typename... Args>
	class Event<void(Args...)> : public Internal::EventBase<Internal::FuncHandle_void<Args...>, Internal::FuncMemberHandle_void<Args...>, void, Args...>
	{
	};


	template <typename R, typename... Args>
	class Event<R(Args...)> : public Internal::EventBase<Internal::FuncHandle_R<R, Args...>, Internal::FuncMemberHandle_R<R, Args...>, R, Args...>
	{
		using FuncHandle = Internal::FuncHandle_R<R, Args...>;
		using FuncMemberHandle = Internal::FuncMemberHandle_R<R, Args...>;
		using Base = Internal::EventBase<FuncHandle, FuncMemberHandle, R, Args...>;

		using Base::mFunctions;
		using Base::mMemberFunctions;

	public:
		using Base::Add;

		void Add(R(*_func)(Args...), R* _result);

		template <typename C>
		void Add(C * _caller, R(C:: * _func)(Args...), R* _result);

		void Execute(const Args&... _args);

		void operator()(const Args&... _args);
	};
}

#include <Core/Types/Variadics/Event.inl>

#endif // GUARD

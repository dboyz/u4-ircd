/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         binder.hpp
 * Description  Object function binders
 *
 * All parts of this program are Copyright(C) 2009 by their
 * respective authors and the UnrealIRCd development team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 ******************************************************************/

#ifndef _UNREALIRCD_BINDER_HPP
#define _UNREALIRCD_BINDER_HPP

/**
 * This is a simple object function binder implementation for UnrealIRCd.
 * The idea behind all it is to provide a way to provide object-based callback
 * methods.
 *
 * Example:
 * You have a class with a basic method to call.
 *
 *   class MyClass {
 *       public: void test(int av);
 *   } cl;
 *
 * And you've got a global function to call.
 *
 *   void testfunc(int av);
 *
 * Let's assume you have some kind of function that triggers an
 * callback function.
 *
 *    registerCallback(testfunc);  // works just fine
 *    registerCallback(cl.test);   // doesn't work
 *
 * Now, we can use a binder object to make it possible to register an object
 * callback:
 *
 *    UnrealBinder1<MyClass, int, void> cb(&MyClass::test, &cl);
 *    registerCallback(cb);
 *
 */

/**
 * Object binder w/o arguments.
 */
template<class ClassType, typename ReturnType = void>
class UnrealBinder0
{
public:
	UnrealBind0(ReturnType (ClassType::*mf)(), ClassType* cl)
			: member_(mf), cl_(cl)
	{ }

	ReturnType operator()()
	{
		(cl_->*member_)();
	}

private:
	ReturnType (ClassType::*member_)();
	ClassType* cl_;
};

/**
 * Object binder w/ 1 argument.
 */
template<class ClassType, typename Arg1Type, typename ReturnType = void>
class UnrealBinder1
{
public:
	UnrealBind1(ReturnType (ClassType::*mf)(Arg1Type), ClassType* cl)
			: member_(mf), cl_(cl)
	{ }

	ReturnType operator()(Arg1Type arg1)
	{
		(cl_->*member_)(arg1);
	}

private:
	ReturnType (ClassType::*member_)(Arg1Type);
	ClassType* cl_;
};

/**
 * Object binder w/ 2 arguments.
 */
template<class ClassType, typename Arg1Type, typename Arg2Type,
		typename ReturnType = void>
class UnrealBinder2
{
public:
	UnrealBind2(ReturnType (ClassType::*mf)(Arg1Type, Arg2Type), ClassType* cl)
			: member_(mf), cl_(cl)
	{ }

	ReturnType operator()(Arg1Type arg1, Arg2Type arg2)
	{
		(cl_->*member_)(arg1, arg2);
	}

private:
	ReturnType (ClassType::*member_)(Arg1Type, Arg2Type);
	ClassType* cl_;
};

#endif /* _UNREALIRCD_BINDER_HPP */


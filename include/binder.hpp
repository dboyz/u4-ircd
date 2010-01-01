/*****************************************************************
 * Unreal Internet Relay Chat Daemon, Version 4
 * File         binder.hpp
 * Description  Object function binders
 *
 * Copyright(C) 2009, 2010
 * The UnrealIRCd development team and contributors
 * http://www.unrealircd.com
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
 * Object binder w/o arguments.
 */
template<class ClassType, typename ReturnType = void>
class UnrealBinder0
{
public:
	UnrealBinder0(ReturnType (ClassType::*mf)() = 0, ClassType* cl = 0)
			: member_(mf), cl_(cl)
	{ }

	void bind(ReturnType (ClassType::*mf)(), ClassType* cl)
	{
		member_ = mf;
		cl_ = cl;
	}

	inline ReturnType emit()
	{
		return (cl_->*member_)();
	}

	inline ReturnType operator()()
	{
		return emit();
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
	UnrealBinder1(ReturnType (ClassType::*mf)(Arg1Type) = 0, ClassType* cl = 0)
			: member_(mf), cl_(cl)
	{ }

	void bind(ReturnType (ClassType::*mf)(Arg1Type), ClassType* cl)
	{
		member_ = mf;
		cl_ = cl;
	}

	inline ReturnType emit(Arg1Type arg1)
	{
		return (cl_->*member_)(arg1);
	}

	inline ReturnType operator()(Arg1Type arg1)
	{
		return emit(arg1);
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
	UnrealBinder2(ReturnType (ClassType::*mf)(Arg1Type, Arg2Type) = 0,
	    ClassType* cl = 0)
			: member_(mf), cl_(cl)
	{ }

	void bind(ReturnType (ClassType::*mf)(Arg1Type, Arg2Type), ClassType* cl)
	{
		member_ = mf;
		cl_ = cl;
	}

	inline ReturnType emit(Arg1Type arg1, Arg2Type arg2)
	{
		return (cl_->*member_)(arg1, arg2);
	}

	inline ReturnType operator()(Arg1Type arg1, Arg2Type arg2)
	{
		return emit(arg1, arg2);
	}

private:
	ReturnType (ClassType::*member_)(Arg1Type, Arg2Type);
	ClassType* cl_;
};

#endif /* _UNREALIRCD_BINDER_HPP */


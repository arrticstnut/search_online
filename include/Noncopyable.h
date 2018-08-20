///
/// @file    Noncopyable.h
/// @author
///

#ifndef __CC_NONCOPYABLE_H__
#define __CC_NONCOPYABLE_H__


namespace cc
{

	class Noncopyable
	{
		protected:
			Noncopyable(){}
			~Noncopyable(){}

		private:
			Noncopyable(const Noncopyable &);
			Noncopyable & operator=(const Noncopyable &);
	};
}//end of namespace cc

#endif

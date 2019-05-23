// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OUROBOROS_PROFILE_H
#define OUROBOROS_PROFILE_H

#include "debug_helper.h"
#include "common/common.h"
#include "common/timer.h"
#include "common/timestamp.h"

namespace Ouroboros
{

#if ENABLE_WATCHERS

class ProfileVal;

class ProfileGroup
{
public:
	ProfileGroup(std::string name = "default");
	~ProfileGroup();

	typedef std::vector<ProfileVal*> PROFILEVALS;
	typedef PROFILEVALS::iterator iterator;

	static ProfileGroup & defaultGroup();

	PROFILEVALS & stack() { return stack_; }
	void add(ProfileVal * pVal);

	iterator begin() { return profiles_.begin(); }
	iterator end() { return profiles_.end(); }

	ProfileVal * pRunningTime() { return profiles_[0]; }
	const ProfileVal * pRunningTime() const { return profiles_[0]; }
	TimeStamp runningTime() const;

	INLINE const char* name() const;

	bool initializeWatcher();

	static void finalise(void);

	INLINE const ProfileGroup::PROFILEVALS& profiles() const;

private:
	PROFILEVALS profiles_;
	PROFILEVALS stack_;
	std::string name_;
};

class ProfileVal
{
public:
	ProfileVal(std::string name, ProfileGroup * pGroup = NULL);
	~ProfileVal();

	bool initializeWatcher();

	void start()
	{
		if(!initWatcher_ && count_ > 10)
			initializeWatcher();

		TimeStamp now = timestamp();

		// Record the first few processing
		if (inProgress_++ == 0)
			lastTime_ = now;

		ProfileGroup::PROFILEVALS & stack = pProfileGroup_->stack();

		// If there is an object in the stack, it is called from the previous ProfileVal function.
		// We can get a piece of time before the previous function enters this function.
		// then add it to sumIntTime_
		if (!stack.empty()){
			ProfileVal & profile = *stack.back();
			profile.lastIntTime_ = now - profile.lastIntTime_;
			profile.sumIntTime_ += profile.lastIntTime_;
		}

		// push yourself on the stack
		stack.push_back(this);

		// Record start time
		lastIntTime_ = now;
	}

	void stop(uint32 qty = 0)
	{
		TimeStamp now = timestamp();

		// If it is 0, it means that it is the generation of the call stack.
		// Here we can get the total time spent on this function
		if (--inProgress_ == 0){
			lastTime_ = now - lastTime_;
			sumTime_ += lastTime_;
		}

		lastQuantity_ = qty;
		sumQuantity_ += qty;
		++count_;

		ProfileGroup::PROFILEVALS & stack = pProfileGroup_->stack();
		OURO_ASSERT( stack.back() == this );

		stack.pop_back();

		// Get the time spent by this function
		lastIntTime_ = now - lastIntTime_;
		sumIntTime_ += lastIntTime_;

		// We need to reset the last internal time of the profile object in the previous function.
		// Make it possible to get the correct start at the start when you enter the next profile function after calling this function
		// time slice
		if (!stack.empty())
			stack.back()->lastIntTime_ = now;
	}

	INLINE bool stop( const char * filename, int lineNum, uint32 qty = 0);

	INLINE bool running() const;

	INLINE const char * c_str() const;

	INLINE TimeStamp sumTime() const;
	INLINE TimeStamp lastIntTime() const ;
	INLINE TimeStamp sumIntTime() const ;
	INLINE TimeStamp lastTime() const;

	INLINE double lastTimeInSeconds() const;
	INLINE double sumTimeInSeconds() const ;
	INLINE double lastIntTimeInSeconds() const ;
	INLINE double sumIntTimeInSeconds() const;

	INLINE const char* name() const;

	INLINE uint32 count() const;

	INLINE bool isTooLong() const;

	static void setWarningPeriod(TimeStamp warningPeriod) { warningPeriod_ = warningPeriod; }

	// name
	std::string		name_;

	// ProfileGroup pointer
	ProfileGroup * pProfileGroup_;

	// The time after startd.
	TimeStamp		lastTime_;

	// total time of count_ times
	TimeStamp		sumTime_;

	// Record the last internal time slice
	TimeStamp		lastIntTime_;

	// count_ times internal total time
	TimeStamp		sumIntTime_;

	uint32			lastQuantity_;	///< The last value passed into stop.
	uint32			sumQuantity_;	///< The total of all values passed into stop.
	uint32			count_;			///< The number of times stop has been called.

	// Record the first few processing, such as recursion, etc.
	int				inProgress_;

	bool			initWatcher_;

private:
	static TimeStamp warningPeriod_;

};

class ScopedProfile
{
public:
	ScopedProfile(ProfileVal & profile, const char * filename, int lineNum) :
		profile_(profile),
		filename_(filename),
		lineNum_(lineNum)
	{
		profile_.start();
	}

	~ScopedProfile()
	{
		profile_.stop(filename_, lineNum_);
	}

private:
	ProfileVal& profile_;
	const char* filename_;
	int lineNum_;

};

#define START_PROFILE( PROFILE ) PROFILE.start();

#define STOP_PROFILE( PROFILE )	PROFILE.stop( __FILE__, __LINE__ );

#define AUTO_SCOPED_PROFILE( NAME )												\
	static ProfileVal _localProfile( NAME );									\
	ScopedProfile _autoScopedProfile( _localProfile, __FILE__, __LINE__ );

#define SCOPED_PROFILE(PROFILE)													\
	ScopedProfile PROFILE##_scopedProfile(PROFILE, __FILE__, __LINE__);

#define STOP_PROFILE_WITH_CHECK( PROFILE )										\
	if (PROFILE.stop( __FILE__, __LINE__ ))

#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )									\
	PROFILE.stop( __FILE__, __LINE__ , DATA );

// This gives the system profile time
uint64 runningTime();

#else

#define AUTO_SCOPED_PROFILE( NAME )
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )
#define STOP_PROFILE_WITH_CHECK( PROFILE )
#define SCOPED_PROFILE(PROFILE)
#define STOP_PROFILE( PROFILE )
#define START_PROFILE( PROFILE )

uint64 runningTime(){
	return 0;
}

#endif //ENABLE_WATCHERS


}

#ifdef CODE_INLINE
#include "profile.inl"
#endif

#endif // OUROBOROS_PROFILE_H



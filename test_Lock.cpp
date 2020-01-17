#include "Lock.h"
#include <gtest/gtest.h>

using namespace julia;

TEST(test_pthreadSpinLock, test)
{
	PthreadSpinLock l1, l2;
	bool rc = l1.lock();
	ASSERT_EQ(true, rc);
	rc = l2.lock();
	ASSERT_EQ(true, rc);

	rc = l1.unlock();
	ASSERT_EQ(true, rc);
	rc = l2.unlock();
	ASSERT_EQ(true, rc);
}

TEST(test_pthreadMutexLock, test)
{
	PthreadMutexLock l1, l2;
	bool rc = l1.lock();
	ASSERT_EQ(true, rc);
	rc = l1.isLockedByThisThread();
	ASSERT_EQ(true, rc);
	
	rc = l2.lock();
	ASSERT_EQ(true, rc);

	rc = l1.unlock();
	ASSERT_EQ(true, rc);
	rc = l2.unlock();
	ASSERT_EQ(true, rc);
}


TEST(test_atomicSpinLock, test)
{
	AtomicSpinLock l1, l2;
	bool rc = l1.lock();
	ASSERT_EQ(true, rc);
	rc = l2.lock();
	ASSERT_EQ(true, rc);

	rc = l1.unlock();
	ASSERT_EQ(true, rc);
	rc = l2.unlock();
	ASSERT_EQ(true, rc);
}

TEST(test_guard_pthread, test)
{
	PthreadSpinLock lock;
	bool rc;
	{
		LockGuard<PthreadSpinLock> guard(lock);
	}
	// try locking again, this should work
	rc = lock.lock();
	ASSERT_EQ(true, rc);
	lock.unlock();
}

TEST(test_guard_atomic, test)
{
	AtomicSpinLock lock;
	bool rc;
	{
		LockGuard<AtomicSpinLock> guard(lock);
	}
	rc = lock.lock();
	ASSERT_EQ(true, rc);
	lock.unlock();
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

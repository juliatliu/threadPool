#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <mutex>
#include "ThreadPool.h"

using namespace julia;
std::mutex g_mutex;

TEST(test_ThreadPool, test)
{
	boost::shared_ptr<ThreadPool> pool( new ThreadPool("MainThreadPool") );
	pool->start(5);
	

	for(int i = 0; i < 100; ++i) {
		std::ostringstream ostr;
		ostr << "task " << i << " ";
		pool->run( [&](){
			std::cout << "tid = " << ::syscall(SYS_gettid) << ", str = " << ostr.str() << std::endl; 
		});
	}
	pool->stop();
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

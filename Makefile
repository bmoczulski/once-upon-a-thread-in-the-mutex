CXXFLAGS += -Wall -Wextra -Werror -ggdb3 -O1
CXXFLAGS += -std=c++20
GCC_15=docker run --rm --workdir "$(CURDIR)" -v "$(CURDIR):$(CURDIR)" gcc:15.1 g++

TARGETS = ptmutex-test ptmutex-test-gcc12 ptmutex-test-clang account account-TSA-gcc account-NO-TSA account-tsan account-TSA sv-bm sv-gcc avoid
all: $(TARGETS)

ptmutex-test-gcc12: CXX=g++-12
ptmutex-test-gcc12: ptmutex-test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

ptmutex-test-clang: CXX=clang++
ptmutex-test-clang: CXXFLAGS+=-stdlib=libc++ -fexperimental-library
ptmutex-test-clang: ptmutex-test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

account-TSA-gcc: account-TSA.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

account-TSA: CXX=clang++
account-TSA: CXXFLAGS+=-stdlib=libc++ -Wthread-safety -Wthread-safety-beta -D_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS
account-TSA: account-TSA.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

account-NO-TSA: CXX=clang++
account-NO-TSA: account-TSA.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

account-tsan: CXX=clang++
account-tsan: CXXFLAGS+=-fsanitize=thread
account-tsan: account-TSA.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

sv-gcc: CXX=$(GCC_15)
sv-gcc: CPPFLAGS+=-DUSE_BM_SV=0
sv-gcc: sv.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

sv-bm: CXX=$(GCC_15)
sv-bm: CPPFLAGS+=-DUSE_BM_SV=1
sv-bm: sv.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

avoid: CXX=$(GCC_15)

synchronized_value:
	docker run --rm gcc:15.1 cat /usr/local/include/c++/15.1.0/experimental/synchronized_value > $@

clean:
	$(RM) $(TARGETS)
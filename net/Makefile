FILES = $(wildcard *.cpp)
OBJS =  $(patsubst %.cpp, %.o , $(FILES))

CPPFLAGS = -O2 -pthread -std=c++11 -D__STDC_FORMAT_MACROS 
all:$(OBJS)
$(OBJS):%.o:%.cpp
	$(CXX) -c $< -o $(OBJSDIR)/$@  $(CPPFLAGS)
clean:
	-$(RM) $(OBJS)

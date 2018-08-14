OBJS = TimeStamp.o Exception.o ThreadPool.o StrUtil.o

all:$(OBJS)
$(OBJS):%.o:%.cpp
	$(CXX) -c $< -o $(OBJSDIR)/$@  $(CPPFLAGS)
clean:
	-$(RM) $(OBJS)

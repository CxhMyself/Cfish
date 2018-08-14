CXX = g++
#这里要考虑是不是定义MUDUO_STD_STRING
export OBJSDIR = ${pwd}/objs
TARGET = webServer


#这里注意下Json的链接 之后还有MySql的链接
JSONLIB_A = /usr/local/lib/libjson.a
CRYPTOPPLIB_A = /usr/local/lib/libcryptopp.a
MYSQLLIB_SO = -L/usr/lib/x86_64-linux-gnu -lmysqlclient

export CPPFLAGS = -O2 -pthread -std=c++11 -D__STDC_FORMAT_MACROS

$(TARGET): $(OBJSDIR) main.o
	$(MAKE) -C base
	$(MAKE) -C net
	${MAKE} -C http
	$(CXX) -o $(TARGET) -g $(OBJSDIR)/*.o $(CPPFLAGS) -ldl ${MYSQLLIB_SO} ${JSONLIB_A} ${CRYPTOPPLIB_A} -I /usr/include/mysql
main.o:%.o:%.cpp
	$(CXX) -c $< -o $(OBJSDIR)/$@ $(CPPFLAGS) -g 
$(OBJSDIR):
	mkdir -p $(OBJSDIR)
clean:
	-$(RM) $(TARGET)
	-$(RM) $(OBJSDIR)/*.o

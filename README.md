所用的网络库模仿muduo 可能区别很小 但是其中所有的 boost的东西全部换为STL C++11 中的使用
比如 boost::function<> boost::bind() 被替换为 std::function<> std::bind();
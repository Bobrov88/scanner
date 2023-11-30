#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/asio/impl/src.hpp>
#include <boost/regex.hpp>

int main() {
    using boost::asio;
    using namespace boost::filesystem;
    path p{"C:\\.conan\\"};
  try
  {
    file_status s = status(p);
    std::cout << std::boolalpha << is_directory(s) << '\n';
  }
  catch (filesystem_error &e)
  {
    std::cerr << e.what() << '\n';
  }
    std::cout<<"Success!"<<std::endl;

// io_service service;
// ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 2001);
// ip::tcp::socket sock(service);
// sock.connect(ep);


 std::string s = "Boost Libraries";
  boost::regex expr{"\\w+\\s\\w+"};
  std::cout << std::boolalpha << boost::regex_match(s, expr) << '\n';
    return 0;
}
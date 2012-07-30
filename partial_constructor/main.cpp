#include "named_constructor_list.hpp"

#include <boost/date_time.hpp>
#include <functional>
#include <memory>
#include <string>

using std::string;
using std::unique_ptr;
using boost::gregorian::date;
namespace mpl = boost::mpl;
namespace fusion = boost::fusion;

struct Foo
{
    Foo(int a, int b, string const& c, date const& d)
    : a_(a), b_(b), c_(c), d_(d)
    { }
    int a_;
    int b_;
    string c_;
    date d_;
};

struct A {}; struct B {}; struct C {}; struct D {};

typedef fusion::map<
    fusion::pair<A, int>,
    fusion::pair<B, int>,
    fusion::pair<C, string>,
    fusion::pair<D, date>> FooArguments;

template <typename... Tags>
struct list_of_foo : named_constructor_list<Foo, FooArguments, Tags...>
{
    list_of_foo() : named_constructor_list<Foo, FooArguments, Tags...>(
        fusion::make_map<A, B, C, D>(
            5, 8, "default", date(2012, 12, 25))) {}
    
    template <typename... Args>
    list_of_foo(Args const&... args) : list_of_foo() {
        (*this)(args...);
    }
};

int main()
{
    date d(2012, 7, 28);
    std::string s = "Bye";
    std::vector<Foo> f = list_of_foo<B, D, C>
        (13, d, "Hie")
        (21, date(2012, 7, 29), s);
    for (Foo const& foo : f) {
        using std::cout;
        using std::endl;
        cout << foo.a_ << ' ';
        cout << foo.b_ << ' ';
        cout << foo.c_ << ' ';
        cout << foo.d_ << ' ';
        cout << endl;
    }
}

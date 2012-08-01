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

struct A {}; struct B {}; struct C {}; struct D {};

typedef fusion::map<
fusion::pair<A, int>,
fusion::pair<B, int>,
fusion::pair<C, string>,
fusion::pair<D, date>> FooArguments;

struct Foo : named_constructor_list<Foo, FooArguments>
{
    Foo(int a, int b, string const& c, date const& d)
    : a_(a), b_(b), c_(c), d_(d)
    { }
    int a_;
    int b_;
    string c_;
    date d_;
};

int main()
{
    date d(2012, 7, 28);
    std::string s = "Bye";
    std::vector<Foo> f = Foo::list_of<B, D, C>
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

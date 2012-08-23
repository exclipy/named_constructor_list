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

struct Height {}; struct Weight {}; struct Name {}; struct Birthday {};

typedef fusion::map<
fusion::pair<Height, int>,
fusion::pair<Weight, int>,
fusion::pair<Name, string>,
fusion::pair<Birthday, date>> PersonArguments;

struct Person : named_constructor_list<Person, PersonArguments>
{
    Person(int height, int weight, string const& name, date const& birthday)
    : height_(height), weight_(weight), name_(name), birthday_(birthday)
    { }
    int height_;
    int weight_;
    string name_;
    date birthday_;
};

int main()
{
    std::vector<Person> f = Person::list_of<Name, Weight, Birthday>
        ("Millie", 73, date(1969, 7, 16))
        ("Sid", 23, date(2002, 5, 21));
    for (Person const& foo : f) {
        using std::cout;
        using std::endl;
        cout << foo.height_ << ' ';
        cout << foo.weight_ << ' ';
        cout << foo.name_ << ' ';
        cout << foo.birthday_ << ' ';
        cout << endl;
    }
}

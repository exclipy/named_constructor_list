#include <boost/date_time.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/value_at_key.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
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

struct A;
struct B;
struct C;
struct D;
typedef mpl::vector<A, B, C, D> arg_list;

template <typename... Tags>
struct FooBuilder
{
private:
    typedef mpl::vector<Tags...> tag_list;
    typedef fusion::map<
    fusion::pair<A, int>,
    fusion::pair<B, int>,
    fusion::pair<C, string>,
    fusion::pair<D, date>> argument_map;
    argument_map defaults_;
    
    struct ArgumentBuilder
    {
        argument_map args_;
        
        explicit ArgumentBuilder(argument_map const& args) : args_(args) {}
        
        template <typename TagList>
        void add() {
        }
        
        template <typename TagList, typename First, typename... Rest>
        void add(First const& first, Rest const&... rest) {
            fusion::at_key<typename mpl::front<TagList>::type>(args_) = first;
            add<typename mpl::pop_front<TagList>::type>(rest...);
        }
    };
    
    template <typename Tag>
    struct tag_type
    {
        typedef typename fusion::result_of::value_at_key<argument_map, Tag>::type type;
    };
    
public:
    FooBuilder() :
    defaults_(
              fusion::make_pair<A>(5),
              fusion::make_pair<B>(8),
              fusion::make_pair<C>("default"),
              fusion::make_pair<D>(date(2012, 12, 25)))
    {}
    unique_ptr<Foo> create(typename tag_type<Tags>::type const&... args) {
        ArgumentBuilder builder(defaults_);
        builder.template add<tag_list>(args...);
        return unique_ptr<Foo>(new Foo(
                                       fusion::at_key<A>(builder.args_),
                                       fusion::at_key<B>(builder.args_),
                                       fusion::at_key<C>(builder.args_),
                                       fusion::at_key<D>(builder.args_)));
    }
};

int main()
{
    date d(2012, 7, 28);
    unique_ptr<Foo> f = FooBuilder<B, D, C>().create(3, d, "Hi");
    std::cout << f->a_ << std::endl;
    std::cout << f->b_ << std::endl;
    std::cout << f->c_ << std::endl;
    std::cout << f->d_ << std::endl;
}

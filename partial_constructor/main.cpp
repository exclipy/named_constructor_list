#include <boost/date_time.hpp>
#include <boost/functional/factory.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/value_at_key.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/type_traits.hpp>
#include <functional>
#include <memory>
#include <string>

using std::string;
using std::unique_ptr;
using boost::gregorian::date;
namespace mpl = boost::mpl;
namespace fusion = boost::fusion;

template <typename Foo, typename ArgumentMap, typename... Tags>
struct GenericBuilder
{
private:
    typedef mpl::vector<Tags...> tag_list;
    ArgumentMap defaults_;
    
    struct ArgumentBuilder
    {
        ArgumentMap args_;
        
        explicit ArgumentBuilder(ArgumentMap const& args) : args_(args) {}
        
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
        typedef typename fusion::result_of::value_at_key<ArgumentMap, Tag>::type type;
    };
    
    struct ExtractValue
    {
        template<typename Sig>
        struct result;
        
        template<typename U>
        struct result<ExtractValue(U)>
        {
            typedef typename boost::remove_reference<U>::type::second_type const& type;
        };
        
        template <typename T>
        typename ExtractValue::result<ExtractValue(T)>::type const&
        operator()(T const& x) const
        {
            return x.second;
        }
    };
    
public:
    GenericBuilder(ArgumentMap const& defaults) : defaults_(defaults) {}
    
    unique_ptr<Foo> create(typename tag_type<Tags>::type const&... args) {
        ArgumentBuilder builder(defaults_);
        builder.template add<tag_list>(args...);
        return unique_ptr<Foo>(
            fusion::invoke(
                boost::factory<Foo*>(),
                fusion::transform_view<ArgumentMap, ExtractValue>(builder.args_, ExtractValue())));
    }
};

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

struct A {};
struct B {};
struct C {};
struct D {};

typedef fusion::map<
    fusion::pair<A, int>,
    fusion::pair<B, int>,
    fusion::pair<C, string>,
    fusion::pair<D, date>> FooArguments;

template <typename... Tags>
struct FooBuilder : GenericBuilder<Foo, FooArguments, Tags...>
{
    FooBuilder() : GenericBuilder<Foo, FooArguments, Tags...>(
        fusion::make_map<A, B, C, D>(
            5, 8, "default", date(2012, 12, 25))) {}
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

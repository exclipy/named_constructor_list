#ifndef partial_constructor_named_constructor_list_hpp
#define partial_constructor_named_constructor_list_hpp

#include <boost/assign/list_of.hpp>
#include <boost/functional/value_factory.hpp>
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

template <typename T, typename ArgumentMap, typename... Tags>
struct named_constructor_list :
public boost::assign_detail::converter<
named_constructor_list< BOOST_DEDUCED_TYPENAME boost::assign_detail::assign_decay<T>::type, ArgumentMap, Tags... >,
BOOST_DEDUCED_TYPENAME std::deque< BOOST_DEDUCED_TYPENAME
boost::assign_detail::assign_decay<T>::type>::iterator >
{
    typedef BOOST_DEDUCED_TYPENAME boost::assign_detail::assign_decay<T>::type Ty;
    typedef std::deque<Ty>  impl_type;
    mutable impl_type       values_;
    
public:
    typedef BOOST_DEDUCED_TYPENAME impl_type::iterator         iterator;
    typedef iterator                                           const_iterator;
    typedef BOOST_DEDUCED_TYPENAME impl_type::value_type       value_type;
    typedef BOOST_DEDUCED_TYPENAME impl_type::size_type        size_type;
    typedef BOOST_DEDUCED_TYPENAME impl_type::difference_type  difference_type;
    
public:
    iterator begin() const       { return values_.begin(); }
    iterator end() const         { return values_.end(); }
    bool empty() const           { return values_.empty(); }
    size_type size() const       { return values_.size(); }
    
private:
    typedef boost::mpl::vector<Tags...> tag_list;
    ArgumentMap defaults_;
    
    struct argument_builder
    {
        ArgumentMap args_;
        
        explicit argument_builder(ArgumentMap const& args) : args_(args) {}
        
        template <typename TagList>
        void add() {
        }
        
        template <typename TagList, typename First, typename... Rest>
        void add(First const& first, Rest const&... rest) {
            boost::fusion::at_key<typename boost::mpl::front<TagList>::type>(args_) = first;
            add<typename boost::mpl::pop_front<TagList>::type>(rest...);
        }
    };
    
    template <typename Tag>
    struct tag_type
    {
        typedef typename boost::fusion::result_of::value_at_key<ArgumentMap, Tag>::type type;
    };
    
    struct extract_value
    {
        template<typename Sig>
        struct result;
        
        template<typename U>
        struct result<extract_value(U)>
        {
            typedef typename boost::remove_reference<U>::type::second_type const& type;
        };
        
        template <typename U>
        typename extract_value::result<extract_value(U)>::type const&
        operator()(U const& x) const
        {
            return x.second;
        }
    };
    
public:
    named_constructor_list(ArgumentMap const& defaults) : defaults_(defaults) {}
    
    named_constructor_list& operator()(typename tag_type<Tags>::type const&... args) {
        argument_builder builder(defaults_);
        builder.template add<tag_list>(args...);
        this->values_.push_back(
            boost::fusion::invoke(
                boost::value_factory<T>(),
                boost::fusion::transform_view<ArgumentMap, extract_value>(builder.args_, extract_value())));
        return *this;
    }
    
    template<class Container>
    operator Container() const
    {
        return this-> BOOST_NESTED_TEMPLATE convert_to_container<Container>();
    }
};

#endif

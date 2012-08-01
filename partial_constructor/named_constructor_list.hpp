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

template <typename T, typename ArgumentMap>
struct named_constructor_list
{
    template <typename... Tags>
    struct list_of : public boost::assign_detail::converter<
        list_of< Tags... >,
        typename std::deque< typename boost::assign_detail::assign_decay<T>::type>::iterator >
    {
        typedef typename boost::assign_detail::assign_decay<T>::type Ty;
        typedef std::deque<Ty>  impl_type;
        mutable impl_type       values_;
        
    public:
        typedef typename impl_type::iterator         iterator;
        typedef iterator                                           const_iterator;
        typedef typename impl_type::value_type       value_type;
        typedef typename impl_type::size_type        size_type;
        typedef typename impl_type::difference_type  difference_type;
        
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
        
        list_of(typename tag_type<Tags>::type const&... args) {
            (*this)(args...);
        }
    
        list_of& operator()(typename tag_type<Tags>::type const&... args) {
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
            return this->template convert_to_container<Container>();
        }
    };
};

#endif

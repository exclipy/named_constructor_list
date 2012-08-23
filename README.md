Named parameter idiom for repeated construction
===============================================

When writing unit tests for data processing code, we often need to construct a list objects of a heterogeneous type.  Usually, we are interested only in setting a few fields of the class, which may have many arguments in its constructor.  One possibility for relieving the test code from specifying every argument of the constructor is the [named parameter idiom](http://www.parashift.com/c++-faq-lite/named-parameter-idiom.html).  This effectively allows the call site to decide which parameters it wants to specify, and in what order.  For example, suppose there is a class:

    struct Person
    {
        Person(int height, int weight, string const& name, date const& birthday)
        : height_(height), weight_(weight), name_(name), birthday_(birthday)
        { }
        int height_;
        int weight_;
        string name_;
        date birthday_;
    };

The named parameter idiom would let you construct one with:

    Person p = PersonBuilder().set_name("Ollie").set_height(131);

This is all well and good, but can get verbose if you need to construct tens of these objects for a test.  If we need to construct tables of test data with a subset of the fields of Person, how about syntax like this?:

    std::vector<Person> f = Person::list_of<Name, Weight, Birthday>
                                          ("Millie", 73, date(1969, 7, 16))
                                          ("Sid",    23, date(2002, 5, 21));

This has the benefits of the named parameter idiom - the caller specifies which parameters it wants to set, and the parameters are named in the code close to where they are specified so we don't need to go hunting for the definition of Person to work out what the values mean.  But it achieves this whilst being much more terse.

To enable this, simply include:

    #include "partial_constructor/named_constructor_list.hpp"

and for each class you want to enable, derive from `named_constructor_list`:

    struct Person : named_constructor_list<Person, PersonArguments>
    {
        ...
    };

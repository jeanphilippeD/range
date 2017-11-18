// Boost.Range library
//
//  Copyright Jean-Philippe Dufraigne 2017.
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_JOINED_HPP
#define BOOST_RANGE_ADAPTOR_JOINED_HPP

#include <boost/range/adaptor/argument_fwd.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>
#include <iterator>

namespace boost
{
    namespace range_detail
    {
        template<class Iter>
        class join_iterator;

        template<class Iter>
        struct join_iterator_base_gen
        {
            typedef typename std::iterator_traits<Iter>::value_type sub_range_t;
            typedef typename range_iterator<sub_range_t>::type sub_iter_t;

            typedef typename std::iterator_traits<sub_iter_t>::value_type
                value_type;

            // bi-directional would require keeping
            // begin, end and current iterator
            typedef iterator_facade<join_iterator<Iter>,
                                        value_type,
                                        forward_traversal_tag,
                                        value_type> base_t;
        };

        // join_iterator for wrapping a forward traversal iterator
        template<class Iter>
        class join_iterator : public join_iterator_base_gen<Iter>::base_t
        {
            typedef typename join_iterator_base_gen<Iter>::base_t super_t;
            typedef typename join_iterator_base_gen<Iter>::sub_iter_t sub_iter_t;
            typedef boost::optional<sub_iter_t> optional_sub_iter_t;

            friend class ::boost::iterator_core_access;

        public:
            typedef Iter iter_t;

            join_iterator()
                : m_it()
                , m_last()
                , m_sub_it()
                , m_sub_last()
            {
            }

            join_iterator(iter_t it, iter_t last)
                : m_it(it)
                , m_last(last)
                , m_sub_it()
                , m_sub_last()
            {
                if (emplace_valid_sub_it())
                {
                    ensure_valid_sub_it_or_last();
                }
            }

        private:
            void increment()
            {
                BOOST_ASSERT(m_sub_it && m_sub_last);

                ++(*m_sub_it);
                ensure_valid_sub_it_or_last();
            }

            typename super_t::reference dereference() const
            {
                BOOST_ASSERT(m_sub_it && m_sub_last);

                return **m_sub_it;
            }

            bool equal(const join_iterator& other) const
            {
                return m_it == other.m_it &&
                       m_sub_it == other.m_sub_it;
            }

            bool emplace_valid_sub_it()
            {
                BOOST_ASSERT(!m_sub_it && !m_sub_last);

                if (m_it != m_last)
                {
                    // Allow non assignable sub iterators
                    // Only two iterator copy per sub-range
                    m_sub_it.emplace(boost::begin(*m_it));
                    m_sub_last.emplace(boost::end(*m_it));
                    return true;
                }
                return false;
            }

            void ensure_valid_sub_it_or_last()
            {
                BOOST_ASSERT(m_sub_it && m_sub_last);

                while (*m_sub_it == *m_sub_last)
                {
                    ++m_it;
                    m_sub_it.reset();
                    m_sub_last.reset();

                    if (!emplace_valid_sub_it())
                    {
                        return;
                    }
                }
            }

            iter_t m_it;
            iter_t m_last;
            optional_sub_iter_t m_sub_it;
            optional_sub_iter_t m_sub_last;
        };

        template<class Range>
        struct join_range
            : iterator_range<
                    join_iterator<typename range_iterator<Range>::type> >
        {
        private:
            typedef join_iterator<
                typename range_iterator<Range>::type> join_iter_t;
            typedef boost::iterator_range<join_iter_t> base;

        public:
            join_range(Range& r)
                : base(join_iter_t(boost::begin(r), boost::end(r)),
                       join_iter_t(boost::end(r), boost::end(r)))
            {
            }
        };

        class join_forwarder
        {
        };

        template<class ForwardRng>
        inline join_range<ForwardRng> operator|(
            ForwardRng& rng,
            const join_forwarder& /*tag*/)
        {
            BOOST_RANGE_CONCEPT_ASSERT(
                (ForwardRangeConcept<ForwardRng>));

            return join_range<ForwardRng>(rng);
        }

        template<class ForwardRng>
        inline join_range<const ForwardRng> operator|(
            const ForwardRng& rng,
            const join_forwarder& /*tag*/)
        {
            BOOST_RANGE_CONCEPT_ASSERT(
                (ForwardRangeConcept<const ForwardRng>));

            return join_range<const ForwardRng>(rng);
        }
    } // namespace range_detail

    using range_detail::join_range;

    namespace adaptors
    {
        namespace
        {
            const range_detail::join_forwarder joined =
                range_detail::join_forwarder();
        }

        template<class ForwardRng>
        inline join_range<ForwardRng> join(ForwardRng& rng)
        {
            BOOST_RANGE_CONCEPT_ASSERT(
                (ForwardRangeConcept<ForwardRng>));

            return join_range<ForwardRng>(rng);
        }

        template<class ForwardRng>
        inline join_range<const ForwardRng> join(const ForwardRng& rng)
        {
            BOOST_RANGE_CONCEPT_ASSERT(
                (ForwardRangeConcept<const ForwardRng>));

            return join_range<const ForwardRng>(rng);
        }
    } // namespace 'adaptors'
} // namespace 'boost'

#endif

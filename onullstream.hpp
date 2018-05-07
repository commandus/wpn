/**
 * https://asmodehn.wordpress.com/2010/06/20/busy-c-coding-and-testing/
 */
#include <ostream>

template <class cT, class traits = std::char_traits<cT> >
class basic_nullstreambuf: public std::basic_streambuf<cT, traits>
{
public:
	basic_nullstreambuf();
	~basic_nullstreambuf();
	typename traits::int_type overflow(typename traits::int_type c)
	{
		return traits::not_eof(c); // indicate success
	}
};

template <class _CharT, class _Traits>
basic_nullstreambuf<_CharT, _Traits>::basic_nullstreambuf()
	: std::basic_streambuf<_CharT, _Traits>()
{

}

template <class _CharT, class _Traits>
basic_nullstreambuf<_CharT, _Traits>::~basic_nullstreambuf()
{

}

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits> 
{
public:
	basic_onullstream():
	std::basic_ios<cT, traits>(),
	std::basic_ostream<cT, traits>(0),
	m_sbuf()
	{
		// init(&m_sbuf);
	}
private:
	basic_nullstreambuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> onullstream;

typedef basic_onullstream<wchar_t> wonullstream;

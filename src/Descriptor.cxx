#include "rpg2k/Debug.hxx"
#include "rpg2k/Descriptor.hxx"
#include "rpg2k/Stream.hxx"
#include "rpg2k/Structure.hxx"

#include <boost/preprocessor/stringize.hpp>


namespace rpg2k
{
	namespace structure
	{
		ElementType::ElementType()
		{
			#define PP_insert(r, data, elem) \
				table_.insert(Table::value_type(BOOST_PP_CAT(elem, data), BOOST_PP_STRINGIZE(elem)));
			BOOST_PP_SEQ_FOR_EACH(PP_insert, _, PP_allTypes)
			#undef PP_insert
		}
		ElementType::Enum ElementType::toEnum(String const& name) const
		{
			Table::right_map::const_iterator it = table_.right.find(name);
			rpg2k_assert(it != table_.right.end());
			return it->second;
		}
		String const& ElementType::toString(ElementType::Enum const e) const
		{
			Table::left_map::const_iterator it = table_.left.find(e);
			rpg2k_assert(it != table_.left.end());
			return it->second;
		}

		Descriptor::Descriptor(Descriptor const& src)
		: type_(src.type_), hasDefault_(src.hasDefault_)
		, arrayTable_(src.arrayTable_.get() ? new ArrayTable(*src.arrayTable_) : NULL)
		{
			if(hasDefault_) switch(type_) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = src.impl_.BOOST_PP_CAT(elem, data); \
						return;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum
				case ElementType::Array1D_:
				case ElementType::Array2D_:
					impl_.arrayDefine
					= new boost::ptr_unordered_map<unsigned, Descriptor>(*src.impl_.arrayDefine);
					break;
				default: rpg2k_assert(false); break;
			}
		}
		Descriptor::Descriptor(String const& type)
		: type_(ElementType::instance().toEnum(type)), hasDefault_(false)
		{
		}
		Descriptor::Descriptor(String const& type, String const& val)
		: type_(ElementType::instance().toEnum(type)), hasDefault_(true)
		{
			switch(this->type_) {
				case ElementType::String_:
					if(
						(val.size() > 2) &&
						(*val.begin() == '\"') && (*val.rbegin() == '\"')
					) impl_.String_ = new String(val.data() + 1, val.size() - 2);
					else impl_.String_ = new String(val);
					break;
				#define PP_enum(r, d, elem) \
					case ElementType::BOOST_PP_CAT(elem, d): { \
						io::stream<io::array_source> s(io::array_source(val.data(), val.size())); \
						s >> std::boolalpha >> (impl_.BOOST_PP_CAT(elem, d)); \
					} break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum
			default: rpg2k_assert(false); break;
			}
		}
		Descriptor::Descriptor(String const& type
		, ArrayDefinePointer def, unique_ptr<ArrayTable>::type table)
		: type_(ElementType::instance().toEnum(type)), hasDefault_(true)
		, arrayTable_(table.release())
		{
			rpg2k_assert((type_ == ElementType::Array1D_) || (type_ == ElementType::Array2D_));
			impl_.arrayDefine = def.release();
		}

		Descriptor::~Descriptor()
		{
			if(hasDefault_) switch(type_) {
				case ElementType::String_:
					delete impl_.String_;
					break;
				case ElementType::Array1D_:
				case ElementType::Array2D_:
					delete impl_.arrayDefine;
					break;
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data):
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum
					break;
				default: rpg2k_assert(false); break;
			}
		}

		#define PP_castOperator(r, data, elem) \
			Descriptor::operator elem const&() const \
			{ \
				rpg2k_assert(this->type_ == ElementType::BOOST_PP_CAT(elem, data)); \
				return impl_.BOOST_PP_CAT(elem, data); \
			}
		BOOST_PP_SEQ_FOR_EACH(PP_castOperator, _, PP_basicTypes)
		#undef PP_castOperator

		Descriptor::operator String const&() const
		{
			rpg2k_assert(this->type_ == ElementType::String_);
			return *impl_.String_;
		}
		ArrayDefine Descriptor::arrayDefine() const
		{
			rpg2k_assert((this->type_ == ElementType::Array1D_)
			|| (this->type_ == ElementType::Array2D_));
			return *impl_.arrayDefine;
		}

		String const& Descriptor::typeName() const
		{
			return ElementType::instance().toString(type_);
		}
	} // namespace structure
} // namespace rpg2k

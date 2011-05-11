#include "ntfmt_string.hpp"

#include "rpg2k/Debug.hxx"
#include "rpg2k/SaveData.hxx"
#include "rpg2k/Structure.hxx"

#include <boost/range/counting_range.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/find.hpp>


namespace rpg2k
{
	namespace model
	{
		SaveData::SaveData()
		: Base(SystemString(), SystemString()), id_(-1)
		{
			Base::reset();

		// reset map chip info
			resetReplace();
		}
		SaveData::SaveData(SystemString const& dir, SystemString const& name)
		: Base(dir, name), id_(0)
		{
			load();
		}
		SaveData::SaveData(SystemString const& dir, unsigned const id)
		: Base(dir, ""), id_(id)
		{
			std::string fileName;
			ntfmt::sink_string(fileName)
				<< "Save" << ntfmt::fmt(id, "%02d") << ".lsd";
			setFileName(fileName);

			checkExists();

			if(!exists()) return;

			load();
		}
		SaveData::~SaveData()
		{
		#if RPG2K_DEBUG
			debug::ANALYZE_RESULT << header() << ": " << int(id_) << endl;
		#endif
		}

		SaveData const& SaveData::operator =(SaveData const& src)
		{
			this->data() = src.data();

			this->item_ = src.item_;

			this->variable_ = src.variable_;
			this->switch_   = src.switch_  ;

			this->member_ = src.member_;

			this->chipReplace_ = src.chipReplace_;

			return *this;
		}

		void SaveData::loadImpl()
		{
			structure::Array1D& sys    = (*this)[101];
			structure::Array1D& status = (*this)[109];
			structure::Array1D& event  = (*this)[111];

		// item
			{
				int itemTypeNum = status[11];
				std::vector<uint16_t> id  = status[12].toBinary().toVector<uint16_t>();
				std::vector<uint8_t > num = status[13].toBinary().toVector<uint8_t>();
				std::vector<uint8_t > use = status[14].toBinary().toVector<uint8_t>();

				for(int i = 0; i < itemTypeNum; i++) {
					if(!num[i]) continue;

					Item info = { num[i], use[i] };
					item_.insert(std::make_pair(id[i], info));
				}
			}
		// switch and variable
			// switch_.resize(sys[31].to_int());
			switch_ = sys[32].toBinary().toVector<uint8_t>();
			// variable_.resize(sys[33].to_int());
			variable_ = sys[34].toBinary().toVector<int32_t>();
		// member
			member_.resize(status[1].to_int());
			member_ = status[2].toBinary().toVector<uint16_t>();
		// chip replace
			chipReplace_.resize(int(ChipSet::END));
			for(auto i : boost::counting_range(0, int(ChipSet::END)))
			{ chipReplace_[i] = event[21 + i].toBinary(); }
		}

		void SaveData::saveImpl()
		{
			structure::Array1D& status = (*this)[109];
			structure::Array1D& sys = (*this)[101];

		// item
			{
				int itemNum = item_.size();
				status[11] = itemNum;

				std::vector<uint16_t> id (itemNum);
				std::vector<uint8_t > num(itemNum);
				std::vector<uint8_t > use(itemNum);

				int i = 0;
				for(ItemTable::const_iterator it = item_.begin(); it != item_.end(); ++it) {
					id [i] = it->first;
					num[i] = it->second.num;
					use[i] = it->second.use;

					i++;
				}
				status[12].toBinary().assign(id);
				status[13].toBinary().assign(num);
				status[14].toBinary().assign(use);
			}
		// switch and variable
			sys[31] = int(switch_.size());
			sys[32].toBinary().assign(switch_);
			sys[33] = int(variable_.size());
			sys[34].toBinary().assign(variable_);
		// member
			(*this)[109].toArray1D()[1] = int(member_.size());
			(*this)[109].toArray1D()[2].toBinary().assign(member_);
		// chip replace
			for(auto i : boost::counting_range(int(ChipSet::BEGIN), int(ChipSet::END)))
			{ (*this)[111].toArray1D()[21 + i].toBinary().assign(chipReplace_[i]); }
		}

		bool SaveData::addMember(unsigned const charID)
		{
			if((member_.size() > rpg2k::MEMBER_MAX)
			|| boost::find(member_, charID) == member_.end()) return false;
			else {
				member_.push_back(charID);
				return true;
			}
		}
		bool SaveData::removeMember(unsigned const charID)
		{
			std::vector<uint16_t>::iterator it = std::find(member_.begin(), member_.end(), charID);
			if(it != member_.end()) {
				member_.erase(it);
				return true;
			} else return false;
		}

		bool SaveData::flag(unsigned const id) const
		{
			return (id < switch_.size()) ? switch_[id - ID_MIN] : bool(SWITCH_DEF_VAL);
		}
		void SaveData::setFlag(unsigned id, bool data)
		{
			if(id >= switch_.size()) switch_.resize(id, SWITCH_DEF_VAL);
			switch_[id - ID_MIN] = data;
		}

		int32_t SaveData::var(unsigned const id) const
		{
			return (id < variable_.size()) ? variable_[id - ID_MIN] : VAR_DEF_VAL;
		}
		void SaveData::setVar(unsigned const id, int32_t const data)
		{
			if(id >= variable_.size()) variable_.resize(id, int32_t(VAR_DEF_VAL));
			variable_[id - ID_MIN] = data;
		}

		int SaveData::money() const
		{
			return (*this)[109].toArray1D()[21];
		}
		void SaveData::setMoney(int const data)
		{
			if(data < MONEY_MIN) (*this)[109].toArray1D()[21] = int(MONEY_MIN);
			else if(MONEY_MAX < data) (*this)[109].toArray1D()[21] = int(MONEY_MAX);
			else (*this)[109].toArray1D()[21] = data;
		}

		unsigned SaveData::itemNum(unsigned const id) const
		{
			ItemTable::const_iterator it = item_.find(id);
			return (it == item_.end())? unsigned(ITEM_MIN) : it->second.num;
		}
		void SaveData::setItemNum(unsigned const id, unsigned const val)
		{
			unsigned const validVal = std::min(unsigned(ITEM_MAX), val);

			if(item_.find(id) == item_.end()) {
				Item const i = { uint8_t(validVal), 0 };
				item_.insert(std::make_pair(id, i));
			} else item_[id].num = validVal;

			if(validVal == ITEM_MIN) item_.erase(id);
		}

		unsigned SaveData::itemUse(unsigned const id) const
		{
			ItemTable::const_iterator it = item_.find(id);
			return (it == item_.end()) ? unsigned(ITEM_MIN) : it->second.use;
		}
		void SaveData::setItemUse(unsigned const id, unsigned const val)
		{
			ItemTable::iterator it = item_.find(id);
			if(it != item_.end()) it->second.use = val;
		}

		unsigned SaveData::member(unsigned const index) const
		{
			rpg2k_assert(rpg2k::within<unsigned>(index, member_.size()));
			return member_[index];
		}

		structure::EventState& SaveData::eventState(unsigned id)
		{
			switch(id) {
				case ID_PARTY: case ID_BOAT: case ID_SHIP: case ID_AIRSHIP:
					 return (*this)[ 104 + (id-ID_PARTY) ];
				case ID_THIS: id = currentEventID(); // TODO
				default:
					return reinterpret_cast< structure::EventState& >(
						(*this)[111].toArray1D()[11].toArray2D()[id]);
			}
		}

		void SaveData::replace(ChipSet const type, unsigned const dstNo, unsigned const srcNo)
		{
			rpg2k_assert(rpg2k::within<unsigned>(dstNo, CHIP_REPLACE_MAX));
			rpg2k_assert(rpg2k::within<unsigned>(srcNo, CHIP_REPLACE_MAX));

			std::swap(chipReplace_[int(type)][srcNo], chipReplace_[int(type)][dstNo]);
		}
		void SaveData::resetReplace()
		{
			chipReplace_.clear();
			chipReplace_.resize(int(ChipSet::END));
			for(int i = int(ChipSet::BEGIN); i < int(ChipSet::END); i++) {
				chipReplace_[i].resize(CHIP_REPLACE_MAX);
				boost::copy(boost::counting_range(0, int(CHIP_REPLACE_MAX)), back_inserter(chipReplace_[i]));
			}
		}
	} // namespace model
} // namespace rpg2k

















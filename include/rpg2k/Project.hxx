#ifndef _INC__RPG2K__MODEL__PROJECT__HPP
#define _INC__RPG2K__MODEL__PROJECT__HPP

#include "Array1DWrapper.hxx"
#include "DataBase.hxx"
#include "MapTree.hxx"
#include "MapUnit.hxx"
#include "SaveData.hxx"

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>

#include <set>


namespace rpg2k
{
	enum { INVALID_PAGE_ID = -1, };

	namespace model
	{
		class Project : boost::noncopyable
		{
		public:
			class Character
			{
			public:
				typedef boost::array<uint16_t, int(rpg2k::Equip::END)> Equip;
			private:
				unsigned const charID_;

				structure::Array1D const& ldb_;
				structure::Array1D& lsd_;

				std::vector<uint16_t> const basicParam_;
				std::set<uint16_t> skill_;
				std::vector<uint8_t> condition_; // std::vector<bool>
				std::vector<uint16_t> conditionStep_;
				Equip equip_;

				template<typename T, unsigned LsdID, unsigned LdbID>
				T const& get() const { return lsd_.exists(LsdID)? lsd_[LsdID] : ldb_[LdbID]; }
			public:
				Character(unsigned charID, structure::Array1D const& ldb, structure::Array1D& lsd);

				String const& charSet() const { return get<String, 11, 3>(); }
				String const& faceSet() const { return get<String, 21, 15>(); }
				int charSetPos() const { return get<int, 12, 4>(); }
				int faceSetPos() const { return get<int, 22, 16>(); }

				String const& name() const { return get<String, 1, 1>(); }
				String const& title() const { return get<String, 2, 2>(); }
				void setName(String const& val) { lsd_[1] = val; }
				void setTitle(String const& val) { lsd_[2] = val; }

				std::vector<uint8_t>& condition() { return condition_; }
				std::vector<uint8_t> const& condition() const { return condition_; }

				int hp() const { return lsd_[71]; }
				int mp() const { return lsd_[72]; }
				void setHP(unsigned const val) const { lsd_[71] = val; }
				void setMP(unsigned const val) const { lsd_[72] = val; }

				int basicParam(int level, Param t) const;
				int param(Param t) const;

				int level() const { return get<int, 31, 7>(); }
				void setLevel(unsigned val);
				void addLevel(int const val = 1) { setLevel(level() + val); }
				int exp(unsigned level) const;
				int exp() const { return lsd_[32]; }
				bool setExp(unsigned val);
				bool addExp(int const val) { return setExp(exp() + val); }
				int nextLevelExp() const { return exp(this->level() + 1); }
				int levelExp() const { return exp(this->level()); }

				void cure();

				Equip& equip() { return equip_; }
				Equip const& equip() const { return equip_; }

				std::set<uint16_t>& skill() { return skill_; }
				std::set<uint16_t> const& skill() const { return skill_; }

				void sync();
			private:
				bool canLevelUp() const { return(exp() > nextLevelExp()); }
			}; // class Character
		private:
			SystemString baseDir_, rtpDir_;

			DataBase ldb_;
			MapTree  lmt_;
			typedef boost::ptr_unordered_map<unsigned, MapUnit> MapUnitTable;
			MapUnitTable lmu_;
			boost::ptr_vector<SaveData> lsd_;

			typedef boost::ptr_unordered_map<unsigned, Character> CharacterTable;
			CharacterTable charTable_;
			unsigned lastSaveDataID_;
			double lastSaveDataStamp_;
		protected:
			void init();
		public:
			Project(SystemString baseDir=".");

			String const& gameTitle() const { return lmt_[0][1]; }

			SystemString const& gameDir() const { return baseDir_; }

			unsigned currentMapID();

			DataBase const& getLDB() const { return ldb_; }
			 MapTree const& getLMT() const { return lmt_; }
			SaveData const& getLSD() const { return lsd_.front(); }

			structure::Array1D const& getLMT(unsigned const id) const { return lmt_[id]; }
			structure::Array1D& getLMT(unsigned const id) { return lmt_[id]; }
			 MapUnit& getLMU(unsigned id);
			SaveData& getLSD(unsigned id);

			DataBase& getLDB() { return ldb_; }
			 MapTree& getLMT() { return lmt_; }
			 MapUnit& getLMU() { return getLMU(currentMapID()); }
			SaveData& getLSD() { return lsd_.front(); }

			int lastLSD() const { return lastSaveDataID_; }
			double lastLSDStamp() const { return lastSaveDataStamp_; }

			int chipSetID();
			structure::Array1D& chipSet() { return getLDB().chipSet()[chipSetID()]; }
			String panorama();

			void newGame();

			void loadLSD(unsigned id);
			void saveLSD(unsigned id);

			int paramWithEquip(unsigned charID, Param t) const;
			bool   equip(unsigned charID, unsigned itemID);
			void unequip(unsigned charID, Equip type);
			unsigned equipNum(unsigned itemID) const;

			bool validPageMap   (structure::Array1D const& term) const;
			bool validPageBattle(structure::Array1D const& term) const;

			bool canTeleport();
			bool canEscape  ();
			bool canSave    ();
			bool canOpenMenu() const;

			bool isAbove(int chipID);
			bool isBelow(int chipID);
			bool isCounter(int chipID);
			uint8_t pass(int chipID);
			int terrainID(int chipID);

			bool processAction(unsigned eventID, Action::Type act, std::istream& is);

			int currentPageID(unsigned eventID);
			structure::Array1D const* currentPage(structure::Array2D const& pages) const;

			String const& systemGraphic() const;
			Wallpaper wallpaperType() const;
			Face fontType() const;

			Character const& character(unsigned const id) const;
			Character& character(unsigned const id);

			void move(unsigned mapID, int x, int y);

			std::vector<unsigned> sortLSD() const;

			bool hasItem(unsigned id) const;
		}; // class Project
	} // namespace model
} // namespace rpg2k

#endif
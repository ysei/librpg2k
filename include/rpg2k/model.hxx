#ifndef _INC__RPG2K__MODEL_HPP
#define _INC__RPG2K__MODEL_HPP

#include <deque>

#include <algorithm>
#include <map>
#include <set>

#include "array1d.hxx"
#include "array2d.hxx"
#include "element.hxx"
#include "singleton.hxx"

#include <boost/ptr_container/ptr_vector.hpp>


namespace rpg2k
{

	namespace model
	{
		bool fileExists(SystemString const& fileName);

		class Base
		{
		private:
			bool exists_;

			SystemString fileDir_, fileName_;
			boost::ptr_vector<structure::Element> data_;

			virtual void loadImpl() = 0;
			virtual void saveImpl() = 0;

			virtual char const* header() const = 0;
			virtual char const* defaultName() const = 0;
		protected:
			void setFileName(SystemString const& name) { fileName_ = name; }
			boost::ptr_vector<structure::Element>& data() { return data_; }
			boost::ptr_vector<structure::Element> const& data() const { return data_; }

			void checkExists();

			boost::ptr_vector<structure::Descriptor> const& descriptor() const;

			Base(SystemString const& dir);
			Base(SystemString const& dir, SystemString const& name);

			Base(Base const& src);
			Base const& operator =(Base const& src);

			void load();
		public:
			virtual ~Base() {}

			bool exists() const { return exists_; }

			void reset();

			structure::Element& operator [](unsigned index);
			structure::Element const& operator [](unsigned index) const;

			SystemString const& fileName() const { return fileName_; }
			SystemString const& directory() const { return fileDir_; }
			SystemString fullPath() const
			{
				return SystemString(fileDir_).append(PATH_SEPR).append(fileName_);
			} // not absolute

			void saveAs(SystemString const& filename);
			void save() { saveAs(fullPath()); }

			void serialize(std::ostream& s);
		}; // class Base

		class DefineLoader : public Singleton<DefineLoader>
		{
			friend class Singleton<DefineLoader>;
		private:
			typedef std::map< String, boost::ptr_vector<structure::Descriptor> > DefineBuffer;
			DefineBuffer defineBuff_;
			typedef std::map<String, const char*> DefineText;
			DefineText defineText_;
			std::set<String> isArray_;
		protected:
			void parse(boost::ptr_vector<structure::Descriptor>& dst, std::deque<String> const& token);
			void load(boost::ptr_vector<structure::Descriptor>& dst, String const& name);

			DefineLoader();
			DefineLoader(DefineLoader const& dl);
		public:
			boost::ptr_vector<structure::Descriptor> const& get(String const& name);
			structure::ArrayDefine arrayDefine(String const& name);

			bool isArray(String const& typeName) const
			{
				return isArray_.find(typeName) != isArray_.end();
			}

			static void toToken(std::deque<String>& token, std::istream& stream);
		}; // class DefineLoader
	} // namespace model
} // namespace rpg2k

#endif
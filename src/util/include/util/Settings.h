#pragma once
template <typename T>
using ValueTypeMap = std::unordered_map<std::string, T>;

namespace util
{
	class Settings
	{
	private:
		enum class Index : size_t
		{
			INT,
			FLOAT,
			UINT,
			BOOL,
			STR,
		};

	private:
		template <typename T>
		struct TypeToIndex;

		template <>
		struct TypeToIndex<int>
		{
			static constexpr Index value = Index::INT;
		};
		template <>
		struct TypeToIndex<float>
		{
			static constexpr Index value = Index::FLOAT;
		};
		template <>
		struct TypeToIndex<unsigned int>
		{
			static constexpr Index value = Index::UINT;
		};
		template <>
		struct TypeToIndex<bool>
		{
			static constexpr Index value = Index::BOOL;
		};
		template <>
		struct TypeToIndex<std::string>
		{
			static constexpr Index value = Index::STR;
		};

	public:
		static Settings*
		GetSingleton();

		class ConfigModule
		{
		public:
			ConfigModule(std::string_view section) : sectionName(section) {}

			template <typename T>
			T
			Get(std::string_view key, T defaultVal) const
			{
				return Settings::GetOrDefault<T>(sectionName, key, defaultVal);
			}

		private:
			std::string sectionName;
		};

		static ConfigModule
		Module(std::string_view section)
		{
			return ConfigModule(section);
		}

		template <typename T>
		static T
		GetOrDefault(std::string_view mod, std::string_view key, T defaultVal)
		{
			std::string fullKey;
			fullKey.reserve(mod.size() + 1 + key.size());
			fullKey.append(mod).push_back('#');
			fullKey.append(key);

			auto& map = GetSingleton()->GetMap<T>();

			if (auto it = map.find(fullKey); it != map.end())
			{
				return it->second;
			}

			return defaultVal;
		}

		template <typename T>
		static bool
		Get(std::string_view a_section, std::string_view a_key, T& a_buf)
		{
			std::string fullKey = std::string(a_section) + "#" + std::string(a_key);
			auto        map     = GetSingleton()->GetMap<T>(*std::atomic_load(&data));

			if (auto it = map.find(fullKey); it != map.end())
			{
				a_buf = it->second;
				return true;
			}
			return false;
		}

		void
		ReadAllSettings();

	private:
		using Data = std::tuple<
			ValueTypeMap<int>,
			ValueTypeMap<float>,
			ValueTypeMap<unsigned int>,
			ValueTypeMap<bool>,
			ValueTypeMap<std::string>>;

		Settings() = default;

		template <typename T>
		static void
		Set(Data& data, std::string_view a_section, std::string_view a_key, const T& a_value)
		{
			std::string fullKey = std::string(a_section) + "#" + std::string(a_key);

			std::get<static_cast<size_t>(TypeToIndex<T>::value)>(data)[fullKey] = a_value;
		}

		template <typename T>
		const ValueTypeMap<T>&
		GetMap() const
		{
			return std::get<static_cast<size_t>(TypeToIndex<T>::value)>(
				*std::atomic_load(&GetSingleton()->data));
		}

		template <typename T>
		ValueTypeMap<T>&
		GetMap()
		{
			return std::get<static_cast<size_t>(TypeToIndex<T>::value)>(
				*std::atomic_load(&GetSingleton()->data));
		}

		void
		ReadSettings(std::string_view filePath, Data& newSectionMap);

		std::atomic<std::shared_ptr<Data>> data;
	};

}
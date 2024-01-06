#pragma once
#include <vector>
#include <string>
#include <format>
#include <map>

using StringArray = std::vector<std::string>;

struct PortraitTexture
{
	virtual StringArray to_strings() const = 0;
	virtual ~PortraitTexture() = default;

	using shared_ptr = std::shared_ptr<PortraitTexture>;
};

struct PortraitTextureStatic : public PortraitTexture
{
	std::string textureFile;				// filepath

	virtual StringArray to_strings() const override
	{
		return StringArray(1, std::format("textureFile = {}", textureFile));
	}

	PortraitTextureStatic(const std::string& textureFile)
		: textureFile(textureFile) {}
	using shared_ptr = std::shared_ptr<PortraitTextureStatic>;
};

struct PortraitTextureNonStatic : public PortraitTexture
{
	std::string entity;						// <model_entity>
	std::string clothes_selector;			// no_texture | <asset_selector.asset>
	std::string attachment_selector;		// no_texture | <asset_selector.asset>
	std::string custom_attachment_label;	// localisation							0..1
	std::string greeting_sound;				// no_sound   | <sound_effect>			0..1

	StringArray character_textures;			// filepath

	using shared_ptr = std::shared_ptr<PortraitTextureNonStatic>;

	std::map<std::string, std::string> addition;
	// has the following:
	// Offset custom_close_up_position_offset;
	// float  custom_close_up_scale;
	// Offset custom_mid_close_up_position_offset;
	// float  custom_mid_close_up_scale;

	PortraitTextureNonStatic(const std::string& entity
		, const std::string& clothes_selector = std::string("no_texture")
		, const std::string& attachment_selector = std::string("no_texture")
		, const std::string& custom_attachment_label = std::string()
		, const std::string& greeting_sound = std::string()
	) : entity(entity), clothes_selector(clothes_selector), attachment_selector(attachment_selector)
		, custom_attachment_label(custom_attachment_label), greeting_sound(greeting_sound)
	{}

	void append_character_texture(const std::string& texturepath)
	{
		character_textures.push_back(texturepath);
	}

	void append_character_texture(const StringArray& l)
	{
		character_textures.insert(character_textures.end(), l.begin(), l.end());
	}

	bool insert_addition(const std::string& key, const std::string& value)
	{
		auto [it, success] = addition.insert_or_assign(key, value);
		return success;
	}

	virtual StringArray to_strings() const override
	{
#define append_single_property(prop) \
		if (!prop.empty()) \
			ret.push_back(#prop" = " + prop)

		StringArray ret;
		ret.push_back("entity = " + entity);
		ret.push_back("clothes_selector = " + clothes_selector);
		ret.push_back("attachment_selector = " + attachment_selector);
		append_single_property(custom_attachment_label);
		append_single_property(greeting_sound);

		ret.push_back("character_textures = {");
		for (auto& i : character_textures)
			ret.push_back("\t" + i);
		ret.push_back("}");

		for (const auto& [key, value] : addition)
			ret.push_back(std::format("{} = {}", key, value));

#undef append_single_property
		return ret;
	}
};

struct PortraitGroupPropertyClause
{
	std::string trigger;
	StringArray portraits;

	template<typename InputIt>
	PortraitGroupPropertyClause(InputIt begin, InputIt end, const std::string& trigger)
		: portraits(begin, end), trigger(trigger) {}

	StringArray to_strings() const
	{
		StringArray ret;
		if (!trigger.empty())
			ret.push_back(std::format("trigger = {{{}}}", trigger));

		ret.push_back("portraits = {");
		for (auto& i : portraits)
			ret.push_back("\t" + i);
		ret.push_back("}");
		return ret;
	}
};

struct PortraitGroupProperty
{
	std::vector<PortraitGroupPropertyClause> add_;
	std::vector<PortraitGroupPropertyClause> set_;

	using shared_ptr = std::shared_ptr<PortraitGroupProperty>;

#pragma region PortraitGroupPropertyREG
#define declare_method(name)\
	void name(const StringArray& arr, const std::string& trigger = std::string())\
	{ name##_.push_back(PortraitGroupPropertyClause{ arr.begin(), arr.end(), trigger }); }
	/*template<typename InputIt>\
	void name(InputIt begin, InputIt end, const std::string& trigger = std::string())\
	{ name##_.push_back(PortraitGroupPropertyClause{ begin, end, trigger }); }*/

	declare_method(add)
	declare_method(set)
#undef declare_method
#pragma endregion

	bool empty() const
	{
		return add_.empty() && set_.empty();
	}

	StringArray to_strings() const
	{
		StringArray ret;

		for (auto& a : add_)
		{
			ret.push_back("add = {");
			for (const auto& str : a.to_strings())
				ret.push_back("\t" + str);
			ret.push_back("}");
		}
		for (auto& s : set_)
		{
			ret.push_back("set = {");
			for (const auto& str : s.to_strings())
				ret.push_back("\t" + str);
			ret.push_back("}");
		}
		return ret;
	}
};

struct PortraitGroup
{
	std::string default_portrait;
	PortraitGroupProperty game_setup;
	PortraitGroupProperty species;
	PortraitGroupProperty pop;
	PortraitGroupProperty leader;
	PortraitGroupProperty ruler;

	using shared_ptr = std::shared_ptr<PortraitGroup>;

	PortraitGroup(const std::string& default_)
		: default_portrait(default_) {}

	StringArray to_strings() const
	{
		StringArray ret;
		ret.push_back("default = " + default_portrait);
#define append_group_property(prop) \
		if (!prop.empty())	\
		{\
			ret.push_back(#prop" = {");	\
			for (auto& i : prop.to_strings())\
				ret.push_back("\t" + i);\
			ret.push_back("}"); \
		}
		append_group_property(game_setup);
		append_group_property(species);
		append_group_property(pop);
		append_group_property(leader);
		append_group_property(ruler);
#undef append_group_property

		return ret;
	}
};
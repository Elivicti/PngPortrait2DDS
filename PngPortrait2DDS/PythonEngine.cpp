#include "PythonEngine.h"
#include "PdxPortraits.h"
#include "GlobalConfigManager.h"
#include <chrono>

#include <QWidget>
#include <QMenu>
#include <QActionGroup>

#include <QDebug>

namespace py = pybind11;
using PyModule = py::module_;

#pragma region PYTHON_CLASS_DEF

struct PortraitData
{
	std::string file_name;
	bool use_as_species;
	bool use_as_leaders;
	bool use_as_rulers;

	PortraitData(const std::string& name, bool species, bool leaders, bool rulers)
		: file_name(name), use_as_species(species), use_as_leaders(leaders), use_as_rulers(rulers) {}
	
	using shared_ptr = std::shared_ptr<PortraitData>;
};

struct PyPortraitTextureHelper : public PortraitTexture
{
	using PortraitTexture::PortraitTexture;
	virtual StringArray to_strings() const override
	{
		PYBIND11_OVERRIDE_PURE(StringArray, PortraitTexture, to_strings);
	}
};

PythonLogger::PythonLogger(const std::string& logfile)
	: log_stream{ logfile, std::ios::app | std::ios::out }
	, calling_frame{}, script{ "" }
{
}

void PythonLogger::init(const QString& script)
{
	calling_frame = PyModule::import("sys").attr("_getframe");
	this->script = script.toStdString();
}
void PythonLogger::reopen(const std::string& log)
{
	log_stream.sync();
	if (log_stream.is_open())
		log_stream.close();
	log_stream.open(log, std::ios::app | std::ios::out);
}

#pragma region PYTHON_LOGGER_DEF

#define PYTHON_LOGGER(level)																				\
	py::object frame{ calling_frame(0) };																	\
	int lineno{ frame.attr("f_lineno").cast<int>() };														\
	auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());				\
	log_stream << std::format("[{:%OH:%OM:%OS}][" #level "][{}:{}]: \"{}\"\n", now, script, lineno, msg);	\
	log_stream.sync()

void PythonLogger::debug(const std::string& msg) { PYTHON_LOGGER(DEBG); }
void PythonLogger::info(const std::string& msg) { PYTHON_LOGGER(INFO); }
void PythonLogger::warning(const std::string& msg) { PYTHON_LOGGER(WARN); }
void PythonLogger::critical(const std::string& msg) { PYTHON_LOGGER(CRIT); }
void PythonLogger::fatal(const std::string& msg) { PYTHON_LOGGER(FATL); }

#pragma endregion

void PythonLogger::cpp_info_log(const std::string& msg)
{
	auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	log_stream << std::format("[{:%OH:%OM:%OS}][INFO][Script Init]: \"{}\"\n", now, msg);
	log_stream.sync();
}


PYBIND11_EMBEDDED_MODULE(PdxPortraits, m)
{
	py::class_<PortraitData, PortraitData::shared_ptr>(m, "PortraitData")
		.def(py::init<const std::string&, bool, bool, bool>())
		.def_readwrite("file_name", &PortraitData::file_name)
		.def_readwrite("use_as_species", &PortraitData::use_as_species)
		.def_readwrite("use_as_leaders", &PortraitData::use_as_leaders)
		.def_readwrite("use_as_rulers", &PortraitData::use_as_rulers);

	// portraits base class
	py::class_<PortraitTexture, PyPortraitTextureHelper, PortraitTexture::shared_ptr>
		(m, "PortraitTexture")
		.def(py::init<>())
		.def("to_strings", &PortraitTexture::to_strings);

	// portraits
	py::class_<PortraitTextureStatic, PortraitTexture, PortraitTextureStatic::shared_ptr>
		(m, "PortraitTextureStatic")
		.def(py::init<const std::string&>())
		.def("to_strings", &PortraitTextureStatic::to_strings)
		.def_readwrite("textureFile", &PortraitTextureStatic::textureFile);

	py::class_<PortraitTextureNonStatic, PortraitTexture, PortraitTextureNonStatic::shared_ptr>
		(m, "PortraitTextureNonStatic")
		.def(py::init<const std::string&, const std::string&, const std::string&, const std::string&, const std::string&>()
			, py::arg("entity")
			, py::arg("clothes_selector")        = std::string("no_texture")
			, py::arg("attachment_selector")     = std::string("no_texture")
			, py::arg("custom_attachment_label") = std::string("")
			, py::arg("greeting_sound")          = std::string(""))
		.def("append_character_texture", py::overload_cast<const std::string&>(&PortraitTextureNonStatic::append_character_texture))
		.def("append_character_texture", py::overload_cast<const StringArray&>(&PortraitTextureNonStatic::append_character_texture))
		.def("insert_addition", &PortraitTextureNonStatic::insert_addition)
		.def("to_strings",      &PortraitTextureNonStatic::to_strings)
		.def_readwrite("entity",                  &PortraitTextureNonStatic::entity)
		.def_readwrite("clothes_selector",        &PortraitTextureNonStatic::clothes_selector)
		.def_readwrite("attachment_selector",     &PortraitTextureNonStatic::attachment_selector)
		.def_readwrite("custom_attachment_label", &PortraitTextureNonStatic::custom_attachment_label)
		.def_readwrite("greeting_sound",          &PortraitTextureNonStatic::greeting_sound);

	// portrait_groups
	py::class_<PortraitGroupProperty, PortraitGroupProperty::shared_ptr>(m, "PortraitGroupProperty")
		.def(py::init<>())
		.def("empty", &PortraitGroupProperty::empty)
		.def("add", &PortraitGroupProperty::add, py::arg("portraits"), py::arg("trigger") = std::string(""))
		.def("set", &PortraitGroupProperty::set, py::arg("portraits"), py::arg("trigger") = std::string(""))
		.def("to_strings", &PortraitGroupProperty::to_strings);

	py::class_<PortraitGroup, PortraitGroup::shared_ptr>(m, "PortraitGroup")
		.def(py::init<const std::string&>())
		.def_readwrite("default_portrait", &PortraitGroup::default_portrait)
		.def_readwrite("game_setup", &PortraitGroup::game_setup)
		.def_readwrite("species", &PortraitGroup::species)
		.def_readwrite("pop", &PortraitGroup::pop)
		.def_readwrite("leader", &PortraitGroup::leader)
		.def_readwrite("ruler", &PortraitGroup::ruler)
		.def("to_strings", &PortraitGroup::to_strings);

	py::class_<PythonLogger, PythonLogger::shared_ptr>(m, "Logger")
		.def("debug", &PythonLogger::debug)
		.def("info", &PythonLogger::info)
		.def("warning", &PythonLogger::warning)
		.def("critical", &PythonLogger::critical)
		.def("fatal", &PythonLogger::fatal);
}

#pragma endregion

#define DIR_LOG     "log"
#define DIR_SCRIPTS "scripts"
#define PY_PdxPortraits   "PdxPortraits"
#define PY_F_PdxPortraits PY_PdxPortraits".py"
#define PY_F_Default      "default.py"

PythonEngine::PythonEngine(QWidget* parent)
	: QObject(dynamic_cast<QObject*>(parent))
	, logger{ } , error_str{ "" }
	, script_dir{ "./" }
	, mainwindow{ parent }, script_menu{ nullptr }
	, script_group{ new QActionGroup{ parent } }
{
	script_group->setExclusive(true);

	if (!script_dir.exists(DIR_SCRIPTS))
		script_dir.mkdir(DIR_SCRIPTS);
	script_dir.cd(DIR_SCRIPTS);

	if (!script_dir.exists(DIR_LOG))
		script_dir.mkdir(DIR_LOG);

	auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	logger = std::make_shared<PythonLogger>(std::format("./" DIR_SCRIPTS "/" DIR_LOG "/{:%F}.log", now));

	defaultScriptsCheck();
}

void PythonEngine::defaultScriptsCheck()
{
	auto io_flag_rwt{ QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text };
	auto io_flag_r{ QIODevice::ReadOnly | QIODevice::Text };

	QDir dir;

	QFile default_py;
	QFile default_py_rc;

	// reset defualt PortraitData.py
	default_py.setFileName(dir.absoluteFilePath(DIR_SCRIPTS "/" PY_F_PdxPortraits));
	default_py_rc.setFileName(":/scripts/" PY_F_PdxPortraits);
	default_py.open(io_flag_rwt);
	default_py_rc.open(io_flag_r);

	default_py.write(default_py_rc.readAll());

	default_py.close();
	default_py_rc.close();

	// reset defualt default.py
	default_py.setFileName(dir.absoluteFilePath(DIR_SCRIPTS "/" PY_F_Default));
	default_py_rc.setFileName(":/scripts/" PY_F_Default);
	default_py.open(io_flag_rwt);
	default_py_rc.open(io_flag_r);

	default_py.write(default_py_rc.readAll());

	default_py.close();
	default_py_rc.close();

	if (!script_dir.exists(gConfig->script()))
		gConfig->setScript(PY_F_Default);
}

#define METHOD_Preprocess   "Preprocess"	// (std::vector<Portraitdata>, std::string) -> void
#define METHOD_Registration "Registration"	// (void) -> std::vector<std::string>
#define METHOD_ExtraEffect  "ExtraEffect"	// (void) -> void

void PythonEngine::setUpScriptMenu(QMenu* menu)
{
	script_menu = menu;
	QFileInfoList script_list{ script_dir.entryInfoList(QStringList{"*.py"}) };

	for (const auto& script : script_list)
	{
		QString fname{ script.fileName() };
		try
		{
			if (fname == PY_F_PdxPortraits)
				continue;
			std::string base_script_name{ "scripts." + script.baseName().toStdString() };
			auto script_module{ py::module_::import(base_script_name.c_str()) };

			if (py::hasattr(script_module, METHOD_Registration) &&
				py::hasattr(script_module, METHOD_ExtraEffect) &&
				py::hasattr(script_module, METHOD_Preprocess))
			{
				QAction* script_action = new QAction(script.baseName(), script_group);
				script_action->setObjectName(fname);
				script_action->setCheckable(true);

				if (fname == gConfig->script())
					script_action->setChecked(true);

				/*connect(script_action, &QAction::triggered, [this, script_action](bool checked) {
					qDebug() << "Checked:" << script_group->checkedAction()->objectName();
				});*/
			}
			else
			{
				std::string _fname{ fname.toStdString() };
				// Skipping <script>: function METHOD_Preprocess, METHOD_Registration or METHOD_ExtraEffect is not defined.
				logger->cpp_info_log(
					"Skipping " + _fname + ": function " METHOD_Preprocess ", "\
					METHOD_Registration " or " METHOD_ExtraEffect " is not defined."
				);
			}
		}
		catch (const std::exception& e)
		{
			std::string _fname{ fname.toStdString() };
			// Skipping <script>: <exception msg>
			logger->cpp_info_log("Skipping " + _fname + ": " + e.what());
		}
	}
	script_menu->addActions(script_group->actions());
}
bool PythonEngine::runScript(const PresetData& data, const QString& name, bool reg, bool effect,
	std::function<void(const std::vector<std::string>&)> registration_callback,
	const QString& output_path)
{
	std::vector<PortraitData> vec;
	for (const auto& i : *data.portraitData())
	{
		QString name{ i.fileName() };
		auto usage{ data.usage(name) };
		vec.push_back(PortraitData{
			name.replace(".png", ".dds").toStdString(),
			usage.species, usage.leaders, usage.rulers
		});
	}
	
	QString script{ script_group->checkedAction()->objectName() };
	try
	{
		std::string module_name{ "scripts." + script.left(script.indexOf('.')).toStdString() };
		PyModule py_script = PyModule::import(module_name.c_str());
		py_script.reload();
		py_script.import("scripts." PY_PdxPortraits);
		logger->init(script);
		py_script.attr("logger") = logger;

		py_script.attr(METHOD_Preprocess)(vec, name.toStdString(), reg, effect);

		if (reg)
		{
			auto registration_fn = py_script.attr(METHOD_Registration);
			auto reg_vec = registration_fn().cast<std::vector<std::string>>();
			registration_callback(reg_vec);
		}

		if (effect)
		{
			auto proper_name_effect_fn = py_script.attr(METHOD_ExtraEffect);
			proper_name_effect_fn(output_path.toStdString());
		}

		error_str.clear();
		return true;
	}
	catch (const std::exception& e)
	{
		qDebug() << e.what();
		error_str = QString::fromStdString(e.what());
		return false;
	}
}

void PythonEngine::setEnabledScript(const QString& script)
{
	auto action = script_group->findChild<QAction*>(script, Qt::FindDirectChildrenOnly);
	if (action == nullptr)
		action = script_group->findChild<QAction*>(gConfig->script(), Qt::FindDirectChildrenOnly);

	action->setChecked(true);
}
QString PythonEngine::enabledScript() const
{
	return script_group->checkedAction()->objectName();
}
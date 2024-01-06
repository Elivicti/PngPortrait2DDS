# 说明：本文件并无实际作用，但可用于让智能提示明白如何自动补全
# 注释中的成员变量/方法/代码逻辑实际存在，但未向Python脚本暴露，在本文件内仅用于协助编写者理解
#
# 需要注意，list中的内容都是按行写入的，例如["str1", "str2"]将被处理为：
#   str1
#   str2

# 未处理的肖像数据
class PortraitData:
	file_name: str			# 肖像文件名，后缀为.dds
	use_as_species: bool
	use_as_leaders: bool
	use_as_rulers:  bool

	def __init__(self, name: str, species: bool, leaders: bool, rulers: bool):
		self.file_name      = name
		self.use_as_species = species
		self.use_as_leaders = leaders
		self.use_as_rulers  = rulers

# 肖像基类，不应使用
class PortraitTexture:
	pass

# 静态肖像
class PortraitTextureStatic(PortraitTexture):
	textureFile: str

	def __init__(self, textureFile: str):
		self.textureFile = textureFile
	
	def to_strings() -> list[str]:
		# 将数据转换为字符串，每个索引为一行（不含换行符），结构如下：
		# textureFile = <textureFile>
		pass

# 非静态肖像
class PortraitTextureNonStatic(PortraitTexture):
	entity: str
	clothes_selector: str
	attachment_selector: str
	custom_attachment_label: str
	greeting_sound: str

	# 以下成员存在，但对Python脚本隐藏：
	# std::vector<std::string>          character_textures
	# std::map<std::string,std::string> addition

	# addition的key应只包含以下内容：
	#     custom_close_up_position_offset
	#     custom_close_up_scale
	#     custom_mid_close_up_position_offset
	#     custom_mid_close_up_scale

	def __init__(self
			, entity: str
			, clothes_selector:        str = "no_texture"
			, attachment_selector:     str = "no_texture"
			, custom_attachment_label: str = ""
			, greeting_sound:          str = ""):
		self.entity = entity
		self.clothes_selector = clothes_selector
		self.attachment_selector = attachment_selector
		self.custom_attachment_label = custom_attachment_label
		self.greeting_sound = greeting_sound

	def append_character_texture(texturepath: str):
		# 添加一个肖像
		pass
	def append_character_texture(texturepaths: list[str]):
		# 添加一组肖像
		pass

	def to_strings() -> list[str]:
		# 将数据转换为字符串，每个索引为一行（不含换行符），结构如下：
		# entity = <entity>
		# clothes_selector = <clothes_selector>
		# attachment_selector = <attachment_selector>
		# custom_attachment_label = <custom_attachment_label>（如果不是空字符串）
		# greeting_sound = <greeting_sound>                  （如果不是空字符串）
		# character_textures = {
		#     <<character_textures>的内容>
		# }
		# <<addition>的内容>
		pass

# 肖像组属性
class PortraitGroupProperty:
	# 以下成员存在，但对Python脚本隐藏：
	# std::vector<PortraitGroupPropertyClause> add
	# std::vector<PortraitGroupPropertyClause> set
	#### PortraitGroupPropertyClause用于记录单独的属性组

	def empty() -> bool:
		# 返回"add"类型与"set"类型的属性是否都为空
		pass

	def add(self, portrait_ids: list[str], trigger: str = ""):
		# 添加"add"类型的属性，示例如下：
		# add = {
		#     trigger = {<trigger>}（如果不是空字符串）
		#     portraits = {
		#         <<portrait_ids>的内容>
		#     }
		# }
		pass
	def set(self, portrait_ids: list[str], trigger: str = ""):
		# 添加"set"类型的属性，示例如下：
		# set = {
		#     trigger = {<trigger>}（如果不是空字符串）
		#     portraits = {
		#         <<portrait_ids>的内容>
		#     }
		# }
		pass
	
	# 通常无需调用此方法，因为在PortraitGroup的to_string方法中包含了此方法的调用
	def to_strings() -> list[str]:
		# 将数据转换为字符串，每个索引为一行（不含换行符），结构如下：
		# ...
		# add = {
		#     trigger = {<trigger>}（如果不是空字符串）
		#     portraits = {
		#         <<portrait_ids>的内容>
		#     }
		# }
		# ...
		# set = {
		#     trigger = {<trigger>}（如果不是空字符串）
		#     portraits = {
		#         <<portrait_ids>的内容>
		#     }
		# }
		# ...
		pass

# 肖像组
class PortraitGroup:
	default_portrait: str
	game_setup: PortraitGroupProperty
	species:    PortraitGroupProperty
	pop:        PortraitGroupProperty
	leader:     PortraitGroupProperty
	ruler:      PortraitGroupProperty

	def __init__(self, default_portrait: str) -> None:
		self.default_portrait = default_portrait
		pass

	def to_strings() -> list[str]:
		# 将数据转换为字符串，每个索引为一行（不含换行符），结构如下：
		# default = <default_portrait>
		# game_setup = {
		#     <game_setup.to_strings()>
		# }
		# species = {
		#     <species.to_strings()>
		# }
		# pop = {
		#     <pop.to_strings()>
		# }
		# leader = {
		#     <leader.to_strings()>
		# }
		# ruler = {
		#     <ruler.to_strings()>
		# }
		pass

# 日志记录器
class PythonLogger:
	## 以下类成员变量实际存在，但未向Python脚本暴露
	# log: TextIOWrapper	# 日志文件，通常为<年>-<月>-<日>.log

	# 以下展示了日志的输出格式
	def debug(self, msg: str):
		print("[<Current Time>][DEBG][<Script File>:<Line>]: {}\n".format(msg))
	def info(self, msg: str):
		print("[<Current Time>][INFO][<Script File>:<Line>]: {}\n".format(msg))
	def warning(self, msg: str):
		print("[<Current Time>][WARN][<Script File>:<Line>]: {}\n".format(msg))
	def critical(self, msg: str):
		print("[<Current Time>][CRIT][<Script File>:<Line>]: {}\n".format(msg))
	def fatal(self, msg: str):
		print("[<Current Time>][FATL][<Script File>:<Line>]: {}\n".format(msg))

# 应使用logger代替print来调试和输出信息
logger = PythonLogger()
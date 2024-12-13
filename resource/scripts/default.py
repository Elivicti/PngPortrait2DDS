# 必须import，否则会导致脚本无法读取
from PdxPortraits import *

class ProcessedData:
	id: str
	data: PortraitData

	def __init__(self, id: str, data: PortraitData) -> None:
		self.id = id
		self.data = data

reg_name: str = ""
portrait_list: list[ProcessedData] = []

def Preprocess(data: list[PortraitData], name: str, regi: bool, effect: bool) -> None:
	global reg_name
	reg_name = name
	i = 0
	for portrait in data:
		portrait_list.append(ProcessedData("{}_{:03d}".format(name, i), portrait))
		i += 1

def Registration() -> list[str]:
	logger.info("Registering <{}>".format(reg_name))

	ret: list[str] = [
		"portraits = {"
	]
	species_portraits: list[str] = []
	leader_portraits: list[str] = []
	ruler_portraits: list[str] = []
	
	# 静态类型只用指定图片路径
	for portrait in portrait_list:
		texture = PortraitTextureStatic("\"gfx/models/portraits/{}/{}\"".format(reg_name, portrait.data.file_name))
		ret.append("\t{} = {{ {} }}".format(portrait.id, texture.to_strings()[0]))
		# portrait groups
		if portrait.data.use_as_species:			# 开局、物种、人口
			species_portraits.append(portrait.id)
		if portrait.data.use_as_leaders:			# 领袖
			leader_portraits.append(portrait.id)
		if portrait.data.use_as_rulers:				# 统治者
			ruler_portraits.append(portrait.id)
	
	# 非静态肖像的处理示例：
	# for portrait in portrait_list:
	#	# 非静态肖像应指定一个entity
	# 	texture = PortraitTextureNonStatic("{}_leader_entity".format(reg_name))
	# 	texture.append_character_texture("\"gfx/models/portraits/{}/{}\"".format(reg_name, portrait.data.file_name))
	# 	ret.append("\t{} = {{".format(portrait.id))
	# 	for line in texture.to_strings():
	# 		ret.append("\t\t" + line)
	# 	ret.append("\t}")

	ret += [
		"}",
		"portrait_groups = {"
	]
	
	# portrait groups
	group = PortraitGroup(reg_name + "_000")
	if species_portraits:
		group.game_setup.add(species_portraits)
		group.species.add(species_portraits)
		group.pop.add(species_portraits)
	if leader_portraits:
		group.leader.add(leader_portraits)
	if ruler_portraits:
		group.ruler.add(ruler_portraits)

	for line in group.to_strings():
		ret.append("\t" + line)

	ret.append("}")
	return ret

def ExtraEffect(output_path: str) -> None:
	logger.info("Registering extra effect for <{}>@{}".format(reg_name, output_path))
	feffect_path = "{}/{}_name_effect.txt".format(output_path, reg_name)
	fyml_path    = "{}/{}_names.yml".format(output_path, reg_name)

	with open(feffect_path, "w+t") as effect:
		with open(fyml_path, "w+t", encoding="utf-8-sig") as yml:
			# random_list = {
			#     1 = { set_name = NAME_<name> set_leader_flag = l_flag_<name> change_leader_portrait = <id>
			#     ...
			# }
			###
			# NAME_<name>: "<name>"
			# ...
			effect.write("random_list = {\n")
			for i in portrait_list:
				name = i.data.file_name.removesuffix(".dds")
				effect.write(
					"\t1 = {{ change_leader_portrait = {id}  set_leder_flag = l_flag_{id}  set_name = NAME_{name} }}\n"
					.format(name=name, id=i.id))
				yml.write(" NAME_{name}: \"{name}\"\n".format(name=name))
			
			effect.write("}")


from typing import Self
from enum import Enum
from os import listdir, makedirs, path, rename
from shutil import move


class FormatTokenType(Enum):
	"""Type of token in the format string."""
	NONE     = 0
	VARIABLE = 1
	OTHER    = 2


class FormatToken:
	"""Token representing a part of the format string."""
	def __init__(self: Self, type: FormatTokenType, value: str) -> None:
		self.type  = type
		self.value = value

	def __repr__(self: Self) -> str:
		return f"FormatToken(type={self.type.name}, value='{self.value}')"


"""
class TVShow:
	def __init__(self: Self, filepath: str) -> None:
		self.filepath = filepath
		self.show    = None
		self.title   = None
		self.episode = None
		self.season  = None
"""


class NameScheme:
	"""Class to handle re-formatting name scheme for TV shows."""
	def __init__(self):
		self.variables: list[str]             = None

		self.input_format: list[FormatToken]  = None
		self.output_format: list[FormatToken] = None


	def load_format(self: Self, file: str) -> None:
		"""Load a format from a file."""
		# read format
		input_format: str = ""
		output_format: str = ""
		
		if not path.exists(file):
			raise FileNotFoundError(f"Format file '{file}' does not exist.")
		with open(file, "r") as file:
			format: list[str]  = file.read().strip().splitlines()
			input_format  = format[0]
			output_format = format[1]
		
		# parse input format to find variables
		self.variables     = []
		self.input_format  = []
		self.output_format = []

		token_type: FormatTokenType = FormatTokenType.NONE
		token: str = ""

		# parse input format
		for i in range(len(input_format)):
			if token_type == FormatTokenType.NONE:
				if input_format[i] == "<" and \
					(i == 0 or input_format[i - 1] != "\\"):
					if i > 0 and input_format[i - 1] == ">":
						raise ValueError("Invalid format: cannot have two variables with no characters separating them.")

					token_type = FormatTokenType.VARIABLE
					token = ""
				else:
					token_type = FormatTokenType.OTHER
					token = input_format[i]

			elif token_type == FormatTokenType.VARIABLE:
				if input_format[i] == ">":
					token_type = FormatTokenType.NONE
					self.variables.append(token)
					self.input_format.append(FormatToken(FormatTokenType.VARIABLE, token))
					token = ""
				else:
					token += input_format[i]

			elif token_type == FormatTokenType.OTHER:
				if input_format[i] == "<" and \
					(i == 0 or input_format[i - 1] != "\\"):
					token_type = FormatTokenType.VARIABLE
					self.input_format.append(FormatToken(FormatTokenType.OTHER, token))
					token = ""
				else:
					token += input_format[i]
		
		if token != "":
			if token_type == FormatTokenType.VARIABLE:
				raise ValueError("Invalid format: variable not closed with '>'.")
			elif token_type == FormatTokenType.OTHER:
				self.input_format.append(FormatToken(FormatTokenType.OTHER, token))
		
		token_type = FormatTokenType.NONE
		token = ""

		# parse output format
		for i in range(len(output_format)):
			if token_type == FormatTokenType.NONE:
				if output_format[i] == "<" and \
					(i == 0 or output_format[i - 1] != "\\"):
					token_type = FormatTokenType.VARIABLE
					token = ""
				else:
					token_type = FormatTokenType.OTHER
					token = output_format[i]

			elif token_type == FormatTokenType.VARIABLE:
				if output_format[i] == ">":
					token_type = FormatTokenType.NONE
					if token not in self.variables:
						raise ValueError(f"Variable '{token}' not found in input format.")
					self.output_format.append(FormatToken(FormatTokenType.VARIABLE, token))
					token = ""
				else:
					token += output_format[i]

			elif token_type == FormatTokenType.OTHER:
				if output_format[i] == "<" and \
					(i == 0 or output_format[i - 1] != "\\"):
					token_type = FormatTokenType.VARIABLE
					self.output_format.append(FormatToken(FormatTokenType.OTHER, token))
					token = ""
				else:
					token += output_format[i]

	def parse_file(self: Self, file: str) -> str:
		"""Parse a file name according to the input format."""
		extracting_type: FormatTokenType = FormatTokenType.NONE
		extracting: str = ""

		variable_values: dict[str, str] = {}

		file, extension = path.splitext(file)

		# parse the original file
		index: int = 0
		for i in range(len(self.input_format)):
			if extracting_type == FormatTokenType.NONE:
				if self.input_format[i].type == FormatTokenType.VARIABLE:
					extracting_type = FormatTokenType.VARIABLE
					extracting = ""
					if i == len(self.input_format) - 1:
						variable_values[self.input_format[i].value] = file[index:]
				elif self.input_format[i].type == FormatTokenType.OTHER:
					index += len(self.input_format[i].value)

			elif extracting_type == FormatTokenType.VARIABLE:
				next_match: str = self.input_format[i].value
				extracting = ""
				while file[index:index+len(next_match)] != next_match and index < len(file):
					extracting += file[index]
					index += 1
				variable_values[self.input_format[i - 1].value] = extracting
				extracting_type = FormatTokenType.NONE
				index += len(next_match)
				extracting = ""

			elif extracting_type == FormatTokenType.OTHER:
				index += len(self.input_format[i].value)

		# return the reformatted file
		reformatted: str = ""
		for i in range(len(self.output_format)):
			if self.output_format[i].type == FormatTokenType.VARIABLE:
				reformatted += variable_values[self.output_format[i].value]

			elif self.output_format[i].type == FormatTokenType.OTHER:
				reformatted += self.output_format[i].value

		return reformatted + extension


"""
Format for format file:
<VARIABLE_NAME>
"""

if __name__ == "__main__":
	FILE_EXTENSIONS: tuple[str, ...] = (".mp4", ".m4v", ".mov", ".qt", ".avi", ".wmv", ".mkv", ".webm", ".flv", ".f4v", ".3gp", ".3g2", ".mpg", ".mpeg", ".m2v", ".ts", ".m2ts", ".mts", ".vob", ".rm", ".rmvb", ".ogv", ".mxf")

	current_directory = path.dirname(path.abspath(__file__))

	formatter: NameScheme = NameScheme()
	if path.exists(path.join(current_directory, "name-scheme.format")):
		formatter.load_format(path.join(current_directory, "name-scheme.format"))
	else:
		input_format: str = input("Enter input format (e.g. <show> - <season>x<episode> - <title>): ")
		output_format: str = input("Enter output format (e.g. <show> - <season>x<episode> - <title>): ")

		with open(path.join(current_directory, "name-scheme.format"), "w") as file:
			file.write(f"{input_format}\n{output_format}")

		formatter.load_format(path.join(current_directory, "name-scheme.format"))

	files = listdir(current_directory)
	
	for file in files:
		if path.isfile(path.join(current_directory, file)) and \
			file.endswith(FILE_EXTENSIONS):
			new_file_path: str = formatter.parse_file(file)
			new_file = new_file_path.split("/")[-1]
			new_path = "/".join(new_file_path.split("/")[:-1])

			makedirs(path.join(current_directory, new_path), exist_ok=True)
			move(path.join(current_directory, file), path.join(current_directory, new_path, new_file))


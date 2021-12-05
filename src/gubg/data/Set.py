import csv
import io

class Set:
	def __init__(self):
		self.names = []
		self.items = []

	def __str__(self):
		return self.write_csv(io.StringIO(), "\t").getvalue()

	def item_count(self):
		return len(self.items)

	def read_csv(filepath, sep, has_header):
		with open(filepath, 'r') as file:
			return Set.load_csv(file, sep, has_header)

	def load_csv(file, sep, has_header):
		s = Set()

		csv_reader = csv.reader(file, delimiter=sep)
		if has_header:
			s.names = tuple(next(csv_reader))

		def interpret(v):
			try:
				return int(v)
			except:
				try:
					return float(v)	
				except:
					return v

		for row in csv_reader:
			if not has_header:
				s.names = tuple(f"row{ix}" for ix in range(len(row)))
				has_header = True
			s.items.append(tuple(interpret(value) for value in row))

		return s


	def save_csv(self, filepath, sep):
		with open(filepath, 'w') as file:
			self.write_csv(file, sep)

	def write_csv(self, file, sep):
		print(sep.join(self.names), file=file)
		for row in self.items:
			print(sep.join(str(v) for v in row), file=file)
		return file

	def create(names, item_gen):
		s = Set()
		s.names = tuple(names)
		s.items = [tuple(row) for row in item_gen]
		return s

	def indices(self, names):
		return [self.names.index(name) for name in names]
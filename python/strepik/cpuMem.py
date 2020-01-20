
import os

# проверяем установлен ли бит [ P ]
def is_p_not_set(row):
    return not bool(int(row) & 1)

# проверяем установлен ли бит [ PS ]
def is_ps_set(row):
    return bool(int(row) & 80)

# get phisical address
def get_phisical_address(row):
    return int(row) & 0xFFFFFFFFFF000

# получить значение индекса таблицы
# выбираем номер индекса [0, 1, 2, 3]
def get_table_index(row, num):
    if num == 0:
        return (int(row) >> 39) & 511
    if num == 1:
        return (int(row) >> 30) & 511
    if num == 2:
        return (int(row) >> 21) & 511
    if num == 3:
        return (int(row) >> 12) & 511

# получить значение индекса таблицы
# выбираем номер индекса [0, 1, 2, 3]
def get_offset(row, num):
    if num == 0:
        return int(row) & 0xFFFFFFFFF
    if num == 1:
        return int(row) & 0xFFFFFFF
    if num == 2:
        return int(row) & 0xFFFFF
    if num == 3:
        return int(row) & 0xFFF


def get_result_address(logicalValue, rootTableAddress, memoryMap):
  
  tableAddress = rootTableAddress
  for i in range(4): # i = [0, 1, 2, 3]
    index = get_table_index(logicalValue, i) # получаем индекс первой таблицы
    tableRow = memoryMap.get('{0}'.format(tableAddress + (index * 8)), 0) # получаем значение из памяти

    if is_p_not_set(tableRow):
      return "fault"
    else:
      if is_ps_set(tableRow): # значит это последняя таблица
        paddress = get_phisical_address(tableRow)
        offset = get_offset(tableRow, i)
        return paddress + offset
      else:
        tableAddress = get_phisical_address(tableRow)

  # дошли до самой последней таблицы
  return tableAddress + get_offset(logicalValue, 3)

      





print("Hello world!")

SCRIPT_DIR = os.path.dirname(__file__)
f_in = open(os.path.join(SCRIPT_DIR, "data.txt"),"r")
f_out = open(os.path.join(SCRIPT_DIR, "result.txt"), "w")

inputData = f_in.readlines()

ferstLine = inputData[0].split()
m = int(ferstLine[0])
q = int(ferstLine[1])
r = int(ferstLine[2])
print('аргумент1: {0}'.format(m))
print('аргумент1: {0}'.format(q))
print('аргумент1: {0}'.format(r)) # это число первой таблицы
print("------------")

# перебрать все пары
memory_map = {} # пустой словарь
OFFSET_1 = 1
for value in range(OFFSET_1, m + OFFSET_1):
  addressValue = inputData[value].split()
  memory_map.update({addressValue[0]: addressValue[1]})
  # print(inputData[value])

print("------------")
print("Memory map:")
print(memory_map)
print("------------")

# перебрать все значения
logicalAdresses = [] # пустой список
OFFSET_2 = OFFSET_1 + m
for value in range(OFFSET_2, q + OFFSET_2):
  logicalAdresses.append(inputData[value].strip())
  # print(inputData[value])

print("------------")
print("logicalAdresses:")
print(logicalAdresses)
print("------------")

# обработка
for value in logicalAdresses:
  result = get_result_address(value, r, memory_map)
  print('{0} : {1}'.format(value, result))
  f_out.write(str(result) + "\n")





f_in.close() 
f_out.close()



